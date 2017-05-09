/*
 * GrammarRule.cpp
 *
 *  Created on: Apr 8, 2017
 *      Author: nikolay
 */
#include <unicode/ustream.h>

#include "grammar-rule.h"

namespace tproc {

std::ostream &operator<<(std::ostream &os, const GRuleWord &ruleWord) {
    os << ruleWord.rawValue;
    return os;
}

bool GRuleWord::operator==(const GRuleWord &other) const {
    return this->rawValue == other.rawValue;
}

bool GRuleWord::operator!=(const GRuleWord &other) const {
    return this->rawValue != other.rawValue;
}

bool GRuleWord::operator==(const UnicodeString &other) const {
    return this->rawValue == other;
}

bool GRuleWord::operator!=(const UnicodeString &other) const {
    return  this->rawValue != other;
}

void GRuleWord::swap(GRuleWord &other) {
    this->rawValue.swap(other.rawValue);
    std::swap(this->maxRep, other.maxRep);
    std::swap(this->minRep, other.minRep);
    std::swap(this->propertiesMask, other.propertiesMask);
}

std::ostream &operator<<(std::ostream &os, const SimpleGrammarRule &rule) {
    os << rule.leftPart << " = ";
    for (auto &word : rule.rightHandle) {
        os << word << " ";
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const ComplexGrammarRule &rule) {
    os << rule.leftPart << " = ";
    for (auto &simpleRule : rule.rightHandles) {
        os << simpleRule << " | ";
    }
    return os;
}

bool SimpleGrammarRule::operator==(const SimpleGrammarRule &other) const {
    if (isValid != other.isValid) {
        return false;
    }
    if (rightHandle.size() != other.rightHandle.size()) {
        return false;
    }
    if (leftPart != other.leftPart) {
        return false;
    }
    for (int i = 0; i < rightHandle.size(); ++i) {
        if (rightHandle[i] != other.rightHandle[i]) {
            return false;
        }
    }
    return true;
}

bool SimpleGrammarRule::operator!=(const SimpleGrammarRule &other) const {
    return !(*this == other);
}

void SimpleGrammarRule::swap(SimpleGrammarRule &other) {
    this->leftPart.swap(other.leftPart);
    this->rightHandle.swap(other.rightHandle);
    std::swap(isValid, other.isValid);
}

void ComplexGrammarRule::append(const std::vector<GRuleWord> &ruleWords) {
    this->rightHandles.emplace_back(this->leftPart, ruleWords);
}

void ComplexGrammarRule::swap(ComplexGrammarRule &other) {
    this->leftPart.swap(other.leftPart);
    this->rightHandles.swap(other.rightHandles);
    std::swap(this->isValid, other.isValid);
}

} /* namespace tproc */
