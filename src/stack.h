#ifndef STACK_H_
#define STACK_H_

#include <vector>
#include <memory>
#include <set>

#include "tokenizer.h"

namespace tproc {

struct GSSNode;

using GSSNodePtr = std::shared_ptr<GSSNode>;

struct GSSNode {
    virtual std::set<GSSNodePtr> getSucc() const = 0;
//    virtual std::set<GSSNodePtr> getPred() const = 0;
    virtual void addSucc(const GSSNodePtr &node) = 0;
    virtual std::ostream &print(std::ostream &os) const = 0;
//    virtual void addPred(const GSSNodePtr &node) = 0;
};

std::ostream &operator<<(std::ostream &os, const GSSNode &node);

struct GSSNodePrivate {
    std::set<GSSNodePtr> succ;
//    std::set<GSSNodePtr> pred;
};

struct GSSSymbolNode : public GSSNode, private GSSNodePrivate {
    GSSSymbolNode(const UnicodeString &word) : word {word} {}
    UnicodeString word;
    std::set<GSSNodePtr> getSucc() const override;
//    std::set<GSSNodePtr> getPred() const override;
    void addSucc(const GSSNodePtr &node) override { this->succ.insert(node); }
    std::ostream &print(std::ostream &os) const override;
//    void addPred(const GSSNodePtr &node) override { this->pred.insert(node); }
};

struct GSSStateNode : public GSSNode, private GSSNodePrivate {
    GSSStateNode(int state) : state {state} {}
    int state;
    std::set<GSSNodePtr> getSucc() const override;
//    std::set<GSSNodePtr> getPred() const override;
    void addSucc(const GSSNodePtr &node) override { this->succ.insert(node); }
    std::ostream &print(std::ostream &os) const override;
//    void addPred(const GSSNodePtr &node) override { this->pred.insert(node); }
};

std::vector<GSSNodePtr> findAllDestsForPath(const GSSNodePtr &node, int pathLength);
bool isExistsPath(const GSSNodePtr &startNode, const GSSNodePtr &destNode);

}

#endif //STACK_H_

