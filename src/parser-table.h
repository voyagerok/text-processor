#ifndef PARSER_TABLE_H_
#define PARSER_TABLE_H_

#include <unicode/unistr.h>
#include <map>
#include <vector>
#include <set>

#include "grammar.h"

namespace tproc {

//class Grammar;
//struct RuleIndex;

enum class ActionName {SHIFT, REDUCE, ACCEPT, ERROR};

struct ParserAction {
    ActionName actionName;
};

struct ShiftAction : public ParserAction {
    ShiftAction(int nextState) : ParserAction {ActionName::SHIFT}, nextState {nextState} {}
    int nextState;
};

struct ReduceAction : public ParserAction {
    ReduceAction(RuleIndex ruleIndex) : ParserAction {ActionName::REDUCE}, ruleIndex {ruleIndex} {}
    RuleIndex ruleIndex;
};

class ParserTable {
public:
    using ParserActionSet = std::vector<ParserAction>;
    using ActionTable = std::vector<std::map<UnicodeString, ParserActionSet>>;
    using GotoTable = std::vector<std::map<UnicodeString, int>>;

    ~ParserTable();

    bool buildTableFromGrammar(const Grammar &grammar);
private:
    void addNewAction(int currentState, const UnicodeString &currentWord, ParserAction action);
//    std::set<UnicodeString> followSetForWord(const Grammar &grammar, const UnicodeString &word);
//    std::set<UnicodeString> firstSetForWord(const Grammar &grammar, const UnicodeString &word);

    ActionTable *actionTable {nullptr};
    GotoTable *gotoTable {nullptr};
};

}

#endif //PARSER_TABLE_H_

