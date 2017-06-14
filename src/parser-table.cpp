#include <iostream>
#include <unicode/ustream.h>
#include <algorithm>

#include "parser-table.h"
#include "grammar.h"
#include "lr0-items.h"
#include "utils/logger.h"
#include "tokenizer.h"
#include "grammar-words-storage.hpp"

namespace tproc {

std::ostream &operator<<(std::ostream &os, ActionName actionName) {
    switch(actionName) {
    case ActionName::ACCEPT:
        os << "ACCEPT";
        break;
    case ActionName::PARSE_ERROR:
        os << "ERROR";
        break;
    case ActionName::REDUCE:
        os << "REDUCE";
        break;
    case ActionName::SHIFT:
        os << "SHIFT";
        break;
    }
    return os;
}

std::ostream &ParserAction::print(std::ostream &os) const {
    os << actionName;
    return os;
}

bool ParserAction::equals(const ParserActionPtr &other) const {
    if (this->actionName != other->actionName) {
        return false;
    }

    return true;
}

unsigned long ParserAction::hash() const {
    int rawValue = static_cast<int>(this->actionName);
    return std::hash<int>()(rawValue);
}

std::ostream &ShiftAction::print(std::ostream &os) const {
    os << actionName << ", nextState: " << nextState;
    return os;
}

bool ShiftAction::equals(const ParserActionPtr &other) const {
//    try {
//        const ShiftAction &otherShiftAction = dynamic_cast<const ShiftAction&>(other);
//        if (this->actionName != otherShiftAction.actionName) {
//            return  false;
//        }
//        if (this->nextState != otherShiftAction.nextState) {
//            return false;
//        }
//    } catch (std::bad_cast &err) {
//        return false;
//    }
    auto shiftActionPtr = std::dynamic_pointer_cast<ShiftAction>(other);
    if (shiftActionPtr == nullptr) {
        return false;
    }
    if (this->actionName != shiftActionPtr->actionName) {
        return false;
    }
    if (this->nextState != shiftActionPtr->nextState) {
        return false;
    }

    return true;
}

unsigned long ShiftAction::hash() const {
    int rawValue = static_cast<int>(this->actionName);
    return std::hash<int>()(rawValue) + std::hash<int>()(this->nextState);
}

std::ostream &ReduceAction::print(std::ostream &os) const {
    os << actionName << ruleIndex;
    return os;
}

bool ReduceAction::equals(const ParserActionPtr &other) const {
//    try {
//        const ReduceAction &otherReduceAction = dynamic_cast<const ReduceAction&>(other);
//        if (this->actionName != otherReduceAction.actionName) {
//            return false;
//        }
//        if (this->ruleIndex != otherReduceAction.ruleIndex) {
//            return false;
//        }
//    } catch (std::bad_cast &err) {
//        return false;
//    }

    auto reduceActionPtr = std::dynamic_pointer_cast<ReduceAction>(other);
    if (reduceActionPtr == nullptr) {
        return false;
    }
    if (this->actionName != reduceActionPtr->actionName) {
        return false;
    }
    if (this->ruleIndex != reduceActionPtr->ruleIndex) {
        return false;
    }

    return true;
}

unsigned long ReduceAction::hash() const {
    int rawValue = static_cast<int>(this->actionName);
    return std::hash<int>()(rawValue) + ruleIndex.hash();
}

ParserTable::~ParserTable() {
//    if (actionTable != nullptr) {
//        delete actionTable;
//    }

//    if (gotoTable != nullptr) {
//        delete  gotoTable;
//    }
}

bool ParserTable::buildTableFromGrammar(const Grammar &grammar) {
    LR0ItemSetCollection itemsetCollection;
    if (!itemsetCollection.build(grammar)) {
        Logger::getErrLogger() << "Failed to build LR0 items" << std::endl;
        return false;
    }
//    if (actionTable != nullptr) {
//        delete actionTable;
//    }
//    actionTable = new ActionTable(itemsetCollection.size());

//    if (gotoTable != nullptr) {
//        delete gotoTable;
//    }
//    gotoTable = new GotoTable(itemsetCollection.size());
    actionTable = std::make_shared<ActionTable>(itemsetCollection.size());
    gotoTable = std::make_shared<GotoTable>(itemsetCollection.size());

    auto itemSets = itemsetCollection.getItemSetCollection();
    for (int i = 0; i < itemSets.size(); ++i) {
        LR0ItemSet &currentItemSet = itemSets[i];
        for (auto &item : currentItemSet) {
            if (item.atEndPosition()) {
                std::set<GRuleWordPtr> followWords;
                if (grammar.followWordsForNterminal(item.rule, followWords)) {
                    RuleIndex ruleIndex { item.rule, item.wordIndex.ruleIndex };
                    for (auto &followWord : followWords) {
//                        if (grammar.isEndOfInput(followWord) && grammar.isStartRule(item.rule)) {
                        if (followWord->isEndOfInput() && item.rule == grammar.getRoot()) {
                            addNewAction(i, followWord, std::make_shared<ParserAction> (ActionName::ACCEPT));
                        } else {
                            addNewAction(i, followWord, std::make_shared<ReduceAction> (ruleIndex));
                        }
                    }
                } else {
                    Logger::getErrLogger() << "Failed to get follow words" << std::endl;
                    return false;
                }
            } else {
//                UnicodeString nextWord = item.getWordAtCurrentPosition();
                GRuleWordPtr nextWord = item.getWordAtCurrentPosition();
//                if (!grammar.isNonTerminal(nextWord)) {
                if(!nextWord->isNonTerminal()) {
                    int nextState = currentItemSet.getNextStateForWord(nextWord);
                    addNewAction(i, nextWord, std::make_shared<ShiftAction> (nextState));
                }
            }
        }

        auto transitions = currentItemSet.getTransitions();
        std::map<GRuleWordPtr, int> ntermTransitions;
        for (auto &transition : transitions) {
//            if (grammar.isNonTerminal(transition.first)) {
//                ntermTransitions.insert(transition);
//            }
            if (transition.first->isNonTerminal()) {
                ntermTransitions.insert(transition);
            }
        }
        (*gotoTable)[i] = std::move(ntermTransitions);
    }

    return true;
}

void ParserTable::addNewAction(int currentState, const GRuleWordPtr &currentWord, std::shared_ptr<ParserAction> action) {
    if (actionTable == nullptr) {
        return;
    }
    if (actionTable->size() <= currentState) {
        return;
    }
    auto &actionsForState = actionTable->at(currentState);
    actionsForState[currentWord->getRawValue()][currentWord].insert(action);
//    auto wordFound = actionsForState.find(currentWord->getRawValue());
//    if (wordFound != actionsForState.end()) {
//        auto &actionsForRuleWord = wordFound->second;
//        auto ruleWordFound = actionsForRuleWord.find(currentWord);
//        if (ruleWordFound != actionsForRuleWord.end()) {
//            ruleWordFound->second.insert(action);
//        } else {
//            actionsForRuleWord[currentWord] = { action };
//        }
//    } else {
//        actionsForState[currentWord->getRawValue()][currentWord] = { action };
//    }
//    auto it = actionForState.find(currentWord);
//    if (it != actionForState.end()) {
//        it->second.insert(action);
//    } else {
//        actionForState[currentWord] = {action};
//    }
}

void ParserTable::printActionTable() {
    if (actionTable == nullptr) {
        return;
    }

    for (int i = 0; i < actionTable->size(); ++i) {
        Logger::getLogger() << "Actions for state " << i << std::endl;
        auto actions = actionTable->at(i);
        for (auto &action : actions) {
            for (auto &ruleWordActs : action.second) {
                Logger::getLogger() << "Word: " << ruleWordActs.first->getRawValue() << std::endl;
                for (auto &parserAction : ruleWordActs.second) {
                    Logger::getLogger() << "Action: " << *parserAction << std::endl;
                }
            }
        }
    }
}

void ParserTable::printGotoTable() {
    if (gotoTable == nullptr) {
        return;
    }

    for (int i = 0; i < gotoTable->size(); ++i) {
        Logger::getLogger() << "Goto for state: " << i << std::endl;
        auto gotos = gotoTable->at(i);
        for (auto &gotoJump : gotos) {
            Logger::getLogger() << "Word: " << gotoJump.first->getRawValue() << std::endl;
            Logger::getLogger() << "State: " << gotoJump.second << std::endl;
        }
    }
}

bool ParserTable::getActionsForStateAndWord(int state, const UnicodeString &word, const Token &token, ParserActionSet &actionSet) const {
//    Logger::getLogger() << "Word: " << word << std::endl;
    if (state < 0 || state >= actionTable->size()) {
        return false;
    }
    auto &actionsForState = actionTable->at(state);
//    auto actionsForWord = actionsForState.find(word);
//    auto actionsForWord = std::find_if(actionsForState.begin(), actionsForState.end(), [&word] (auto &actionInfo){
////        Logger::getLogger() << "Word: " << word << std::endl;
////        Logger::getLogger() << "Raw value: " << actionInfo.first->getRawValue() << std::endl;
//        return actionInfo.first->getRawValue() == word;
//    });
//    if (actionsForWord == actionsForState.end()) {
//        return false;
//    }
//    actionSet = actionsForWord->second;
    auto actionsForLiteral = actionsForState.find(word);
    if (actionsForLiteral == actionsForState.end()) {
        return false;
    }
//    actionSet = actionsForLiteral->second.rbegin()->second;
    for (auto ruleWordActions = actionsForLiteral->second.rbegin(); ruleWordActions != actionsForLiteral->second.rend(); ++ruleWordActions) {
        if (ruleWordActions->first->checkToken(token, word)) {
            actionSet = ruleWordActions->second;
            return true;
        }
    }
    return false;
}

bool ParserTable::getGotoStateForStateAndNterm(int state, const UnicodeString &nterm, int &targetState) const {
    if (state < 0 || state >= gotoTable->size()) {
        return false;
    }
    auto &jumpsForState = gotoTable->at(state);
//    auto jumpForNterm = jumpsForState.find(nterm);
    auto jumpForNterm = std::find_if(jumpsForState.begin(), jumpsForState.end(), [&nterm] (auto &jumpInfo) {
        return jumpInfo.first->getRawValue() == nterm;
    });
    if (jumpForNterm == jumpsForState.end()) {
        return false;
    }
    targetState = jumpForNterm->second;
    return true;
}

bool ParserTable::getActionsForStateAndReservedWord(int state, ReservedWord reservedWordType, const Token &token, ParserActionSet &actionSet) const {
//    UnicodeString &reservedWord = reservedWordsTable[reservedWordType];
    UnicodeString &reservedWord = GWordStorage::getReservedWord(reservedWordType);
    return this->getActionsForStateAndWord(state, reservedWord, token, actionSet);
}

}
