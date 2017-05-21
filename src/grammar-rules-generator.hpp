#ifndef GRAMMAR_RULES_GENERATOR_HPP_
#define GRAMMAR_RULES_GENERATOR_HPP_

#include "grammar-rule.h"

namespace tproc {

class FullPersonNameRuleGen {
public:
    static GRuleWordPtr generate();
};

class WordsRuleGen {
public:
    static GRuleWordPtr generate();
};

template<class RuleGenType>
GRuleWordPtr generateRule(const RuleGenType &ruleGen) { return ruleGen->generate(); }

}

#endif //GRAMMAR_RULES_GENERATOR_HPP_
