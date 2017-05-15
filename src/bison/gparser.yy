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

%token END 0 "end of file"
%token <UnicodeString> WORD
%token <int> NUM
%token ACTION_QUOTED_TOK PROP_START_UPPER_TOK
%token ASSIGN
%token DELIM
%token NEWLINE
%token <UnicodeString> CAPITAL_WORD
%token RBRACKET
%token LBRACKET
%token RULE_END
%token ACTION_RANGE ACTION_MIN_REP_TOK ACTION_MAX_REP_TOK
%token ELLIPSIS
%token ACT_SECT
%token PRED_SECT
%token COLON
%token <UnicodeString> START_RULE_SYMBOL EMPTY_WORD

%type <std::vector<GRuleWordPtr>> rule_list
%type <GRuleWordPtr> rule complex_rule
%type <GRuleWordPtr> simple_rule
%type <std::vector<GRuleWordPtr>> rhs_chain
%type <GRuleWordPtr> labeled_rhs_term labeled_rhs_nterm
%type <UnicodeString> rhs_term rhs_nterm rhs_term_empty
%type <std::vector<PredicatePtr>> prop_list
%type <PredicatePtr> prop simple_prop
%type <ActionPtr> action
%type <std::vector<ActionPtr>> action_list

%locations

%%

rule_list
    : %empty
    | rule_list rule { driver.appendRule($2); }
    ;

rule 
    : complex_rule RULE_END { $$ = $1; }
    ;

simple_rule
    : CAPITAL_WORD ASSIGN rhs_chain { $$ = std::make_shared<NonTerminal> ( std::move($1), std::move($3)); driver.fixParentInfo($$); }
    | START_RULE_SYMBOL ASSIGN rhs_chain { $$ = std::make_shared<NonTerminal>(std::move($1), std::move($3)); driver.fixParentInfo($$); }
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
    | "\"" WORD "\"" {$$.swap($2); }
    ;

rhs_term_empty
    : EMPTY_WORD {$$.swap($1); }
    ;

labeled_rhs_term
    : rhs_term_empty { $$ = StandardTerminalStorage::getEmptyTerminal(); }
    | rhs_term { $$ = driver.handleTermReduction(std::move($1)); }
    | rhs_term LBRACKET prop_list RBRACKET { $$ = driver.handleTermReduction(std::move($1), std::move($3));  }
    | rhs_term LBRACKET action_list RBRACKET { $$ = driver.handleTermReduction(std::move($1)); driver.fixAndSaveActionList($$, std::move($3)); }
    | rhs_term LBRACKET action_list prop_list RBRACKET { $$ = driver.handleTermReduction(std::move($1), std::move($4)); driver.fixAndSaveActionList($$, std::move($3)); }
    ;

action_list
    : ACT_SECT COLON action { $$ = { $3 }; }
    | action_list action { $1.push_back($2); $$.swap($1); }
    ;

action
    : ACTION_RANGE ASSIGN NUM ELLIPSIS NUM { $$ = std::make_shared<MinMaxAction>($3, $5); }
    | ACTION_QUOTED_TOK { $$ = std::make_shared<QuoteAction>(); }
    ;

prop_list
    : PRED_SECT COLON prop { $$ = { $3 }; }
    | prop_list prop { $1.push_back($2); $$.swap($1); }
    ;

prop
    : simple_prop { $$.swap($1); }
    ;

simple_prop
    : PROP_START_UPPER_TOK { $$ = std::make_shared<UpperCaseFirstPredicate> (); }
    ;


%%


void 
tproc::GParser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
