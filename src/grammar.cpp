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
//        std::cerr << err.what() << std::endl;
//    }
//}

//Grammar::Grammar(const char *filename) {
//    try {
//        parser = new GrammarParser();
//        parser->beginParseFromFile(filename);
//        readRules();
//    } catch (GrammarParserException &err) {
//        std::cerr << err.what() << std::endl;
//    }
//}

bool Grammar::initFromFile(const char *filename) {
    try {
        parser = new GrammarParser();
        parser->beginParseFromFile(filename);
        readRules();
        if (!validateRules()) {
            return false;
        }
        addExplicitRule();
        buildFirstSet();
        buildFollowSet();
    } catch (GrammarParserException &err) {
        std::cerr << err.what() << std::endl;
        return false;
    }

    return true;
}

bool Grammar::initFromPlainText(const UnicodeString &plainText) {
    try {
        parser = new GrammarParser();
        parser->beginParseFromPlainText(plainText);
        readRules();
        if (!validateRules()) {
            return false;
        }
        addExplicitRule();
        buildFirstSet();
        buildFollowSet();
    } catch (GrammarParserException &err) {
        std::cerr << err.what() << std::endl;
        return false;
    }

    return true;
}

void Grammar::addExplicitRule() {
    startRule = new SimpleGrammarRule {EXPLICIT_START_SYMBOL, {START_SYMBOL}, true};
    rules[EXPLICIT_START_SYMBOL] = {*startRule};
    nonTerminals[EXPLICIT_START_SYMBOL] = {};
    WordInfo startSymbolInfo {{EXPLICIT_START_SYMBOL, 0}, 0};
    nonTerminals[START_SYMBOL].push_back(startSymbolInfo);
}

Grammar::~Grammar() {
    delete parser;
    if (startRule) {
        delete startRule;
    }
}

void Grammar::readRules() {
    if (!parser) {
        return;
    }

    ComplexGrammarRule rule;
    std::map<UnicodeString, std::vector<WordInfo>> tempSet;
    while(parser->getNextRule(rule)) {
//        rules.insert(std::make_pair(std::move(rule.leftPart), std::move(rule.rightHandles)));
//        std::cout << "get next rule: current rule is " << rule << std::endl;

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

        for (int i = 0; i < rule.rightHandles.size(); ++i) {
            auto simpleRule = rule.rightHandles[i];
            for (int j = 0; j < simpleRule.rightHandle.size(); ++j) {
                auto word = simpleRule.rightHandle[j];
                WordInfo wordInfo {{word, i}, j};
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

        auto it = rules.find(rule.leftPart);
        if (it == rules.end()) {
            rules[rule.leftPart] = rule.rightHandles;
        } else {
            auto &rightHandles = it->second;
            rightHandles.insert(rightHandles.end(), rule.rightHandles.begin(), rule.rightHandles.end());
        }
    }

    for (auto &terminal : tempSet) {
        terminals.insert(std::move(terminal.first));
    }

//    std::cout << "Terminals are:" << std::endl;
//    for (auto &term : terminals) {
//        std::cout << term << std::endl;
//    }
    std::cout << "Non terminals are:" << std::endl;
    for (auto &nterm : nonTerminals) {
        std::cout << nterm.first << std::endl;
        for (auto &wordInfo : nterm.second) {
            std::cout << wordInfo.position << ", ";
        }
        std::cout << std::endl;
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
//        std::cout << "First set: current rule: " << rule << " for nonterminal: " << word << std::endl;
        auto firstWord = rule.rightHandle[0];
//        std::cout << "First word: " << firstWord << " for rule: " << rule << std::endl;
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

    std::set<UnicodeString> follow;
    auto info = nonTerminals[word];
    for (auto infoRecord : info) {
        RuleIndex ruleIndex = infoRecord.ruleIndex;
        auto simpleRules = rules[ruleIndex.leftHandle];
        SimpleGrammarRule simpleRule = simpleRules[ruleIndex.simpleRuleNumber];
        if (infoRecord.position < simpleRule.rightHandle.size() - 1) {
            auto nextWord = simpleRule.rightHandle[infoRecord.position + 1];
            auto firstSetForNextWord = firstSet[nextWord];
            follow.insert(firstSetForNextWord.begin(), firstSetForNextWord.end());
        } else if (simpleRule.rightHandle[infoRecord.position] != simpleRule.leftPart) {
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
        std::cout << "First set for word: " << set.first << std::endl;
        for (auto &word : set.second) {
            std::cout << "Word in first set: " << word << std::endl;
        }
    }
}

void Grammar::printFollowSet() {
    for (auto &set : followSet) {
        std::cout << "Follow set for word: " << set.first << std::endl;
        for (auto &word : set.second) {
            std::cout << "Word in follow set: " << word << std::endl;
        }
    }
}

bool Grammar::isEndOfInput(const UnicodeString &word) const {
    return word == END_OF_INPUT;
}

bool Grammar::isStartRule(const SimpleGrammarRule &rule) const {
    return rule.leftPart == EXPLICIT_START_SYMBOL;
}

} /* namespace tproc */
