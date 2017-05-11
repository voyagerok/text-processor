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

#include "grammar-parser.h"
#include "grammar.h"
#include "grammar-rule.h"
#include "utils/logger.h"

namespace tproc {

std::ostream &operator<<(std::ostream &os, RuleIndex ruleIndex) {
    os << "word: " << ruleIndex.leftHandle << ", ruleNumber" << ruleIndex.simpleRuleNumber << std::endl;
    return os;
}

bool RuleIndex::operator==(const RuleIndex &other) const {
    return this->leftHandle == other.leftHandle && this->simpleRuleNumber == other.simpleRuleNumber;
}

bool RuleIndex::operator!=(const RuleIndex &other) const {
    return !(*this == other);
}

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
    try {
//        parser = std::make_shared<GrammarParser>();
//        parser = new GrammarParser();
//        parser = std::move(std::unique_ptr<GrammarParser>(new GrammarParser()));
//        parser->beginParseFromFile(filename);
        if (parserDriver.parse(filename)) {
            readRules(parserDriver.getRules());
            if (!validateRules()) {
                return false;
            }
            addExplicitRule();
            buildFirstSet();
            buildFollowSet();
        }
    } catch (GrammarParserException &err) {
        Logger::getErrLogger() << err.what() << std::endl;
        return false;
    }

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
    GRuleWord ruleWord { START_SYMBOL };
    startRule = std::make_shared<SimpleGrammarRule>(EXPLICIT_START_SYMBOL, std::vector<GRuleWord>{ruleWord});
    rules[EXPLICIT_START_SYMBOL] = {*startRule};
    nonTerminals[EXPLICIT_START_SYMBOL] = {};
    WordInfo startSymbolInfo {{EXPLICIT_START_SYMBOL, 0}, 0};
    nonTerminals[START_SYMBOL].push_back(startSymbolInfo);
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
    std::map<UnicodeString, std::vector<WordInfo>> tempSet;


    for (auto &rule : plainRules) {

//    }
//    while(parser->getNextRule(rule)) {
//        rules.insert(std::make_pair(std::move(rule.leftPart), std::move(rule.rightHandles)));
//        Logger::getLogger() << "get next rule: current rule is " << rule << std::endl;

        std::vector<WordInfo> nterm_wordinfo;
        auto found = tempSet.find(rule.leftPart);
        if (found != tempSet.end()) {
            nterm_wordinfo = found->second;
            tempSet.erase(rule.leftPart);
        } else {
            nterm_wordinfo = {};
        }
        auto ntermFound = nonTerminals.find(rule.leftPart);
        if (ntermFound == nonTerminals.end()) {
            nonTerminals[rule.leftPart] = std::move(nterm_wordinfo);
        } else {
            ntermFound->second.insert(ntermFound->second.end(), nterm_wordinfo.begin(), nterm_wordinfo.end());
        }

        auto ruleInStorage = rules.find(rule.leftPart);
        int n_of_rules_for_left = ruleInStorage == rules.end() ? 0 : ruleInStorage->second.size();

        for (int i = 0; i < rule.rightHandles.size(); ++i) {
            auto simpleRule = rule.rightHandles[i];
            for (int j = 0; j < simpleRule.rightHandle.size(); ++j) {
                auto word = simpleRule.rightHandle[j].rawValue;
                WordInfo wordInfo {{rule.leftPart, n_of_rules_for_left}, j};
                auto ntermFound = nonTerminals.find(word);
                if (ntermFound == nonTerminals.end()) {
                    auto wordFound = tempSet.find(word);
                    if (wordFound != tempSet.end()) {
                        wordFound->second.push_back(std::move(wordInfo));
                    } else {
                        tempSet[word] = {std::move(wordInfo)};
                    }
                } else {
                    ntermFound->second.push_back(std::move(wordInfo));
                }
            }
        }

//        auto it = rules.find(rule.leftPart);
        if (ruleInStorage == rules.end()) {
            rules[rule.leftPart] = rule.rightHandles;
        } else {
            auto &rightHandles = ruleInStorage->second;
            rightHandles.insert(rightHandles.end(), rule.rightHandles.begin(), rule.rightHandles.end());
        }
    }

    for (auto &terminal : tempSet) {
        terminals.insert(std::move(terminal.first));
    }

//    Logger::getLogger() << "Terminals are:" << std::endl;
//    for (auto &term : terminals) {
//        Logger::getLogger() << term << std::endl;
//    }
    Logger::getLogger() << "Non terminals are:" << std::endl;
    for (auto &nterm : nonTerminals) {
        Logger::getLogger() << nterm.first << std::endl;
        for (auto &wordInfo : nterm.second) {
            Logger::getLogger() << wordInfo.position << ", ";
        }
        Logger::getLogger() << std::endl;
    }
}

void Grammar::buildFirstSet() {
    for (auto &terminal : terminals) {
        firstSet[terminal] = {terminal};
    }

    for (auto &nterminal : nonTerminals) {
        auto nterm_word = nterminal.first;
        firstSet[nterm_word] = firstSetForNonTerminal(nterm_word);
    }
}

