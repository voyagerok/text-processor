#include <iostream>
#include <unicode/ustream.h>
#include <algorithm>

#include "parser.h"

namespace tproc {

bool Parser::tryParse(const Tokenizer::Sentence &sentence, std::vector<UnicodeString> &result) {
    bool isAccepted = false;

    ActiveSet currentSet;
    currentSet.insert(std::make_shared<GSSStateNode>(0));

    UnicodeString currentChain;
    for (auto &token : sentence) {
        std::cout << "checking current token: " << token.word << ", normal form: " << token.normalForm << std::endl;
        currentSet = parseToken(token, currentSet, isAccepted);
        if (isAccepted || currentSet.size() == 0) {
            currentSet = {std::make_shared<GSSStateNode>(0)};
            if (isAccepted) {
                result.push_back(currentChain);
                currentChain = "";
                isAccepted = false;
            }
        } else {
            currentChain.append(token.word + " ");
        }
    }

//    return  isAccepted;
    return true;
}

Parser::ActiveSet Parser::parseToken(const Token &token, ActiveSet &currentLevelSet, bool &accepted) {
    ActiveSet activeNodes = currentLevelSet;
    ActiveSet nextLevelNodes;
    ReduceSet reduceSet;
    ShiftSet shiftSet;
    std::cout << "parseToken: current active nodes:" << std::endl;
    for (auto &activeNode : activeNodes) {
        std::cout << *activeNode << std::endl;
    }
    while (reduceSet.size() > 0 || activeNodes.size() > 0) {
        if (activeNodes.size() > 0) {
            std::cout << "parseToken: active nodes not empty - perform actor" << std::endl;
            actor(token, activeNodes, reduceSet, shiftSet, accepted);
        }
        if (reduceSet.size() > 0) {
            std::cout << "parseToken: reduce set not epmpty - perform reducer" << std::endl;
            reducer(token, activeNodes, currentLevelSet, reduceSet);
        }
    }

    std::cout << "parseToken: perform shifter" << std::endl;

    shifter(nextLevelNodes, shiftSet);

    return nextLevelNodes;
}

void Parser::actor(const Token &token, ActiveSet &activeNodes, ReduceSet &reduceSet, ShiftSet &shiftSet, bool &isAccepted) {
    ActiveSet::iterator next_element = std::max_element(activeNodes.begin(), activeNodes.end());
    GSSNodePtr activeNode = *next_element;
    activeNodes.erase(next_element);
    std::cout << "actor: next element is " << *activeNode << std::endl;
//    GSSNodePtr activeNode = *activeNodes.rbegin(); //activeNodes.front();
//    activeNodes.erase(activeNodes.rbegin());
    auto activeStateNode = std::dynamic_pointer_cast<GSSStateNode>(activeNode);
    if (activeStateNode == nullptr) {
        std::cerr << "Failed to cast ective node to state node" << std::endl;
        return;
    }

//    ParserTable::ParserActionSet actionSet;


//    // check for raw word first
//    ParserTable::ParserActionSet rawActionSet;
//    parserTable.getActionsForStateAndWord(activeStateNode->state, token.normalForm, rawActionSet);
//    ParserTable::ParserActionSet tagActionSet;
//    auto &tags = token.tags;
//    if (tags.size() > 0) {
//        auto partOfSpeechTag = tags[0];
//        if (parserTable.getActionsForStateAndWord(activeStateNode->state, partOfSpeechTag, tagActionSet)) {
//            std::cout << "Found action for part of speech" << std::endl;
//        }
//    }
//    actionSet.insert(rawActionSet.begin(), rawActionSet.end());
//    actionSet.insert(tagActionSet.begin(), tagActionSet.end());
//    parserTable.getActionsForStateAndWord()

//    if (parserTable.getActionsForStateAndWord(activeStateNode->state, token.normalForm, rawActionSet)) {
//        std::cout << "Found action for raw word" << std::endl;
////        for (auto &parserAction : actionSet) {

////        }
//    } else { //check all tags
//        auto &tags = token.tags;
//        if (tags.size() > 0) {
//            auto partOfSpeechTag = tags[0];
//            if (parserTable.getActionsForStateAndWord(activeStateNode->state, partOfSpeechTag, actionSet)) {
//                std::cout << "Found action for part of speech" << std::endl;
//            }
//        }
//    }

//    ParserTable::ParserActionSet actionSet = getActionSetForToken(token, activeStateNode->state);
    auto actionSetWithInfo = getActionSetForTokenWithTokenInfo(token, activeStateNode->state);

    for (auto &tokenActionPair : actionSetWithInfo) {
        auto tokenValue = tokenActionPair.first;
        auto actionsForToken = tokenActionPair.second;
        for (auto &parserAction : actionsForToken) {
            switch (parserAction->actionName) {
            case ActionName::SHIFT: {
                auto shiftAction = std::dynamic_pointer_cast<ShiftAction>(parserAction);
                shiftSet.emplace_back(activeStateNode, shiftAction->nextState, tokenValue);
            }
                break;
            case ActionName::REDUCE: {
                auto reduceAction = std::dynamic_pointer_cast<ReduceAction>(parserAction);
                auto successors = activeNode->getSucc();
                for (auto &succ : successors) {
                    reduceSet.emplace_back(activeStateNode, succ, reduceAction->ruleIndex);
                }
            }
                break;
            case ActionName::ACCEPT:
                isAccepted = true;
                break;
            case ActionName::ERROR:
                break;
            }
        }
    }


//    for (auto &parserAction : actionSet) {
//        switch (parserAction->actionName) {
//        case ActionName::SHIFT: {
//            auto shiftAction = std::dynamic_pointer_cast<ShiftAction>(parserAction);
//            shiftSet.emplace_back(activeStateNode, shiftAction->nextState);
//        }
//            break;
//        case ActionName::REDUCE: {
//            auto reduceAction = std::dynamic_pointer_cast<ReduceAction>(parserAction);
//            auto successors = activeNode->getSucc();
//            for (auto &succ : successors) {
//                reduceSet.emplace_back(activeStateNode, succ, reduceAction->ruleIndex);
//            }
//        }
//            break;
//        case ActionName::ACCEPT:
//            isAccepted = true;
//            break;
//        case ActionName::ERROR:
//            break;
//        }
//    }
}

void Parser::reducer(const Token &token, ActiveSet &activeSet, ActiveSet &currentLevelSet, ReduceSet &reduceSet) {
    ReduceInfo reduceInfo = reduceSet.back();
    reduceSet.pop_back();
//    auto next_element = std::max_element(reduceSet.begin(), reduceSet.end());
//    ReduceInfo reduceInfo= *next_element;
//    reduceSet.erase(next_element);
//    ReduceInfo reduceInfo = *reduceSet.end();//reduceSet.front();
//    reduceSet.erase(reduceSet.end());
//    auto reductionRule = grammar.getRuleForRuleIndex(reduceInfo.ruleIndex)
    SimpleGrammarRule reductionRule;
    if (!grammar.getRuleForRuleIndex(reduceInfo.ruleIndex, reductionRule)) {
        std::cerr << "Failed to get reduction rule for index" << std::endl;
        return;
    }
    std::cout << "Reducer: found grammar rule: " << reductionRule << std::endl;

    auto nterm = reductionRule.leftPart;
    auto startNode = reduceInfo.endNode;
    int reductionPathLength = 2 * reductionRule.rightHandle.size() - 1;
    auto pathEndNodes = findAllDestsForPath(startNode, reductionPathLength);

    std::cout << "Reducer: reduction path end nodes:" << std::endl;
    for (auto &endNode : pathEndNodes) {
        std::cout << *endNode << std::endl;
    }

    for (auto &destNode : pathEndNodes) {
        auto destStateNode = std::dynamic_pointer_cast<GSSStateNode>(destNode);
        if (destStateNode == nullptr) {
            std::cerr << "Failed to cast destination node to state node" << std::endl;
            return;
        }
        int targetState;
        if (!parserTable.getGotoStateForStateAndNterm(destStateNode->state, nterm, targetState)) {
            std::cerr << "Failed to get target state for nterm: " << nterm << "and current state: " << destStateNode->state << std::endl;
            return;
        }

        auto result = std::find_if(currentLevelSet.begin(), currentLevelSet.end(), [targetState](const GSSNodePtr &node){
            auto stateNode = std::dynamic_pointer_cast<GSSStateNode>(node);
            if (stateNode != nullptr) {
                if (stateNode->state == targetState) {
                    return true;
                }
            }
            return false;
        });

        if (result != std::end(currentLevelSet)) {
            if (isExistsPath(*result, destStateNode)) {
                return;
            }
            auto symbolNode = std::make_shared<GSSSymbolNode>(nterm);
            (*result)->addSucc(symbolNode);
//            destStateNode->addPred(symbolNode);
            symbolNode->addSucc(destStateNode);
//            symbolNode->addPred(*result);

            auto resultInActive = activeSet.find(*result);
            if (resultInActive == activeSet.end()) {
//                auto actions = parserTable.getActionsForStateAndWord()
//                auto actionSet = getActionSetForToken(token, (*result)->)
                auto stateNode = std::dynamic_pointer_cast<GSSStateNode>(*result);
                auto actionSet = getActionSetForToken(token, stateNode->state);
                for (auto &action : actionSet) {
                    auto reduceAction = std::dynamic_pointer_cast<ReduceAction>(action);
                    if (reduceAction != nullptr) {
                        reduceSet.emplace_back(stateNode, symbolNode, reduceAction->ruleIndex);
                    }
                }
            }
        } else {
            auto stateNode = std::make_shared<GSSStateNode>(targetState);
            auto symbolNode = std::make_shared<GSSSymbolNode>(nterm);

            stateNode->addSucc(symbolNode);
//            symbolNode->addPred(stateNode);
            symbolNode->addSucc(destStateNode);
//            destStateNode->addPred(symbolNode);
            currentLevelSet.insert(stateNode);
            activeSet.insert(stateNode);
        }
//        parserTable.getGotoStateForStateAndNterm()

    }
}

void Parser::shifter(ActiveSet &nextStateSet, ShiftSet &shiftSet) {
    std::set<std::shared_ptr<GSSStateNode>> stateNodes;
    for (auto &shiftAction : shiftSet) {
        int targetState = shiftAction.targetState;
        auto result = std::find_if(stateNodes.begin(), stateNodes.end(), [targetState](const std::shared_ptr<GSSStateNode> &node){
            return node->state == targetState;
        });
        std::shared_ptr<GSSStateNode> stateNode;
        if (result == std::end(stateNodes)) {
            stateNode = std::make_shared<GSSStateNode>(targetState);
            stateNodes.insert(stateNode);
        } else {
            stateNode = *result;
        }
        auto symbolNode = std::make_shared<GSSSymbolNode>(shiftAction.word);
        stateNode->addSucc(symbolNode);
        symbolNode->addSucc(shiftAction.node);
    }

    for (auto &stateNode : stateNodes) {
        nextStateSet.insert(stateNode);
    }
}

std::map<UnicodeString, ParserTable::ParserActionSet> Parser::getActionSetForTokenWithTokenInfo(const Token &token, int state) {
    std::map<UnicodeString, ParserTable::ParserActionSet> actionSet;

    // check for raw word first
    ParserTable::ParserActionSet rawActionSet;
    if (parserTable.getActionsForStateAndWord(state, token.normalForm, rawActionSet)) {
//        actionSet.insert(rawActionSet.begin(), rawActionSet.end());
        actionSet[token.normalForm] = rawActionSet;
    }

    ParserTable::ParserActionSet tagActionSet;
    auto &tags = token.tags;
    if (tags.size() > 0) {
        auto partOfSpeechTag = tags[0];
        if (parserTable.getActionsForStateAndWord(state, partOfSpeechTag, tagActionSet)) {
            std::cout << "Found action for part of speech" << std::endl;
//            actionSet.insert(tagActionSet.begin(), tagActionSet.end());
            actionSet[partOfSpeechTag] = tagActionSet;
        }
    }

    ParserTable::ParserActionSet endOfInputActions;
    if (parserTable.getActionsForStateAndWord(state, "$", endOfInputActions)) {
        actionSet["$"] = endOfInputActions;
    }

    return actionSet;
}

ParserTable::ParserActionSet Parser::getActionSetForToken(const Token &token, int state) {
    ParserTable::ParserActionSet actionSet;

    // check for raw word first
    ParserTable::ParserActionSet rawActionSet;
    if (parserTable.getActionsForStateAndWord(state, token.normalForm, rawActionSet)) {
        actionSet.insert(rawActionSet.begin(), rawActionSet.end());
//        actionSet[token.normalForm] = rawActionSet;
    }

    if (actionSet.size() == 0) {
        ParserTable::ParserActionSet tagActionSet;
        auto &tags = token.tags;
        if (tags.size() > 0) {
            auto partOfSpeechTag = tags[0];
            if (parserTable.getActionsForStateAndWord(state, partOfSpeechTag, tagActionSet)) {
                std::cout << "Found action for part of speech" << std::endl;
                actionSet.insert(tagActionSet.begin(), tagActionSet.end());
    //            actionSet[partOfSpeechTag] = tagActionSet;
            }
        }
    }

    if (actionSet.size() == 0) {
        ParserTable::ParserActionSet endOfInputActions;
        if (parserTable.getActionsForStateAndWord(state, "$", endOfInputActions)) {
            actionSet.insert(endOfInputActions.begin(), endOfInputActions.end());
        }
    }

    return actionSet;
}

}

