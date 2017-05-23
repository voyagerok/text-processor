#ifndef GRAMMAR_RULES_GENERATOR_HPP_
#define GRAMMAR_RULES_GENERATOR_HPP_

#include <unordered_map>
#include "grammar-rule.h"

namespace tproc {

enum class ReservedRule {
    WORDS,
    PERSON_FULL_NAME
};

class GRulesGenerator {
public:
    static GRuleWordPtr generateRule(ReservedRule reservedRule);
private:
//    static std::unordered_map<ReservedRule, generatorFunc> ruleGenerators;
    static GRuleWordPtr generateWordsRule();
    static GRuleWordPtr generatePersonFullName();
};

}

#endif //GRAMMAR_RULES_GENERATOR_HPP_
