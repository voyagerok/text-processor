#include <unicode/ustream.h>

#include "lr0-items.h"
#include "utils/logger.h"

namespace tproc {

std::ostream &operator<<(std::ostream &os, const LR0Item &item) {
//    os << item.rule << ", " << item.position;
    return os << item.rule << ", " << item.wordIndex.ruleIndex << ", " << item.wordIndex.position;
}

bool LR0Item::operator==(const LR0Item &other) {
    return wordIndex == other.wordIndex && rule == other.rule;
}

bool LR0Item::operator!=(const LR0Item &other) {
    return !(*this == other);
}

GRuleWordPtr LR0Item::getWordAtCurrentPosition() {
//    return this->rule.rhs.at(this->position).rawValue;
    return this->rule->getChildWords().at(wordIndex.ruleIndex).at(wordIndex.position);
}

bool LR0ItemSet::operator==(const LR0ItemSet &other) {
    if (items.size() != other.items.size()) {
        return false;
    }
    for (auto i = 0; i < items.size(); ++i) {
        if (items[i] != other.items[i])
            return false;
    }

    return true;
}

std::ostream &operator<<(std::ostream &os, const LR0ItemSet &itemSet) {
//    for (auto &item : itemSet.items) {
//        os << item << std::endl;
//    }
    for (int i = 0; i < itemSet.items.size(); ++i) {
        os << itemSet.items[i];
        if (i < itemSet.items.size() - 1) {
            os << std::endl;
        }
    }
    return os;
}

int LR0ItemSet::getNextStateForWord(const GRuleWordPtr &word) {
    auto it = transitions.find(word);
    if (it != transitions.end()) {
        return it->second;
    } else {
        return -1;
    }
}

LR0ItemSetCollection::LR0ItemSetCollection() {

}

bool LR0ItemSetCollection::build(const Grammar &grammar) {
    history.clear();
//    Logger::getLogger() << "get start rule" << std::endl;
//    SimpleGrammarRule startRule = grammar.getStartRule();
    GRuleWordPtr root = grammar.getRoot();
//    Logger::getLogger() << "build: root " << root->getRawValue() << std::endl;
//    Logger::getLogger() << "got start rule:" << startRule << std::endl;
//    if (!startRule.isValid) {
//        return false;
//    }
//    Logger::getLogger() << "Building start item" << std::endl;
    LR0ItemSet startItemSet;
    startItemSet.addItem({root, 0, 0});
//    Logger::getLogger() << "start item is " << startItem << std::endl;
    build(grammar, startItemSet, 0);

    return true;
}

//struct Transition {
//    UnicodeString word;

//};

void LR0ItemSetCollection::build(const Grammar &grammar, LR0ItemSet &itemSet, int itemSetIndex) {
//    Logger::getLogger() << "Starting iteration for item with index: " << itemIndex << std::endl;
    for (auto i = 0; i < itemSet.items.size(); ++i) {
//        Logger::getLogger() << "closure for item " << item << "and rule " << item.rules[i] << std::endl;
//        Logger::getLogger() << "build: clousre for itemset " << itemSet << ", rule:\n" << itemSet.items[i] << std::endl;
//        auto &rule = itemSet.items[i].rule;
//        auto &ruleIndex = itemSet.items[i].wordIndex;
//        auto &targetWords = rule->getChildWords().at(ruleIndex.ruleIndex);
//        if (ruleIndex.position < targetWords.size()) {
//            auto &targetWord = targetWords.at(ruleIndex.position);
//            if (targetWord->isNonTerminal()) {
//                auto &targetChildWords = targetWord->getChildWords();
//                for (auto i = 0; i < targetChildWords.size(); ++i) {
//                    if (targetWord->isNonTerminal() && targetWord->isRhsEmpty(i)) {
//                        LR0Item newItem { rule, ruleIndex.ruleIndex, ruleIndex.position + 1 };
//                        itemSet.addItem(std::move(newItem));
//                    }
//                }
//            }
//        }
        closure(itemSet, itemSet.items[i]);
    }

//    Logger::getLogger() << "Item after clousre, index:" << itemSetIndex << std::endl;
//    Logger::getLogger() << itemSet << std::endl;

//    auto nextItemSetIndex = itemSetCollection.size();
//    std::map<int, std::shared_ptr<LR0Item>> nextItems;
    std::map<GRuleWordPtr, LR0ItemSet> nextItemSets;
    for (auto i = 0; i < itemSet.items.size(); ++i) {
        auto wordIndex = itemSet.items[i].wordIndex;
        auto rule = itemSet.items[i].rule;
        if (wordIndex.ruleIndex < rule->getChildWords().size() &&
                wordIndex.position < rule->getChildWords().at(wordIndex.ruleIndex).size())
        {
//            UnicodeString wordToRead = rule.rightHandle[wordIndex].rawValue;
            GRuleWordPtr wordToRead = rule->getChildWords().at(wordIndex.ruleIndex).at(wordIndex.position);
//            Logger::getLogger() << "Word to read: " << wordToRead << std::endl;
//            Logger::getLogger() << "Rule: " << rule << ", position: " << position << std::endl;
//            auto it = itemSet.transitions.find(wordToRead);
            auto it = nextItemSets.find(wordToRead);
            if (it == nextItemSets.end()) {
//                ++nextItemSetIndex;
                LR0ItemSet nextItemSet;
                nextItemSet.addItem({rule, wordIndex.ruleIndex, wordIndex.position + 1});
                nextItemSet.incomingWord = wordToRead;
                nextItemSets[wordToRead] = nextItemSet;
//                nextItemSets.push_back(nextItemSet);
//                nextItemSet.itemsetIndex = nextItemSetIndex;
//                nextItemSets[nextItemSetIndex] = nextItemSet;
//                itemSet.transitions[wordToRead] = nextItemSetIndex;
            } else {
                auto &nextItemSet = it->second;
                nextItemSet.addItem({rule, wordIndex.ruleIndex, wordIndex.position + 1});
            }
        }
    }

    itemSetCollection.push_back(itemSet);
//    Logger::getLogger() << "build: to itemset collection:\n" << itemSet << std::endl;

    for (auto &nextItemSetWordPair : nextItemSets) {
        auto nextItemSet = nextItemSetWordPair.second;
        bool itemSetFound = false;
        if (nextItemSet.incomingWord != nullptr) {
            auto wordItemPair = history.find(nextItemSet.incomingWord);
            if (wordItemPair != history.end()) {
                auto items = wordItemPair->second;
                for (auto &historyItemSet : items) {
                    if (historyItemSet == nextItemSet) {
                        auto &itemSet = itemSetCollection.at(itemSetIndex);
                        itemSet.transitions[nextItemSet.incomingWord] = historyItemSet.itemsetIndex;
                        itemSetFound = true;
                        break;
                    }
                }
            }
        }

        if (!itemSetFound) {
            int nextItemSetIndex = itemSetCollection.size();
            nextItemSet.itemsetIndex = nextItemSetIndex;
            auto &itemSet = itemSetCollection.at(itemSetIndex);
            itemSet.transitions[nextItemSet.incomingWord] = nextItemSetIndex;
            addItemSetToHistory(nextItemSet);
            build(grammar, nextItemSet, nextItemSetIndex);
        }
    }

////    Logger::getLogger() << "Next items are:" << std::endl;
//    for (auto &itemIndexPair : nextItemSets) {
////        Logger::getLogger() << itemIndexPair.second << std::endl;
//        auto currentItemSet = itemIndexPair.second;
//        auto currentItemSetIndex = itemIndexPair.first;
//        bool currentItemSetFound = false;
//        if (!currentItemSet.incomingWord.isEmpty()) {
//            auto wordItemsPair = history.find(currentItemSet.incomingWord);
//            if (wordItemsPair != history.end()) {
//                auto items = wordItemsPair->second;
//                for (auto &historyItem : items) {
//                    if (historyItem == currentItemSet) {
//                        auto &itemSet = itemSetCollection.at(itemSetIndex);
//                        itemSet.transitions[currentItemSet.incomingWord] = historyItem.itemsetIndex;
//                        currentItemSetFound = true;
//                        break;
//                    }
//                }
//            }
//        }
////        Logger::getLogger() << "Current item found: " << currentItemFound << std::endl;
//        if (!currentItemSetFound) {
//            addItemSetToHistory(currentItemSet);
//            build(grammar, currentItemSet, currentItemSetIndex);
//        }
//    }
}

//bool LR0ItemSet::build(const Grammar &grammar, const SimpleGrammarRule &currentRule) {
//    if (!currentRule.isValid) {
//        return false;
//    }
//    LR0Item currentItem;
//}

void LR0ItemSetCollection::addItemSetToHistory(const LR0ItemSet &itemSet) {
    auto incomingWord = itemSet.incomingWord;
    history[incomingWord].push_back(itemSet);
//    auto it = history.find(incomingWord);
//    if (it == history.end()) {
//        history[incomingWord] = {itemSet};
//    } else {
//        auto &itemsForWord = history[incomingWord];
//        itemsForWord.push_back(itemSet);
//    }
}

void LR0ItemSetCollection::closure(LR0ItemSet &itemSet, const LR0Item &currentItem) {
    if (currentItem.rule->getChildWords().size() <= currentItem.wordIndex.ruleIndex ||
            currentItem.rule->getChildWords().at(currentItem.wordIndex.ruleIndex).size() <= currentItem.wordIndex.position)
    {
        return;
    }
//    UnicodeString currentWord = currentItem.rule.rhs.at(currentItem.position).rawValue;
    WordIndex currentItemIndex = currentItem.wordIndex;
    GRuleWordPtr currentWord = currentItem.rule->getChildWords().at(currentItemIndex.ruleIndex).at(currentItemIndex.position);
    if (!currentWord->isNonTerminal()) {
        return;
    }
//    UnicodeString currentWord = currentItem.rule.rightHandle[currentItem.position];
//    Logger::getLogger() << "Closure: currentWord is " << currentWord << std::endl;
//    auto rulesForWord = grammar.getRulesForLeftHandle(currentWord);
//    auto rulesForWord = currentWord->getChildWords();
//    Logger::getLogger() << "Rules for current word:" << std::endl;
//    for (auto &rule : rulesForWord) {
//        Logger::getLogger() << rule << std::endl;
//    }

//    for (int i = 0; i < rulesForWord.size(); ++i) {
//        LR0Item itemRule {rulesForWord[i], 0};
////        Logger::getLogger() << "got item for rule: " << itemRule << std::endl;
//        if (itemRule != currentItem) {
////            Logger::getLogger() << "recurs closure call for rule " << itemRule << std::endl;
//            closure(grammar, itemSet, itemRule);
////            Logger::getLogger() << "adding rule " << itemRule << " in item " << item << std::endl;
//            itemSet.addItem(itemRule);
//        }
//    }
    for (int i = 0; i < currentWord->getChildWords().size(); ++i) {
        int position = 0;
        if (currentWord->isRhsEmpty(i)) {
            ++position;
        }
        LR0Item itemRule { currentWord, i, position };
        if (itemRule != currentItem) {
            closure(itemSet, itemRule);
            itemSet.addItem(itemRule);
        }
    }
}

}
