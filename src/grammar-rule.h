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

struct SimpleGrammarRule {
    SimpleGrammarRule() {}
    SimpleGrammarRule(const UnicodeString &leftPart,
                      const std::vector<UnicodeString> &rightHandle,
                      bool isValid = true) : leftPart {leftPart}, rightHandle {rightHandle}, isValid {isValid} {}

    UnicodeString leftPart;
    std::vector<UnicodeString> rightHandle;
    bool isValid;

    bool operator==(const SimpleGrammarRule &other) const;
    bool operator!=(const SimpleGrammarRule &other) const;
    friend std::ostream &operator<<(std::ostream &os, const SimpleGrammarRule &rule);
};

struct ComplexGrammarRule {
    UnicodeString leftPart;
    std::vector<SimpleGrammarRule> rightHandles;
    bool isValid;

    friend std::ostream &operator<<(std::ostream &os, const ComplexGrammarRule &rule);
};

} /* namespace tproc */

#endif /* GRAMMAR_RULE_H_ */
