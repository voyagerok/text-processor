#include "grammar-rules-generator.hpp"
//#include "grammar-words-factory.hpp"
#include "grammar-words-storage.hpp"

namespace tproc {

GRuleWordPtr FullPersonNameRuleGen::generate() {

}

GRuleWordPtr WordsRuleGen::generate() {
//    UnicodeString anyWordRawValue { "word" };
//    UnicodeString resultRuleRawValue { "Words" };

    GRuleWordPtr emptyTerm = GWordStorage::getTerminal(EMPTY);
    GRuleWordPtr anyWordTerm = GWordStorage::getTerminal("word");

    GRuleWordPtr finalRule = GWordStorage::getNonTerminal("Words");
    finalRule->getChildWords().push_back({emptyTerm});
    finalRule->getChildWords().push_back({anyWordTerm});
    finalRule->getChildWords().push_back({finalRule, anyWordTerm});

    finalRule->getParentNterms().push_back({finalRule, 2, 0});
    emptyTerm->getParentNterms().push_back({finalRule, 0, 0});
    anyWordTerm->getParentNterms().push_back({finalRule, 1, 0});

    return finalRule;
}

}

