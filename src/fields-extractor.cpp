#include <sstream>
#include <algorithm>
#include <fstream>

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

std::vector<FieldInfo> FieldsExtractor::extractFromFile(const std::string &filename) {
    UnicodeString plainText;
    if (readAllTextFromFile(filename, plainText)) {
        return extract(plainText);
    } else {
        std::cerr << "Failed to read file with text." << std::endl;
        return {};
    }
}

std::vector<FieldInfo> FieldsExtractor::extract(const UnicodeString &plainText) {
    std::vector<FieldInfo> extractionResult;
    Tokenizer tokenizer { plainText };
    auto sentences = tokenizer.getSentences();
    for (auto &sentence : sentences) {
        for (auto &parserWrapper : definedParsers) {
            std::vector<std::pair<UnicodeString, int>> result;
            if (parserWrapper.parser.tryParse(sentence, result)) {
                if (result.size() > 0) {
                    UnicodeString &lastResult = result[result.size() - 1].first;
                    bool passed = true;
                    for (auto &hintWord : parserWrapper.hintWords) {
                        auto hintWordFound = std::find_if(sentence.begin(),sentence.end(),[&hintWord](const Token &token){
                            return token.word == hintWord || token.normalForm == hintWord;
                        });
                        if (hintWordFound == sentence.end()) {
                            passed = false;
                        }
                    }
                    if (passed) {
                        extractionResult.push_back({parserWrapper.parser.getGrammar().getRoot()->getRawValue(), lastResult});
                    }
                }
            }
        }
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
        definedParsers.push_back({parser, depRule->hintWords});
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

