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
#include "parser.h"
#include "utils/logger.h"

/*
 * Грамемы:
 * NOUN
 * ADJF - прилагательное полное
 * ADJS - прилагательное краткое
 * COMP - сравнительная форма
 * VERB - глагол (лич форма)
 * INFN - глагол (инфинитив)
 * NUMR - числительное
 * GRND - деепричастие
 * PRTS - причастие краткое
 * PRTF - причастие полное
 * ADVB - наречие
 * NPRO - местоимение (сущ)
 * PREP - предлог
 * CONJ - союз
 * PRCL - частица
 * INTJ - междометие
 * */


int main(void) {
    Py_Initialize();
    initmorph();

    icu::UnicodeString ustring = "S = numr \"год\"\nS = numr \"день\"\nS = adjf MONTHSUMMER\nMONTHSUMMER = \"июнь\"\nMONTHSUMMER = \"июль\"\nMONTHSUMMER = \"август\"";
    tproc::Grammar grammar;
    tproc::LR0ItemSetCollection itemSet;
    if (grammar.initFromPlainText(ustring)) {
        itemSet.build(grammar);
//        auto items = itemSet.getItemSetCollection();
//        Logger::getLogger() << "Final itemset is:" << std::endl;
//        for (auto &item : items) {
//            Logger::getLogger() << "Incoming word is " << item.incomingWord << std::endl;
//            Logger::getLogger() << "State number is:" << item.itemsetIndex << std::endl;
//            Logger::getLogger() << item << "\n\n";
//        }

//        grammar.printFirstSet();
//        grammar.printFollowSet();

        tproc::ParserTable table;
        table.buildTableFromGrammar(grammar);
        table.printActionTable();
        table.printGotoTable();

        const UnicodeString inputText = "Тридцатого июня был мой первый рабочий день. В тот же день мне исполнлось тридцать лет.";
//        const UnicodeString inputText = "черный седан.";
        tproc::Parser parser(grammar, table);
        tproc::Tokenizer tokenizer(inputText);
        auto sentences = tokenizer.getSentences();
        for (auto &sentence : sentences) {
            std::vector<UnicodeString> resultChains;
            parser.tryParse(sentence, resultChains);
            for (auto &chain : resultChains) {
                std::cout << "Parser result:" << std::endl;
                std::cout << chain << std::endl;
            }
        }
//        parser.tryParse()
    }

////    tproc::analyzeTokens({""});
////    std::vector<std::map<std::string, std::string>> result;
//    std::vector<AnalysisResult> results;
//    analyzeTokens({"Николай", "медведь", "Камни", "Река"}, results);
//    for (auto &result : results) {
//        Logger::getLogger() << "Tags for " << result.normalForm << std::endl;
//        for (auto &tag : result.tags) {
//            Logger::getLogger() << tag << std::endl;
//        }
////        Logger::getLogger() << result.tag.partOfSpeech << std::endl;
////        Logger::getLogger() << result.tag.number << std::endl;
////        Logger::getLogger() << result.tag.animacy << std::endl;
//    }
//    const UnicodeString inputText = "Тридцатого июня был мой первый рабочий день.";

//    const UnicodeString inputText = "Красный внедорожник выехал на трассу.";
//    tproc::Tokenizer tokenizer(inputText);
//    auto sentences = tokenizer.getSentences();
//    for (auto &sentence : sentences) {
//        for (auto &token : sentence) {
//            Logger::getLogger() << token << std::endl;
//        }
//    }

    Py_Finalize();

    return 0;
}
