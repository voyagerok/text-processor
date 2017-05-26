#include "g-parser-driver.hpp"

#include <fstream>
#include <sstream>
#include <unicode/ustream.h>
#include <sstream>

#include "gparser.tab.hh"
#include "g-scanner.hpp"
#include "utils/logger.h"
#include "grammar-words-storage.hpp"

namespace tproc {

//void GParserDriver::appendRule(ComplexGrammarRule &rule) {
//    this->rules.push_back(std::move(rule));
//}

//GRuleWordPtr GParserDriver::makeRuleWord(UnicodeString &rawValue, std::vector<std::shared_ptr<Property>> &props) {
//    unsigned mask = 0;
//    int minRep = 1;
//    int maxRep = 1;
//    for (auto &prop : props) {
//        std::shared_ptr<SimpleProperty> simpleProp { nullptr };
//        std::shared_ptr<ComplexNumProperty> complexNumProp { nullptr };
//        if ((simpleProp  = std::dynamic_pointer_cast<SimpleProperty>(prop)) != nullptr) {
//            mask |= static_cast<unsigned>(simpleProp->propType);
//        } else if ((complexNumProp = std::dynamic_pointer_cast<ComplexNumProperty>(prop)) != nullptr) {
//            mask |= static_cast<unsigned>(complexNumProp->propType);
//            switch (complexNumProp->propType) {
//            case GRuleWordPropType::MAX_REP:
//                maxRep = complexNumProp->propValue;
//                break;
//            case GRuleWordPropType::MIN_REP:
//                minRep = complexNumProp->propValue;
//                break;
//            default:
//                break;
//            }
//        }
//    }

//    return GRuleWord { rawValue, mask, minRep, maxRep };
//}

//void GParserDriver::onComplexRuleReduce(const GRuleWordPtr &nterm) {
//    currentSimpleRuleNum = 0;
//    for (auto &action : actionsForRule) {
//        action->getWordInfo().ruleIndex.nterm = nterm;
//    }
//}

static void printParents(const GRuleWordPtr &ruleWord) {
    Logger::getLogger() << "Parent:" << std::endl;
    for (auto &parent : ruleWord->getParentNterms()) {
        Logger::getLogger() << parent.nterm << std::endl;
    }
}

void GParserDriver::appendRule(const GRuleWordPtr &ntermPtr) {
//    currentRuleWordNum =0;
//    incSimpleRuleIndex();

//    Logger::getLogger() << "appendRule: " << ntermPtr << std::endl;
//    Logger::getLogger() << "Nterm parents:" << std::endl;

//    auto defFound = std::find_if(definedNterms.begin(), definedNterms.end(), [&ntermPtr](auto &wordPtr){
//        return wordPtr->getRawValue() == ntermPtr->getRawValue();
//    });
//    if (defFound != definedNterms.end()) {
//        auto &childWords = (*defFound)->getChildWords();
//        childWords.insert(childWords.end(), ntermPtr->getChildWords().begin(), ntermPtr->getChildWords().end());
//        return;
//    }

//    auto pendFound = std::find_if(pendingNterms.begin(), pendingNterms.end(), [&ntermPtr](auto &wordPtr){ return wordPtr->getRawValue() == ntermPtr->getRawValue(); });
//    if (pendFound != pendingNterms.end()) {
//        GRuleWordPtr pendPtr = *pendFound;
//        pendingNterms.erase(pendFound);
//        std::move(ntermPtr->getChildWords().begin(), ntermPtr->getChildWords().end(), std::back_inserter(pendPtr->getChildWords()));
//        definedNterms.insert(pendPtr);
//        return;
//    }

////    auto newNterm = std::make_shared<NonTerminal>(lhs, rhs);
//    definedNterms.insert(ntermPtr);
//    rootCandidates.insert(ntermPtr);

//    auto pendFound = pendingNterms.find(ntermPtr);
//    if (pendFound != pendingNterms.end()) {
//        pendingNterms.erase(pendFound);
//    } else {
//        rootCandidates.insert(ntermPtr);
//    }
    definedNterms.insert(ntermPtr);
}

void GParserDriver::fixAndSaveActionList(const GRuleWordPtr &actionWord, std::vector<ActionPtr> &&actions) {
    for (auto &action : actions) {
        action->getRuleWord() = actionWord;
    }
    std::move(actions.begin(), actions.end(), std::back_inserter(pendingActions));
}

void GParserDriver::fixParentInfo(/*const std::vector<GRuleWordPtr> &words,*/ const GRuleWordPtr &parent/*, int baseRuleIndex*/) {
//    Logger::getLogger() << "Fix parent info: " << parent->getRawValue() << std::endl;

    std::vector<GRuleWordPtr> &words = parent->getChildWords().back();
    int baseRuleIndex = parent->getChildWords().size() - 1;

//    auto defFound = definedNterms.find(parent);
//    if (defFound != definedNterms.end()) {
//        baseRuleIndex += (*defFound)->getChildWords().size();
//    }

    for (int i = 0; i < words.size(); ++i) {
//        word->getParentNterms().push_back(parent);
//        rootCandidates.erase(word);
//        Logger::getLogger() << "Fixing for " << words[i]->getRawValue() << std::endl;
//        Logger::getLogger() << "Before fixing" << std::endl;
        printParents(words[i]);
        ParentInfo wordIndex { parent, baseRuleIndex, i };
        words[i]->getParentNterms().push_back(wordIndex);
        rootCandidates.erase(words[i]);
//        Logger::getLogger() << "After fixing" << std::endl;
//        printParents(words[i]);
//        Logger::getLogger() << "Done" << std::endl;
    }
}

GRuleWordPtr GParserDriver::handleNtermReduction(UnicodeString &&rawValue) {
    Logger::getLogger() << "handleNtermReduction: " << rawValue << std::endl;
    auto nterm = GWordStorage::getNonTerminal(rawValue);

//    auto defFound = std::find_if(definedNterms.begin(), definedNterms.end(), [&rawValue](const GRuleWordPtr &wordPtr){ return wordPtr->getRawValue() == rawValue; });
    auto defFound = definedNterms.find(nterm);
    if (defFound != definedNterms.end()) {
        rootCandidates.erase(*defFound);
        return (*defFound);
    }

//    auto pendFound = std::find_if(pendingNterms.begin(), pendingNterms.end(),[&rawValue](const GRuleWordPtr &wordPtr){ return wordPtr->getRawValue() == rawValue; });
    auto pendFound = pendingNterms.find(nterm);
    if (pendFound != pendingNterms.end()) {
        return *pendFound;
    }

//    auto newPendingNterm = std::make_shared<NonTerminal>(std::move(rawValue));
//    auto newPendingNterm = GWordStorage::getNonTerminal
    pendingNterms.insert(nterm);
    return nterm;
}

GRuleWordPtr GParserDriver::handleTermReduction(UnicodeString &&rawValue) {
    Logger::getLogger() << "handleTermReduction: " << rawValue << std::endl;
    auto term = GWordStorage::getTerminal(rawValue);

    auto termFound = terminals.find(term);
    if (termFound != terminals.end()) {
        return (*termFound);
    } else {
        terminals.insert(term);
        return term;
    }
//    auto termFoud = std::find_if(terminals.begin(), terminals.end(), [&rawValue](const GRuleWordPtr &wordPtr){
//        if (wordPtr->getRawValue() != rawValue) return false;
//        auto termPtr = std::dynamic_pointer_cast<Terminal>(wordPtr);
//        return (termPtr != nullptr && termPtr->getPredicates().size() == 0);
//    });
//    if (termFoud != terminals.end()) {
//        return (*termFoud);
//    } else {
//        auto newTerm = std::make_shared<Terminal>(std::move(rawValue));
//        terminals.insert(newTerm);
//        return newTerm;
//    }
}

GRuleWordPtr GParserDriver::handleTermReduction(UnicodeString &&rawValue, std::vector<PredicatePtr> &&predicates) {
    Logger::getLogger() << "handleTermReduction: " << rawValue << ", " << predicates.size() << std::endl;
    auto term = GWordStorage::getTerminal(rawValue, predicates);
    auto termFound = terminals.find(term);

    if (termFound != terminals.end()) {
        return (*termFound);
    } else {
        terminals.insert(term);
        return term;
    }
}

GRuleWordPtr GParserDriver::handleNumTermReduction() {
    std::vector<PredicatePtr> predicates = { std::make_shared<RegexPredicate>("[0-9]+") };
    GRuleWordPtr term = GWordStorage::getTerminal(GWordStorage::getReservedWord(ReservedWord::NUM), predicates);

    auto termFound = terminals.find(term);
    if (termFound != terminals.end()) {
        return *termFound;
    } else {
        terminals.insert(term);
        return term;
    }
}

GRuleWordPtr GParserDriver::handleNumTermReduction(std::vector<PredicatePtr> &&predicates) {
    predicates.push_back(std::make_shared<RegexPredicate>("[0-9]+"));
    GRuleWordPtr term = GWordStorage::getTerminal(GWordStorage::getReservedWord(ReservedWord::NUM), predicates);
    auto termFound = terminals.find(term);
    if (termFound != terminals.end()) {
        return *termFound;
    } else {
        terminals.insert(term);
        return term;
    }
}

GRuleWordPtr GParserDriver::createRule(UnicodeString &&word, std::vector<GRuleWordPtr> &&wordChain) {
    auto rule = GWordStorage::getNonTerminal(word);


//    auto defFound = std::find_if(definedNterms.begin(), definedNterms.end(), [&word](auto &wordPtr){
//        return  wordPtr->getRawValue() == word;
//    });
    auto defFound = definedNterms.find(rule);
    if (defFound != definedNterms.end()) {
        (*defFound)->getChildWords().push_back(std::move(wordChain));
        return *defFound;
    }

//    auto pendFound = std::find_if(pendingNterms.begin(), pendingNterms.end(), [&word](auto &wordPtr){
//        return  wordPtr->getRawValue() == word;
//    });
    auto pendFound = pendingNterms.find(rule);
    if (pendFound != pendingNterms.end()) {
        GRuleWordPtr pendPtr = *pendFound;
        pendingNterms.erase(pendFound);
        pendPtr->getChildWords().push_back(std::move(wordChain));
//        definedNterms.insert(pendPtr);
        return pendPtr;
    }

//    GRuleWordPtr newRule = std::make_shared<NonTerminal>(std::move(word), std::move(wordChain));
//    definedNterms.insert(newRule);
    rule->getChildWords().push_back(std::move(wordChain));
    rootCandidates.insert(rule);
    return rule;
}

static void extractTermsAndNterms(const GRuleWordPtr &word,
                                  std::set<GRuleWordPtr> &outTerms,
                                  std::set<GRuleWordPtr> &outNterms)
{
    if (word->isNonTerminal()) {
        outNterms.insert(word);
        for (auto &ruleBody : word->getChildWords()) {
            for (auto &childWord : ruleBody) {
                if (word == childWord) {
                    continue;
                }
                extractTermsAndNterms(childWord, outTerms, outNterms);
            }
        }
    } else {
        outTerms.insert(word);
    }
}

void GParserDriver::handleDependencies(std::vector<DependencyStruct> &deps) {
    for (auto &dep : deps) {
        auto depRootFound = std::find_if(definedDepRules.begin(), definedDepRules.end(), [&dep](auto &definedDep){
            return definedDep->root = dep.target;
        });
        if (depRootFound != definedDepRules.end()) {
            Logger::getErrLogger() << "Error: redefenition of dependency rule with target " << dep.target->getRawValue() << std::endl;
            return;
        }

        std::set<DependencyRulePtr> forwardDeps;
        std::set<DependencyRulePtr> backwardDeps;
        bool targetFound = false;
        for (auto &depSymbol : dep.depSymbols) {
            if (depSymbol == dep.target) {
                targetFound = true;
                continue;
            }
            DependencyRulePtr symbolDepRule = nullptr;

            auto defSymbolFound = std::find_if(definedDepRules.begin(), definedDepRules.end(), [&depSymbol](auto &defDep){
                return depSymbol == defDep->root;
            });
            if (defSymbolFound != definedDepRules.end()) {
                symbolDepRule = *defSymbolFound;
            }

            if (!symbolDepRule) {
                auto pendSymbolFound = std::find_if(pendingDepRules.begin(), pendingDepRules.end(), [&depSymbol](auto &pendDep){
                    return depSymbol == pendDep->root;
                });
                if (pendSymbolFound != pendingDepRules.end()) {
                    symbolDepRule = *pendSymbolFound;
                }
            }

            if (!symbolDepRule) {
                symbolDepRule = std::make_shared<DependencyRule>(depSymbol);

                std::set<GRuleWordPtr> terms, nterms;
                extractTermsAndNterms(depSymbol, terms, nterms);
                symbolDepRule->nterms = std::move(nterms);
                symbolDepRule->terms = std::move(terms);

                pendingDepRules.insert(symbolDepRule);
            }

            targetFound ? backwardDeps.insert(symbolDepRule) : forwardDeps.insert(symbolDepRule);
        }

        if (!targetFound) {
            Logger::getErrLogger() << "Error: no " << dep.target->getRawValue() << " in right part of rule" << std::endl;
            return;
        }

        auto pendingFound = std::find_if(pendingDepRules.begin(), pendingDepRules.end(), [&dep](auto &pendDep){
            return dep.target == pendDep->root;
        });

        DependencyRulePtr newDepRule = nullptr;
        if (pendingFound != pendingDepRules.end()) {
            (*pendingFound)->backwardDeps.insert(backwardDeps.begin(), backwardDeps.end());
            (*pendingFound)->forwardDeps.insert(forwardDeps.begin(), forwardDeps.end());
            newDepRule = *pendingFound;
            pendingDepRules.erase(pendingFound);
        } else {
            newDepRule = std::make_shared<DependencyRule>(dep.target);
            newDepRule->backwardDeps = std::move(backwardDeps);
            newDepRule->forwardDeps = std::move(forwardDeps);

            std::set<GRuleWordPtr> terms, nterms;
            extractTermsAndNterms(dep.target, terms, nterms);
            newDepRule->nterms = std::move(nterms);
            newDepRule->terms = std::move(terms);
        }

        definedDepRules.insert(newDepRule);
    }

    std::move(pendingDepRules.begin(), pendingDepRules.end(), std::inserter(definedDepRules, definedDepRules.begin()));
    pendingDepRules.clear();
}

bool GParserDriver::createDependencyStruct(const UnicodeString &target,
                                          const UnicodeString &firstSym,
                                          const UnicodeString &secondSym,
                                          DependencyType depType,
                                          DependencyStruct &outStruct,
                                          std::string &errMessage)
{
    std::ostringstream err_os;
    auto targetFound = std::find_if(definedNterms.begin(), definedNterms.end(), [&target](auto &nterm){
        return nterm->getRawValue() == target;
    });
    if (targetFound == definedNterms.end()) {
        err_os << "Error:" << target << " not defined";
        errMessage = err_os.str();
        return false;
    }

    auto firstSymFound = std::find_if(definedNterms.begin(), definedNterms.end(), [&firstSym](auto &nterm){
        return nterm->getRawValue() == firstSym;
    });
    if (firstSymFound == definedNterms.end()) {
        err_os << "Error:" << target << " not defined";
        errMessage = err_os.str();
        return false;
    }

    auto secondSymFound = std::find_if(definedNterms.begin(), definedNterms.end(), [&secondSym](auto &nterm){
        return nterm->getRawValue() == secondSym;
    });
    if (secondSymFound == definedNterms.end()) {
        err_os << "Error:" << target << " not defined";
        errMessage = err_os.str();
        return false;
    }

    outStruct = DependencyStruct(*targetFound, {*firstSymFound, *secondSymFound}, depType);
    return true;
}

bool GParserDriver::appendDependencyStruct(const UnicodeString &symbol,
                                           DependencyStruct &outStruct,
                                           std::string &errMessage)
{
    std::ostringstream err_os;
    auto symbolFound = std::find_if(definedNterms.begin(), definedNterms.end(), [&symbol](auto &nterm){
        return nterm->getRawValue() == symbol;
    });
    if (symbolFound == definedNterms.end()) {
        err_os << "Error:" << symbol << " not defined";
        errMessage = err_os.str();
        return false;
    }

    outStruct.appendSymbol(*symbolFound);

    return true;
}

bool GParserDriver::handleCommandFindReduction(GRuleWordPtr &rawWord, DependencyRulePtr &result, std::string &errMsg) {
    std::stringstream os;

//    auto rule = GWordStorage::getNonTerminal(rawWord);
    auto rule = rawWord;
    auto ruleFound = definedNterms.find(rule);
    if (ruleFound == definedNterms.end()) {
        os << "Failed to find rule for " << rawWord;
        errMsg = os.str();
        return  false;
    }

    auto depRuleFound = std::find_if(definedDepRules.begin(), definedDepRules.end(), [&rule](const DependencyRulePtr &rulePtr) {
        return rulePtr->root == rule;
    });
    if (depRuleFound != definedDepRules.end()) {
        result = *depRuleFound;
    } else {
        result = std::make_shared<DependencyRule>(rule);
        std::set<GRuleWordPtr> terms, nterms;
        extractTermsAndNterms(rule, terms, nterms);
        result->terms = std::move(terms);
        result->nterms = std::move(nterms);
    }

    return true;
}

void GParserDriver::processCommandList(std::vector<DependencyRulePtr> &&commands) {
    std::move(commands.begin(), commands.end(), std::inserter(targetRules, targetRules.begin()));
}

void GParserDriver::applyPendingActions() {
    for (auto &action : pendingActions) {
        action->operator()();
    }
}

static void loadBuiltinNterms(std::set<GRuleWordPtr> &nterms) {
    nterms.insert(GRulesGenerator::generateRule(ReservedRule::PERSON_FULL_NAME));
    nterms.insert(GRulesGenerator::generateRule(ReservedRule::WORDS));
    nterms.insert(GRulesGenerator::generateRule(ReservedRule::FULL_DATE));
    nterms.insert(GRulesGenerator::generateRule(ReservedRule::AGREEMENT_DATE));
    nterms.insert(GRulesGenerator::generateRule(ReservedRule::APARTMENT_NUM));
    nterms.insert(GRulesGenerator::generateRule(ReservedRule::STREET_RULE));
    nterms.insert(GRulesGenerator::generateRule(ReservedRule::TOWN_RULE));
}

bool GParserDriver::parse(const std::string &fname) {
    std::ifstream ifs { fname };
    return parseHelper(ifs);
}

bool GParserDriver::parse(const char *const filename) {
    std::ifstream ifs { filename };
    return parseHelper(ifs);
}

bool GParserDriver::parse(std::istream &iss) {
    return parseHelper(iss);
}

bool GParserDriver::parseHelper(std::istream &iss) {

    this->scanner = std::make_shared<GScanner>( &iss );

    this->parser = std::make_shared<GParser>(*scanner, *this);


    loadBuiltinNterms(definedNterms);
    if( parser->parse() != 0 )
    {
       std::cerr << "Parse failed!!\n";
       return false;
    }

    return true;
}

}

