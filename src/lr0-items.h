#ifndef LRO_ITEMS_H_
#define LRO_ITEMS_H_


#include "grammar.h"
#include "grammar-rule.h"

#include <memory>
#include <iostream>

namespace tproc {

struct LR0Item {
    SimpleGrammarRule rule;
    int position;
    UnicodeString getWordAtCurrentPosition();
    bool atEndPosition() { return rule.rightHandle.size() <= position; }

    bool operator==(const LR0Item &other);
    bool operator!=(const LR0Item &other);
    friend std::ostream &operator<<(std::ostream &os, const LR0Item &item);
};

struct LR0ItemSet {
    using iter = std::vector<LR0Item>::iterator;
    using const_iter = std::vector<LR0Item>::const_iterator;

    UnicodeString incomingWord;
    int itemsetIndex;
    std::vector<LR0Item> items;
    std::map<UnicodeString, int> transitions;

    void addItem(const LR0Item &item) { items.push_back(item); }
    int getNextStateForWord(const UnicodeString &word);
    std::map<UnicodeString, int> getTransitions() const { return transitions; }

    iter begin() { return items.begin(); }
    iter end() { return items.end(); }
    const_iter begin() const { return items.begin(); }
    const_iter end() const { return items.end(); }

    bool operator==(const LR0ItemSet &other);
    friend std::ostream &operator<<(std::ostream &os, const LR0ItemSet &itemSet);
};

class LR0ItemSetCollection {
public:
    LR0ItemSetCollection();
    bool build(const Grammar &grammar);
    std::vector<LR0ItemSet> getItemSetCollection() const { return itemSetCollection; }
    int size() const { return itemSetCollection.size(); }
private:
    void build(const Grammar &grammar, LR0ItemSet &itemSet, int itemSetIndex);
    void closure(const Grammar &grammar, LR0ItemSet &itemSet, const LR0Item &currentItem);
    void addItemSetToHistory(const LR0ItemSet &itemSet);

    std::vector<LR0ItemSet> itemSetCollection;
    std::map<UnicodeString, std::vector<LR0ItemSet>> history;
};

}

#endif //LR0_ITEMS_H_
