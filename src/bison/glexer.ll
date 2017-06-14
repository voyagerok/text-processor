%{
/* C++ string header, for string ops below */
#include <string>
#include <unicode/unistr.h>
#include "gparser.tab.hh"
#include "utils/logger.h"

/* Implementation of yyFlexScanner */ 
#include "g-scanner.hpp"
#undef  YY_DECL
#define YY_DECL int tproc::GScanner::yylex( tproc::GParser::semantic_type * const lval, tproc::GParser::location_type *loc )

/* typedef to make the returns for the tokens shorter */
using token = tproc::GParser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option debug
%option nodefault
%option yyclass="tproc::GScanner"
%option noyywrap
%option c++
%option yylineno

cyr_pref1 \xd0
cyr_pref2 \xd1
cyr_cap_suf [\x90-\xaf]
cyr_suf1 [\xb0-\xbf]
cyr_suf2 [\x80-\x8f]
cyr_cap {cyr_pref1}{cyr_cap_suf}
cyr_low {cyr_pref1}{cyr_suf1}|{cyr_pref2}{cyr_suf2}

lword [a-z][a-z]*(\_*[a-z]*)*
uword [A-Z][a-zA-Z]*(\_*[a-zA-Z]*)*
lbracket "("
rbracket ")"
predicate_section "predicates"
action_section "actions"
colon ":"
find_keyword [Ff][Ii][Nn][Dd]
near_before "near_before"
near_after "near_after"
before "before"
after "after"
double_quote "\""


%%
%{          /** Code executed at the beginning of yylex **/
    yylval = lval;
%}

=               {
                    Logger::getLogger() << "Found assign" << std::endl;
                    return token::ASSIGN;
                }

\|              {
                    Logger::getLogger() << "Found delimeter" << std::endl;
                    return token::DELIM;
                }

"props"         {
                    return token::PROPS_SECT;
                }

"actions"       {
                    return token::ACT_SECT;
                }

"hint_words"    {
                    return token::HINT_WORDS_SECT;
                }

{colon}         {
                    return token::COLON;
                }


"max_rep"       {
                    /** yylval->build<UnicodeString>(yytext); **/
                    return token::ACTION_MAX_REP_TOK;
                }

"min_rep"       {
                    /** yylval->build<UnicodeString>(yytext); **/
                    return token::ACTION_MIN_REP_TOK;
                }

"range"         {
                    return token::ACTION_RANGE;
                }

"\.\."        {
                    return token::ELLIPSIS;
                }

"quoted"        {
                    /** yylval->build<UnicodeString>(yytext); **/
                    return token::ACTION_QUOTED_TOK;
                }

"upper1"        {
                    /** yylval->build<UnicodeString>(yytext); **/
                    return token::PROP_START_UPPER_TOK; 
                }

"len"           {
                    return token::LENGTH_SECT;
                }

"empty"         {
                    Logger::getLogger() << "Found empty word" << std::endl;
                    yylval->build<UnicodeString>(yytext);
                    return token::EMPTY_WORD;
                }

"num"           {
                    return token::NUM_TERM;
                }

"PersonName"    {
                    return token::PERSON_NAME;
                }

"AgreementDate" {
                    return token::AGREEMENT_DATE;
                }

"FullDate"      {
                    return token::FULL_DATE;
                }

"ApartmentNum"  {
                    return token::APARTMENT_NUM;
                }

"WordSequence"  {
                    return token::WORD_SEQUENCE;    
                }

"Town"          {
                    return token::TOWN;
                }

"Street"        {
                    return token::STREET;
                }

"as"            {
                    return token::ALIAS_OP;
                }

"with"          {
                    return token::WITH;
                }

"deps"          {
                    return token::DEPS;
                }

"left"          {
                    return token::LEFT_DEPS_SECT;
                }

"right"         {
                    return token::RIGHT_DEPS_SECT;
                }

"upper"         {
                    return token::UPPER_DEPS_SECT;
                }

"lower"         {
                    return token::LOWER_DEPS_SECT;
                }

"%rules"        {
                    return token::RULE_SECTION_HEADER;
                }

"%commands"     {
                    return token::COMMAND_SECTION_HEADER;
                }

"%dependencies" {
                    return token::DEP_SECTION_HEADER;
                }

{near_before}   {
                    return token::NEAR_BEFORE_KEYWORD;
                }

{near_after}    {
                    return token::NEAR_AFTER_KEYWORD;
                }

{before}        {
                    return token::BEFORE_KEYWORD;
                }

{after}         {
                    return token::AFTER_KEYWORD;
                }

{find_keyword}  {
                    return token::FIND;
                }

\n              {
                    loc->lines(1);
                    /** return token::NEWLINE; **/
                }

{lword}         {
                    Logger::getLogger() << "Found word" << std::endl;
                    yylval->build<UnicodeString>( yytext );
                    return( token::WORD );
                }

{cyr_low}+      {
                    Logger::getLogger() << "Found word" << std::endl; 
                    yylval->build<UnicodeString>( yytext );
                    return( token::WORD );
                }

{uword}         {
                    Logger::getLogger() << "Found capital word" << std::endl;
                    yylval->build<UnicodeString>( yytext );
                    return( token::CAPITAL_WORD );
                }

{cyr_cap}+      {
                    Logger::getLogger() << "Found capital word" << std::endl;
                    yylval->build<UnicodeString>( yytext );
                    return( token::CAPITAL_WORD );
                }

";"             {
                    Logger::getLogger() << "Found semicolon" << std::endl;
                    return token::SEMICOLON;
                }

[0-9]+          {
                    /** yylval->build<int>(yytext); **/
                    int num = atoi(yytext);
                    yylval->build<int>(num);
                    return token::NUM;
                }

[ \t]+          { Logger::getLogger() << "Found whitespace" << std::endl;  }

{lbracket}      { Logger::getLogger() << "Found left bracket" << std::endl; return token::LBRACKET; }

{rbracket}      { return token::RBRACKET; }

{double_quote}  { Logger::getLogger() << "Found quote" << std::endl; return token::DOUBLE_QUOTE; }


.               { }

%%


