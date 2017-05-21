#include "action.hpp"
#include "grammar.h"

namespace tproc {

bool MinMaxAction::equals(const ActionPtr &other) const {
    auto ptr = std::dynamic_pointer_cast<MinMaxAction>(other);
    if (ptr == nullptr) {
        return false;
    }
    return minOccurences == ptr->minOccurences &&
            maxOccurences == ptr->maxOccurences;
}

bool QuoteAction::equals(const ActionPtr &other) const {
    return typeid (this) == typeid (other.get());
}

void QuoteAction::operator()(/*const Grammar &grammar, const ParentInfo &wordInfo*/) {
    auto &ruleWord = getRuleWord();
    if (ruleWord->isNonTerminal()) {
        //TODO: implement
    } else {
//        GRuleWordPtr nterm = wordInfo.nterm;
//        auto &rawValue = nterm->getRawValue();
//        rawValue = "\'" + rawValue + "\'";
        auto &ruleWordValue = ruleWord->getRawValue();
        ruleWordValue = "\"" + ruleWordValue + "\"";
    }
}

}
