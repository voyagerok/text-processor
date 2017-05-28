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

template<class T>
struct DependencyStorage {
    std::vector<T> leftDeps;
    std::vector<T> rightDeps;
    std::vector<T> upperDeps;
    std::vector<T> lowerDeps;
};

using GrammarDepStorage = DependencyStorage<std::shared_ptr<Grammar>>;

struct DependencyGrammar {
    DependencyGrammar() = default;
    DependencyGrammar(const GRuleWordPtr &root);
//    GRuleWordPtr root;
    std::shared_ptr<Grammar> grammar;
    UnicodeString alias;
//    std::set<GRuleWordPtr> nterms;
//    std::set<GRuleWordPtr> terms;
    std::vector<UnicodeString> hintWords;
    GrammarDepStorage dependencies;
    void swap(DependencyGrammar &other) { std::swap(*this, other); }
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
    std::vector<DependencyGrammar> getTargetGrammars() { return targetGrammars; }

    UnicodeString &getCurrentNterm() { return this->currentNterm; }
    int &getCurrentSimpleRuleNum() { return this->currentSimpleRuleNum; }
    int &getCurrentRuleWordNum() { return currentRuleWordNum; }

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

    bool handleCommandFindReduction(GRuleWordPtr &rawWord, DependencyGrammar &result, std::string &errMsg);
    void processCommandList(std::vector<DependencyGrammar> &&commandList);

    void processAlias(DependencyGrammar &depRule, UnicodeString &alias) {
        if (!alias.isEmpty()) {
            depRule.alias = alias;
        }
    }

    void handleHintWordsAndAlias(DependencyGrammar &depRule, std::vector<UnicodeString> &&hintWords, UnicodeString &alias) {
        depRule.hintWords = std::move(hintWords);
//        depRule.alias = alias.isEmpty() ? depRule.root->getRawValue() : alias;
        if (!alias.isEmpty()) {
            depRule.alias = alias;
        }
    }

    bool handleDependencyReduction(std::vector<std::shared_ptr<Grammar>> &depList, GRuleWordPtr &dep, std::string &errMsg);
    void processDependencies(DependencyGrammar &depRule, GrammarDepStorage &&deps);

    void applyPendingActions();

private:

    UnicodeString currentNterm;
    int currentSimpleRuleNum = 0;
    int currentRuleWordNum = 0;

    bool parseHelper(std::istream &iss);

//    std::vector<ComplexGrammarRule> rules;
    std::vector<ActionPtr> pendingActions;

    // fields to find
    std::vector<DependencyGrammar> targetGrammars;
    std::unordered_map<GRuleWordPtr, std::shared_ptr<Grammar>> definedGrammars;

    std::shared_ptr<GParser> parser { nullptr };
    std::shared_ptr<GScanner> scanner { nullptr };

    std::set<GRuleWordPtr> pendingNterms; //nterms, which do not have yet definitions
    std::set<GRuleWordPtr> definedNterms; //list of nterms which are already defined
    std::set<GRuleWordPtr> rootCandidates; //possible root nterms
    std::set<GRuleWordPtr> terminals;
};

}

#endif //G_PARSER_H_
