#ifndef LRO_ITEMS_H_
#define LRO_ITEMS_H_


#include "grammar.h"
#include "grammar-rule.h"

#include <memory>
#include <iostream>

namespace tproc {

struct LR0Item {
    GRuleWordPtr rule;
//    int position;
    WordIndex wordIndex;
    GRuleWordPtr getWordAtCurrentPosition();
//    bool atEndPosition() { return rule-> <= position; }
    bool atEndPosition() {
        return rule->getChildWords().at(wordIndex.ruleIndex).size() <= wordIndex.position;
    }

    bool operator==(const LR0Item &other);
    bool operator!=(const LR0Item &other);
    friend std::ostream &operator<<(std::ostream &os, const LR0Item &item);
};

struct LR0ItemSet {
    using iter = std::vector<LR0Item>::iterator;
    using const_iter = std::vector<LR0Item>::const_iterator;

//    UnicodeString incomingWord;
    GRuleWordPtr incomingWord = nullptr;
    int itemsetIndex = 0;
    std::vector<LR0Item> items;
    std::map<GRuleWordPtr, int> transitions;

    void addItem(const LR0Item &item) { items.push_back(item); }
    void addItem(LR0Item &&item) { items.push_back(std::move(item)); }

    template<class ...Args>
    void emplaceItem(Args&&...args) { items.emplace_back(&args...); }

    int getNextStateForWord(const GRuleWordPtr &word);
    std::map<GRuleWordPtr, int> getTransitions() const { return transitions; }

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
    void closure(LR0ItemSet &itemSet, const LR0Item &currentItem);
    void addItemSetToHistory(const LR0ItemSet &itemSet);

    std::vector<LR0ItemSet> itemSetCollection;
    std::map<GRuleWordPtr, std::vector<LR0ItemSet>> history;
};

}

#endif //LR0_ITEMS_H_
