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
#include <set>

namespace tproc {

//class UnicodeString;
class GrammarParser;
struct SimpleGrammarRule;

struct RuleIndex {
    UnicodeString leftHandle;
    int simpleRuleNumber;
    bool operator==(const RuleIndex &other) const;
    bool operator!=(const RuleIndex &other) const;
    unsigned long hash() const { return leftHandle.hashCode() + std::hash<int>()(simpleRuleNumber); }
};

struct WordInfo {
    RuleIndex ruleIndex;
    int position;
};

std::ostream &operator<<(std::ostream &os, RuleIndex ruleIndex);

class Grammar {
    using RulesContainer = std::map<UnicodeString, std::vector<SimpleGrammarRule>>;
public:
    using iter = std::map<UnicodeString, std::vector<SimpleGrammarRule>>::iterator;
    using const_iter = std::map<UnicodeString, std::vector<SimpleGrammarRule>>::const_iterator;

    ~Grammar();
    bool initFromFile(const char *filename);
    bool initFromPlainText(const UnicodeString &plainText);
    std::vector<SimpleGrammarRule> getRulesForLeftHandle(const UnicodeString &leftHandle) const;
    SimpleGrammarRule &getStartRule() const;
    bool isNonTerminal(const UnicodeString &word) const { return rules.find(word) != rules.end(); }
    RuleIndex getRuleIndex(const SimpleGrammarRule &rule) const;
    bool followWordsForNterminal(const UnicodeString &nterm, std::set<UnicodeString> &followWords) const;
    bool isEndOfInput(const UnicodeString &word) const;
    bool isStartRule(const SimpleGrammarRule &rule) const;
    void printFirstSet();
    void printFollowSet();

    iter begin();
    iter end();
    const_iter begin() const;
    const_iter end() const;

private:
    void readRules();
    bool validateRules();
    void buildFirstSet();
    void buildFollowSet();
    void addExplicitRule();
    std::set<UnicodeString> firstSetForNonTerminal(const UnicodeString &word);
    std::set<UnicodeString> followSetForNonTerminal(const UnicodeString &word);

    SimpleGrammarRule *startRule = nullptr;
    RulesContainer rules;
    GrammarParser *parser {nullptr};

    std::map<UnicodeString, std::set<UnicodeString>> firstSet;
    std::map<UnicodeString, std::set<UnicodeString>> followSet;

    std::set<UnicodeString> terminals;
    std::map<UnicodeString, std::vector<WordInfo>> nonTerminals;
};

} /* namespace tproc */

#endif /* GRAMMAR_H_ */
