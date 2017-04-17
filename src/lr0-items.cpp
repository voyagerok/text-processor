#include <unicode/ustream.h>

#include "lr0-items.h"

namespace tproc {

std::ostream &operator<<(std::ostream &os, const LR0Item &item) {
    os << item.rule << ", " << item.position;
    return os;
}

bool LR0Item::operator==(const LR0Item &other) {
    return position == other.position && rule == other.rule;
}

bool LR0Item::operator!=(const LR0Item &other) {
    return !(*this == other);
}

UnicodeString LR0Item::getWordAtCurrentPosition() {
    return this->rule.rightHandle.at(this->position);
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
    for (auto &item : itemSet.items) {
        os << item << std::endl;
    }
    return os;
}

int LR0ItemSet::getNextStateForWord(const UnicodeString &word) {
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
//    std::cout << "get start rule" << std::endl;
    SimpleGrammarRule startRule = grammar.getStartRule();
//    std::cout << "got start rule:" << startRule << std::endl;
    if (!startRule.isValid) {
        return false;
    }
//    std::cout << "Building start item" << std::endl;
    LR0ItemSet startItemSet;
    startItemSet.addItem({startRule, 0});
//    std::cout << "start item is " << startItem << std::endl;
    build(grammar, startItemSet, 0);

    return true;
}

//struct Transition {
//    UnicodeString word;

//};

void LR0ItemSetCollection::build(const Grammar &grammar, LR0ItemSet &itemSet, int itemSetIndex) {
//    std::cout << "Starting iteration for item with index: " << itemIndex << std::endl;
    for (auto i = 0; i < itemSet.items.size(); ++i) {
//        std::cout << "closure for item " << item << "and rule " << item.rules[i] << std::endl;
        closure(grammar, itemSet, itemSet.items[i]);
    }

//    std::cout << "Item after clousre, index:" << itemSetIndex << std::endl;
//    std::cout << itemSet << std::endl;

//    auto nextItemSetIndex = itemSetCollection.size();
//    std::map<int, std::shared_ptr<LR0Item>> nextItems;
    std::map<UnicodeString, LR0ItemSet> nextItemSets;
    for (auto i = 0; i < itemSet.items.size(); ++i) {
        auto position = itemSet.items[i].position;
        auto rule = itemSet.items[i].rule;
        if (position < rule.rightHandle.size()) {
            UnicodeString wordToRead = rule.rightHandle[position];
//            std::cout << "Word to read: " << wordToRead << std::endl;
//            std::cout << "Rule: " << rule << ", position: " << position << std::endl;
//            auto it = itemSet.transitions.find(wordToRead);
            auto it = nextItemSets.find(wordToRead);
            if (it == nextItemSets.end()) {
//                ++nextItemSetIndex;
                LR0ItemSet nextItemSet;
                nextItemSet.addItem({rule, position + 1});
                nextItemSet.incomingWord = wordToRead;
                nextItemSets[wordToRead] = nextItemSet;
//                nextItemSets.push_back(nextItemSet);
//                nextItemSet.itemsetIndex = nextItemSetIndex;
//                nextItemSets[nextItemSetIndex] = nextItemSet;
//                itemSet.transitions[wordToRead] = nextItemSetIndex;
            } else {
                auto &nextItemSet = it->second;
                nextItemSet.addItem({rule, position + 1});
            }
        }
    }

    itemSetCollection.push_back(itemSet);

    for (auto &nextItemSetWordPair : nextItemSets) {
        auto nextItemSet = nextItemSetWordPair.second;
        bool itemSetFound = false;
        if (!nextItemSet.incomingWord.isEmpty()) {
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

////    std::cout << "Next items are:" << std::endl;
//    for (auto &itemIndexPair : nextItemSets) {
////        std::cout << itemIndexPair.second << std::endl;
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
////        std::cout << "Current item found: " << currentItemFound << std::endl;
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
    auto it = history.find(incomingWord);
    if (it == history.end()) {
        history[incomingWord] = {itemSet};
    } else {
        auto &itemsForWord = history[incomingWord];
        itemsForWord.push_back(itemSet);
    }
}

void LR0ItemSetCollection::closure(const Grammar &grammar, LR0ItemSet &itemSet, const LR0Item &currentItem) {
    UnicodeString currentWord = currentItem.rule.rightHandle[currentItem.position];
//    std::cout << "Closure: currentWord is " << currentWord << std::endl;
    auto rulesForWord = grammar.getRulesForLeftHandle(currentWord);
//    std::cout << "Rules for current word:" << std::endl;
//    for (auto &rule : rulesForWord) {
//        std::cout << rule << std::endl;
//    }
    for (int i = 0; i < rulesForWord.size(); ++i) {
        LR0Item itemRule {rulesForWord[i], 0};
//        std::cout << "got item for rule: " << itemRule << std::endl;
        if (itemRule != currentItem) {
//            std::cout << "recurs closure call for rule " << itemRule << std::endl;
            closure(grammar, itemSet, itemRule);
//            std::cout << "adding rule " << itemRule << " in item " << item << std::endl;
            itemSet.addItem(itemRule);
        }
    }
}

}
