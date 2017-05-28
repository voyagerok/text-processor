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

struct DependencyGrammar;
//using DependencyRulePtr = std::shared_ptr<DependencyRule>;

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

//struct ParserDepenndenciesStruct {
//    std::vector<ParserPtr> leftDeps;
//    std::vector<ParserPtr> rightDeps;
//    std::vector<ParserPtr> upperDeps;
//    std::vector<ParserPtr> lowerDeps;
//};

using ParserPtr = std::shared_ptr<Parser>;
using ParserDepStorage = DependencyStorage<ParserPtr>;
struct ParserWrapper {
    ParserPtr parser;
    std::vector<UnicodeString> hintWords;
    UnicodeString name;
    ParserDepStorage dependencies;
};

struct FieldIndex {
    int sentence;
    int position;
};

class FieldsExtractor {
public:
//    FieldsExtractor(const char *grammarFilename);
    FieldsExtractor(const std::string &grammarFilename);
//    std::vector<FieldInfo> extractFromFile(const char *fileName);
    std::map<UnicodeString, std::vector<FieldInfo>> extractFromFile(const std::string &fileName);
private:
    void processDependencyGrammars(const std::vector<DependencyGrammar> &depRules);
    void processRulesWithoutDependencies(const std::vector<DependencyGrammar> &depRules);
    std::map<UnicodeString, std::vector<FieldInfo>> extract(const UnicodeString &plainText);
    std::vector<ParserWrapper> definedParserWrappers;
};

}

#endif //FIELDS_EXTRACTOR_HPP_
