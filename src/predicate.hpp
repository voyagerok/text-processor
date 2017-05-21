#ifndef PREDICATE_HPP_
#define PREDICATE_HPP_

#include <unicode/unistr.h>
#include <memory>

namespace tproc {

class Predicate;

using PredicatePtr = std::shared_ptr<Predicate>;

class Token;

class Predicate {
protected:
    virtual bool equals(const PredicatePtr &other) const = 0;
public:
    virtual ~Predicate() = default;
    virtual bool operator()(const Token &token) = 0;
    friend bool operator==(const PredicatePtr &lhs, const PredicatePtr &rhs) { return lhs->equals(rhs); }
    friend bool operator!=(const PredicatePtr &lhs, const PredicatePtr &rhs) { return !(lhs->equals(rhs)); }
    virtual unsigned long hash() const = 0;
};

class UpperCaseFirstPredicate final: public Predicate {
protected:
    bool equals(const PredicatePtr &other) const override { return typeid(this) == typeid (other.get()); }
public:
    UpperCaseFirstPredicate() = default;
    bool operator()(const Token &token) override;
    unsigned long hash() const override { return typeid (this).hash_code(); }
};

}

#endif //PREDICATE_HPP_
