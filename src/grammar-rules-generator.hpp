#ifndef GRAMMAR_RULES_GENERATOR_HPP_
#define GRAMMAR_RULES_GENERATOR_HPP_

#include <unordered_map>
#include "grammar-rule.h"

namespace tproc {

enum class ReservedRule {
    WORDS,
    PERSON_FULL_NAME,
    FULL_DATE,
    AGREEMENT_DATE,
    APARTMENT_NUM,
    STREET_RULE,
    TOWN_RULE
};

class GRulesGenerator {
public:
    static GRuleWordPtr generateRule(ReservedRule reservedRule);
};

}

#endif //GRAMMAR_RULES_GENERATOR_HPP_
