#ifndef PREDICATE_HPP_
#define PREDICATE_HPP_

#include <unicode/unistr.h>
#include <memory>

namespace tproc {

class Predicate;

using PredicatePtr = std::shared_ptr<Predicate>;

class Predicate {
protected:
    virtual bool equals(const PredicatePtr &other) const = 0;
public:
    virtual ~Predicate() = default;
    virtual bool operator()(const UnicodeString &token) = 0;
    friend bool operator==(const PredicatePtr &lhs, const PredicatePtr &rhs) { return lhs->equals(rhs); }
    friend bool operator!=(const PredicatePtr &lhs, const PredicatePtr &rhs) { return !(lhs->equals(rhs)); }
};

class UpperCaseFirstPredicate final: public Predicate {
protected:
    bool equals(const PredicatePtr &other) const override { return typeid(this) == typeid (other.get()); }
public:
    UpperCaseFirstPredicate() = default;
    bool operator()(const UnicodeString &token) override;
};

}

#endif //PREDICATE_HPP_
