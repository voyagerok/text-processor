#include <sstream>
#include <algorithm>
#include <fstream>
#include <queue>
#include <map>
#include <unicode/ustream.h>
#include <cmath>

#include "fields-extractor.hpp"
#include "grammar.h"
#include "g-parser-driver.hpp"
#include "utils/logger.h"
#include "tokenizer.h"
#include "parser-table.h"
#include "parser.h"


namespace tproc {

static bool readAllTextFromFile(const std::string &filename, UnicodeString &outText) {
    std::ifstream ifs(filename);
    if (ifs.fail()) {
        std::cerr << "Error: failed to open " << filename << std::endl;
        return false;
    }
    std::string text {std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};

    outText = UnicodeString(text.c_str());

    return true;
}

//FieldsExtractor::FieldsExtractor(const char *grammarFilename) {
//    GParserDriver parserDriver;
//    if (parserDriver.parse(grammarFilename)) {
////        processDependencyRules(parserDriver.getTargetRules());
//        processRulesWithoutDependencies(parserDriver.getTargetRules());
//    } else {
//        std::ostringstream os;
//        os << "Failed to parse file named " << grammarFilename;
//        throw std::runtime_error(os.str());
//    }
//}

FieldsExtractor::FieldsExtractor(const std::string &grammarFilename) {
    GParserDriver parserDriver;
    if (parserDriver.parse(grammarFilename)) {
//        processDependencyRules(parserDriver.getTargetRules());
//        processRulesWithoutDependencies(parserDriver.getTargetGrammars());
        processDependencyGrammars(parserDriver.getTargetGrammars());
    } else {
        std::ostringstream os;
        os << "Failed to parse file named " << grammarFilename;
        throw std::runtime_error(os.str());
    }
}

//std::vector<FieldInfo> FieldsExtractor::extractFromFile(const char *filename) {
//    UnicodeString plainText;
//    if (readAllTextFromFile(filename, plainText)) {
//        return extract(plainText);
//    } else {
//        std::cerr << "Failed to read file with text." << std::endl;
//        return {};
//    }
//}

std::map<UnicodeString, std::vector<FieldInfo>> FieldsExtractor::extractFromFile(const std::string &filename) {
    UnicodeString plainText;
    if (readAllTextFromFile(filename, plainText)) {
        return extract(plainText);
    } else {
        std::cerr << "Failed to read file with text." << std::endl;
        return {};
    }
}

static double calcHeuristics(const std::vector<int> &hintWordPositions, int parserResultPosition) {
    double result = 0.0;
    for (auto pos : hintWordPositions) {
        result += 1 / (double)std::abs(parserResultPosition - pos);
    }
    return result;
}

static std::vector<int> calcHintWordsPositions(const std::vector<UnicodeString> &hintWords, const Tokenizer::Sentence &sentence) {
    std::vector<int> result;
    for (auto &hintWord : hintWords) {
        for (auto it = sentence.begin(); it != sentence.end(); ++it) {
            if (it->pureTokenNormal == hintWord) {
                result.push_back(it - sentence.begin());
            }
        }
    }

    return result;
}

//static void checkDependencyList(const Tokenizer::Sentence &sentence,
//                                int sentenceIndex,
//                                const std::vector<ParserPtr> &depList,
//                                std::unordered_map<ParserPtr, std::vector<FieldIndex>> &foundParsers)
//{
//    for (auto &dep : depList) {
//        std::vector<std::pair<UnicodeString, int>> result;
//        if (dep->tryParse(sentence, result)) {
//            for (auto &parseResult : result) {
//                foundParsers[dep].push_back({sentenceIndex, parseResult.second});
//            }
//        }
//    }
//}

//static void checkDependencies(const Tokenizer::Sentence &sentence,
//                              int sentenceIndex,
//                              const ParserDepStorage &depStorage,
//                              std::unordered_map<ParserPtr, std::vector<FieldIndex>> &foundParsers) {
//    checkDependencyList(sentence, sentenceIndex, depStorage.leftDeps, foundParsers);
//    checkDependencyList(sentence, sentenceIndex, depStorage.rightDeps, foundParsers);
//    checkDependencyList(sentence, sentenceIndex, depStorage.upperDeps, foundParsers);
//    checkDependencyList(sentence, sentenceIndex, depStorage.lowerDeps, foundParsers);
//}

using ExtractionResultsQueue = std::priority_queue<FieldInfo, std::vector<FieldInfo>, FieldInfoComparator>;
using PendingResultsContainer = std::map<UnicodeString, std::vector<std::pair<FieldInfo, std::vector<ParserPtr>>>>;
static void checkPendingResults(PendingResultsContainer &pendingResults,
                                std::unordered_set<ParserPtr> &foundParsers,
                                const Tokenizer::Sentence &sentence,
                                std::map<UnicodeString, ExtractionResultsQueue> &extractionResultsQueue) {
    for (auto &pendingResult : pendingResults) {
        for (auto &pendingResultForName : pendingResult.second) {
            for (auto &dep : pendingResultForName.second) {
                if (foundParsers.find(dep) != foundParsers.end()) {
                    extractionResultsQueue[pendingResult.first].push(pendingResultForName.first);
                    continue;
                }
                std::vector<std::pair<UnicodeString, int>> result;
                if (dep->tryParse(sentence, result)) {
                    if (result.size()) {
                        foundParsers.insert(dep);
                        extractionResultsQueue[pendingResult.first].push(pendingResultForName.first);
                    }
                }
            }
        }
    }
}


static bool checkLeftDependencies(const Tokenizer::Sentence &sentence,
                              int index,
                              std::vector<ParserPtr> &leftDeps,
                              std::unordered_set<ParserPtr> &foundParsers)
{
    for (auto &leftDep : leftDeps) {
//        if (foundParsers.find(leftDep) == foundParsers.end()) {
            std::vector<std::pair<UnicodeString, int>> result;
            Logger::getLogger() << "Checking dependency: " << leftDep->getGrammar()->getRoot()->getChildWords().at(0).at(0)->getRawValue() << std::endl;
            if (!leftDep->tryParse(sentence, result)) {
                return  false;
            }
            if (result.size() == 0) {
                return false;
            }
            foundParsers.insert(leftDep);
            for (auto &parserResult : result) {
                if (parserResult.second >= index) {
                    return false;
                }
            }
//        }
    }
    return true;
}

static bool checkRightDependencies(const Tokenizer::Sentence &sentence,
                                   int index,
                                   std::vector<ParserPtr> &rightDeps,
                                   std::unordered_set<ParserPtr> &foundParsers)
{
    for (auto &rightDep : rightDeps) {
        std::vector<std::pair<UnicodeString, int>> result;
        if (!rightDep->tryParse(sentence, result)) {
            return false;
        }
        if (result.size() == 0) {
            return false;
        }
        foundParsers.insert(rightDep);
        for (auto &parserResult : result) {
            if (parserResult.second <= index) {
                return false;
            }
        }
    }
    return true;
}

static bool checkUpperDependencies(const Tokenizer::Sentence &sentence,
                                   std::vector<ParserPtr> &depList,
                                   std::unordered_set<ParserPtr> &foundParsers)
{
    bool checked = true;
    for (auto &upperDep : depList) {
        Logger::getLogger() << "checkUpperDependencies: " << upperDep->getGrammar()->getRoot()->getChildWords().at(0).at(0)->getRawValue() << std::endl;
        if (foundParsers.find(upperDep) == foundParsers.end()) {
            checked = false;
            std::vector<std::pair<UnicodeString, int>> result;
            if (upperDep->tryParse(sentence, result)) {
                if (result.size() > 0) {
                    foundParsers.insert(upperDep);
                }
            }
        }
    }

    return checked;
}

std::map<UnicodeString, std::vector<FieldInfo>> FieldsExtractor::extract(const UnicodeString &plainText) {
    std::unordered_set<ParserPtr> foundParsers;
    PendingResultsContainer pendingResults;

    using ExtractionResultsQueue = std::priority_queue<FieldInfo, std::vector<FieldInfo>, FieldInfoComparator>;
    std::map<UnicodeString, ExtractionResultsQueue> extractionResultsQueue;
    Tokenizer tokenizer { plainText };
    auto sentences = tokenizer.getSentences();
    for (int i = 0; i < sentences.size(); ++i) {
        auto &sentence = sentences[i];
        checkPendingResults(pendingResults, foundParsers, sentence, extractionResultsQueue);
        for (auto &parserWrapper : definedParserWrappers) {
//            checkDependencies(sentence, i, parserWrapper.dependencies, foundParsers);
            bool upperDepsCheckResult = checkUpperDependencies(sentence, parserWrapper.dependencies.upperDeps, foundParsers);
            std::vector<std::pair<UnicodeString, int>> result;
            if (parserWrapper.parser->tryParse(sentence, result)) {
                if (result.size() > 0) {
                    auto hintWordsPositions = calcHintWordsPositions(parserWrapper.hintWords, sentence);
                    for (auto &resultRecord : result) {
//                        if (checkLeftDependencies(parserWrapper.dependencies.leftDeps, foundParsers, resultRecord.second, sentence)) {
                        if (upperDepsCheckResult && checkLeftDependencies(sentence, resultRecord.second, parserWrapper.dependencies.leftDeps, foundParsers))
                        {
                            double heuristics = calcHeuristics(hintWordsPositions, resultRecord.second);
//                            std::cout << "Inserting results in queue: " << parserWrapper.name << ", " << resultRecord.first <<
//                                         ", " << heuristics << std::endl;
//                            extractionResultsQueue[parserWrapper.name].emplace(resultRecord.first, heuristics);
                            foundParsers.insert(parserWrapper.parser);
//                            if (!checkRightDependencies(sentence, resultRecord.second, parserWrapper.dependencies.rightDeps, foundParsers)) {
//                                pendingResults[parserWrapper.name].push_back(std::make_pair(FieldInfo {resultRecord.second, heuristics}, parserWrapper.dependencies.rightDeps));
                            if (checkRightDependencies(sentence, resultRecord.second, parserWrapper.dependencies.rightDeps, foundParsers)) {
                                if (parserWrapper.dependencies.lowerDeps.size() > 0) {
                                    pendingResults[parserWrapper.name].push_back(std::make_pair(FieldInfo {resultRecord.second, heuristics}, parserWrapper.dependencies.rightDeps));
                                } else {
                                    extractionResultsQueue[parserWrapper.name].emplace(resultRecord.first, heuristics);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    std::map<UnicodeString, std::vector<FieldInfo>> extractionResult;
    for (auto &queue : extractionResultsQueue) {
        std::vector<FieldInfo> results;
        double heuristics;
        do {
            heuristics = queue.second.top().heuristics;
            results.push_back(queue.second.top());
            queue.second.pop();
        } while (queue.second.size() && std::fabs(heuristics - queue.second.top().heuristics) <= 0.001);
        extractionResult.insert(std::make_pair(queue.first, results));
    }
    return extractionResult;
}

static ParserPtr buildParserFromDepGrammar(const std::shared_ptr<Grammar> &grammar, const std::unordered_map<std::shared_ptr<Grammar>, ParserPtr> definedParsers) {
    auto parserFound = definedParsers.find(grammar);
    if (parserFound != definedParsers.end()) {
        return parserFound->second;
    }
    ParserTable pTable;
    pTable.buildTableFromGrammar(*grammar);
    pTable.printActionTable();
    pTable.printGotoTable();
    return std::make_shared<Parser>(grammar, pTable);
}

enum class DependencyType { LEFT, RIGHT, UPPER, LOWER };

static void processDependencyList(ParserWrapper &parserWrapper,
                                  const std::vector<std::shared_ptr<Grammar>> &depList,
                                  DependencyType depType,
                                  const std::unordered_map<std::shared_ptr<Grammar>, ParserPtr> definedParsers)
{
    std::vector<ParserPtr> *depParserList = nullptr;
    switch (depType) {
    case DependencyType::LEFT:
        depParserList = &parserWrapper.dependencies.leftDeps;
        break;
    case DependencyType::RIGHT:
        depParserList = &parserWrapper.dependencies.rightDeps;
        break;
    case DependencyType::UPPER:
        depParserList = &parserWrapper.dependencies.upperDeps;
        break;
    case DependencyType::LOWER:
        depParserList = &parserWrapper.dependencies.lowerDeps;
        break;
    default:
        Logger::getErrLogger() << "processDependencyList(): invalid dependency type" << std::endl;
        return;
    }
    for (auto &grammarDep : depList) {
        depParserList->push_back(buildParserFromDepGrammar(grammarDep, definedParsers));
    }
}

void FieldsExtractor::processRulesWithoutDependencies(const std::vector<DependencyGrammar> &depRules) {
//    std::unordered_map<std::shared_ptr<Grammar>, ParserPtr> definedParsers;
//    for (auto &depRule : depRules) {
//        ParserWrapper parserWrapper;
//        parserWrapper.parser = buildParserFromDepGrammar(depRule.grammar, definedParsers);
//        parserWrapper.hintWords = std::move(depRule.hintWords);
//        parserWrapper.name = std::move(depRule.alias);
//        processDependencyList(parserWrapper, depRule.dependencies.leftDeps, DependencyType::LEFT, definedParsers);
//        processDependencyList(parserWrapper, depRule.dependencies.rightDeps, DependencyType::RIGHT, definedParsers);
//        processDependencyList(parserWrapper, depRule.dependencies.upperDeps, DependencyType::UPPER, definedParsers);
//        processDependencyList(parserWrapper, depRule.dependencies.lowerDeps, DependencyType::LOWER, definedParsers);
//        definedParserWrappers.push_back(parserWrapper);
//    }
}

void FieldsExtractor::processDependencyGrammars(const std::vector<DependencyGrammar> &depRules) {
    std::unordered_map<std::shared_ptr<Grammar>, ParserPtr> definedParsers;
    for (auto &depRule : depRules) {
        ParserWrapper parserWrapper;
        parserWrapper.parser = buildParserFromDepGrammar(depRule.grammar, definedParsers);
        parserWrapper.hintWords = std::move(depRule.hintWords);
        parserWrapper.name = std::move(depRule.alias);
        processDependencyList(parserWrapper, depRule.dependencies.leftDeps, DependencyType::LEFT, definedParsers);
        processDependencyList(parserWrapper, depRule.dependencies.rightDeps, DependencyType::RIGHT, definedParsers);
        processDependencyList(parserWrapper, depRule.dependencies.upperDeps, DependencyType::UPPER, definedParsers);
        processDependencyList(parserWrapper, depRule.dependencies.lowerDeps, DependencyType::LOWER, definedParsers);
        definedParserWrappers.push_back(parserWrapper);
    }
}

}

