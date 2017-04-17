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
#include "tokenizer.h"

int main(void) {
    Py_Initialize();
    initmorph();

    icu::UnicodeString ustring = "S = a b C\nS = a B C\nB = b\nC = d";
    tproc::Grammar grammar;
    tproc::LR0ItemSetCollection itemSet;
    if (grammar.initFromPlainText(ustring)) {
        itemSet.build(grammar);
        auto items = itemSet.getItemSetCollection();
        std::cout << "Final itemset is:" << std::endl;
        for (auto &item : items) {
            std::cout << "Incoming word is " << item.incomingWord << std::endl;
            std::cout << "State number is:" << item.itemsetIndex << std::endl;
            std::cout << item << "\n\n";
        }

//        grammar.printFirstSet();
//        grammar.printFollowSet();

        tproc::ParserTable table;
        table.buildTableFromGrammar(grammar);
        table.printActionTable();
        table.printGotoTable();
    }

////    tproc::analyzeTokens({""});
////    std::vector<std::map<std::string, std::string>> result;
//    std::vector<AnalysisResult> results;
//    analyzeTokens({"Николай", "медведь", "Камни", "Река"}, results);
//    for (auto &result : results) {
//        std::cout << "Tags for " << result.normalForm << std::endl;
//        for (auto &tag : result.tags) {
//            std::cout << tag << std::endl;
//        }
////        std::cout << result.tag.partOfSpeech << std::endl;
////        std::cout << result.tag.number << std::endl;
////        std::cout << result.tag.animacy << std::endl;
//    }

//    const UnicodeString inputText = "Сбрось оковы угнатателей! Присоединяйся к революции пролетариата! А ты еще не вступил в добровольцы??? Ну так вступай.";
//    tproc::Tokenizer tokenizer(inputText);
//    auto sentences = tokenizer.getSentences();
//    for (auto &sentence : sentences) {
//        for (auto &token : sentence) {
//            std::cout << token << std::endl;
//        }
//    }

    Py_Finalize();

    return 0;
}
