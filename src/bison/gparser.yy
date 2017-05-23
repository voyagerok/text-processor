%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {tproc}
%define parser_class_name {GParser}

%code requires{
#include <memory>
#include <unicode/unistr.h>
#include <unicode/ustream.h>

#include "g-parser-driver.hpp"
#include "grammar-rule.h"
    namespace tproc {
        class GParserDriver;
        class GScanner;
        class GRuleWord;
   }
#include "grammar-words-storage.hpp"

// The following definitions is missing when %locations isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}

%parse-param { GScanner  &scanner  }
%parse-param { GParserDriver  &driver  }

%code{
#include "g-scanner.hpp"
#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert
%define parse.error verbose

%token END 0 "end of file"
%token <UnicodeString> WORD CAPITAL_WORD QUOTED_WORD
%token <int> NUM
%token ACTION_QUOTED_TOK PROP_START_UPPER_TOK
%token ASSIGN
%token DELIM
%token NEWLINE
%token RBRACKET
%token LBRACKET
%token SEMICOLON
%token ACTION_RANGE ACTION_MIN_REP_TOK ACTION_MAX_REP_TOK
%token ELLIPSIS
%token ACT_SECT PROPS_SECT HINT_WORDS_SECT
%token COLON DOUBLE_QUOTE
%token <UnicodeString> START_RULE_SYMBOL EMPTY_WORD
%token RULE_SECTION_HEADER COMMAND_SECTION_HEADER DEP_SECTION_HEADER
%token NEAR_BEFORE_KEYWORD NEAR_AFTER_KEYWORD BEFORE_KEYWORD AFTER_KEYWORD FIND
%token LENGTH_SECT

%type <std::vector<GRuleWordPtr>> rule_list
%type <GRuleWordPtr> rule complex_rule
%type <GRuleWordPtr> simple_rule
%type <std::vector<GRuleWordPtr>> rhs_chain
%type <GRuleWordPtr> labeled_rhs_term labeled_rhs_nterm
%type <UnicodeString> rhs_term rhs_nterm rhs_term_empty
%type <std::vector<PredicatePtr>> prop_list
%type <PredicatePtr> prop simple_prop length_info
%type <ActionPtr> action
%type <std::vector<ActionPtr>> action_list
%type <DependencyStruct> dependency dep_rhs_chain long_dep_rhs_chain
%type <std::vector<DependencyStruct>> dep_list
%type <DependencyRulePtr> command, command_find
%type <std::vector<DependencyRulePtr>> command_list
%type <std::vector<UnicodeString>> hint_word_list

%locations

%%
section_list
    : rule_section dep_section command_section
    ;

rule_list
    : %empty
    | rule_list rule { driver.appendRule($2); }
    ;

rule 
    : complex_rule SEMICOLON { $$ = $1; }
    ;

simple_rule
    : CAPITAL_WORD ASSIGN rhs_chain { $$ = driver.createRule ( std::move($1), std::move($3)); driver.fixParentInfo($$); }
    ;

complex_rule
    : simple_rule { $$ = $1; }
    | complex_rule DELIM rhs_chain { $1->getChildWords().push_back($3); $$.swap($1); driver.fixParentInfo($$); }
    ;

rhs_chain
    : labeled_rhs_term { $$ = { $1 }; }
    | rhs_chain labeled_rhs_term { $1.push_back($2); $$.swap($1); }
    | labeled_rhs_nterm { $$ = { $1 }; }
    | rhs_chain labeled_rhs_nterm { $1.push_back($2); $$.swap($1); }
    | rhs_term_empty { $$ = { GWordStorage::getEmptyTerminal() }; }
    ;

rhs_nterm
    : CAPITAL_WORD { $$.swap($1); }
    ;

labeled_rhs_nterm
    : rhs_nterm { $$ = driver.handleNtermReduction ( std::move($1) ); }
    | rhs_nterm LBRACKET action_list RBRACKET { $$ = driver.handleNtermReduction ( std::move($1) ); driver.fixAndSaveActionList($$, std::move($3)); }
    ;

rhs_term
    : WORD {$$.swap($1); }
    | DOUBLE_QUOTE WORD DOUBLE_QUOTE {$$.swap($2); }
    ;

rhs_term_empty
    : EMPTY_WORD {$$.swap($1); }
    ;

labeled_rhs_term
    : rhs_term { $$ = driver.handleTermReduction(std::move($1)); }
    /*| rhs_term LBRACKET prop_list RBRACKET { $$ = driver.handleTermReduction(std::move($1), std::move($3));  }*/
    /*| rhs_term LBRACKET action_list RBRACKET { $$ = driver.handleTermReduction(std::move($1)); driver.fixAndSaveActionList($$, std::move($3)); }*/
    | rhs_term LBRACKET action_list prop_list length_info RBRACKET 
    { 
        if ($5) 
            $4.push_back($5); 
        $$ = driver.handleTermReduction(std::move($1), std::move($4)); 
        driver.fixAndSaveActionList($$, std::move($3)); 
    }
    ;

action_list
    : %empty { $$ = {}; }
    | ACT_SECT COLON action { $$ = { $3 }; }
    | action_list action { $1.push_back($2); $$.swap($1); }
    ;

action
    : ACTION_RANGE ASSIGN NUM ELLIPSIS NUM { $$ = std::make_shared<MinMaxAction>($3, $5); }
    | ACTION_QUOTED_TOK { $$ = std::make_shared<QuoteAction>(); }
    ;

prop_list
    : %empty { $$ = {}; }
    | PROPS_SECT COLON prop { $$ = { $3 }; }
    | prop_list prop { $1.push_back($2); $$.swap($1); }
    ;

prop
    : simple_prop { $$.swap($1); }
    ;

simple_prop
    : PROP_START_UPPER_TOK { $$ = std::make_shared<UpperCaseFirstPredicate> (); }
    ;

length_info
    : %empty { $$ = nullptr; }
    | LENGTH_SECT COLON NUM { $$ = std::make_shared<LengthPredicate>($3); }
    | LENGTH_SECT COLON NUM ELLIPSIS NUM { $$ = std::make_shared<LengthPredicate>($3,$5); }
    ;

dep_section
    : %empty
    | DEP_SECTION_HEADER dep_list { driver.handleDependencies($2); }
    ;

rule_section
    : RULE_SECTION_HEADER rule_list { driver.applyPendingActions(); }
    ;

command_section
    : COMMAND_SECTION_HEADER command_list { driver.processCommandList(std::move($2)); }
    ;

command_list
    : command { $$ = { $1 }; }
    | command_list command { $1.push_back($2); $$.swap($1); }
    ;

command
    : command_find SEMICOLON { $$.swap($1); }
    | command_find LBRACKET hint_word_list RBRACKET SEMICOLON { auto res = driver.handleHintWords($1, std::move($3)); $$.swap(res); }
    ;

command_find
    : FIND CAPITAL_WORD { std::string err; if(!driver.handleCommandFindReduction($2,$$,err)) { error(@1,err); return -1; } }
    ;

hint_word_list
    : HINT_WORDS_SECT COLON DOUBLE_QUOTE WORD DOUBLE_QUOTE { $$ = { $4 }; }
    | hint_word_list DOUBLE_QUOTE WORD DOUBLE_QUOTE { $1.push_back( $3 ); $$.swap($1); }
    ;

dep_list
    : dependency { $$ = { $1 }; } 
    | dep_list dependency { $1.push_back($2); $$.swap($1); }
    ;

dependency
    : dep_rhs_chain SEMICOLON { $$.swap($1); }
    | long_dep_rhs_chain SEMICOLON { $$.swap($1); }
    ;

dep_rhs_chain
    : CAPITAL_WORD COLON CAPITAL_WORD NEAR_AFTER_KEYWORD CAPITAL_WORD { std::string errMsg; if (!driver.createDependencyStruct($1, $3, $5, DependencyType::NEAR, $$, errMsg)) { error(@1, errMsg); return -1; } }
    | dep_rhs_chain NEAR_AFTER_KEYWORD CAPITAL_WORD { std::string errMsg; if (!driver.appendDependencyStruct($3, $1, errMsg)) { error(@1, errMsg); return -1; } $$.swap($1); }
    ;

long_dep_rhs_chain
    : CAPITAL_WORD COLON CAPITAL_WORD AFTER_KEYWORD CAPITAL_WORD { std::string errMsg; if (!driver.createDependencyStruct($1, $3, $5, DependencyType::FAR, $$, errMsg)) { error(@1, errMsg); return -1; } }
    | long_dep_rhs_chain AFTER_KEYWORD CAPITAL_WORD { std::string errMsg; if (!driver.appendDependencyStruct($3, $1, errMsg)) { error(@1, errMsg); return -1; } $$.swap($1); }
    ;


%%


void 
tproc::GParser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
