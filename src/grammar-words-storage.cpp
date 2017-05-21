#include "grammar-words-storage.hpp"
#include "grammar-rule.h"
#include "predicate.hpp"
#include "utils/logger.h"

#include <unicode/ustream.h>

namespace tproc {

std::map<UnicodeString, GRuleWordPtr> GWordStorage::nterms;
std::map<UnicodeString, std::vector<GRuleWordPtr>> GWordStorage::terms;

GRuleWordPtr GWordStorage::getNonTerminal(const UnicodeString &name) {
    Logger::getLogger() << "getNonTerminal(): " << name << std::endl;
    std::map<UnicodeString, GRuleWordPtr>::iterator ntermFound = nterms.find(name);
    if (ntermFound != nterms.end()) {
        Logger::getLogger() << "getNonTerminal(): already created" << std::endl;
        return ntermFound->second;
    } else {
        Logger::getLogger() << "getNonTerminal(): creating" << std::endl;
        auto nterm = std::make_shared<NonTerminal>(name);
        nterms[name] = nterm;
        return nterm;
    }
}

GRuleWordPtr GWordStorage::getTerminal(const UnicodeString &name) {
    GRuleWordPtr result = nullptr;
    auto termsFound = terms.find(name);
    if (termsFound != terms.end()) {
        auto termFound = std::find_if(termsFound->second.begin(), termsFound->second.end(), [&name](const GRuleWordPtr &term){
            return term->getRawValue() == name && term->getPredciatesSize() == 0;
        });
        if (termFound != termsFound->second.end()) {
            result = *termFound;
        }
    }
    if (result == nullptr) {
        result = std::make_shared<Terminal>(name);
        terms[name] = {result};
    }
    return result;
}

GRuleWordPtr GWordStorage::getTerminal(const UnicodeString &name, const std::vector<PredicatePtr> &predicates) {
    GRuleWordPtr result = nullptr;
    auto termsFound = terms.find(name);
    if (termsFound != terms.end()) {
        auto termFound = std::find_if(termsFound->second.begin(), termsFound->second.end(), [&name, &predicates](const GRuleWordPtr &term){
            if (term->getRawValue() != name) {
                return false;
            }
            if (term->getPredicates().size() != predicates.size()) {
                return false;
            }
            unsigned long hash1 = 0, hash2 = 0;
            for (int i = 0; i < predicates.size(); ++i) {
                hash1 += predicates[i]->hash();
                hash2 += term->getPredicates()[i]->hash();
            }
            return hash1 == hash2;
        });
        if (termFound != termsFound->second.end()) {
            result = *termFound;
        }
    }
    if (result == nullptr) {
        result = std::make_shared<Terminal>(name);
        terms[name] = { result };
    }
    return result;
}

GRuleWordPtr GWordStorage::getEmptyTerminal() {
    static GRuleWordPtr result = nullptr;
    if (!result) {
//        auto termFound = std::find_if(terms.begin(), terms.end(), [](const GRuleWordPtr &term){
//            return term->getRawValue() == EMPTY;
//        });
//        if (termFound != terms.end()) {
//            result = *termFound;
//        } else {
//            result = std::make_shared<Terminal>(EMPTY);
//        }
        auto termsFound = terms.find(EMPTY);
        if (termsFound != terms.end()) {
            result = termsFound->second[0];
        } else {
            result = std::make_shared<Terminal>(EMPTY);
            terms[EMPTY] = { result };
        }
    }
    return result;
}

GRuleWordPtr GWordStorage::getEOITerminal() {
    static GRuleWordPtr result = nullptr;
    if (!result) {
//        auto termFound = std::find_if(terms.begin(), terms.end(), [](const GRuleWordPtr &term){
//            return term->getRawValue() == END_OF_INPUT;
//        });
//        if (termFound != terms.end()) {
//            result = *termFound;
//        } else {
//            result = std::make_shared<Terminal>(END_OF_INPUT);
//        }

        auto termsFound = terms.find(END_OF_INPUT);
        if (termsFound != terms.end()) {
            result = termsFound->second[0];
        } else {
            result = std::make_shared<Terminal>(END_OF_INPUT);
            terms[END_OF_INPUT] = { result };
        }
    }
    return result;
}

}
