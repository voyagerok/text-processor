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

    bool operator==(const LR0Item &other);
    bool operator!=(const LR0Item &other);
    friend std::ostream &operator<<(std::ostream &os, const LR0Item &item);
};

struct LR0ItemSet {
    UnicodeString incomingWord;
    std::vector<LR0Item> items;
    std::map<UnicodeString, int> relations;
    void addItem(const LR0Item &item) { items.push_back(item); }

    bool operator==(const LR0ItemSet &other);
    friend std::ostream &operator<<(std::ostream &os, const LR0ItemSet &itemSet);
};

class LR0ItemSetCollection {
public:
    LR0ItemSetCollection();
    bool build(const Grammar &grammar);
    std::vector<LR0ItemSet> getItemSetCollection() const { return itemSetCollection; }
private:
    void build(const Grammar &grammar, LR0ItemSet &itemSet, int itemSetIndex);
    void closure(const Grammar &grammar, LR0ItemSet &itemSet, const LR0Item &currentItem);
    void addItemSetToHistory(const LR0ItemSet &itemSet);

    std::vector<LR0ItemSet> itemSetCollection;
    std::map<UnicodeString, std::vector<LR0ItemSet>> history;
};

}

#endif //LR0_ITEMS_H_
