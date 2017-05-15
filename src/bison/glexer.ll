%{
/* C++ string header, for string ops below */
#include <string>
#include <unicode/unistr.h>
#include "gparser.tab.hh"

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

lword [a-z][a-z]*\_*[a-z]*
uword [A-ZА-Я][a-zа-я]*\_*[a-zа-я]*
lbracket "("
rbracket ")"
predicate_section "predicates"
action_section "actions"
colon ":"

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

=               {
                    return token::ASSIGN;
                }

\|              {
                    return token::DELIM;
                }

"predicates"    {
                    return token::PRED_SECT;
                }

"actions"       {
                    return token::ACT_SECT;
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

"S"             {
                    yylval->build<UnicodeString>(yytext);
                    return token::START_RULE_SYMBOL;
                }

"empty"         {
                    yylval->build<UnicodeString>(yytext);
                    return token::EMPTY_WORD;
                }

\n              {
                    loc->lines(1);
                    /** return token::NEWLINE; **/
                }

{lword}         {
                    yylval->build<UnicodeString>( yytext );
                    return( token::WORD );
                }

{cyr_low}+      { 
                    yylval->build<UnicodeString>( yytext );
                    return( token::WORD );
                }

{uword}         {
                    yylval->build<UnicodeString>( yytext );
                    return( token::CAPITAL_WORD );
                }

{cyr_cap}+      {
                    yylval->build<UnicodeString>( yytext );
                    return( token::CAPITAL_WORD );
                }

";"             {
                    return token::RULE_END;
                }

[0-9]+          {
                    /** yylval->build<int>(yytext); **/
                    int num = atoi(yytext);
                    yylval->build<int>(num);
                    return token::NUM;
                }

[ \t]+          { std::cout << "Found whitespace" << std::endl;  }

{lbracket}      { std::cout << "Found left bracket" << std::endl; return token::LBRACKET; }

{rbracket}      { return token::RBRACKET; }

\"              { }

.               { std::cout << "Found unknown character at " << loc << std::endl; }

%%


