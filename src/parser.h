#ifndef PARSER_H_
#define PARSER_H_

#include <queue>

#include "tokenizer.h"
#include "grammar.h"
#include "parser-table.h"
#include "grammar-rule.h"
#include "stack.h"

namespace tproc {

struct ShiftInfo {
    ShiftInfo(const GSSNodePtr &node, int targetState, const UnicodeString &word) :
        node {node}, targetState {targetState}, word {word} {}
    GSSNodePtr node;
    int targetState;
    UnicodeString word;
    friend std::ostream &operator<<(std::ostream &os, const ShiftInfo &shift);
};

struct ReduceInfo {
    ReduceInfo(const GSSNodePtr &startNode, const GSSNodePtr &endNode, const RuleIndex &ruleIndex) :
        startNode {startNode}, endNode {endNode}, ruleIndex {ruleIndex} {}
    GSSNodePtr startNode;
    GSSNodePtr endNode;
    RuleIndex ruleIndex;
    friend std::ostream &operator<<(std::ostream &os, const ReduceInfo &reduce);
};

class Parser {
public:
    using ActiveSet = std::set<GSSNodePtr>;
    using ReduceSet = std::vector<ReduceInfo>;
    using ShiftSet = std::vector<ShiftInfo>;

    Parser(const Grammar &grammar, const ParserTable &parserTable) : grammar {grammar}, parserTable {parserTable} {}
    bool tryParse(const Tokenizer::Sentence &sentence, std::vector<std::pair<UnicodeString,int>> &result);
    Grammar &getGrammar() { return grammar; }
private:
    ActiveSet parseToken(const Token &token, ActiveSet &currentLevelNodes, bool &accepted);
    void actor(const Token &token, ActiveSet &activeNodes, ReduceSet &reduceSet, ShiftSet &shiftSet, bool &isAccepted);
    void reducer(const Token &toke, ActiveSet &activeNodes, ActiveSet &currentLevelNodes, ReduceSet &reduceSet);
    void shifter(ActiveSet &nextStateSet, ShiftSet &shiftSet);
    std::map<UnicodeString, ParserTable::ParserActionSet> getActionSetForTokenWithTokenInfo(const Token &token, int state);
    ParserTable::ParserActionSet getActionSetForToken(const Token &token, int state);

    Grammar grammar;
    ParserTable parserTable;
};

}

#endif

