#include "grammar-rules-generator.hpp"
//#include "grammar-words-factory.hpp"
#include "grammar-words-storage.hpp"

namespace tproc {


GRuleWordPtr GRulesGenerator::generateRule(ReservedRule rule) {
    switch (rule) {
    case ReservedRule::PERSON_FULL_NAME:
        return generatePersonFullName();
    case ReservedRule::WORDS:
        return generateWordsRule();
    default:
        throw std::runtime_error("generateRule: rule type not implemented");
    }
}

// * PersonFullName = Surname name patr;
// * Surname = surn | word(predicates: upper1);

GRuleWordPtr GRulesGenerator::generatePersonFullName() {
    static GRuleWordPtr finalRule = nullptr;
    if (finalRule != nullptr) {
        return finalRule;
    }

    UnicodeString &nameWordName = GWordStorage::getReservedWord(ReservedWord::NAME);
    UnicodeString &surnWordName = GWordStorage::getReservedWord(ReservedWord::SURNAME);
    UnicodeString &patrWordName = GWordStorage::getReservedWord(ReservedWord::PATR);
    UnicodeString &anywordName = GWordStorage::getReservedWord(ReservedWord::ANYWORD);

    GRuleWordPtr nameTerm = GWordStorage::getTerminal(nameWordName);
    GRuleWordPtr surnTerm = GWordStorage::getTerminal(surnWordName);
    GRuleWordPtr patrTerm = GWordStorage::getTerminal(patrWordName);

    std::vector<PredicatePtr> predicates = { std::make_shared<UpperCaseFirstPredicate>() };
    GRuleWordPtr anyWordTerm = GWordStorage::getTerminal(anywordName, predicates);

    GRuleWordPtr surnNterm = GWordStorage::getNonTerminal("Surname");
    surnNterm->getChildWords().push_back({ surnTerm });
    surnTerm->getParentNterms().push_back({surnNterm, 0, 0});
    surnNterm->getChildWords().push_back({ anyWordTerm });
    anyWordTerm->getParentNterms().push_back({surnNterm, 1, 9});

    finalRule = GWordStorage::getNonTerminal("PersonFullName");
    finalRule->getChildWords().push_back({ surnNterm, nameTerm, patrTerm});
    surnNterm->getParentNterms().push_back({finalRule, 0, 0});
    nameTerm->getParentNterms().push_back({finalRule, 0, 1});
    patrTerm->getParentNterms().push_back({finalRule, 0, 2});

    return finalRule;
}




// * Words = empty;
// * Words = word;
// * Words = Words word

GRuleWordPtr GRulesGenerator::generateWordsRule() {
//    UnicodeString anyWordRawValue { "word" };
//    UnicodeString resultRuleRawValue { "Words" };

    static GRuleWordPtr finalRule = nullptr;
    if (finalRule != nullptr) {
        return finalRule;
    }

    UnicodeString &emptyWordName = GWordStorage::getReservedWord(ReservedWord::EMPTY);
    UnicodeString &anyWordName = GWordStorage::getReservedWord(ReservedWord::ANYWORD);

    GRuleWordPtr emptyTerm = GWordStorage::getTerminal(emptyWordName);
    GRuleWordPtr anyWordTerm = GWordStorage::getTerminal(anyWordName);

    finalRule = GWordStorage::getNonTerminal("Words");
    finalRule->getChildWords().push_back({emptyTerm});
    finalRule->getChildWords().push_back({anyWordTerm});
    finalRule->getChildWords().push_back({finalRule, anyWordTerm});

    finalRule->getParentNterms().push_back({finalRule, 2, 0});
    emptyTerm->getParentNterms().push_back({finalRule, 0, 0});
    anyWordTerm->getParentNterms().push_back({finalRule, 1, 0});

    return finalRule;
}

}

