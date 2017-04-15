/*
 * Logger.h
 *
 *  Created on: Apr 8, 2017
 *      Author: nikolay
 */

#ifndef UTILS_LOGGER_H_
#define UTILS_LOGGER_H_

#include <string>
#include <unicode/unistr.h>
#include <iostream>

namespace tproc {

class Logger {
public:
    static void printStr(const std::string &str);
    static void printStr(const UnicodeString &str);
    static void printStr(const char *str);
    static void printFormatStr(const char *fmt, ...);

    //	std::ostream &operator<<(std::ostream &output)
};

} /* namespace tproc */

#endif /* UTILS_LOGGER_H_ */
