#ifndef FIELDS_EXTRACTOR_HPP_
#define FIELDS_EXTRACTOR_HPP_

#include <string>
#include <vector>
#include <unicode/unistr.h>
#include <memory>
#include <set>
#include <map>

#include "parser.h"

namespace tproc {

class Grammar;

struct DependencyParserObject;
using DependencyGrammarObjectPtr = std::shared_ptr<DependencyParserObject>;

struct DependencyRule;
using DependencyRulePtr = std::shared_ptr<DependencyRule>;

struct FieldInfo {
    FieldInfo() = default;
    FieldInfo(const UnicodeString &value, const double heuristics) :
        fieldValue { value }, heuristics { heuristics } {}
    UnicodeString fieldValue;
    double heuristics;
};

struct FieldInfoComparator {
    bool operator()(const FieldInfo &lhs, const FieldInfo &rhs) const { return lhs.heuristics < rhs.heuristics; }
};

struct ParserWrapper {
    Parser parser;
    std::vector<UnicodeString> hintWords;
    UnicodeString name;
};

//struct DependencyParserObject {
//    GrammarPtr grammar;
//    std::vector<DependencyGrammarObjectPtr> forwardDeps;
//    std::vector<DependencyGrammarObjectPtr> backwardDeps;
//};

class FieldsExtractor {
public:
//    FieldsExtractor(const char *grammarFilename);
    FieldsExtractor(const std::string &grammarFilename);
//    std::vector<FieldInfo> extractFromFile(const char *fileName);
    std::map<UnicodeString, FieldInfo> extractFromFile(const std::string &fileName);
private:
    void processDependencyRules(const std::set<DependencyRulePtr> &depRules);
    void processRulesWithoutDependencies(const std::set<DependencyRulePtr> &depRules);
    std::map<UnicodeString, FieldInfo> extract(const UnicodeString &plainText);
    std::vector<ParserWrapper> definedParsers;
//    std::set<DependencyGrammarObjectPtr> definedDepGrammars;
//    std::set<DependencyGrammarObjectPtr> pendingDepGrammars;
//    std::set<DependencyGrammarObjectPtr> checkedDepGrammars;
};

}

#endif //FIELDS_EXTRACTOR_HPP_
