/*
 ============================================================================
 Name        : text-processor.cpp
 Author      : Nikolay Linetskiy
 Version     :
 Copyright   : This file is part of Text Processor.
 Text Processor is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software Foundation,
 either version 3 of the License, or (at your option) any later version.
 Text Processor is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.
 You should have received a copy of the GNU General Public License  along with Text Processor.
 If not, see <http://www.gnu.org/licenses/>.
 Description : Hello World in C++,
 ============================================================================
 */

//#include <iostream>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/schriter.h>
#include <iostream>

#include "grammar-parser.h"
#include "grammar.h"
#include "lr0-items.h"

int main(void) {
    icu::UnicodeString ustring = "S = \"Вася\" S \"Петя\" S\nS = a\n";
    tproc::Grammar grammar;
    tproc::LR0ItemSet itemSet;
    if (grammar.initFromPlainText(ustring)) {
        itemSet.build(grammar);
        auto items = itemSet.getItems();
        std::cout << "Final itemset is:" << std::endl;
        for (auto &item : items) {
            std::cout << "Incoming word is " << item.incomingWord << std::endl;
            std::cout << item << "\n\n";
        }
    }
//    if (grammar.initFromPlainText(ustring)) {
//        for (auto &rule : grammar) {
//            for (auto &rule_right_handle : rule.second) {
//                std::cout << rule.first << " = " << rule_right_handle << std::endl;
//            }
//        }
//    }

    return 0;
}
