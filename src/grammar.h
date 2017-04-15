/*
 * grammar.h
 *
 *  Created on: Apr 9, 2017
 *      Author: nikolay
 */

#ifndef GRAMMAR_H_
#define GRAMMAR_H_

#include <vector>
#include <map>
#include <unicode/unistr.h>

namespace tproc {

//class UnicodeString;
class GrammarParser;
struct SimpleGrammarRule;

class Grammar {
    using RulesContainer = std::map<UnicodeString, std::vector<SimpleGrammarRule>>;
public:
    using iter = std::map<UnicodeString, std::vector<SimpleGrammarRule>>::iterator;
    using const_iter = std::map<UnicodeString, std::vector<SimpleGrammarRule>>::const_iterator;
//    Grammar(const char *filename);
//    Grammar(const UnicodeString &plainText);
    ~Grammar();
    bool initFromFile(const char *filename);
    bool initFromPlainText(const UnicodeString &plainText);
//    std::vector<UnicodeString> getAllLeftHandles();
    std::vector<SimpleGrammarRule> getRulesForLeftHandle(const UnicodeString &leftHandle) const;
    iter begin();
    iter end();
    const_iter begin() const;
    const_iter end() const;
    SimpleGrammarRule &getStartRule() const;
private:
    void readRules();
    bool validateRules();

    SimpleGrammarRule *startRule = nullptr;
    RulesContainer rules;
    GrammarParser *parser {nullptr};
};

} /* namespace tproc */

#endif /* GRAMMAR_H_ */
