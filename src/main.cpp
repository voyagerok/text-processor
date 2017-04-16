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
#include <Python.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/schriter.h>
#include <iostream>

#include "grammar-parser.h"
#include "grammar.h"
#include "lr0-items.h"
#include "parser-table.h"
#include "morph-analyzer.h"

int main(void) {
    Py_Initialize();
    initmorph();

//    icu::UnicodeString ustring = "S = a S b S\nS = a\n";
//    tproc::Grammar grammar;
//    tproc::LR0ItemSetCollection itemSet;
//    if (grammar.initFromPlainText(ustring)) {
//        itemSet.build(grammar);
//        auto items = itemSet.getItemSetCollection();
//        std::cout << "Final itemset is:" << std::endl;
//        for (auto &item : items) {
//            std::cout << "Incoming word is " << item.incomingWord << std::endl;
////            std::cout
//            std::cout << item << "\n\n";
//        }

////        grammar.printFirstSet();
////        grammar.printFollowSet();

//        tproc::ParserTable table;
//        table.buildTableFromGrammar(grammar);
//        table.printActionTable();
//        table.printGotoTable();
//    }

//    tproc::analyzeTokens({""});
//    std::vector<std::map<std::string, std::string>> result;
    std::vector<AnalysisResult> results;
    analyzeTokens({"Николай", "медведь"}, results);
    for (auto &result : results) {
        std::cout << result.normalForm << std::endl;
        std::cout << result.tag.partOfSpeech << std::endl;
        std::cout << result.tag.number << std::endl;
    }

    Py_Finalize();

    return 0;
}
