#include <iostream>
#include <unicode/ustream.h>

#include "parser-table.h"
#include "grammar.h"
#include "lr0-items.h"

namespace tproc {

std::ostream &operator<<(std::ostream &os, ActionName actionName) {
    switch(actionName) {
    case ActionName::ACCEPT:
        os << "ACCEPT";
        break;
    case ActionName::ERROR:
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

bool ParserAction::equals(const ParserAction &other) const {
    if (this->actionName != other.actionName) {
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

bool ShiftAction::equals(const ParserAction &other) const {
    try {
        const ShiftAction &otherShiftAction = dynamic_cast<const ShiftAction&>(other);
        if (this->actionName != otherShiftAction.actionName) {
            return  false;
        }
        if (this->nextState != otherShiftAction.nextState) {
            return false;
        }
    } catch (std::bad_cast &err) {
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

bool ReduceAction::equals(const ParserAction &other) const {
    try {
        const ReduceAction &otherReduceAction = dynamic_cast<const ReduceAction&>(other);
        if (this->actionName != otherReduceAction.actionName) {
            return false;
        }
        if (this->ruleIndex != otherReduceAction.ruleIndex) {
            return false;
        }
    } catch (std::bad_cast &err) {
        return false;
    }

    return true;
}

unsigned long ReduceAction::hash() const {
    int rawValue = static_cast<int>(this->actionName);
    return std::hash<int>()(rawValue) + this->ruleIndex.hash();
}

ParserTable::~ParserTable() {
    if (actionTable != nullptr) {
        delete actionTable;
    }

    if (gotoTable != nullptr) {
        delete  gotoTable;
    }
}

bool ParserTable::buildTableFromGrammar(const Grammar &grammar) {
    LR0ItemSetCollection itemsetCollection;
    if (!itemsetCollection.build(grammar)) {
        std::cerr << "Failed to build LR0 items" << std::endl;
        return false;
    }
    if (actionTable != nullptr) {
        delete actionTable;
    }
    actionTable = new ActionTable(itemsetCollection.size());

    if (gotoTable != nullptr) {
        delete gotoTable;
    }
    gotoTable = new GotoTable(itemsetCollection.size());

    auto itemSets = itemsetCollection.getItemSetCollection();
    for (int i = 0; i < itemSets.size(); ++i) {
        LR0ItemSet currentItemSet = itemSets[i];
        for (auto &item : currentItemSet) {
            if (item.atEndPosition()) {
                std::set<UnicodeString> followWords;
                if (grammar.followWordsForNterminal(item.rule.leftPart, followWords)) {
                    auto ruleIndex = grammar.getRuleIndex(item.rule);
                    for (auto &followWord : followWords) {
                        if (grammar.isEndOfInput(followWord) && grammar.isStartRule(item.rule)) {
                            addNewAction(i, followWord, std::make_shared<ParserAction> (ActionName::ACCEPT));
                        } else {
                            addNewAction(i, followWord, std::make_shared<ReduceAction> (ruleIndex));
                        }
                    }
                } else {
                    std::cerr << "Failed to get follow words" << std::endl;
                    return false;
                }
            } else {
                UnicodeString nextWord = item.getWordAtCurrentPosition();
                if (!grammar.isNonTerminal(nextWord)) {
                    int nextState = currentItemSet.getNextStateForWord(nextWord);
                    addNewAction(i, nextWord, std::make_shared<ShiftAction> (nextState));
                }
            }
        }

        auto transitions = currentItemSet.getTransitions();
        std::map<UnicodeString, int> ntermTransitions;
        for (auto &transition : transitions) {
            if (grammar.isNonTerminal(transition.first)) {
                ntermTransitions.insert(transition);
            }
        }
        (*gotoTable)[i] = std::move(ntermTransitions);
    }

    return true;
}

void ParserTable::addNewAction(int currentState, const UnicodeString &currentWord, std::shared_ptr<ParserAction> action) {
    if (actionTable == nullptr) {
        return;
    }
    if (actionTable->size() <= currentState) {
        return;
    }
    auto &actionForState = actionTable->at(currentState);
    auto it = actionForState.find(currentWord);
    if (it != actionForState.end()) {
//        bool found = false;
//        for (auto &currentAction : it->second) {
//            if (currentAction->equals(*action)) {
//                found = true;
//                break;
//            }
//        }
//        if (!found) {
//            it->second.push_back(action);
//        }
        it->second.insert(action);
    } else {
        actionForState[currentWord] = {action};
    }
}

void ParserTable::printActionTable() {
    if (actionTable == nullptr) {
        return;
    }

    for (int i = 0; i < actionTable->size(); ++i) {
        std::cout << "Actions for state " << i << std::endl;
        auto actions = actionTable->at(i);
        for (auto &action : actions) {
            std::cout << "Word: " << action.first << std::endl;
            for (auto &parserAction : action.second) {
                std::cout << "Action: " << *parserAction << std::endl;
            }
        }
    }
}

void ParserTable::printGotoTable() {
    if (gotoTable == nullptr) {
        return;
    }

    for (int i = 0; i < gotoTable->size(); ++i) {
        std::cout << "Goto for state: " << i << std::endl;
        auto gotos = gotoTable->at(i);
        for (auto &gotoJump : gotos) {
            std::cout << "Word: " << gotoJump.first << std::endl;
            std::cout << "State: " << gotoJump.second << std::endl;
        }
    }
}

}
