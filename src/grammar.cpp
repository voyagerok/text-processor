/*
 * grammar.cpp
 *
 *  Created on: Apr 9, 2017
 *      Author: nikolay
 */

#include <vector>
#include <map>
#include <unicode/unistr.h>
#include <iostream>
#include <algorithm>

#include "grammar-parser.h"
#include "grammar.h"
#include "grammar-rule.h"

namespace tproc {

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
        startRule = new SimpleGrammarRule {EXPLICIT_START_SYMBOL, {START_SYMBOL}, true};
        rules[EXPLICIT_START_SYMBOL] = {*startRule};
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
        startRule = new SimpleGrammarRule {EXPLICIT_START_SYMBOL, {START_SYMBOL}, true};
        rules[EXPLICIT_START_SYMBOL] = {*startRule};
    } catch (GrammarParserException &err) {
        std::cerr << err.what() << std::endl;
        return false;
    }

    return true;
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

        auto found = tempSet.find(rule.leftPart);
        if (found != tempSet.end()) {
            nonTerminals[rule.leftPart] = found->second;
            tempSet.erase(rule.leftPart);
        } else {
            nonTerminals[rule.leftPart] = {};
        }

        for (int i = 0; i < rule.rightHandles.size(); ++i) {
            auto simpleRule = rule.rightHandles[i];
            for (int j = 0; j < simpleRule.rightHandle.size(); ++j) {
                auto word = simpleRule.rightHandle[j];
                if (nonTerminals.find(word) == nonTerminals.end()) {
                    WordInfo wordInfo {{word, i}, j};
                    auto wordFound = tempSet.find(word);
                    if (wordFound != tempSet.end()) {
                        wordFound->second.push_back(std::move(wordInfo));
                    } else {
                        tempSet[word] = {std::move(wordInfo)};
                    }
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

    buildFirstSet();
    buildFollowSet();
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
    for (auto &nterm : nonTerminals) {
        auto nterm_word = nterm.first;
        followSet[nterm_word] = followSetForNonTerminal(nterm_word);
    }
}

std::set<UnicodeString> Grammar::firstSetForNonTerminal(const UnicodeString &word) {
    std::set<UnicodeString> firstSet;
    auto rulesForWord = getRulesForLeftHandle(word);
    for (auto &rule : rulesForWord) {
        auto firstWord = rule.rightHandle[0];
        if (firstWord == word) {
            continue;
        }
        auto result = firstSetForNonTerminal(firstWord);
        if (result.size() > 0) {
            firstSet.insert(result.begin(), result.end());
        }
    }

    return firstSet;
}

std::set<UnicodeString> Grammar::followSetForNonTerminal(const UnicodeString &word) {
    if (word == EXPLICIT_START_SYMBOL) {
        return {END_OF_INPUT};
    }

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
        } else {
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

} /* namespace tproc */
