/*
 * GrammarRule.h
 *
 *  Created on: Apr 8, 2017
 *      Author: nikolay
 */

#ifndef GRAMMAR_RULE_H_
#define GRAMMAR_RULE_H_

#include <unicode/unistr.h>
#include <vector>
#include <map>
#include <iostream>
#include <memory>

#include "global-defs.h"
#include "predicate.hpp"

namespace tproc {

//#define PROP_QUOTED 01
//#define PROP_START_UPPER 02
//#define PROP_MIN_REP 04
//#define PROP_MAX_REP 010


enum class GRuleWordPropType {
    QUOTED = 01,
    START_UPPER = 02,
    MIN_REP = 04,
    MAX_REP = 010
};

//struct GRuleWord {
//    GRuleWord() = default;
//    GRuleWord(const UnicodeString &rawValue) :
//        rawValue { rawValue }, propertiesMask { 0 }, minRep { 1 }, maxRep { 1 } {}
//    GRuleWord(const UnicodeString &rawValue, unsigned mask, int minRep, int maxRep):
//        rawValue { rawValue }, propertiesMask { mask }, minRep { minRep }, maxRep { maxRep } {}

//    GRuleWord(GRuleWord &&otherWord) = default;
//    GRuleWord &operator=(GRuleWord &&otherWord) = default;
//    GRuleWord(const GRuleWord&) = default;
//    GRuleWord &operator=(const GRuleWord&) = default;

//    UnicodeString rawValue;
//    unsigned propertiesMask = 0;
//    int minRep = 1;
//    int maxRep = 1;

//    void swap(GRuleWord &other);

//    friend std::ostream &operator<<(std::ostream &os, const GRuleWord &ruleWord);
//    bool operator==(const GRuleWord &other) const;
//    bool operator!=(const GRuleWord &other) const;
//    bool operator==(const UnicodeString &other) const;
//    bool operator!=(const UnicodeString &other) const;
//};


class Token;

class GRuleWord;

using GRuleWordPtr = std::shared_ptr<GRuleWord>;
using ChildWords = std::vector<GRuleWordPtr>;

struct WordIndex {
    int ruleIndex;
    int position;

    bool operator==(const WordIndex other) const {
        return ruleIndex == other.ruleIndex &&
                position == other.position;
    }
    bool operator!=(const WordIndex other) const {
        return !(*this == other);
    }
};

struct ParentInfo {
//    WordInfo() = default;
//    WordInfo(const GRuleWordPtr &nterm, int ruleNum, int position) :
//        ruleIndex { nterm, ruleNum }, position { position } {}
//    RuleIndex ruleIndex;
//    int position;

    ParentInfo(const GRuleWordPtr &wordPtr, int ruleIndex, int position):
        nterm { wordPtr }, wordIndex { ruleIndex, position } {}

    GRuleWordPtr nterm;
    WordIndex wordIndex;

    bool operator==(const ParentInfo &other) const {
        return nterm == other.nterm && wordIndex == other.wordIndex;
    }
    bool operator!=(const ParentInfo &other) const {
        return !(*this == other);
    }
};

struct RuleIndex {
    GRuleWordPtr nterm;
    int index;
    bool operator==(const RuleIndex &other) const;
    bool operator!=(const RuleIndex &other) const;
    unsigned long hash() const { return std::hash<GRuleWordPtr>()(nterm) + std::hash<int>()(index); }
};

std::ostream &operator<<(std::ostream &os, RuleIndex ruleIndex);

//struct ParentInfo {
//    WordIndex wordIndex;
//    GRuleWordPtr parent;
//};

class GRuleWord {
private:
    std::vector<ParentInfo> parentNterms;
protected:
    UnicodeString rawValue;
    virtual bool equals(const GRuleWordPtr &other) const = 0;
    virtual std::ostream &print(std::ostream &os) const = 0;
public:
    GRuleWord() = default;
    GRuleWord(const UnicodeString &word) : rawValue { word } {}
    GRuleWord(UnicodeString &&word): rawValue { std::move(word) } {}
    virtual ~GRuleWord() = default;
    UnicodeString &getRawValue() { return this->rawValue; }
//    virtual bool checkProperty(GRuleWordPropType prop) const = 0;
    virtual bool checkToken(const Token&, const UnicodeString &word) const = 0;
    virtual bool isNonTerminal() const = 0;
    virtual unsigned getPredciatesSize() const = 0;
    virtual std::vector<PredicatePtr> &getPredicates() = 0;
//    friend bool operator==(const GRuleWordPtr &lhs, const GRuleWordPtr &rhs) { return lhs->equals(rhs); }
//    friend bool operator!=(const GRuleWordPtr &lhs, const GRuleWordPtr &rhs) { return !(lhs->equals(rhs)); }
    friend std::ostream &operator<<(std::ostream &os, const GRuleWordPtr &word) { return word->print(os); }

