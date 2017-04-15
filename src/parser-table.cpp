#include <iostream>

#include "parser-table.h"
#include "grammar.h"
#include "lr0-items.h"

namespace tproc {

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
                        addNewAction(i, followWord, ReduceAction {ruleIndex});
                    }
                } else {
                    std::cerr << "Failed to get follow words" << std::endl;
                    return false;
                }
            } else {
                UnicodeString nextWord = item.getWordAtCurrentPosition();
                if (!grammar.isNonTerminal(nextWord)) {
                    int nextState = currentItemSet.getNextStateForWord(nextWord);
                    addNewAction(i, nextWord, ShiftAction {nextState});
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
        (*gotoTable)[i] = ntermTransitions;
    }

    return true;
}

void ParserTable::addNewAction(int currentState, const UnicodeString &currentWord, ParserAction action) {
    if (actionTable == nullptr) {
        return;
    }
    if (actionTable->size() <= currentState) {
        return;
    }
    auto &actionForState = actionTable->at(currentState);
    auto it = actionForState.find(currentWord);
    if (it != actionForState.end()) {
        it->second.push_back(action);
    } else {
        actionForState[currentWord] = {action};
    }
}

//std::set<UnicodeString> ParserTable::firstSetForWord(const Grammar &grammar, const UnicodeString &word) {
//    if (!grammar.isNonTerminal(word)) {
//        return {word};
//    } else {
//        std::set<UnicodeString> firstSet;
//        auto rulesForWord = grammar.getRulesForLeftHandle(word);
//        for (auto &rule : rulesForWord) {
//            auto firstWord = rule.rightHandle[0];
//            if (firstWord == word) {
//                continue;
//            }
//            auto result = firstSetForWord(grammar, firstWord);
//            if (result.size() > 0) {
//                firstSet.insert(result.begin(), result.end());
//            }
//        }

//        return firstSet;
//    }
//}

//std::set<UnicodeString> ParserTable::followSetForWord(const Grammar &grammar, const UnicodeString &word) {

//}

}
