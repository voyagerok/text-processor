/*
 * GrammarParser.cpp
 *
 *  Created on: Apr 8, 2017
 *      Author: nikolay
 */

#include <fstream>
#include <string>
#include <unicode/unistr.h>
#include <unicode/regex.h>
#include <sstream>
#include <iostream>
#include <unicode/ustream.h>

#include "utils/converter.h"
#include "grammar-rule.h"
#include "grammar-parser.h"
#include "utils/logger.h"
#include "utils/string-helper.h"

static char const* regular_rule_pattern = "(?<left>[A-Za-z]+|[А-Яа-я]+)\\s=(?<right>(?:\\s*(?:[A-Za-z]+|\\\"*[А-Яа-я]+\\\"*)\\s*\\|*)+)";
//static char const* start_rule_pattern = "S\\s=(?<right>(?:\\s(?:[A-Za-z]+|[А-Яа-я]+|\\\"[А-Яа-я]+\\\")\\s*\\|*)+)";
static char const* right_handle_pattern = "(?<var>(?:[A-Za-z]+|[А-Яа-я]+)+)";

#define REGULAR_PAT_LEFT_GROUP_NUM 1
#define REGULAR_PAT_RIGHT_GROUP_NUM 2
#define RIGHT_HANDLE_PAT_GROUP_NUM 1

namespace tproc {

//static const

static int currentRule = 0;

GrammarParser::GrammarParser() {
    // TODO Auto-generated constructor stub


    UErrorCode status = U_ZERO_ERROR;
    this->m_RegularRuleMatcher = new icu::RegexMatcher(regular_rule_pattern, 0, status);

    if (U_FAILURE(status)) {
        // do something
        throw GrammarParserException("Failed to create regular rule macther");
    }

    this->m_RightHandleMatcher = new icu::RegexMatcher(right_handle_pattern, 0, status);
    if (U_FAILURE(status)) {
        delete m_RegularRuleMatcher;
        throw GrammarParserException("Failed to create right handle rule macther");
    }

//    this->m_StartRuleMatcher = new icu::RegexMatcher(start_rule_pattern, 0, status);
//    if (U_FAILURE(status)) {
//        delete m_RegularRuleMatcher;
//        delete m_RightHandleMatcher;
//        throw GrammarParserException("Failed to create start rule matcher");
//    }
}

GrammarParser::~GrammarParser() {
    delete m_RegularRuleMatcher;
    delete m_RightHandleMatcher;
//    delete m_StartRuleMatcher;
}

bool GrammarParser::beginParseFromFile(const char *filename) {
    std::ifstream ifs;
    //    std::string utf8_filename = stdStringFromUnistr(filename);
    ifs.open(filename);

    if (ifs.fail()) {
        return false;
    }

    std::string next_line;
    while (std::getline(ifs, next_line)) {
        this->m_GrammarLines.emplace_back(next_line.c_str());
    }

    currentRule = 0;

    return true;
}

bool GrammarParser::beginParseFromPlainText(const icu::UnicodeString &grammar) {
    //	m_Splitter->reset(grammar);
//    auto capacity = grammar.length();
//    UnicodeString unicode_lines[capacity];

//    UErrorCode status = U_ZERO_ERROR;
//    auto n_of_lines = m_Splitter->split(grammar, unicode_lines, capacity, status);
//    if (U_FAILURE(status)) {
//        return false;
//    }
//    for (int i = 0; i < n_of_lines; ++i) {
//        this->m_GrammarLines.push_back(std::move(unicode_lines[i]));
//    }

    if (!split_unistring(grammar, {"\n"}, m_GrammarLines)) {
        return false;
    }

    currentRule = 0;
    return true;
}

bool GrammarParser::getNextRule(ComplexGrammarRule &rule) {
//    static int currentRule = 0;

    if (currentRule >= m_GrammarLines.size()) {
        return false;
    }

    UnicodeString ruleLeftPart;
    std::vector<SimpleGrammarRule> rulesForLeftPart;

    UnicodeString grammarRuleString = m_GrammarLines[currentRule++];
    m_RegularRuleMatcher->reset(grammarRuleString);
    while (m_RegularRuleMatcher->find()) {
//        SimpleGrammarRule simpleRule;

        auto groupsCount = m_RegularRuleMatcher->groupCount();
        if (REGULAR_PAT_LEFT_GROUP_NUM >= groupsCount || REGULAR_PAT_LEFT_GROUP_NUM >= groupsCount) {
            Logger::getErrLogger() << "Invalid number of groups";
            return false;
        }

        UErrorCode status = U_ZERO_ERROR;
        UnicodeString leftPart = m_RegularRuleMatcher->group(REGULAR_PAT_LEFT_GROUP_NUM, status);
        if (U_FAILURE(status)) {
//            LogStream::printStr("Failed to get left part of rule");
            Logger::getErrLogger() << "Failed to get left part of rule";
            return false;
        }
//        simpleRule.leftPart = leftPart;
        ruleLeftPart = leftPart;

        UnicodeString rightPart = m_RegularRuleMatcher->group(REGULAR_PAT_RIGHT_GROUP_NUM, status);
        if (U_FAILURE(status)) {
//            Logger::printStr("Failed to get right part of rule");
            Logger::getErrLogger() << "Failed to get right part of rule";
            return false;
        }

        // Parsing right part
//        m_RightHandleMatcher->reset(rightPart);
//        while (m_RightHandleMatcher->find()) {
//            UnicodeString groupVarRes = m_RightHandleMatcher->group(1, status);
//            UnicodeString groupLitRes = m_RightHandleMatcher->group(2, status);
//            if (!groupVarRes.isEmpty()) {
////                Logger::getLogger() << "Found variable in grammar: " << groupVarRes << std::endl;
//                std::vector<UnicodeString> words;
//                split_unistring(groupVarRes, {"\\s"}, words);
//                SimpleGrammarRule currentRule {leftPart, words, true};
//                Logger::getLogger() << "Current parsed simple rule is " << currentRule << std::endl;
//                rulesForLeftPart.push_back(SimpleGrammarRule{leftPart, words, true});
//            }
//            else if (!groupLitRes.isEmpty()) {
////                Logger::getLogger() << "Found literal in grammar: " << groupLitRes << std::endl;
//                std::vector<UnicodeString> words;
//                split_unistring(groupLitRes, {"\\s"}, words);
//                rulesForLeftPart.push_back(SimpleGrammarRule {leftPart, words, true});
//            }
//        }

        std::vector<UnicodeString> rightPartRules;
        split_unistring(rightPart, {"\\|"}, rightPartRules);
        for (auto &rawRule : rightPartRules) {
            std::vector<UnicodeString> ruleWords;
            split_unistring(rawRule, {"\\s","\\'","\\\""}, ruleWords);
//            rulesForLeftPart.push_back(SimpleGrammarRule {leftPart, })
            if (ruleWords.size() > 0) {
                SimpleGrammarRule rule {leftPart, ruleWords, true};
//                Logger::getLogger() << "Current parsed rule is " << rule << std::endl;
                Logger::getLogger() << "Current parsed rule is " << rule << std::endl;
                rulesForLeftPart.push_back(std::move(rule));
            }
        }
    }

    rule.leftPart = std::move(ruleLeftPart);
    rule.rightHandles = std::move(rulesForLeftPart);

    return true;

}

} /* namespace tproc */