    virtual std::vector<ChildWords> &getChildWords() = 0;
    std::vector<ParentInfo> &getParentNterms() { return  this->parentNterms; }
    bool isEndOfInput() { return rawValue == END_OF_INPUT; }
    bool isEmptyWord() { return rawValue == EMPTY; }
    bool isRhsEmpty(int index) {
        auto childWords = getChildWords();
        if (childWords[index].size() == 1) {
            return childWords[index][0]->isEmptyWord();
        }
        return false;
    }
    int getRhsLength(int index) {
        if (isRhsEmpty(index)) {
            return 0;
        } else {
            return getChildWords().at(index).size();
        }
    }

//    virtual unsigned long hash_code() const = 0;
};

class NonTerminal final: public GRuleWord {
protected:
    bool equals(const GRuleWordPtr &other) const override;
    std::ostream &print(std::ostream &os) const override;
    std::vector<ChildWords> childWords;
public:
    NonTerminal() = default;
    NonTerminal(const UnicodeString &word) : GRuleWord { word } {}
    NonTerminal(UnicodeString &&word): GRuleWord { std::move(word) } {}
    NonTerminal(const UnicodeString &word, const ChildWords &words) : GRuleWord { word }, childWords { { words } } {}
    NonTerminal(const UnicodeString &word, ChildWords &&words) : GRuleWord { word }, childWords { { std::move(words) } } {}
    NonTerminal(UnicodeString &&word, ChildWords &&words) : GRuleWord { std::move(word) }, childWords { { std::move(words) } } {}
    bool isNonTerminal() const override { return true; }
//    bool checkProperty(GRuleWordPropType) const override { throw std::runtime_error("bad call"); }
    bool checkToken(const Token&, const UnicodeString &word) const override {
        throw std::runtime_error("bad call: NonTerminal class has no implementation for checkToken(const UnicodeString&)");
    }
    unsigned getPredciatesSize() const override { throw std::runtime_error("bad call: NonTermonal class has no implementation for getPredicatesSize()"); }
    std::vector<PredicatePtr> &getPredicates() override { throw std::runtime_error("bad call: NonTermonal class has no implementation for getPredicates()"); }
//    bool operator==(const NonTerminal &other) const { return this->rawValue == other.rawValue; }
//    bool operator!=(const NonTerminal &other) const { return this->rawValue == other.rawValue; }
    void swap(NonTerminal &other);

    std::vector<ChildWords> &getChildWords() override { return this->childWords; }

//    unsigned long hash_code() const override { return rawValue.hashCode(); }
};

class Terminal final: public GRuleWord {
private:
    std::vector<PredicatePtr> predicates;
protected:
    bool equals(const GRuleWordPtr &other) const override;
    std::ostream &print(std::ostream &os) const override;
public:
    Terminal() = default;
    Terminal(const UnicodeString &word, const std::vector<PredicatePtr> &predicates) :
        GRuleWord { word }, predicates { predicates } {}
    Terminal(UnicodeString &&word, std::vector<PredicatePtr> &&predicates):
        GRuleWord { std::move(word) }, predicates { std::move (predicates) } {}
    Terminal(UnicodeString &&word): GRuleWord(std::move(word)) {}
    Terminal(const UnicodeString &word): GRuleWord(word) {}
    bool isNonTerminal() const override { return false; }
//    unsigned &getPropertyMask() { return propsMask; }

