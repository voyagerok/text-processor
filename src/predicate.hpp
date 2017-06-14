#ifndef PREDICATE_HPP_
#define PREDICATE_HPP_

#include <unicode/unistr.h>
#include <memory>
#include <unicode/regex.h>

namespace tproc {

class Predicate;

using PredicatePtr = std::shared_ptr<Predicate>;

struct Token;

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

class RegexPredicate final: public Predicate {
protected:
    bool equals(const PredicatePtr &other) const override;
public:
    RegexPredicate(const UnicodeString &regexPattern);
    bool operator()(const Token &token) override;
    unsigned long hash() const override;
    std::shared_ptr<RegexMatcher> getRegex() { return regex; }
    UnicodeString getPattern() const { return regex->pattern().pattern(); }
private:
    std::shared_ptr<RegexMatcher> regex = nullptr;
};

class LengthPredicate final: public Predicate {
protected:
    bool equals(const PredicatePtr &other) const override;
public:
    LengthPredicate(int length):
        minLength { length },
        maxLength { length } {}
    LengthPredicate(int minLength, int maxLength):
        minLength { minLength },
        maxLength { maxLength } {}
    bool operator()(const Token &token) override;
    unsigned long hash() const override { return std::hash<int>()(minLength) ^ std::hash<int>()(maxLength) * typeid (this).hash_code(); }
private:
    int minLength, maxLength;
};

}

#endif //PREDICATE_HPP_
