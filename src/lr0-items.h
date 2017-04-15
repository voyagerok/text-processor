#ifndef LRO_ITEMS_H_
#define LRO_ITEMS_H_


#include "grammar.h"
#include "grammar-rule.h"

#include <memory>
#include <iostream>

namespace tproc {

struct LR0ItemRule {
    SimpleGrammarRule rule;
    int position;

    bool operator==(const LR0ItemRule &other);
    bool operator!=(const LR0ItemRule &other);
    friend std::ostream &operator<<(std::ostream &os, const LR0ItemRule &itemRule);
};

struct LR0Item {
    UnicodeString incomingWord;
    std::vector<LR0ItemRule> rules;
    std::map<UnicodeString, int> relations;
    void addItemRule(const LR0ItemRule &rule) { rules.push_back(rule); }

    bool operator==(const LR0Item &other);
    friend std::ostream &operator<<(std::ostream &os, const LR0Item &item);
};

class LR0ItemSet {
public:
    LR0ItemSet();
    bool build(const Grammar &grammar);
    std::vector<LR0Item> getItems() const { return items; }
private:
    void build(const Grammar &grammar, LR0Item &item, int itemIndex);
    void closure(const Grammar &grammar, LR0Item &item, const LR0ItemRule &currentRule);
    void addItemToHistory(const LR0Item &item);

    std::vector<LR0Item> items;
    std::map<UnicodeString, std::vector<LR0Item>> history;
};

}

#endif //LR0_ITEMS_H_
