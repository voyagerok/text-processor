#ifndef ACTION_HPP_
#define ACTION_HPP_

#include <queue>

//#include "grammar.h"
#include "grammar-rule.h"

namespace tproc {

class Grammar;

class Action;

using ActionPtr = std::shared_ptr<Action>;

class Action {
private:
    GRuleWordPtr ruleWord;
protected:
    virtual bool equals(const ActionPtr &other) const = 0;
public:
    Action(const GRuleWordPtr &ruleWord):
        ruleWord { ruleWord } {}
    virtual ~Action() = default;

    GRuleWordPtr &getRuleWord() { return ruleWord; }

    virtual void operator()(const Grammar &grammar/*, const ParentInfo &wordInfo*/) = 0;
    friend bool operator==(const ActionPtr &lhs, const ActionPtr &rhs) { return lhs->equals(rhs); }
    friend bool operator!=(const ActionPtr &lhs, const ActionPtr &rhs) { return !(lhs->equals(rhs)); }
};

class MinMaxAction final: public Action {
private:
    int minOccurences = 1;
    int maxOccurences = 1;
protected:
    bool equals(const ActionPtr &other) const override;
public:
    MinMaxAction(const GRuleWordPtr &ruleWord, int minOccur, int maxOccur):
        Action(ruleWord), minOccurences { minOccur}, maxOccurences { maxOccur } {}
    MinMaxAction(int minOccur, int maxOccur):
        Action(nullptr), minOccurences { minOccur}, maxOccurences { maxOccur } {}
    int getMinOccurences() const { return this->minOccurences; }
    int getMaxOccurences () const { return this->maxOccurences; }
    void operator()(const Grammar &grammar/*, const ParentInfo &wordInfo*/) override {}
};

class QuoteAction final: public Action {
protected:
    bool equals(const ActionPtr &other) const override;
public:
    QuoteAction(const GRuleWordPtr &ruleWord):
        Action(ruleWord) {}
    QuoteAction() : Action(nullptr) {}
    void operator()(const Grammar &grammar/*, const ParentInfo &wordInfo*/) override;
};

template<class ActionType, class ...Args>
class ActionBuilder {
    std::shared_ptr<ActionType> build(const GRuleWordPtr &wordPtr, Args...args) {
        return std::make_shared<ActionType>(wordPtr, &args...);
    }
};

}

#endif //ACTION_HPP_
