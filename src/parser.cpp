#include <iostream>
#include <unicode/ustream.h>
#include <algorithm>

#include "parser.h"
#include "utils/logger.h"

namespace tproc {

std::ostream &operator<<(std::ostream &os, const ShiftInfo &shift) {
    os << "Shift info: target state: " << shift.targetState << ", word: " << shift.word;
    return os;
}

bool Parser::tryParse(const Tokenizer::Sentence &sentence, std::vector<UnicodeString> &result) {
    bool isAccepted = false;

    ActiveSet currentSet;
    currentSet.insert(std::make_shared<GSSStateNode>(0));

    auto sentenceBegin = sentence.begin();

    while (sentenceBegin != sentence.end()) {
        UnicodeString currentChain;
        for (auto it = sentenceBegin; it != sentence.end(); ++it) {
            currentSet = parseToken(*it, currentSet, isAccepted);
            if (isAccepted || currentSet.size() == 0 || it == sentence.end() - 1) {
                if (isAccepted) {
//                    Logger::getLogger() << "Accepted: " << currentChain << std::endl;
                    result.push_back(currentChain);
                    isAccepted = false;
                }
                currentSet = {std::make_shared<GSSStateNode>(0)};
                sentenceBegin++;
                break;
            } else if (currentSet.size() > 0) {
                Logger::getLogger() << "Appending current chain: " << it->word << std::endl;
                currentChain.append(it->word + " ");
            }
        }
    }

//    UnicodeString currentChain;
//    for (auto &token : sentence) {
//        Logger::getLogger() << "checking current token: " << token.word << ", normal form: " << token.normalForm << std::endl;
//        currentSet = parseToken(token, currentSet, isAccepted);
//        if (isAccepted || currentSet.size() == 0) {
//            currentSet = {std::make_shared<GSSStateNode>(0)};
//            if (isAccepted) {
//                Logger::getLogger() << "Accepted, current chain: " <<  currentChain << std::endl;
//                result.push_back(currentChain);
//                currentChain = "";
//                isAccepted = false;
//            }
//        } else {
//            currentChain.append(token.word + " ");
//        }
//    }

//    return  isAccepted;
    return true;
}

Parser::ActiveSet Parser::parseToken(const Token &token, ActiveSet &currentLevelSet, bool &accepted) {
    Logger::getLogger() << "parseToken: " << token.word << std::endl;
    ActiveSet activeNodes = currentLevelSet;
    ActiveSet nextLevelNodes;
    ReduceSet reduceSet;
    ShiftSet shiftSet;
    Logger::getLogger() << "parseToken: current active nodes:" << std::endl;
    for (auto &activeNode : activeNodes) {
        Logger::getLogger() << *activeNode << std::endl;
    }
    while (reduceSet.size() > 0 || activeNodes.size() > 0) {
        if (activeNodes.size() > 0) {
            Logger::getLogger() << "parseToken: active nodes not empty - perform actor" << std::endl;
            actor(token, activeNodes, reduceSet, shiftSet, accepted);
        }
        if (reduceSet.size() > 0) {
            Logger::getLogger() << "parseToken: reduce set not epmpty - perform reducer" << std::endl;
            reducer(token, activeNodes, currentLevelSet, reduceSet);
        }
    }

    Logger::getLogger() << "parseToken: perform shifter" << std::endl;

    shifter(nextLevelNodes, shiftSet);

    return nextLevelNodes;
}

void Parser::actor(const Token &token, ActiveSet &activeNodes, ReduceSet &reduceSet, ShiftSet &shiftSet, bool &isAccepted) {
    ActiveSet::iterator next_element = std::max_element(activeNodes.begin(), activeNodes.end());
    GSSNodePtr activeNode = *next_element;
    activeNodes.erase(next_element);
    Logger::getLogger() << "actor: next element is " << *activeNode << std::endl;
//    GSSNodePtr activeNode = *activeNodes.rbegin(); //activeNodes.front();
//    activeNodes.erase(activeNodes.rbegin());
    auto activeStateNode = std::dynamic_pointer_cast<GSSStateNode>(activeNode);
    if (activeStateNode == nullptr) {
        Logger::getErrLogger() << "Failed to cast ective node to state node" << std::endl;
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
//            Logger::getLogger() << "Found action for part of speech" << std::endl;
//        }
//    }
//    actionSet.insert(rawActionSet.begin(), rawActionSet.end());
//    actionSet.insert(tagActionSet.begin(), tagActionSet.end());
//    parserTable.getActionsForStateAndWord()

//    if (parserTable.getActionsForStateAndWord(activeStateNode->state, token.normalForm, rawActionSet)) {
//        Logger::getLogger() << "Found action for raw word" << std::endl;
////        for (auto &parserAction : actionSet) {

////        }
//    } else { //check all tags
//        auto &tags = token.tags;
//        if (tags.size() > 0) {
//            auto partOfSpeechTag = tags[0];
//            if (parserTable.getActionsForStateAndWord(activeStateNode->state, partOfSpeechTag, actionSet)) {
//                Logger::getLogger() << "Found action for part of speech" << std::endl;
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
//    SimpleGrammarRule reductionRule;
//    if (!grammar.getRuleForRuleIndex(reduceInfo.ruleIndex, reductionRule)) {
//        Logger::getErrLogger() << "Failed to get reduction rule for index" << std::endl;
//        return;
//    }
//    GRuleWordPtr reductionRule = reduceInfo.rule;
    GRuleWordPtr reductionRule = reduceInfo.ruleIndex.nterm;
    Logger::getLogger() << "Reducer: found grammar rule: " << reductionRule << std::endl;

    auto nterm = reductionRule->getRawValue();
    auto startNode = reduceInfo.endNode;
//    int reductionPathLength = 2 * reductionRule->getChildWords().at(reduceInfo.ruleIndex.index).size() - 1;
    int reductionPathLength = 2 * reductionRule->getRhsLength(reduceInfo.ruleIndex.index) - 1;
//    int reductionPathLength = reductionRule->isRhsEmpty(reduceInfo.ruleIndex.index) ? 0 :
//                                                                                      2 * reductionRule->getChildWords().at(reduceInfo.ruleIndex.index).size() - 1;
//    auto pathEndNodes = findAllDestsForPath(startNode, reductionPathLength);
    auto pathEndNodes = reductionPathLength > 0 ? findAllDestsForPath(startNode, reductionPathLength) :
                                                  std::vector<GSSNodePtr> { reduceInfo.startNode };

    Logger::getLogger() << "Reducer: reduction path end nodes:" << std::endl;
    for (auto &endNode : pathEndNodes) {
        Logger::getLogger() << *endNode << std::endl;
    }

    for (auto &destNode : pathEndNodes) {
        auto destStateNode = std::dynamic_pointer_cast<GSSStateNode>(destNode);
        if (destStateNode == nullptr) {
            Logger::getErrLogger() << "Failed to cast destination node to state node" << std::endl;
            return;
        }
        int targetState;
        if (!parserTable.getGotoStateForStateAndNterm(destStateNode->state, nterm, targetState)) {
            Logger::getErrLogger() << "Failed to get target state for nterm: " << nterm << "and current state: " << destStateNode->state << std::endl;
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
        Logger::getLogger() << "Shifter: current shift:" << std::endl;
        Logger::getLogger() << shiftAction << std::endl;
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
    if (parserTable.getActionsForStateAndWord(state, token.normalForm, token, rawActionSet)) {
        Logger::getLogger() << "Found action for normal form" << std::endl;
//        actionSet.insert(rawActionSet.begin(), rawActionSet.end());
        actionSet[token.normalForm] = rawActionSet;
    }

//    if (actionSet.size() == 0) {
        ParserTable::ParserActionSet tagActionSet;
        if (parserTable.getActionsForStateAndWord(state, token.partOfSpeech, token, tagActionSet)) {
            Logger::getLogger() << "Found action for part of speech" << std::endl;
            actionSet[token.partOfSpeech] = tagActionSet;
        }
//    }

    // check name
//    if (actionSet.size() == 0) {
        ParserTable::ParserActionSet nameCheckActionSet;
        ReservedWord namePart = ReservedWord::NONE;
        if ((token.propMask & MorphProperty::PATR)) {
            namePart = ReservedWord::PATR;
        } else if ((token.propMask & MorphProperty::SECOND_NAME)) {
            namePart = ReservedWord::SURNAME;
        } else if ((token.propMask & MorphProperty::FIRST_NAME)) {
            namePart = ReservedWord::NAME;
        }
        if (namePart != ReservedWord::NONE) {
            if (parserTable.getActionsForStateAndReservedWord(state, namePart, token, nameCheckActionSet)) {
                Logger::getLogger() << "Found action for name part" << std::endl;
//                actionSet.insert(nameCheckActionSet.begin(), nameCheckActionSet.end());
                actionSet[token.word] = nameCheckActionSet;
            }
        }
//    }

//    if (actionSet.size() == 0) {
        ParserTable::ParserActionSet initActionSet;
        if ((token.propMask & MorphProperty::INIT)) {
            if (parserTable.getActionsForStateAndReservedWord(state, ReservedWord::INIT, token, initActionSet)) {
                Logger::getLogger() << "Found action for init" << std::endl;
//                actionSet.insert(initActionSet.begin(), initActionSet.end());
                actionSet[token.word] = initActionSet;
            }
        }
//    }

//    if (actionSet.size() == 0) {
        ParserTable::ParserActionSet anyWordActionSet;
        if (parserTable.getActionsForStateAndReservedWord(state, ReservedWord::ANYWORD, token, anyWordActionSet)) {
            Logger::getLogger() << "Found action for anyword" << std::endl;
            actionSet[token.word] = anyWordActionSet;
        }
//    }

//    if (actionSet.size() == 0) {
        ParserTable::ParserActionSet endOfInputActions;
        if (parserTable.getActionsForStateAndWord(state, "$", token, endOfInputActions)) {
            actionSet["$"] = endOfInputActions;
        }
//    }

    return actionSet;
}

ParserTable::ParserActionSet Parser::getActionSetForToken(const Token &token, int state) {
    ParserTable::ParserActionSet actionSet;

    // check for raw word first
    ParserTable::ParserActionSet rawActionSet;
    if (parserTable.getActionsForStateAndWord(state, token.normalForm, token, rawActionSet)) {
        actionSet.insert(rawActionSet.begin(), rawActionSet.end());
//        actionSet[token.normalForm] = rawActionSet;
    }

    // check name
//    if (actionSet.size() == 0) {
        ParserTable::ParserActionSet nameCheckActionSet;
        ReservedWord namePart = ReservedWord::NONE;
        if ((token.propMask & MorphProperty::PATR)) {
            namePart = ReservedWord::PATR;
        } else if ((token.propMask & MorphProperty::SECOND_NAME)) {
            namePart = ReservedWord::SURNAME;
        } else if ((token.propMask & MorphProperty::FIRST_NAME)) {
            namePart = ReservedWord::NAME;
        }
        if (namePart != ReservedWord::NONE) {
            if (parserTable.getActionsForStateAndReservedWord(state, namePart, token, nameCheckActionSet)) {
                Logger::getLogger() << "Found action for name part" << std::endl;
                actionSet.insert(nameCheckActionSet.begin(), nameCheckActionSet.end());
            }
        }
//    }

//    if (actionSet.size() == 0) {
        ParserTable::ParserActionSet initActionSet;
        if ((token.propMask & MorphProperty::INIT)) {
            if (parserTable.getActionsForStateAndReservedWord(state, ReservedWord::INIT, token, initActionSet)) {
                Logger::getLogger() << "Found action for init" << std::endl;
                actionSet.insert(initActionSet.begin(), initActionSet.end());
            }
        }
//    }

//    if (actionSet.size() == 0) {
        ParserTable::ParserActionSet tagActionSet;
        if (parserTable.getActionsForStateAndWord(state, token.partOfSpeech, token, tagActionSet)) {
            Logger::getLogger() << "Found action for part of speech" << std::endl;
            actionSet.insert(tagActionSet.begin(), tagActionSet.end());
        }
//        auto &tags = token.tags;
//        if (tags.size() > 0) {
//            auto partOfSpeechTag = tags[0];
//            if (parserTable.getActionsForStateAndWord(state, partOfSpeechTag, tagActionSet)) {
//                Logger::getLogger() << "Found action for part of speech" << std::endl;
//                actionSet.insert(tagActionSet.begin(), tagActionSet.end());
//    //            actionSet[partOfSpeechTag] = tagActionSet;
//            }
//        }
//    }

//    if (actionSet.size() == 0) {
        ParserTable::ParserActionSet anyWordActionSet;
        if (parserTable.getActionsForStateAndReservedWord(state, ReservedWord::ANYWORD, token, anyWordActionSet)) {
            Logger::getLogger() << "Found action for anyword" << std::endl;
//            actionSet[token.word] = anyWordActionSet;
            actionSet.insert(anyWordActionSet.begin(), anyWordActionSet.end());
        }
//    }

//    if (actionSet.size() == 0) {
        ParserTable::ParserActionSet endOfInputActions;
        if (parserTable.getActionsForStateAndWord(state, "$", token, endOfInputActions)) {
            actionSet.insert(endOfInputActions.begin(), endOfInputActions.end());
        }
//    }

    return actionSet;
}

}

