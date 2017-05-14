#include "predicate.hpp"

namespace tproc {

bool UpperCaseFirstPredicate::operator()(const UnicodeString &token) {
    if (token.isEmpty()) {
        return false;
    }
    auto firstChar = token.tempSubString(0, 1);
    return firstChar.toUpper() == firstChar;
}

}