void Grammar::buildFollowSet() {
    followSet[EXPLICIT_START_SYMBOL] = {END_OF_INPUT};
    for (auto &nterm : nonTerminals) {
        auto nterm_word = nterm.first;
        followSet[nterm_word] = followSetForNonTerminal(nterm_word);
    }
}

std::set<UnicodeString> Grammar::firstSetForNonTerminal(const UnicodeString &word) {
    std::set<UnicodeString> firstSet;
    auto rulesForWord = getRulesForLeftHandle(word);
    for (auto &rule : rulesForWord) {
//        Logger::getLogger() << "First set: current rule: " << rule << " for nonterminal: " << word << std::endl;
        auto firstWord = rule.rightHandle[0].rawValue;
//        Logger::getLogger() << "First word: " << firstWord << " for rule: " << rule << std::endl;
        if (firstWord == word) {
            continue;
        }
        if (isNonTerminal(firstWord)) {
            auto result = firstSetForNonTerminal(firstWord);
            if (result.size() > 0) {
                firstSet.insert(result.begin(), result.end());
            }
        } else {
            firstSet.insert(firstWord);
        }
    }

    return firstSet;
}

std::set<UnicodeString> Grammar::followSetForNonTerminal(const UnicodeString &word) {
//    if (word == EXPLICIT_START_SYMBOL) {
//        return {END_OF_INPUT};
//    }

    auto followForWord = followSet.find(word);
    if (followForWord != followSet.end()) {
        return  followForWord->second;
    }

    Logger::getLogger() << "word: " << word << std::endl;

    std::set<UnicodeString> follow;
    auto info = nonTerminals[word];
    for (auto infoRecord : info) {
        RuleIndex ruleIndex = infoRecord.ruleIndex;
        auto simpleRules = rules[ruleIndex.leftHandle];
        SimpleGrammarRule simpleRule = simpleRules[ruleIndex.simpleRuleNumber];
        if (infoRecord.position < simpleRule.rightHandle.size() - 1) {
            auto nextWord = simpleRule.rightHandle[infoRecord.position + 1].rawValue;
            auto firstSetForNextWord = firstSet[nextWord];
            follow.insert(firstSetForNextWord.begin(), firstSetForNextWord.end());
        } else if (simpleRule.rightHandle[infoRecord.position] != simpleRule.leftPart) {
            Logger::getLogger() << simpleRule << std::endl;
            Logger::getLogger() << "current word: " << simpleRule.rightHandle[infoRecord.position] << std::endl;
            auto followSetForLeftHandle = followSetForNonTerminal(simpleRule.leftPart);
            follow.insert(followSetForLeftHandle.begin(), followSetForLeftHandle.end());
        }
    }

    return follow;
}

std::vector<SimpleGrammarRule> Grammar::getRulesForLeftHandle(const UnicodeString &leftHandle) const {
    auto it = rules.find(leftHandle);
    if (it == rules.end()) {
        return std::vector<SimpleGrammarRule>();
    } else {
        return it->second;
    }
}

Grammar::iter Grammar::begin() {
    return rules.begin();
}

Grammar::iter Grammar::end() {
    return rules.end();
}

Grammar::const_iter Grammar::begin() const {
    return rules.begin();
}

Grammar::const_iter Grammar::end() const {
    return rules.end();
}

bool Grammar::validateRules() {
    return rules.find(START_SYMBOL) != rules.end();
}

SimpleGrammarRule &Grammar::getStartRule() const {
    static SimpleGrammarRule emptyRule {"", {}, false};

    if (startRule) {
        return *startRule;
    } else {
        return emptyRule;
    }
}

RuleIndex Grammar::getRuleIndex(const SimpleGrammarRule &rule) const {
    auto it = rules.find(rule.leftPart);
    if (it != rules.end()) {
        auto &rulesForLeftPart = it->second;
        for (int i = 0; i < rulesForLeftPart.size(); ++i) {
            if (rulesForLeftPart[i] == rule) {
                return {it->first, i};
            }
        }
    }

    return {"", -1};
}

bool Grammar::followWordsForNterminal(const UnicodeString &nterm, std::set<UnicodeString> &followWords) const {
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

bool Grammar::isEndOfInput(const UnicodeString &word) const {
    return word == END_OF_INPUT;
}

bool Grammar::isStartRule(const SimpleGrammarRule &rule) const {
    return rule.leftPart == EXPLICIT_START_SYMBOL;
}

bool Grammar::getRuleForRuleIndex(const RuleIndex &ruleIndex, SimpleGrammarRule &rule) const {
    auto rulesForNterm = rules.find(ruleIndex.leftHandle);
    if (rulesForNterm == rules.end()) {
        return false;
    }
    if (ruleIndex.simpleRuleNumber < 0 || ruleIndex.simpleRuleNumber >= rulesForNterm->second.size()) {
        return false;
    }
    rule = rulesForNterm->second.at(ruleIndex.simpleRuleNumber);
    return true;
}

} /* namespace tproc */
