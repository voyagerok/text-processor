%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {tproc}
%define parser_class_name {GParser}

%code requires{
#include <memory>
#include <unicode/unistr.h>

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
%token <UnicodeString> PROP_MAX_REP_TOK
%token <UnicodeString> PROP_MIN_REP_TOK
%token <UnicodeString> PROP_QUOTED_TOK
%token <UnicodeString> PROP_START_UPPER_TOK
%token ASSIGN
%token DELIM
%token NEWLINE
%token <UnicodeString> CAPITAL_WORD
%token RBRACKET
%token LBRACKET
%token RULE_END

%type <std::vector<ComplexGrammarRule>> rule_list
%type <ComplexGrammarRule> rule
%type <SimpleGrammarRule> simple_rule
%type <ComplexGrammarRule> complex_rule
%type <std::vector<GRuleWord>> left_handle_chain
%type <GRuleWord> labeled_left_handle
%type <UnicodeString> left_handle
%type <std::vector<std::shared_ptr<Property>>> prop_list
%type <std::shared_ptr<Property>> prop
%type <std::shared_ptr<Property>> complex_prop_num
%type <std::shared_ptr<Property>> simple_prop

%locations

%%

rule_list
    : %empty
    | rule_list rule { driver.appendRule($2); }
    ;

rule 
    : complex_rule RULE_END { $$.swap($1); }
    ;

simple_rule
    : CAPITAL_WORD ASSIGN left_handle_chain { std::cout << "Reduce rule" << std::endl; $$ = SimpleGrammarRule { $1, $3}; }
    ;

complex_rule
    : simple_rule { $$ = ComplexGrammarRule { $1 }; }
    | complex_rule DELIM left_handle_chain { $1.append($3); $$.swap($1); }
    ;

left_handle_chain 
    : labeled_left_handle { $$ = { $1 }; }
    | left_handle_chain labeled_left_handle { $1.push_back($2); $$.swap($1); }
    ;

left_handle
    : WORD { std::cout << "Reduce left handle" << std::endl; $$.swap($1); }
    | CAPITAL_WORD { std::cout << "Reduce left handle" << std::endl; $$.swap($1); }
    | "\"" WORD "\"" { std::cout << "Reduce left handle" << std::endl; $$.swap($2); }
    ;

labeled_left_handle
    : left_handle { $$ = GRuleWord { $1 }; }
    | left_handle LBRACKET prop_list RBRACKET { GRuleWord temp = driver.makeRuleWord($1, $3); $$.swap(temp); }
    ;

prop_list
    : prop { $$ = std::vector<std::shared_ptr<Property>> { $1 }; }
    | prop_list prop { $1.push_back($2); $$.swap($1); }
    ;

prop
    : complex_prop_num
    | simple_prop
    ;

complex_prop_num
    : PROP_MIN_REP_TOK ASSIGN NUM { $$ = std::make_shared<ComplexNumProperty> ( GRuleWordPropType::MIN_REP, $3 ); }
    | PROP_MAX_REP_TOK ASSIGN NUM { $$ = std::make_shared<ComplexNumProperty> ( GRuleWordPropType::MAX_REP, $3 ); }
    ;

simple_prop
    : PROP_QUOTED_TOK { $$ = std::make_shared<SimpleProperty> ( GRuleWordPropType::QUOTED ); }
    | PROP_START_UPPER_TOK { $$ = std::make_shared<SimpleProperty> ( GRuleWordPropType::START_UPPER ); }
    ;

%%


void 
tproc::GParser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
