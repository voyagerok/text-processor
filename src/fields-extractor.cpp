#include <sstream>
#include <algorithm>
#include <fstream>
#include <queue>
#include <map>
#include <unicode/ustream.h>

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
        processRulesWithoutDependencies(parserDriver.getTargetRules());
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

std::map<UnicodeString, FieldInfo> FieldsExtractor::extractFromFile(const std::string &filename) {
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
            if (it->pureToken == hintWord) {
                result.push_back(it - sentence.begin());
            }
        }
    }

    return result;
}

std::map<UnicodeString,FieldInfo> FieldsExtractor::extract(const UnicodeString &plainText) {
    using ExtractionResultsQueue = std::priority_queue<FieldInfo, std::vector<FieldInfo>, FieldInfoComparator>;
    std::map<UnicodeString, ExtractionResultsQueue> extractionResultsQueue;
    Tokenizer tokenizer { plainText };
    auto sentences = tokenizer.getSentences();
    for (auto &sentence : sentences) {
        for (auto &parserWrapper : definedParsers) {
            std::vector<std::pair<UnicodeString, int>> result;
            if (parserWrapper.parser.tryParse(sentence, result)) {
                if (result.size() > 0) {
                    auto hintWordsPositions = calcHintWordsPositions(parserWrapper.hintWords, sentence);
                    for (auto &resultRecord : result) {
                        double heuristics = calcHeuristics(hintWordsPositions, resultRecord.second);
                        std::cout << "Inserting results in queue: " << parserWrapper.name << ", " << resultRecord.first <<
                                     ", " << heuristics << std::endl;
                        extractionResultsQueue[parserWrapper.name].emplace(resultRecord.first, heuristics);
                    }
                }
            }
        }
    }

    std::map<UnicodeString, FieldInfo> extractionResult;
    for (auto &queue : extractionResultsQueue) {
        extractionResult.insert(std::make_pair(queue.first, queue.second.top()));
    }
    return extractionResult;
}

void FieldsExtractor::processRulesWithoutDependencies(const std::set<DependencyRulePtr> &depRules) {
    std::set<std::shared_ptr<Grammar>> definedGrammars;
    for (auto &depRule : depRules) {
        auto grammarFound = std::find_if(definedGrammars.begin(), definedGrammars.end(), [&depRule](auto &grammarPtr){
            return grammarPtr->getRoot() == depRule->root;
        });
        if (grammarFound != definedGrammars.end()) {
            continue;
        }
        auto grammar = std::make_shared<Grammar>();
        grammar->initFromDependencyRule(depRule);
        ParserTable parserTable;
        parserTable.buildTableFromGrammar(*grammar);
        parserTable.printActionTable();
        parserTable.printGotoTable();
        Parser parser { *grammar, parserTable };
        definedParsers.push_back({parser, depRule->hintWords, depRule->root->getRawValue()});
    }
}

void FieldsExtractor::processDependencyRules(const std::set<DependencyRulePtr> &depRules) {
//    for (auto &depRule : depRules) {
//        DependencyGrammarObjectPtr depGrammar = nullptr;
//        auto defGrammarFound = std::find_if(definedDepGrammars.begin(), definedDepGrammars.end(), [&depRule](const DependencyGrammarObjectPtr &depGrammar){
//            return depGrammar->grammar->getRoot() == depRule->root;
//        });
//        if (defGrammarFound != definedDepGrammars.end()) {
//            Logger::getErrLogger() << "Miltiple definition for " << depRule->root->getRawValue() << std::endl;
//            continue;
//        }
//        auto pendGrammarFound = std::find_if(pendingDepGrammars.begin(), pendingDepGrammars.end(), [&depRule](const DependencyGrammarObjectPtr &depGrammar){
//            return depGrammar->grammar->getRoot() == depRule->root;
//        });
//        if (pendGrammarFound != pendingDepGrammars.end()) {
//            depGrammar = *pendGrammarFound;
//            pendingDepGrammars.erase(pendGrammarFound);
////            definedDepGrammars.insert(depGrammar);
//        }

//        if (!depGrammar) {
//            depGrammar = std::make_shared<DependencyParserObject>();
//            depGrammar->grammar = std::make_shared<Grammar>();
//            depGrammar->grammar->initFromDependencyRule(depRule);
//        }
//        definedDepGrammars.insert(depGrammar);

//        for (auto &forwardDep : depRule->forwardDeps) {
//            DependencyGrammarObjectPtr forwardDepGrammar = nullptr;
//            auto defFound = std::find_if(definedDepGrammars.begin(), definedDepGrammars.end(), [&forwardDep](const DependencyGrammarObjectPtr &depGrammar){
//                return depGrammar->grammar->getRoot() == forwardDep->root;
//            });
//            if (defFound != definedDepGrammars.end()) {
//                forwardDepGrammar = *defFound;
//            }

//            if (!forwardDepGrammar) {
//                auto pendFound = std::find_if(pendingDepGrammars.begin(), pendingDepGrammars.end(), [&forwardDep](const DependencyGrammarObjectPtr &depGrammar){
//                    return forwardDep->root == depGrammar->grammar->getRoot();
//                });
//                if (pendFound != pendingDepGrammars.end()) {
//                    forwardDepGrammar = *pendFound;
//                }
//            }

//            if (!forwardDepGrammar) {
//                forwardDepGrammar = std::make_shared<DependencyParserObject>();
//                forwardDepGrammar->grammar = std::make_shared<Grammar>();
//                forwardDepGrammar->grammar->initFromDependencyRule(forwardDep);
//                pendingDepGrammars.insert(forwardDepGrammar);
//            }

//            depGrammar->forwardDeps.push_back(forwardDepGrammar);
//        }

//        for (auto &backwardDep: depRule->backwardDeps) {
//            DependencyGrammarObjectPtr backwardDepGrammar = nullptr;
//            auto defFound = std::find_if(definedDepGrammars.begin(), definedDepGrammars.end(), [&backwardDep](const DependencyGrammarObjectPtr &depGrammar){
//                return depGrammar->grammar->getRoot() == backwardDep->root;
//            });
//            if (defFound != definedDepGrammars.end()) {
//                backwardDepGrammar = *defFound;
//            }

//            if (!backwardDepGrammar) {
//                auto pendFound = std::find_if(pendingDepGrammars.begin(), pendingDepGrammars.end(), [&backwardDep](const DependencyGrammarObjectPtr &depGrammar){
//                    return backwardDep->root == depGrammar->grammar->getRoot();
//                });
//                if (pendFound != pendingDepGrammars.end()) {
//                    backwardDepGrammar = *pendFound;
//                }
//            }

//            if (!backwardDepGrammar) {
//                backwardDepGrammar = std::make_shared<DependencyParserObject>();
//                backwardDepGrammar->grammar = std::make_shared<Grammar>();
//                backwardDepGrammar->grammar->initFromDependencyRule(backwardDep);
//                pendingDepGrammars.insert(backwardDepGrammar);
//            }

//            depGrammar->forwardDeps.push_back(backwardDepGrammar);
//        }
//    }
}

}

