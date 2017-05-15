#include "g-parser-driver.hpp"

#include <fstream>
#include <sstream>
#include <unicode/ustream.h>

#include "gparser.tab.hh"
#include "g-scanner.hpp"
#include "utils/logger.h"

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

void GParserDriver::appendRule(const GRuleWordPtr &ntermPtr) {
//    currentRuleWordNum =0;
//    incSimpleRuleIndex();

    Logger::getLogger() << "appendRule: " << ntermPtr << std::endl;

    auto defFound = std::find_if(definedNterms.begin(), definedNterms.end(), [&ntermPtr](auto &wordPtr){
        return wordPtr->getRawValue() == ntermPtr->getRawValue();
    });
    if (defFound != definedNterms.end()) {
        auto childWords = (*defFound)->getChildWords();
        childWords.insert(childWords.end(), ntermPtr->getChildWords().begin(), ntermPtr->getChildWords().end());
        return;
    }

    auto pendFound = std::find_if(pendingNterms.begin(), pendingNterms.end(), [&ntermPtr](auto &wordPtr){ return wordPtr->getRawValue() == ntermPtr->getRawValue(); });
    if (pendFound != pendingNterms.end()) {
        GRuleWordPtr pendPtr = *pendFound;
        pendingNterms.erase(pendFound);
        std::move(ntermPtr->getChildWords().begin(), ntermPtr->getChildWords().end(), std::back_inserter(pendPtr->getChildWords()));
        definedNterms.insert(pendPtr);
        return;
    }

//    auto newNterm = std::make_shared<NonTerminal>(lhs, rhs);
    definedNterms.insert(ntermPtr);
    rootCandidates.insert(ntermPtr);
}

void GParserDriver::fixAndSaveActionList(const GRuleWordPtr &actionWord, std::vector<ActionPtr> &&actions) {
    for (auto &action : actions) {
        action->getRuleWord() = actionWord;
    }
    std::move(pendingActions.begin(), pendingActions.end(), std::back_inserter(actions));
}

void GParserDriver::fixParentInfo(/*const std::vector<GRuleWordPtr> &words,*/ const GRuleWordPtr &parent/*, int baseRuleIndex*/) {
    Logger::getLogger() << "Fix parent info: " << parent->getRawValue() << std::endl;

    std::vector<GRuleWordPtr> &words = parent->getChildWords().back();
    int baseRuleIndex = parent->getChildWords().size() - 1;

    auto defFound = definedNterms.find(parent);
    if (defFound != definedNterms.end()) {
        baseRuleIndex += (*defFound)->getChildWords().size();
    }

    for (int i = 0; i < words.size(); ++i) {
//        word->getParentNterms().push_back(parent);
//        rootCandidates.erase(word);
        ParentInfo wordIndex { parent, baseRuleIndex, i };
        words[i]->getParentNterms().push_back(wordIndex);
        rootCandidates.erase(words[i]);
    }
}

GRuleWordPtr GParserDriver::handleNtermReduction(UnicodeString &&rawValue) {
    Logger::getLogger() << "handleNtermReduction: " << rawValue << std::endl;
    auto defFound = std::find_if(definedNterms.begin(), definedNterms.end(), [&rawValue](const GRuleWordPtr &wordPtr){ return wordPtr->getRawValue() == rawValue; });
    if (defFound != definedNterms.end()) {
        rootCandidates.erase(*defFound);
        return (*defFound);
    } else {
        auto pendingNterm = std::make_shared<NonTerminal>(std::move(rawValue));
        pendingNterms.insert(pendingNterm);
        return pendingNterm;
    }
}

GRuleWordPtr GParserDriver::handleTermReduction(UnicodeString &&rawValue) {
    Logger::getLogger() << "handleTermReduction: " << rawValue << std::endl;
    auto termFoud = std::find_if(terminals.begin(), terminals.end(), [&rawValue](const GRuleWordPtr &wordPtr){
        if (wordPtr->getRawValue() != rawValue) return false;
        auto termPtr = std::dynamic_pointer_cast<Terminal>(wordPtr);
        return (termPtr != nullptr && termPtr->getPredicates().size() == 0);
    });
    if (termFoud != terminals.end()) {
        return (*termFoud);
    } else {
        auto newTerm = std::make_shared<Terminal>(std::move(rawValue));
        terminals.insert(newTerm);
        return newTerm;
    }
}

GRuleWordPtr GParserDriver::handleTermReduction(UnicodeString &&rawValue, std::vector<PredicatePtr> &&predicates) {
    auto termFoud = std::find_if(terminals.begin(), terminals.end(), [&rawValue, &predicates](const GRuleWordPtr &wordPtr){
        if (wordPtr->getRawValue() != rawValue)
            return false;
        auto termPtr = std::dynamic_pointer_cast<Terminal>(wordPtr);
//        return (termPtr != nullptr && termPtr->getPredicates().size() == 0);
        if (termPtr->getPredicates().size() != predicates.size())
            return false;
        for (int i = 0; i < predicates.size(); ++i) {
            if (termPtr->getPredicates().at(i) != predicates[i]) {
                return false;
            }
        }
        return true;
    });
    if (termFoud != terminals.end()) {
        return (*termFoud);
    } else {
        auto newTerm = std::make_shared<Terminal>(std::move(rawValue), std::move(predicates));
        terminals.insert(newTerm);
        return newTerm;
    }
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

    const int accept( 0 );
    if( parser->parse() != accept )
    {
       std::cerr << "Parse failed!!\n";
       return false;
    }

    return true;
}

}

