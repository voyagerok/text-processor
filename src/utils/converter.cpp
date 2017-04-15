/*
 * converter.cpp
 *
 *  Created on: Apr 8, 2017
 *      Author: nikolay
 */

#include "converter.h"

namespace tproc {

std::string stdStringFromUnistr(const icu::UnicodeString &unistr) {
    std::string result;
    icu::StringByteSink<std::string> sink(&result);
    unistr.toUTF8(sink);
    return result;
}

}
