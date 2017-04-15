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
    while(parser->getNextRule(rule)) {
//        rules.insert(std::make_pair(std::move(rule.leftPart), std::move(rule.rightHandles)));
        std::cout << "get next rule: current rule is " << rule << std::endl;
        auto it = rules.find(rule.leftPart);
        if (it == rules.end()) {
            rules[rule.leftPart] = rule.rightHandles;
        } else {
            auto &rightHandles = rules[rule.leftPart];
            rightHandles.insert(rightHandles.end(), rule.rightHandles.begin(), rule.rightHandles.end());
        }
    }
}

//std::vector<UnicodeString> Grammar::getRightHandlesForLeftHandle(const UnicodeString &leftHandle) {
//    auto iter = rules.find(leftHandle);
//    if (iter != rules.end()) {
//        return iter->second;
//    }
//    return std::vector<UnicodeString>{};
//}

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

} /* namespace tproc */
