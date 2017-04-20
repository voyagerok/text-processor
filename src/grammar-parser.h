/*
 * GrammarParser.h
 *
 *  Created on: Apr 8, 2017
 *      Author: nikolay
 */

#ifndef GRAMMAR_PARSER_H_
#define GRAMMAR_PARSER_H_

#include <memory>
#include <vector>

//#include "grammar-rule.h"
#include <unicode/unistr.h>
#include <unicode/regex.h>

//class GrammarRule;
//using icu::UnicodeString;
//using icu::RegexMatcher;

namespace tproc {

struct ComplexGrammarRule;

class GrammarParser {
public:
    GrammarParser();
    ~GrammarParser();
    bool beginParseFromFile(const std::string &filename);
    bool beginParseFromPlainText(const icu::UnicodeString &grammar);
    bool getNextRule(ComplexGrammarRule &rule);
    std::vector<icu::UnicodeString> gerGrammarLines() { return m_GrammarLines; }
private:
    icu::RegexMatcher *m_RegularRuleMatcher {nullptr};
//    icu::RegexMatcher *m_StartRuleMatcher {nullptr};
    icu::RegexMatcher *m_RightHandleMatcher {nullptr};
    std::vector<icu::UnicodeString> m_GrammarLines;
};

class GrammarParserException: public std::runtime_error {
public:
    GrammarParserException(const char *what_arg): std::runtime_error(what_arg) {}
};

} /* namespace tproc */

#endif /* GRAMMAR_PARSER_H_ */
