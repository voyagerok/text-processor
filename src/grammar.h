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
#include <memory>

//#include "grammar-parser.h"
#include "grammar-rule.h"
#include "g-parser-driver.hpp"
#include "action.hpp"

namespace tproc {

//class UnicodeString;
//class GrammarParser;
//struct SimpleGrammarRule;


class Grammar {
//    using RulesContainer = std::set<GRuleWordPtr>;
public:
//    using iter = std::map<UnicodeString, std::vector<SimpleGrammarRule>>::iterator;
//    using const_iter = std::map<UnicodeString, std::vector<SimpleGrammarRule>>::const_iterator;

//    Grammar(const Grammar&) = delete;
//    Grammar &operator=(const Grammar&) = delete;
    struct GRuleWithInfo {
        GRuleWordPtr root;
        std::set<GRuleWordPtr> nterms;
        std::set<GRuleWordPtr> terms;
    };
    Grammar(GRuleWithInfo &&ruleWithInfo);
    Grammar(const GRuleWithInfo &ruleWithInfo);
    void initAdditioanlParams();
//    ~Grammar();
//    bool initFromFile(const std::string &filename);
//    bool initFromPlainText(const UnicodeString &plainText);
//    void initFromDependencyRule(const DependencyGrammar &depRulePtr);
//    void initWithInfo(GRuleWithInfo &&ruleWithInfo);

//    std::vector<SimpleGrammarRule> getRulesForLeftHandle(const UnicodeString &leftHandle) const;
//    SimpleGrammarRule &getStartRule() const;
//    bool isNonTerminal(const UnicodeString &word) const { return rules.find(word) != rules.end(); }
//    RuleIndex getRuleIndex(const SimpleGrammarRule &rule) const;
    bool followWordsForNterminal(const GRuleWordPtr &nterm, std::set<GRuleWordPtr> &followWords) const;
    GRuleWordPtr getRoot() const { return this->root; }
//    UnicodeString getRootName() { return root->getChildWords().at(0).at(0)->getRawValue(); }
//    bool isEndOfInput(const UnicodeString &word) const;
//    bool isStartRule(const SimpleGrammarRule &rule) const;
    void printFirstSet();
    void printFollowSet();
//    bool getRuleForRuleIndex(const RuleIndex &index, SimpleGrammarRule &rule) const;

//    RulesContainer &getRules() { return parserDriver.getDefinedNterms(); }
//    GRuleWordPtr getRoot() { return parserDriver.getRootNterm(); }

//    iter begin();
//    iter end();
//    const_iter begin() const;
//    const_iter end() const;

private:
//    void readRules();
    void readRules(const std::vector<ComplexGrammarRule> &plainRules) ;
    bool validateRules();
    void buildFirstSet();
    void buildFollowSet();
    void addExplicitRule();
    std::set<GRuleWordPtr> firstSetForNonTerminal(const GRuleWordPtr &word);
    std::set<GRuleWordPtr> followSetForNonTerminal(const GRuleWordPtr &word);

    void applyPendingActions();

    using SimpleGrammarRulePtr = std::shared_ptr<SimpleGrammarRule>;
//    using GrammarParserPtr = std::shared_ptr<GrammarParser>;

//    SimpleGrammarRulePtr startRule {nullptr};
//    GrammarParserPtr parser {nullptr};
    GRuleWordPtr root = nullptr;

    std::vector<ActionPtr> pendingActions;

    std::map<GRuleWordPtr, std::set<GRuleWordPtr>> firstSet;
    std::map<GRuleWordPtr, std::set<GRuleWordPtr>> followSet;

    std::set<GRuleWordPtr> terminals;
    std::set<GRuleWordPtr> nterminals;
//    std::map<UnicodeString, std::vector<WordIndex>> nonTerminals;

//    GParserDriver parserDriver;

};

} /* namespace tproc */

#endif /* GRAMMAR_H_ */
