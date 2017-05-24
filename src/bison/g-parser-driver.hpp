#ifndef G_PARSER_H_
#define G_PARSER_H_

#include <string>
#include <cstddef>
#include <istream>
#include <unicode/unistr.h>
#include <memory>
//#include <queue>
#include <set>

#include "grammar-rule.h"
#include "action.hpp"

namespace tproc {

class GParser;
class GScanner;

//enum class PropertyType {
//    PROP_QUOTED,
//    PROP_START_UPPER,
//    PROP_MIN_REP,
//    PROP_MAX_REP
//};

//struct Property {

//    virtual ~Property() {}
//};

//struct SimpleProperty: public Property {
//    SimpleProperty(GRuleWordPropType propType): propType { propType } {}
//    GRuleWordPropType propType;
//};

//struct ComplexNumProperty: public Property {
//    ComplexNumProperty(GRuleWordPropType propType, int propValue): propType { propType }, propValue { propValue } {}
//    GRuleWordPropType propType;
//    int propValue;
//};

struct DependencyRule;
using DependencyRulePtr = std::shared_ptr<DependencyRule>;
struct DependencyRule {
    DependencyRule() = default;
    DependencyRule(const GRuleWordPtr &root): root { root } {}
    GRuleWordPtr root;
    std::set<GRuleWordPtr> nterms;
    std::set<GRuleWordPtr> terms;
    std::vector<UnicodeString> hintWords;
    std::set<DependencyRulePtr> backwardDeps;
    std::set<DependencyRulePtr> forwardDeps;
};

enum class DependencyType {
    FAR, NEAR
};

struct DependencyStruct {
    DependencyStruct() = default;
    DependencyStruct(const GRuleWordPtr &target, DependencyType depType):
        target { target }, depType { depType } {}
    DependencyStruct(const GRuleWordPtr &target, std::vector<GRuleWordPtr> &&symbols, DependencyType depType):
        target { target }, depSymbols { std::move(symbols)}, depType { depType } {}
//    DependencyStruct(std::vector<UnicodeString> &&symbols, DependencyType depType) :
//        depSymbols { std::move(symbols) }, depType { depType } {}

    GRuleWordPtr target;
    std::vector<GRuleWordPtr> depSymbols;
    DependencyType depType;

    void appendSymbol(const GRuleWordPtr &rule) { depSymbols.push_back(rule); }
    void swap(DependencyStruct &other) {
        target.swap(other.target);
        depSymbols.swap(other.depSymbols);
        std::swap(depType, other.depType);
    }
};

class GParserDriver {
public:
    GParserDriver() = default;
    virtual ~GParserDriver() = default;

//    std::vector<ComplexGrammarRule> &getRules() { return this->rules; }
    std::vector<ActionPtr> &getPendingActions() { return this->pendingActions; }
    std::set<GRuleWordPtr> getDefinedNterms() { return definedNterms; }
    GRuleWordPtr getRootNterm() {
        if (rootCandidates.size() == 0 || rootCandidates.size() > 1)
            return nullptr;
        else
            return (*rootCandidates.begin());
    }
    std::set<GRuleWordPtr> getTerminals() { return terminals; }
    bool hasPendingNterms() { return pendingNterms.size() > 0;}
    std::set<DependencyRulePtr> getTargetRules() { return targetRules; }

    UnicodeString &getCurrentNterm() { return this->currentNterm; }
    int &getCurrentSimpleRuleNum() { return this->currentSimpleRuleNum; }
    int &getCurrentRuleWordNum() { return currentRuleWordNum; }
//    std::vector<ActionPtr> &getActionsForRule() { return actionsForRule; }

//    void incRuleWordIndex() { ++currentRuleWordNum; }
//    void incSimpleRuleIndex() { ++currentSimpleRuleNum; }

//    void onRuleWordReduce() { incRuleWordIndex(); }
//    void onSimpleRuleReduce() { currentRuleWordNum = 0; incSimpleRuleIndex(); }
    void onComplexRuleReduce(const GRuleWordPtr &nterm);

    bool parse( const char * const filename );
    bool parse( std::istream &iss );
    bool parse(const std::string &fname);

    void appendRule(const GRuleWordPtr &nterm);
    void fixParentInfo(/*const std::vector<GRuleWordPtr> &words,*/ const GRuleWordPtr &parent/*, int baseRuleIndex*/);
    void fixAndSaveActionList(const GRuleWordPtr &actionWord, std::vector<ActionPtr> &&actions);
    GRuleWordPtr createRule(UnicodeString &&word, std::vector<GRuleWordPtr> &&wordChain);

    GRuleWordPtr handleNtermReduction(UnicodeString &&rawValue);
    GRuleWordPtr handleTermReduction(UnicodeString &&rawValue);
    GRuleWordPtr handleTermReduction(UnicodeString &&rawValue, std::vector<PredicatePtr> &&predicates);
    GRuleWordPtr handleNumTermReduction();
    GRuleWordPtr handleNumTermReduction(std::vector<PredicatePtr> &&predicates);

    void handleDependencies(std::vector<DependencyStruct> &storage);
//    DependencyStruct createDependencyStruct(UnicodeString &&target, UnicodeString &&fSym, UnicodeString &&secSym);
    bool createDependencyStruct(const UnicodeString &target,
                               const UnicodeString &firstSym,
                               const UnicodeString &secondSym,
                               DependencyType depType,
                               DependencyStruct &outStruct,
                               std::string &errMessage);
    bool appendDependencyStruct(const UnicodeString &symbol,
                                DependencyStruct &outStruct,
                                std::string &errMessage);

    bool handleCommandFindReduction(UnicodeString &rawWord, DependencyRulePtr &result, std::string &errMsg);
    void processCommandList(std::vector<DependencyRulePtr> &&commandList);

    DependencyRulePtr handleHintWords(DependencyRulePtr &depRule, std::vector<UnicodeString> &&hintWords) {
        depRule->hintWords = std::move(hintWords);
        return depRule;
    }

    void applyPendingActions();

//    template<class ActionType, class ...Args>
//    std::shared_ptr<ActionType> buildAction(Args&&...args) {
//        WordInfo wordInfo { currentNterm, currentSimpleRuleNum, currentRuleWordNum };
//        return std::make_shared<ActionType>(std::move(wordInfo), &args...);
////        pendingActions.push(std::move(rule));
//    }

private:

    UnicodeString currentNterm;
    int currentSimpleRuleNum = 0;
    int currentRuleWordNum = 0;

    bool parseHelper(std::istream &iss);

//    std::vector<ComplexGrammarRule> rules;
    std::vector<ActionPtr> pendingActions;

    // fields to find
    std::set<DependencyRulePtr> targetRules;
    std::set<DependencyRulePtr> definedDepRules;
    std::set<DependencyRulePtr> pendingDepRules;

    std::shared_ptr<GParser> parser { nullptr };
    std::shared_ptr<GScanner> scanner { nullptr };

    std::set<GRuleWordPtr> pendingNterms; //nterms, which do not have yet definitions
    std::set<GRuleWordPtr> definedNterms; //list of nterms which are already defined
    std::set<GRuleWordPtr> rootCandidates; //possible root nterms
    std::set<GRuleWordPtr> terminals;
};

}

#endif //G_PARSER_H_
