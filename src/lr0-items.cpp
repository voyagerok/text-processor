#include <unicode/ustream.h>

#include "lr0-items.h"

namespace tproc {

std::ostream &operator<<(std::ostream &os, const LR0ItemRule &itemRule) {
    os << itemRule.rule << ", " << itemRule.position;
    return os;
}

bool LR0ItemRule::operator==(const LR0ItemRule &other) {
    return position == other.position && rule == other.rule;
}

bool LR0ItemRule::operator!=(const LR0ItemRule &other) {
    return !(*this == other);
}

bool LR0Item::operator==(const LR0Item &other) {
    if (rules.size() != other.rules.size()) {
        return false;
    }
    for (auto i = 0; i < rules.size(); ++i) {
        if (rules[i] != other.rules[i])
            return false;
    }

    return true;
}

std::ostream &operator<<(std::ostream &os, const LR0Item &item) {
    for (auto &itemRule : item.rules) {
        os << itemRule << std::endl;
    }
    return os;
}

LR0ItemSet::LR0ItemSet() {

}

bool LR0ItemSet::build(const Grammar &grammar) {
    history.clear();
//    std::cout << "get start rule" << std::endl;
    SimpleGrammarRule startRule = grammar.getStartRule();
//    std::cout << "got start rule:" << startRule << std::endl;
    if (!startRule.isValid) {
        return false;
    }
//    std::cout << "Building start item" << std::endl;
    LR0Item startItem;
    startItem.addItemRule({startRule, 0});
//    std::cout << "start item is " << startItem << std::endl;
    build(grammar, startItem, 0);

    return true;
}

//struct Transition {
//    UnicodeString word;

//};

void LR0ItemSet::build(const Grammar &grammar, LR0Item &item, int itemIndex) {
//    std::cout << "Starting iteration for item with index: " << itemIndex << std::endl;
    for (auto i = 0; i < item.rules.size(); ++i) {
//        std::cout << "closure for item " << item << "and rule " << item.rules[i] << std::endl;
        closure(grammar, item, item.rules[i]);
    }
    items.push_back(item);

    std::cout << "Item after clousre, index:" << itemIndex << std::endl;
    std::cout << item << std::endl;

//    std::map<int, std::shared_ptr<LR0Item>> nextItems;
    std::map<int, LR0Item> nextItems;
    for (auto i = 0; i < item.rules.size(); ++i) {
        auto position = item.rules[i].position;
        auto rule = item.rules[i].rule;
        if (position < rule.rightHandle.size()) {
            UnicodeString wordToRead = rule.rightHandle[position];
            std::cout << "Word to read: " << wordToRead << std::endl;
//            std::cout << "Rule: " << rule << ", position: " << position << std::endl;
            auto it = item.relations.find(wordToRead);
            if (it == item.relations.end()) {
                LR0Item nextItem;
                nextItem.addItemRule({rule, position + 1});
                nextItem.incomingWord = wordToRead;
                nextItems[++itemIndex] = std::move(nextItem);
                item.relations[wordToRead] = itemIndex;
            } else {
                auto &nextItem = nextItems[it->second];
                nextItem.addItemRule({rule, position + 1});
            }
        }
    }

//    std::cout << "Next items are:" << std::endl;
    for (auto &itemIndexPair : nextItems) {
//        std::cout << itemIndexPair.second << std::endl;
        auto currentItem = itemIndexPair.second;
        auto currentItemIndex = itemIndexPair.first;
        bool currentItemFound = false;
        if (!currentItem.incomingWord.isEmpty()) {
            auto wordItemsPair = history.find(currentItem.incomingWord);
            if (wordItemsPair != history.end()) {
                auto items = wordItemsPair->second;
                for (auto &historyItem : items) {
                    if (historyItem == currentItem) {
                        currentItemFound = true;
                        break;
                    }
                }
            }
        }
//        std::cout << "Current item found: " << currentItemFound << std::endl;
        if (!currentItemFound) {
            addItemToHistory(currentItem);
            build(grammar, currentItem, currentItemIndex);
        }
    }
}

//bool LR0ItemSet::build(const Grammar &grammar, const SimpleGrammarRule &currentRule) {
//    if (!currentRule.isValid) {
//        return false;
//    }
//    LR0Item currentItem;
//}

void LR0ItemSet::addItemToHistory(const LR0Item &item) {
    auto incomingWord = item.incomingWord;
    auto it = history.find(incomingWord);
    if (it == history.end()) {
        history[incomingWord] = {item};
    } else {
        auto &itemsForWord = history[incomingWord];
        itemsForWord.push_back(item);
    }
}

void LR0ItemSet::closure(const Grammar &grammar, LR0Item &item, const LR0ItemRule &currentRule) {
    UnicodeString currentWord = currentRule.rule.rightHandle[currentRule.position];
//    std::cout << "Closure: currentWord is " << currentWord << std::endl;
    auto rulesForWord = grammar.getRulesForLeftHandle(currentWord);
//    std::cout << "Rules for current word:" << std::endl;
//    for (auto &rule : rulesForWord) {
//        std::cout << rule << std::endl;
//    }
    for (int i = 0; i < rulesForWord.size(); ++i) {
        LR0ItemRule itemRule {rulesForWord[i], 0};
//        std::cout << "got item for rule: " << itemRule << std::endl;
        if (itemRule != currentRule) {
//            std::cout << "recurs closure call for rule " << itemRule << std::endl;
            closure(grammar, item, itemRule);
//            std::cout << "adding rule " << itemRule << " in item " << item << std::endl;
            item.addItemRule(itemRule);
        }
    }
}

}
