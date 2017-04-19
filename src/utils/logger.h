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
#include <memory>

#include "global-defs.h"

namespace tproc {

class LogStream {
public:
    LogStream(std::ostream &os) : os {os} {}
    template<typename T>
    const LogStream& operator<<(const T& v) const {
#if ENABLE_LOG
        os << v;
#endif
        return *this;
    }
    const LogStream& operator<<(std::ostream &(*func)(std::ostream&)) const {
#if ENABLE_LOG
        func(os);
#endif
        return *this;
    }
private:
    std::ostream &os;
};

class Logger {
public:
    static LogStream &getLogger() {
        static LogStream logger {std::cout};
        return logger;
    }

    static LogStream &getErrLogger() {
        static LogStream errLogger {std::cerr};
        return errLogger;
    }
};


} /* namespace tproc */

#endif /* UTILS_LOGGER_H_ */
