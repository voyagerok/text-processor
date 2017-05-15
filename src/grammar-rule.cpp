/*
 * GrammarRule.cpp
 *
 *  Created on: Apr 8, 2017
 *      Author: nikolay
 */
#include <unicode/ustream.h>

#include "grammar-rule.h"

namespace tproc {

//std::ostream &printRule(const GRuleWordPtr &rule, ) {

//}

std::ostream &operator<<(std::ostream &os, RuleIndex ruleIndex) {
    os << "word: " << ruleIndex.nterm << ", ruleNumber" << ruleIndex.index << std::endl;
    return os;
}

bool RuleIndex::operator==(const RuleIndex &other) const {
    return this->nterm == other.nterm && this->index == other.index;
}

bool RuleIndex::operator!=(const RuleIndex &other) const {
    return !(*this == other);
}

//std::ostream &operator<<(std::ostream &os, const GRuleWord &ruleWord) {
//    os << ruleWord.rawValue;
//    return os;
//}

//bool GRuleWord::operator==(const GRuleWord &other) const {
//    return this->rawValue == other.rawValue;
//}

//bool GRuleWord::operator!=(const GRuleWord &other) const {
//    return this->rawValue != other.rawValue;
//}

//bool GRuleWord::operator==(const UnicodeString &other) const {
//    return this->rawValue == other;
//}

//bool GRuleWord::operator!=(const UnicodeString &other) const {
//    return  this->rawValue != other;
//}

//void GRuleWord::swap(GRuleWord &other) {
//    this->rawValue.swap(other.rawValue);
//    std::swap(this->maxRep, other.maxRep);
//    std::swap(this->minRep, other.minRep);
//    std::swap(this->propertiesMask, other.propertiesMask);
//}

//std::ostream &operator<<(std::ostream &os, const SimpleGrammarRule &rule) {
//    os << rule.lhs << " = ";
//    for (auto &word : rule.rhs) {
//        os << word << " ";
//    }
//    return os;
//}

//std::ostream &operator<<(std::ostream &os, const ComplexGrammarRule &rule) {
//    os << rule.lhs << " = ";
//    for (auto &simpleRule : rule.rhs) {
//        os << simpleRule << " | ";
//    }
//    return os;
//}

std::ostream &NonTerminal::print(std::ostream &os) const {
//    os << this->rawValue;
    for (int i = 0; i < childWords.size(); ++i) {
        auto &ruleBody = childWords[i];
        os << this->rawValue << " =";
        for (auto &ruleWord : ruleBody) {
            os << " " << ruleWord->getRawValue();
        }
        if (i < childWords.size() - 1) {
            os << std::endl;
        }
    }

    return os;
}

bool NonTerminal::equals(const GRuleWordPtr &wordPtr) const {
    auto ntermPtr = std::dynamic_pointer_cast<NonTerminal>(wordPtr);
    if (ntermPtr == nullptr) {
        return false;
    }
    return this->rawValue == ntermPtr->rawValue;
}

void NonTerminal::swap(NonTerminal &other) {
    this->rawValue.swap(other.rawValue);
}

std::ostream& Terminal::print(std::ostream &os) const {
    os << this->rawValue;
    return os;
}

bool Terminal::equals(const GRuleWordPtr &wordPtr) const {
    auto termPtr = std::dynamic_pointer_cast<Terminal>(wordPtr);
    if (termPtr == nullptr) {
        return false;
    }
//    return this->rawValue == termPtr->rawValue &&
//            this->propsMask == termPtr->propsMask;
    if (this->rawValue != termPtr->rawValue) {
        return false;
    }
    if (this->predicates.size() != termPtr->predicates.size()) {
        return false;
    }

    for (int i = 0; i < predicates.size(); ++i) {
        if (predicates[i] != termPtr->predicates[i]) {
            return false;
        }
    }

    return true;
}

bool Terminal::checkToken(const UnicodeString &token) const {
    if (this->rawValue != token) {
        return false;
    }

    for(const auto &pred : predicates) {
        if (!pred->operator()(token)) {
            return false;
        }
    }

    return true;
}

//bool Terminal::operator==(const Terminal &other) const {
//    if (rawValue != other.rawValue) {
//        return false;
//    }
//    if (predicates.size() != other.predicates.size()) {
//        return false;
//    }
//    for (int i = 0; i < predicates.size(); ++i) {
//        if (predicates[i] != other.predicates[i]) {
//            return false;
//        }
//    }

//    return true;
//}

//bool Terminal::operator!=(const Terminal &other) const {
//    return !(*this == other);
//}

bool SimpleGrammarRule::operator==(const SimpleGrammarRule &other) const {
    if (isValid != other.isValid) {
        return false;
    }
    if (rhs.size() != other.rhs.size()) {
        return false;
    }
    if (lhs != other.lhs) {
        return false;
    }
    for (int i = 0; i < rhs.size(); ++i) {
        if (rhs[i] != other.rhs[i]) {
            return false;
        }
    }
    return true;
}

bool SimpleGrammarRule::operator!=(const SimpleGrammarRule &other) const {
    return !(*this == other);
}

void SimpleGrammarRule::swap(SimpleGrammarRule &other) {
    this->lhs.swap(other.lhs);
    this->rhs.swap(other.rhs);
    std::swap(isValid, other.isValid);
}

void ComplexGrammarRule::append(const std::vector<GRuleWordPtr> &ruleWords) {
    this->rhs.emplace_back(this->lhs, ruleWords);
}

void ComplexGrammarRule::swap(ComplexGrammarRule &other) {
    this->lhs.swap(other.lhs);
    this->rhs.swap(other.rhs);
    std::swap(this->isValid, other.isValid);
}

} /* namespace tproc */
