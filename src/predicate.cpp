#include <unicode/ustream.h>
#include <sstream>

#include "predicate.hpp"
#include "utils/logger.h"
#include "tokenizer.h"

namespace tproc {

bool UpperCaseFirstPredicate::operator()(const Token &token) {
    if (token.word.isEmpty()) {
        return false;
    }
    auto firstChar = token.word.tempSubString(0, 1);
    return firstChar.toUpper() == token.word.tempSubString(0,1);
}

bool RegexPredicate::equals(const PredicatePtr &other) const {
    auto regexPredicatePtr = std::dynamic_pointer_cast<RegexPredicate>(other);
    if (regexPredicatePtr == nullptr) {
        return false;
    }
    return getPattern() == regexPredicatePtr->getPattern();
}

RegexPredicate::RegexPredicate(const UnicodeString &pattern) {
    UErrorCode status = U_ZERO_ERROR;
    this->regex = std::make_shared<RegexMatcher>(pattern, 0, status);
    if (U_FAILURE(status)) {
        std::ostringstream os;
        os << "Failed to init regex from pattern " << pattern;
        throw std::runtime_error(os.str());
    }
}

unsigned long RegexPredicate::hash() const {
    return typeid (this).hash_code() ^ getPattern().hashCode();
}

bool RegexPredicate::operator()(const Token &token) {
    if (token.word.isEmpty()) {
        return false;
    }
//    regex->reset(token.word);
    regex->reset(token.pureToken);

    UErrorCode status = U_ZERO_ERROR;
    UBool result = regex->matches(status);
    if (U_FAILURE(status)) {
//        std::cerr << "RegexPredicate: error wh"
        Logger::getErrLogger() << "RegexPredicate: error when matching" << std::endl;
        return false;
    }

    return result;
}

bool LengthPredicate::operator()(const Token &token) {
//    Logger::getLogger() << "LengthPredicate: token " << token.word << ", minLen "
//                        << minLength << ", maxLen " << maxLength << std::endl;
//    UnicodeString pureWord = token.word;
//    pureWord.findAndReplace("\"","");
//    auto result = pureWord.length() >= minLength && pureWord.length() <= maxLength;
//    if (result) {
//        Logger::getLogger() << "Passed" << std::endl;
//    }
    return token.pureToken.length() >= minLength &&
            token.pureToken.length() <= maxLength;

//    return result;
}

bool LengthPredicate::equals(const PredicatePtr &other) const {
    auto lenPredicatePtr = std::dynamic_pointer_cast<LengthPredicate>(other);
    if (lenPredicatePtr == nullptr) {
        return false;
    }
    return minLength == lenPredicatePtr->minLength && maxLength == lenPredicatePtr->maxLength;
}

}

