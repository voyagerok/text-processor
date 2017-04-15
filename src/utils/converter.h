/*
 * converter.h
 *
 *  Created on: Apr 8, 2017
 *      Author: nikolay
 */

#ifndef UTILS_CONVERTER_H_
#define UTILS_CONVERTER_H_

//class icu::UnicodeString;
//class std::string;

#include <string>
#include <unicode/unistr.h>

namespace tproc {

std::string stdStringFromUnistr(const icu::UnicodeString &unistr);

}


#endif /* UTILS_CONVERTER_H_ */
