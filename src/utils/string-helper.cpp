#include <unicode/appendable.h>
#include <unicode/regex.h>
#include <iostream>
#include <unicode/ustream.h>

#include "string-helper.h"
#include "utils/logger.h"

namespace tproc {

bool split_unistring(const UnicodeString &unistring, const std::vector<UnicodeString> &splitSeqs, std::vector<UnicodeString> &result) {
    UnicodeString patternStr = "[";
    for (int i = 0; i < splitSeqs.size(); ++i) {
        patternStr.append(splitSeqs[i]);
//        if (i < splitSeqs.size() - 1) {
//            patternStr.append(",");
//        }
    }
    patternStr.append("]");
    Logger::getLogger() << "spiltter: pattern is " << patternStr << std::endl;

//    return split_unistring(unistring, patternStr, result);

    UErrorCode status = U_ZERO_ERROR;
    RegexMatcher splitter {patternStr, 0, status};
    if (U_FAILURE(status)) {
        return false;
    }

    auto capacity = unistring.length();
    UnicodeString lines[capacity];
    auto n_of_lines = splitter.split(unistring, lines, capacity, status);
    if (U_FAILURE(status)) {
        return false;
    }

//    result = std::vector<UnicodeString> {lines, lines + n_of_lines};
//    result.assign(lines, lines + n_of_lines);
    for (int i = 0; i < n_of_lines; ++i) {
//        Logger::getLogger() << "line is: " << lines[i] << std::endl;
        if (!lines[i].isEmpty())
            if (!lines[i].isEmpty())
                result.push_back(lines[i]);
    }

    return true;
}

//bool split_unistring(const UnicodeString &unistring, const UnicodeString &splitRegex, std::vector<UnicodeString> &result) {
//    UErrorCode status = U_ZERO_ERROR;
//    RegexMatcher splitter {splitRegex, 0, status};
//    if (U_FAILURE(status)) {
//        return false;
//    }

//    auto capacity = unistring.length();
//    UnicodeString lines[capacity];
//    auto n_of_lines = splitter.split(unistring, lines, capacity, status);
//    if (U_FAILURE(status)) {
//        return false;
//    }

////    result = std::vector<UnicodeString> {lines, lines + n_of_lines};
////    result.assign(lines, lines + n_of_lines);
//    for (int i = 0; i < n_of_lines; ++i) {
////        Logger::getLogger() << "line is: " << lines[i] << std::endl;
//        if (!lines[i].isEmpty())
//            if (!lines[i].isEmpty())
//                result.push_back(lines[i]);
//    }

//    return true;
//}

}

