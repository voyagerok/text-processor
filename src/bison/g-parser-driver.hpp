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

struct Property {

    virtual ~Property() {}
};

struct SimpleProperty: public Property {
    SimpleProperty(GRuleWordPropType propType): propType { propType } {}
    GRuleWordPropType propType;
};

struct ComplexNumProperty: public Property {
    ComplexNumProperty(GRuleWordPropType propType, int propValue): propType { propType }, propValue { propValue } {}
    GRuleWordPropType propType;
    int propValue;
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

//    void appendRule(ComplexGrammarRule &rule);
    void appendRule(const GRuleWordPtr &nterm);
//    GRuleWordPtr makeRuleWord(UnicodeString &rawValue, std::vector<std::shared_ptr<Property>> &props);
//    void appendAction(ActionPtr &&action) { pendingActions.push(std::move(action)); }
    void fixParentInfo(/*const std::vector<GRuleWordPtr> &words,*/ const GRuleWordPtr &parent/*, int baseRuleIndex*/);

//    void appendActionsForRule(std::vector<ActionPtr> &actions) { actionsForRule.insert(actionsForRule.end(), actions.begin(), actions.end()); }
    void fixAndSaveActionList(const GRuleWordPtr &actionWord, std::vector<ActionPtr> &&actions);

    GRuleWordPtr handleNtermReduction(UnicodeString &&rawValue);
    GRuleWordPtr handleTermReduction(UnicodeString &&rawValue);
    GRuleWordPtr handleTermReduction(UnicodeString &&rawValue, std::vector<PredicatePtr> &&predicates);

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


    std::shared_ptr<GParser> parser { nullptr };
    std::shared_ptr<GScanner> scanner { nullptr };

    std::set<GRuleWordPtr> pendingNterms; //nterms, which do not have yet definitions
    std::set<GRuleWordPtr> definedNterms; //list of nterms which are already defined
    std::set<GRuleWordPtr> rootCandidates; //possible root nterms
    std::set<GRuleWordPtr> terminals;
};

}

#endif //G_PARSER_H_
