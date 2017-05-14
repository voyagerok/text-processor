/*
 * grammar.cpp
 *
 *  Created on: Apr 9, 2017
 *      Author: nikolay
 */

#include <vector>
#include <map>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <iostream>
#include <algorithm>

//#include "grammar-parser.h"
#include "grammar.h"
#include "grammar-rule.h"
#include "utils/logger.h"

namespace tproc {

//Grammar::Grammar(const UnicodeString &plainText) {
//    try {
//        parser = new GrammarParser();
//        parser->beginParseFromPlainText(plainText);
//        readRules();
//    } catch (GrammarParserException &err) {
//        Logger::getErrLogger() << err.what() << std::endl;
//    }
//}

//Grammar::Grammar(const char *filename) {
//    try {
//        parser = new GrammarParser();
//        parser->beginParseFromFile(filename);
//        readRules();
//    } catch (GrammarParserException &err) {
//        Logger::getErrLogger() << err.what() << std::endl;
//    }
//}

bool Grammar::initFromFile(const std::string &filename) {
//    try {
//        parser = std::make_shared<GrammarParser>();
//        parser = new GrammarParser();
//        parser = std::move(std::unique_ptr<GrammarParser>(new GrammarParser()));
//        parser->beginParseFromFile(filename);
    GParserDriver parserDriver;
    if (parserDriver.parse(filename)) {
        //            readRules(parserDriver.getRules());
        //            if (!validateRules()) {
        //                return false;
        //            }
        if ((root = parserDriver.getRootNterm()) == nullptr) {
            Logger::getLogger() << "Failed to find root rule" << std::endl;
            return false;
        }
        if (parserDriver.hasPendingNterms()) {
            Logger::getLogger() << "Some non terminals don't have definitions" << std::endl;
            return false;
        }

        this->nterminals = std::move(parserDriver.getDefinedNterms());
        this->terminals = std::move(parserDriver.getTerminals());
        this->pendingActions = std::move(parserDriver.getPendingActions());

        addExplicitRule();
        buildFirstSet();
        buildFollowSet();
    }
//    } catch (GrammarParserException &err) {
//        Logger::getErrLogger() << err.what() << std::endl;
//        return false;
//    }

    return true;
}

bool Grammar::initFromPlainText(const UnicodeString &plainText) {
//    try {
//        parser = std::make_shared<GrammarParser>();
////        parser = std::move(std::unique_ptr<GrammarParser>(new GrammarParser()));
//        parser->beginParseFromPlainText(plainText);
//        readRules();
//        if (!validateRules()) {
//            return false;
//        }
//        addExplicitRule();
//        buildFirstSet();
//        buildFollowSet();
//    } catch (GrammarParserException &err) {
//        Logger::getErrLogger() << err.what() << std::endl;
//        return false;
//    }

    return true;
}

void Grammar::addExplicitRule() {
//    startRule = std::unique_ptr<SimpleGrammarRule>{new SimpleGrammarRule {EXPLICIT_START_SYMBOL, {START_SYMBOL}, true}};
//    startRule = new SimpleGrammarRule {EXPLICIT_START_SYMBOL, {START_SYMBOL}, true};
//    startRule = create_unique<SimpleGrammarRule>(EXPLICIT_START_SYMBOL, {START_SYMBOL}, true);
//    startRule = std::move(std::unique_ptr<SimpleGrammarRule>(new SimpleGrammarRule {EXPLICIT_START_SYMBOL, {START_SYMBOL}, true}));

//    GRuleWord ruleWord { START_SYMBOL };
//    startRule = std::make_shared<SimpleGrammarRule>(EXPLICIT_START_SYMBOL, std::vector<GRuleWord>{ruleWord});
//    rules[EXPLICIT_START_SYMBOL] = {*startRule};
//    nonTerminals[EXPLICIT_START_SYMBOL] = {};
//    WordInfo startSymbolInfo {{EXPLICIT_START_SYMBOL, 0}, 0};
//    nonTerminals[START_SYMBOL].push_back(startSymbolInfo);

//    auto currentStartNterm = std::make_shared<NonTerminal>(START_SYMBOL);
//    startRule = std::make_shared<SimpleGrammarRule>(EXPLICIT_START_SYMBOL, std::vector<GRuleWordPtr> { currentStartNterm });
//    rules[EXPLICIT_START_SYMBOL] = { *startRule };

    auto newRoot = std::make_shared<NonTerminal>(EXPLICIT_START_SYMBOL);
    newRoot->getChildWords().push_back({ root });
//    WordIndex oldRootIndex { newRoot, 0, 0};
    root->getParentNterms().emplace_back(newRoot, 0, 0);
    root = newRoot;
    nterminals.insert(root);
}

Grammar::~Grammar() {
//    delete parser;
//    if (startRule) {
//        delete startRule;
//    }
}

void Grammar::readRules(const std::vector<ComplexGrammarRule> &plainRules) {
//    if (!parser) {
//        return;
//    }

//    ComplexGrammarRule rule;
    std::map<UnicodeString, std::vector<ParentInfo>> tempSet;


    for (auto &rule : plainRules) {

//    }
//    while(parser->getNextRule(rule)) {
//        rules.insert(std::make_pair(std::move(rule.leftPart), std::move(rule.rightHandles)));
//        Logger::getLogger() << "get next rule: current rule is " << rule << std::endl;

//        std::vector<WordIndex> nterm_wordinfo;
//        auto found = tempSet.find(rule.leftPart);
//        if (found != tempSet.end()) {
//            nterm_wordinfo = found->second;
//            tempSet.erase(rule.leftPart);
//        } else {
//            nterm_wordinfo = {};
//        }
//        auto ntermFound = nonTerminals.find(rule.leftPart);
//        if (ntermFound == nonTerminals.end()) {
//            nonTerminals[rule.leftPart] = std::move(nterm_wordinfo);
//        } else {
//            ntermFound->second.insert(ntermFound->second.end(), nterm_wordinfo.begin(), nterm_wordinfo.end());
//        }

//        auto ruleInStorage = rules.find(rule.leftPart);
//        int n_of_rules_for_left = ruleInStorage == rules.end() ? 0 : ruleInStorage->second.size();

//        for (int i = 0; i < rule.rightHandles.size(); ++i) {
//            auto simpleRule = rule.rightHandles[i];
//            for (int j = 0; j < simpleRule.rightHandle.size(); ++j) {
//                auto word = simpleRule.rightHandle[j].rawValue;
//                WordIndex wordInfo {{rule.leftPart, n_of_rules_for_left}, j};
//                auto ntermFound = nonTerminals.find(word);
//                if (ntermFound == nonTerminals.end()) {
//                    auto wordFound = tempSet.find(word);
//                    if (wordFound != tempSet.end()) {
//                        wordFound->second.push_back(std::move(wordInfo));
//                    } else {
//                        tempSet[word] = {std::move(wordInfo)};
//                    }
//                } else {
//                    ntermFound->second.push_back(std::move(wordInfo));
//                }
//            }
//        }

////        auto it = rules.find(rule.leftPart);
//        if (ruleInStorage == rules.end()) {
//            rules[rule.leftPart] = rule.rightHandles;
//        } else {
//            auto &rightHandles = ruleInStorage->second;
//            rightHandles.insert(rightHandles.end(), rule.rightHandles.begin(), rule.rightHandles.end());
//        }
//    }

//    for (auto &terminal : tempSet) {
//        terminals.insert(std::move(terminal.first));
//    }

//    Logger::getLogger() << "Terminals are:" << std::endl;
//    for (auto &term : terminals) {
//        Logger::getLogger() << term << std::endl;
    }
//    Logger::getLogger() << "Non terminals are:" << std::endl;
//    for (auto &nterm : nonTerminals) {
//        Logger::getLogger() << nterm.first << std::endl;
//        for (auto &wordInfo : nterm.second) {
//            Logger::getLogger() << wordInfo.position << ", ";
//        }
//        Logger::getLogger() << std::endl;
//    }
}

void Grammar::buildFirstSet() {
    for (auto &terminal : terminals) {
        firstSet[terminal] = {terminal};
    }

    for (auto &nterminal : nterminals) {
//        auto nterm_word = nterminal.first;
        firstSet[nterminal] = firstSetForNonTerminal(nterminal);
    }
}

void Grammar::buildFollowSet() {
//    followSet[EXPLICIT_START_SYMBOL] = {END_OF_INPUT};
//    for (auto &nterm : nonTerminals) {
//        auto nterm_word = nterm.first;
//        followSet[nterm_word] = followSetForNonTerminal(nterm_word);
//    }
    followSet[root] = { StandardTerminalStorage::getEndOfInputTerminal() };
    for (auto &nterm : nterminals) {
        if (nterm == root)
            continue;
        followSet[nterm] = followSetForNonTerminal(nterm);
    }
}

std::set<GRuleWordPtr> Grammar::firstSetForNonTerminal(const GRuleWordPtr &nterm) {
    std::set<GRuleWordPtr> firstSet;
//    if (nterm->isRhsEmpty()) {
//        firstSet.insert(nterm->getChildWords().at(0).at(0));
//        return firstSet;
//    }

//    auto rulesForWord = getRulesForLeftHandle(nterm);
    for (auto &childWords : nterm->getChildWords()) {
//        Logger::getLogger() << "First set: current rule: " << rule << " for nonterminal: " << word << std::endl;
//        auto firstWord = rule.rightHandle[0].rawValue;
//        auto firstWord = childWords[0];
//        GRuleWordPtr firstWord = nullptr;
//        for (int i = 0; i < childWords.size(); ++i) {
//            if (!childWords[i]->isEmptyWord()) {
//                firstWord = childWords[i];
//                break;
//            }
//        }
//        if (firstWord == nullptr) { // empty word case
//            firstSet.insert(*childWords.begin());
//        }
////        Logger::getLogger() << "First word: " << firstWord << " for rule: " << rule << std::endl;
//        else if (firstWord == nterm) {
//            continue;
//        }
//        else if (firstWord->isNonTerminal()) {
//            auto result = firstSetForNonTerminal(firstWord);
//            if (result.size() > 0) {
//                firstSet.insert(result.begin(), result.end());
//            }
//        } else {
//            firstSet.insert(firstWord);
//        }

        if (childWords.size() == 1 && childWords[0]->isEmptyWord()) {
            firstSet.insert(childWords[0]);
        } else {
            GRuleWordPtr firstWord = childWords[0];
            if (firstWord == nterm)
                continue;
            if (firstWord->isNonTerminal()) {
                bool shouldInsertEmptyWord = true;
                for (int i = 1; i <= childWords.size(); ++i ) {
                    auto result = firstSetForNonTerminal(firstWord);
                    if (result.size() > 0) {
                        int resultSize = result.size();
                        result.erase(StandardTerminalStorage::getEmptyTerminal());
                        firstSet.insert(result.begin(), result.end());
                        if (result.size() == resultSize) {
                            shouldInsertEmptyWord = false;
                            break;
                        }
                        firstWord = childWords[i];
                    }
                }
                if (shouldInsertEmptyWord) {
                    firstSet.insert(StandardTerminalStorage::getEmptyTerminal());
                }
            } else {
                firstSet.insert(firstWord);
            }
        }
    }

    return firstSet;
}

std::set<GRuleWordPtr> Grammar::followSetForNonTerminal(const GRuleWordPtr &word) {
//    if (word == EXPLICIT_START_SYMBOL) {
//        return {END_OF_INPUT};
//    }

    auto followForWord = followSet.find(word);
    if (followForWord != followSet.end()) {
        return  followForWord->second;
    }

    Logger::getLogger() << "word: " << word << std::endl;

    std::set<GRuleWordPtr> follow;
    auto wordIndexInfo = word->getParentNterms();
    for (auto infoRecord : wordIndexInfo) {
//        RuleIndex ruleIndex = infoRecord.ruleIndex;
//        auto simpleRules = rules[ruleIndex.nterm];
//        SimpleGrammarRule simpleRule = simpleRules[ruleIndex.index];
//        if (infoRecord.position < simpleRule.rhs.size() - 1) {
//            auto nextWord = simpleRule.rhs[infoRecord.position + 1].rawValue;
//            auto firstSetForNextWord = firstSet[nextWord];
//            follow.insert(firstSetForNextWord.begin(), firstSetForNextWord.end());
//        } else if (simpleRule.rhs[infoRecord.position] != simpleRule.lhs) {
//            Logger::getLogger() << simpleRule << std::endl;
//            Logger::getLogger() << "current word: " << simpleRule.rhs[infoRecord.position] << std::endl;
//            auto followSetForLeftHandle = followSetForNonTerminal(simpleRule.lhs);
//            follow.insert(followSetForLeftHandle.begin(), followSetForLeftHandle.end());
//        }
        GRuleWordPtr parentWord = infoRecord.nterm;
        ChildWords &childWords = parentWord->getChildWords().at(infoRecord.wordIndex.ruleIndex);
        if (infoRecord.wordIndex.position < childWords.size() - 1) {
            GRuleWordPtr nextWord = childWords[infoRecord.wordIndex.position + 1];
            auto &nextWordFS = firstSet[nextWord];
            int firstSetOldSize = nextWordFS.size();
            nextWordFS.erase(StandardTerminalStorage::getEmptyTerminal());
            follow.insert(nextWordFS.begin(), nextWordFS.end());
            if (firstSetOldSize != nextWordFS.size() && childWords[infoRecord.wordIndex.position] != parentWord) {
                auto parentWordFS = followSetForNonTerminal(parentWord);
                follow.insert(parentWordFS.begin(), parentWordFS.end());
            }
        } else if (childWords[infoRecord.wordIndex.position] != parentWord) {
            auto parentWordFS = followSetForNonTerminal(parentWord);
            follow.insert(parentWordFS.begin(), parentWordFS.end());
        }
    }

    return follow;
}

//std::vector<SimpleGrammarRule> Grammar::getRulesForLeftHandle(const UnicodeString &leftHandle) const {
//    auto it = rules.find(leftHandle);
//    if (it == rules.end()) {
//        return std::vector<SimpleGrammarRule>();
//    } else {
//        return it->second;
//    }
//}

//Grammar::iter Grammar::begin() {
//    return rules.begin();
//}

//Grammar::iter Grammar::end() {
//    return rules.end();
//}

//Grammar::const_iter Grammar::begin() const {
//    return rules.begin();
//}

//Grammar::const_iter Grammar::end() const {
//    return rules.end();
//}

//bool Grammar::validateRules() {
//    return rules.find(START_SYMBOL) != rules.end();
//}

//SimpleGrammarRule &Grammar::getStartRule() const {
//    static SimpleGrammarRule emptyRule {"", {}, false};

//    if (startRule) {
//        return *startRule;
//    } else {
//        return emptyRule;
//    }
//}

//RuleIndex Grammar::getRuleIndex(const SimpleGrammarRule &rule) const {
//    auto it = rules.find(rule.lhs);
//    if (it != rules.end()) {
//        auto &rulesForLeftPart = it->second;
//        for (int i = 0; i < rulesForLeftPart.size(); ++i) {
//            if (rulesForLeftPart[i] == rule) {
//                return {it->first, i};
//            }
//        }
//    }

//    return {"", -1};
//}

bool Grammar::followWordsForNterminal(const GRuleWordPtr &nterm, std::set<GRuleWordPtr> &followWords) const {
    auto followItem = followSet.find(nterm);
    if (followItem != followSet.end()) {
        followWords = followItem->second;
        return true;
    }

    return false;
}

void Grammar::printFirstSet() {
    for (auto &set : firstSet) {
        Logger::getLogger() << "First set for word: " << set.first << std::endl;
        for (auto &word : set.second) {
            Logger::getLogger() << "Word in first set: " << word << std::endl;
        }
    }
}

void Grammar::printFollowSet() {
    for (auto &set : followSet) {
        Logger::getLogger() << "Follow set for word: " << set.first << std::endl;
        for (auto &word : set.second) {
            Logger::getLogger() << "Word in follow set: " << word << std::endl;
        }
    }
}

//bool Grammar::isEndOfInput(const UnicodeString &word) const {
//    return word == END_OF_INPUT;
//}

//bool Grammar::isStartRule(const SimpleGrammarRule &rule) const {
//    return rule.lhs == EXPLICIT_START_SYMBOL;
//}

//bool Grammar::getRuleForRuleIndex(const RuleIndex &ruleIndex, SimpleGrammarRule &rule) const {
//    auto rulesForNterm = rules.find(ruleIndex.nterm);
//    if (rulesForNterm == rules.end()) {
//        return false;
//    }
//    if (ruleIndex.index < 0 || ruleIndex.index >= rulesForNterm->second.size()) {
//        return false;
//    }
//    rule = rulesForNterm->second.at(ruleIndex.index);
//    return true;
//}

} /* namespace tproc */
