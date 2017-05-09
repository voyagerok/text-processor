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

#include "global-defs.h"

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

struct GRuleWord {
    GRuleWord() = default;
    GRuleWord(const UnicodeString &rawValue) :
        rawValue { rawValue }, propertiesMask { 0 }, minRep { 1 }, maxRep { 1 } {}
    GRuleWord(const UnicodeString &rawValue, unsigned mask, int minRep, int maxRep):
        rawValue { rawValue }, propertiesMask { mask }, minRep { minRep }, maxRep { maxRep } {}

    GRuleWord(GRuleWord &&otherWord) = default;
    GRuleWord &operator=(GRuleWord &&otherWord) = default;
    GRuleWord(const GRuleWord&) = default;
    GRuleWord &operator=(const GRuleWord&) = default;

    UnicodeString rawValue;
    unsigned propertiesMask = 0;
    int minRep = 1;
    int maxRep = 1;

    void swap(GRuleWord &other);

    friend std::ostream &operator<<(std::ostream &os, const GRuleWord &ruleWord);
    bool operator==(const GRuleWord &other) const;
    bool operator!=(const GRuleWord &other) const;
    bool operator==(const UnicodeString &other) const;
    bool operator!=(const UnicodeString &other) const;
};

struct SimpleGrammarRule {
    SimpleGrammarRule() {}
    SimpleGrammarRule(const UnicodeString &leftPart,
                      const std::vector<GRuleWord> &rightHandle,
                      bool isValid = true) : leftPart {leftPart}, rightHandle {rightHandle}, isValid {isValid} {}

    UnicodeString leftPart;
    std::vector<GRuleWord> rightHandle;
    bool isValid;

    void swap(SimpleGrammarRule &other);

    bool operator==(const SimpleGrammarRule &other) const;
    bool operator!=(const SimpleGrammarRule &other) const;
    friend std::ostream &operator<<(std::ostream &os, const SimpleGrammarRule &rule);
};

struct ComplexGrammarRule {
    ComplexGrammarRule(const UnicodeString &leftPart, const std::vector<SimpleGrammarRule> rightHandles):
        leftPart { leftPart }, rightHandles { rightHandles } {}
    ComplexGrammarRule() = default;
    ComplexGrammarRule(const UnicodeString &leftPart, const std::vector<GRuleWord> &ruleWords):
        leftPart { leftPart }, rightHandles { std::vector<SimpleGrammarRule> { SimpleGrammarRule { leftPart, ruleWords } }} {}
    ComplexGrammarRule(const SimpleGrammarRule &rule):
        leftPart { rule.leftPart }, rightHandles { std::vector<SimpleGrammarRule> { rule }} {}

    UnicodeString leftPart;
    std::vector<SimpleGrammarRule> rightHandles;
    bool isValid = true;

    void append(const std::vector<GRuleWord> &ruleWords);
    void swap(ComplexGrammarRule &other);

    friend std::ostream &operator<<(std::ostream &os, const ComplexGrammarRule &rule);
};

} /* namespace tproc */

#endif /* GRAMMAR_RULE_H_ */
