/*
 * GrammarRule.cpp
 *
 *  Created on: Apr 8, 2017
 *      Author: nikolay
 */
#include <unicode/ustream.h>

#include "grammar-rule.h"

namespace tproc {

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

} /* namespace tproc */
