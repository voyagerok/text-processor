#ifndef __GSCANNER_HPP__
#define __GSCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif


#include "gparser.tab.hh"
#include "location.hh"

namespace tproc {

class GScanner : public yyFlexLexer{
public:
   
   GScanner(std::istream *in) : yyFlexLexer(in)
   {
      loc = new tproc::GParser::location_type();
   };
   virtual ~GScanner() {
      delete loc;
   };

   //get rid of override virtual function warning
   using FlexLexer::yylex;

   virtual
   int yylex( tproc::GParser::semantic_type * const lval, 
              tproc::GParser::location_type *location );
   // YY_DECL defined in mc_lexer.l
   // Method body created by flex in mc_lexer.yy.cc


private:
   /* yyval ptr */
   GParser::semantic_type *yylval = nullptr;
   /* location ptr */
   GParser::location_type *loc    = nullptr;
};

} /* end namespace tproc */

#endif /* END __GSCANNER_HPP__ */