    bool checkToken(const Token&, const UnicodeString & word) const override;
    unsigned getPredciatesSize() const override { return predicates.size(); }
//    bool checkProperty(GRuleWordPropType propType) const override;
//    bool operator==(const Terminal &other) const; //{ return  this->rawValue == other.rawValue && propsMask == other.propsMask; }
//    bool operator!=(const Terminal &other) const; //{ return  this->rawValue != other.rawValue || propsMask != other.propsMask; }

    std::vector<ChildWords> &getChildWords() override {
        throw std::runtime_error("bad call: Terminal class has no implementation for getChildWords()");
    }
//    std::vector<PredicatePtr> &getPredicates() { return predicates; }
    std::vector<PredicatePtr> &getPredicates() override { return predicates; }

//    unsigned long hash_code() const override {
};

void printRule(const GRuleWordPtr &rule);

//struct StandardTerminalStorage {
//    static GRuleWordPtr getEndOfInputTerminal() {
//        static GRuleWordPtr endOfInputTerm = nullptr;
//        if (!endOfInputTerm) {
//            endOfInputTerm = std::make_shared<Terminal>(END_OF_INPUT);
//        }
//        return endOfInputTerm;
//    }
//    static GRuleWordPtr getEmptyTerminal() {
//        static GRuleWordPtr emptyTerm = nullptr;
//        if (!emptyTerm) {
//            emptyTerm = std::make_shared<Terminal>(EMPTY);
//        }
//        return emptyTerm;
//    }
//};

struct SimpleGrammarRule {
    SimpleGrammarRule() {}
    SimpleGrammarRule(const std::shared_ptr<NonTerminal> &lhs,
                      const std::vector<GRuleWordPtr> &rhs,
                      bool isValid = true) : lhs { lhs }, rhs {rhs}, isValid {isValid} {}
    SimpleGrammarRule(const UnicodeString &lhs,
                      const std::vector<GRuleWordPtr> &rhs,
                      bool isValid = true) : lhs { std::make_shared<NonTerminal>(lhs) }, rhs {rhs}, isValid {isValid} {}

    std::shared_ptr<NonTerminal> lhs;
    std::vector<GRuleWordPtr> rhs;
    bool isValid;

    void swap(SimpleGrammarRule &other);

    bool operator==(const SimpleGrammarRule &other) const;
    bool operator!=(const SimpleGrammarRule &other) const;
    friend std::ostream &operator<<(std::ostream &os, const SimpleGrammarRule &rule);
};

struct ComplexGrammarRule {
    ComplexGrammarRule(const UnicodeString &lhs, const std::vector<SimpleGrammarRule> rhs):
        lhs { std::make_shared<NonTerminal>(lhs) }, rhs { rhs } {}
    ComplexGrammarRule() = default;
    ComplexGrammarRule(const UnicodeString &lhs, const std::vector<GRuleWordPtr> &rhs):
        lhs { std::make_shared<NonTerminal>(lhs) }, rhs { std::vector<SimpleGrammarRule> { SimpleGrammarRule { lhs, rhs } }} {}
    ComplexGrammarRule(const SimpleGrammarRule &rule):
        lhs { rule.lhs }, rhs { std::vector<SimpleGrammarRule> { rule }} {}

    std::shared_ptr<NonTerminal> lhs;
    std::vector<SimpleGrammarRule> rhs;
    bool isValid = true;

    void append(const std::vector<GRuleWordPtr> &ruleWords);
    void swap(ComplexGrammarRule &other);

    friend std::ostream &operator<<(std::ostream &os, const ComplexGrammarRule &rule);
};

} /* namespace tproc */

#endif /* GRAMMAR_RULE_H_ */
