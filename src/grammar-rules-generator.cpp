#include "grammar-rules-generator.hpp"
//#include "grammar-words-factory.hpp"
#include "grammar-words-storage.hpp"

namespace tproc {

static GRuleWordPtr generatePersonFullNameRule();
static GRuleWordPtr generateAgreementDateRule();
static GRuleWordPtr generateFullDateRule();
static GRuleWordPtr generateApartmentNumberRule();
static GRuleWordPtr generateWordsRule();
static GRuleWordPtr generateTownRule();
static GRuleWordPtr generateStreetRule();

GRuleWordPtr GRulesGenerator::generateRule(ReservedRule rule) {
    switch (rule) {
    case ReservedRule::PERSON_FULL_NAME:
        return generatePersonFullNameRule();
    case ReservedRule::WORDS:
        return generateWordsRule();
    case ReservedRule::FULL_DATE:
        return generateFullDateRule();
    case ReservedRule::AGREEMENT_DATE:
        return generateAgreementDateRule();
    case ReservedRule::APARTMENT_NUM:
        return generateApartmentNumberRule();
    case ReservedRule::TOWN_RULE:
        return  generateTownRule();
    case ReservedRule::STREET_RULE:
        return generateStreetRule();
    default:
        throw std::runtime_error("generateRule: rule type not implemented");
    }
}

// * PersonFullName = Surname name patr;
// * Surname = surn | word(predicates: upper1);

GRuleWordPtr generatePersonFullNameRule() {
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

GRuleWordPtr generateAgreementDateRule() {
    static GRuleWordPtr agreementDateRule = nullptr;
    if (agreementDateRule) {
        return agreementDateRule;
    }

    UnicodeString &geoxTermName = GWordStorage::getReservedWord(ReservedWord::GEOX);

    agreementDateRule = GWordStorage::getNonTerminal("AgreementDate");
    GRuleWordPtr fullDateRule = GRulesGenerator::generateRule(ReservedRule::FULL_DATE);
    GRuleWordPtr anyWordRule = GRulesGenerator::generateRule(ReservedRule::WORDS);
    GRuleWordPtr geoxTerm = GWordStorage::getTerminal(geoxTermName);
    agreementDateRule->getChildWords().push_back({geoxTerm, anyWordRule, fullDateRule});

    geoxTerm->getParentNterms().push_back({agreementDateRule, 0, 0});
    anyWordRule->getParentNterms().push_back({agreementDateRule, 0, 1});
    fullDateRule->getParentNterms().push_back({agreementDateRule, 0, 2});

    return agreementDateRule;
}

GRuleWordPtr generateFullDateRule() {
    static GRuleWordPtr fullDateRule = nullptr;
    if (fullDateRule) {
        return fullDateRule;
    }

    UnicodeString &numWordName = GWordStorage::getReservedWord(ReservedWord::NUM);
    UnicodeString &monthWordName = GWordStorage::getReservedWord(ReservedWord::MONTH);

    GRuleWordPtr monthTerm = GWordStorage::getTerminal(monthWordName);
    PredicatePtr dayMaxLen = std::make_shared<LengthPredicate>(1,2);
    GRuleWordPtr dayTerm = GWordStorage::getTerminal(numWordName, { dayMaxLen });
    PredicatePtr yearLen = std::make_shared<LengthPredicate>(4);
    GRuleWordPtr yearTerm = GWordStorage::getTerminal(numWordName, { yearLen });

    GRuleWordPtr yearSuffixTern = GWordStorage::getTerminal("г");
    GRuleWordPtr yearSuffixFullTerm = GWordStorage::getTerminal("год");
    GRuleWordPtr emptyTerm = GWordStorage::getEmptyTerminal();
    GRuleWordPtr yearSuffixNterm = GWordStorage::getNonTerminal("YearSuffix");
    yearSuffixNterm->getChildWords().push_back({yearSuffixTern});
    yearSuffixNterm->getChildWords().push_back({yearSuffixFullTerm});
    yearSuffixNterm->getChildWords().push_back({emptyTerm});
    yearSuffixTern->getParentNterms().push_back({yearSuffixNterm, 0, 0});
    yearSuffixFullTerm->getParentNterms().push_back({yearSuffixNterm, 1, 0});
    emptyTerm->getParentNterms().push_back({yearSuffixNterm, 2, 0});

    fullDateRule = GWordStorage::getNonTerminal("AgreementDate");
    fullDateRule->getChildWords().push_back({dayTerm, monthTerm, yearTerm, yearSuffixNterm});
    dayTerm->getParentNterms().push_back({fullDateRule, 0, 0});
    monthTerm->getParentNterms().push_back({fullDateRule, 0, 1});
    yearTerm->getParentNterms().push_back({fullDateRule, 0, 2});
    yearSuffixNterm->getParentNterms().push_back({fullDateRule, 0, 3});

    return fullDateRule;
}

GRuleWordPtr generateApartmentNumberRule() {
    GRuleWordPtr apartmentNumberRule = nullptr;
    if (apartmentNumberRule) {
        return apartmentNumberRule;
    }

    UnicodeString &numberTermName = GWordStorage::getReservedWord(ReservedWord::NUM);

    GRuleWordPtr apartmentPrefixNterm = GWordStorage::getNonTerminal("ApartmentPrefix");
    GRuleWordPtr apartmentPrefixTerm = GWordStorage::getTerminal("квартира");
    GRuleWordPtr apartmentPrefixFullTerm = GWordStorage::getTerminal("кв");
    apartmentPrefixNterm->getChildWords().push_back({apartmentPrefixTerm});
    apartmentPrefixNterm->getChildWords().push_back({apartmentPrefixFullTerm});
    apartmentPrefixTerm->getParentNterms().push_back({apartmentPrefixNterm, 0, 0});
    apartmentPrefixFullTerm->getParentNterms().push_back({apartmentPrefixNterm, 1, 0});

    PredicatePtr lenPredicate = std::make_shared<LengthPredicate>(1,3);
    GRuleWordPtr numberTerm = GWordStorage::getTerminal(numberTermName, {lenPredicate});

    apartmentNumberRule = GWordStorage::getNonTerminal("ApartmentNumber");
    apartmentNumberRule->getChildWords().push_back({apartmentPrefixNterm, numberTerm});
    apartmentPrefixNterm->getParentNterms().push_back({apartmentNumberRule, 0, 0});
    numberTerm->getParentNterms().push_back({apartmentNumberRule, 0, 1});

    return apartmentNumberRule;
}

GRuleWordPtr generateTownRule() {
    GRuleWordPtr townRule = nullptr;
    if (townRule) {
        return townRule;
    }

    UnicodeString &geoxTermName = GWordStorage::getReservedWord(ReservedWord::GEOX);

    GRuleWordPtr townPrefixNterm = GWordStorage::getNonTerminal("TownPrefix");
    GRuleWordPtr townPrefixTerm = GWordStorage::getTerminal("город");
    GRuleWordPtr townPrefixFullTerm = GWordStorage::getTerminal("г");
    townPrefixNterm->getChildWords().push_back({townPrefixTerm});
    townPrefixNterm->getChildWords().push_back({townPrefixFullTerm});
    townPrefixTerm->getParentNterms().push_back({townPrefixNterm, 0, 0});
    townPrefixFullTerm->getParentNterms().push_back({townPrefixNterm, 1, 0});

    GRuleWordPtr geoxTerm = GWordStorage::getTerminal(geoxTermName);

    townRule = GWordStorage::getNonTerminal("Town");
    townRule->getChildWords().push_back({townPrefixNterm, geoxTerm});
    townPrefixNterm->getParentNterms().push_back({townRule, 0, 0});
    geoxTerm->getParentNterms().push_back({townRule, 0, 1});

    return townRule;
}

GRuleWordPtr generateStreetRule() {
    GRuleWordPtr streetRule = nullptr;
    if (streetRule) {
        return streetRule;
    }

    UnicodeString &anyWordTermName = GWordStorage::getReservedWord(ReservedWord::ANYWORD);
    UnicodeString &numTermName = GWordStorage::getReservedWord(ReservedWord::NUM);

    GRuleWordPtr streetPrefix1 = GWordStorage::getTerminal("ул");
    GRuleWordPtr streetPrefix1Full = GWordStorage::getTerminal("улица");
    GRuleWordPtr streetPrefix2 = GWordStorage::getTerminal("бул");
    GRuleWordPtr streetPrefix2Full = GWordStorage::getTerminal("бульвар");
    GRuleWordPtr streetPrefix3 = GWordStorage::getTerminal("просп");
    GRuleWordPtr streetPrefix3Full = GWordStorage::getTerminal("проспект");
    GRuleWordPtr streetPrefix4 = GWordStorage::getTerminal("пер");
    GRuleWordPtr streetPrefix4Full = GWordStorage::getTerminal("переулок");

    GRuleWordPtr streetPrefixNterm = GWordStorage::getNonTerminal("StreetPrefix");
    streetPrefixNterm->getChildWords().push_back({streetPrefix1});
    streetPrefixNterm->getChildWords().push_back({streetPrefix1Full});
    streetPrefixNterm->getChildWords().push_back({streetPrefix2});
    streetPrefixNterm->getChildWords().push_back({streetPrefix2Full});
    streetPrefixNterm->getChildWords().push_back({streetPrefix3});
    streetPrefixNterm->getChildWords().push_back({streetPrefix3Full});
    streetPrefixNterm->getChildWords().push_back({streetPrefix4});
    streetPrefixNterm->getChildWords().push_back({streetPrefix4Full});

    streetPrefix1->getParentNterms().push_back({streetPrefixNterm, 0, 0});
    streetPrefix1Full->getParentNterms().push_back({streetPrefixNterm, 1, 0});
    streetPrefix2->getParentNterms().push_back({streetPrefixNterm, 2, 0});
    streetPrefix2Full->getParentNterms().push_back({streetPrefixNterm, 3, 0});
    streetPrefix3->getParentNterms().push_back({streetPrefixNterm, 4, 0});
    streetPrefix3Full->getParentNterms().push_back({streetPrefixNterm, 5, 0});
    streetPrefix4->getParentNterms().push_back({streetPrefixNterm, 6, 0});
    streetPrefix4Full->getParentNterms().push_back({streetPrefixNterm, 7, 0});

    GRuleWordPtr housePrefNterm = GWordStorage::getTerminal("HouseNumPrefix");
    GRuleWordPtr houseTerm = GWordStorage::getTerminal("дом");
    GRuleWordPtr houseShortTerm = GWordStorage::getTerminal("д");
    GRuleWordPtr emptyWord = GWordStorage::getEmptyTerminal();
    housePrefNterm->getChildWords().push_back({houseTerm});
    housePrefNterm->getChildWords().push_back({houseShortTerm});
    housePrefNterm->getChildWords().push_back({emptyWord});
    houseTerm->getParentNterms().push_back({housePrefNterm, 0, 0});
    houseShortTerm->getParentNterms().push_back({housePrefNterm, 1, 0});
    emptyWord->getParentNterms().push_back({housePrefNterm, 2, 0});

    PredicatePtr anyWordPred = std::make_shared<UpperCaseFirstPredicate>();
    GRuleWordPtr anyWordTerm = GWordStorage::getTerminal(anyWordTermName, {anyWordPred});
    PredicatePtr lenPredicate = std::make_shared<LengthPredicate>(1, 3);
    GRuleWordPtr numTerm = GWordStorage::getTerminal(numTermName, {lenPredicate});

    streetRule = GWordStorage::getNonTerminal("Street");
    streetRule->getChildWords().push_back({streetPrefixNterm, anyWordTerm, housePrefNterm, numTerm});
    streetPrefixNterm->getParentNterms().push_back({streetRule, 0, 0});
    anyWordTerm->getParentNterms().push_back({streetRule, 0, 1});
    housePrefNterm->getParentNterms().push_back({streetRule, 0, 2});
    numTerm->getParentNterms().push_back({streetRule, 0, 3});

    return streetRule;
}

// * Words = empty;
// * Words = word;
// * Words = Words word

GRuleWordPtr generateWordsRule() {
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

