#include <unicode/ustream.h>

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

}

