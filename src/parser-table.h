#ifndef PARSER_TABLE_H_
#define PARSER_TABLE_H_

#include <unicode/unistr.h>
#include <map>
#include <vector>
#include <set>
#include <iostream>
#include <memory>
#include <unordered_set>

#include "grammar.h"

namespace tproc {

//class Grammar;
//struct RuleIndex;

enum class ActionName {SHIFT, REDUCE, ACCEPT, ERROR};

std::ostream &operator<<(std::ostream &os, ActionName actionName);

class ParserAction {
public:
    ParserAction (ActionName actionName) : actionName {actionName} {}
    ActionName actionName;

    virtual bool equals(const ParserAction &other) const;
    virtual unsigned long hash() const;

    friend std::ostream &operator<<(std::ostream &os, const ParserAction &parserAction) { return parserAction.print(os); }
protected:
    virtual std::ostream &print(std::ostream &os) const;
};

class ShiftAction : public ParserAction {
public:
    ShiftAction(int nextState) : ParserAction {ActionName::SHIFT}, nextState {nextState} {}
    int nextState;

    bool equals(const ParserAction &other) const override;
    unsigned long hash() const override;

    friend std::ostream &operator<<(std::ostream &os, const ShiftAction &parserAction) { return parserAction.print(os); }
protected:
    std::ostream &print(std::ostream &os) const override;
};

class ReduceAction : public ParserAction {
public:
    ReduceAction(RuleIndex ruleIndex) : ParserAction {ActionName::REDUCE}, ruleIndex {ruleIndex} {}
    RuleIndex ruleIndex;

    bool equals(const ParserAction &other) const override;
    unsigned long hash() const override;

    friend std::ostream &operator<<(std::ostream &os, const ReduceAction &parserAction) { return parserAction.print(os); }
protected:
    std::ostream &print(std::ostream &os) const override;
};

class ParserActionEquality {
public:
    bool operator()(const std::shared_ptr<ParserAction> &act1, const std::shared_ptr<ParserAction> &act2) const {
        return act1->equals(*act2);
    }
};

class ParserActionHash {
public:
    unsigned long operator()(const std::shared_ptr<ParserAction> &act) const {
        return act->hash();
    }
};

class ParserTable {
public:
    using ParserActionSet = std::unordered_set<std::shared_ptr<ParserAction>, ParserActionHash, ParserActionEquality>;
    using ActionTable = std::vector<std::map<UnicodeString, ParserActionSet>>;
    using GotoTable = std::vector<std::map<UnicodeString, int>>;

    ~ParserTable();

    bool buildTableFromGrammar(const Grammar &grammar);
    void printActionTable();
    void printGotoTable();
private:
    void addNewAction(int currentState, const UnicodeString &currentWord, std::shared_ptr<ParserAction> action);
//    std::set<UnicodeString> followSetForWord(const Grammar &grammar, const UnicodeString &word);
//    std::set<UnicodeString> firstSetForWord(const Grammar &grammar, const UnicodeString &word);

    ActionTable *actionTable {nullptr};
    GotoTable *gotoTable {nullptr};
};

}

#endif //PARSER_TABLE_H_

