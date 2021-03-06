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
#include <unicode/ucnv.h>
#include <iostream>
#include <boost/program_options.hpp>
#include <fstream>

#include "grammar.h"
#include "lr0-items.h"
#include "parser-table.h"
#include "morph-analyzer.h"
#include "tokenizer.h"
#include "parser.h"
#include "utils/logger.h"
#include "g-parser-driver.hpp"
#include "fields-extractor.hpp"

namespace po = boost::program_options;

struct Arguements {
    std::string grammarFilename;
    std::string inputTextFilename;
};

static bool processArgs(int argc, char *argv[], Arguements &result) {

    po::options_description desc {"Program options"};
    desc.add_options()("help,h", "produce help message");
    desc.add_options()("grammar,g", po::value<std::string>(), "grammar filename");
    desc.add_options()("input-file,i", po::value<std::string>(), "text for parsing");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("grammar") == 0) {
        std::cerr << "Error: grammar filename was not set" << std::endl;
        return false;
    }
    if (vm.count("input-file") == 0) {
        std::cerr << "Error: input file was not set" << std::endl;
        return false;
    }

    result.grammarFilename = vm["grammar"].as<std::string>();
    result.inputTextFilename = vm["input-file"].as<std::string>();

    return true;
}

static bool readAllTextFromFile(const std::string &filename, UnicodeString &outText) {
    std::ifstream ifs(filename);
    if (ifs.fail()) {
        std::cerr << "Error: failed to open " << filename << std::endl;
        return false;
    }
    std::string text {std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};

    outText = UnicodeString(text.c_str());

    return true;
}

static void printUnicode(const UnicodeString &unistring) {
#ifdef WIN32_PLATFORM
    static char converted[100];
    int32_t converted_size = unistring.extract(0, unistring.length(), converted, sizeof(converted), "windows1251");
    converted[converted_size] = '\0';
    std::cout << converted;
#else
    std::cout << unistring;
#endif
}

struct UnicodePrinter {
    const UnicodePrinter &operator<<(const UnicodeString &ustring) const {
#ifdef WIN32_PLATFORM
        static char converted[100];
        int32_t converted_size = ustring.extract(0, ustring.length(), converted, sizeof(converted), "windows1251");
        converted[converted_size] = '\0';
        std::cout << converted;
#else
        std::cout << ustring;
#endif
        return *this;
    }
    
    const UnicodePrinter& operator<<(std::ostream &(*func)(std::ostream&)) const {
        func(std::cout);
        return *this;
    }
    
    static UnicodePrinter &get() {
        static UnicodePrinter uPrinter;
        return uPrinter;
    }
};

int main(int argc, char *argv[]) {
    ucnv_setDefaultName("UTF-8");
#ifdef WIN32_PLATFORM
    setlocale(LC_CTYPE, "rus");
#endif
    
    Py_Initialize();
    initmorph();

//    icu::UnicodeString ustring = "S = numr \"год\"\nS = numr \"день\"\nS = adjf MONTHSUMMER\nMONTHSUMMER = \"июнь\"\nMONTHSUMMER = \"июль\"\nMONTHSUMMER = \"август\"";
//    char *grammarFilename;
//    if (!processArgs(argc, argv, &grammarFilename)) {
//        std::cerr << "Program usage: text-processor [GRAMMAR FILENAME]" << std::endl;
//        return 0;
//    }

    Arguements args;
    if (!processArgs(argc, argv, args)) {
        return -1;
    }
//    UnicodeString inputText;
//    if (!readAllTextFromFile(args.inputTextFilename, inputText)) {
//        return -1;
//    }

//    UnicodeString inputText;
//    if (readAllTextFromFile(args.inputTextFilename, inputText)) {
//        tproc::MystemTokenizer tokenizer { inputText };
//        auto lemmaArray = tokenizer.getLemmaArray();
//        for (auto &lemma : lemmaArray) {
//            std::cout << lemma << std::endl;
//        }
//    }

    tproc::FieldsExtractor extractor {args.grammarFilename};
    auto result = extractor.extractFromFile(args.inputTextFilename);
    std::cout << "Extraction result:" << std::endl;
    if (result.size() == 0) {
        std::cout << "No fields extracted." << std::endl;
    } else {
        for (auto &resultRecord : result) {
            std::cout << "Field name: " << resultRecord.first << std::endl;// ", field value: " << resultRecord.second.fieldValue << std::endl;
            for (auto &fieldValueRecord : resultRecord.second) {
                //std::cout << "Field value: " << fieldValueRecord.fieldValue << std::endl;
                UnicodePrinter::get() << "Field value: " << fieldValueRecord.fieldValue << std::endl;
            }
        }
    }

//    tproc::GParserDriver parserDriver;
//    parserDriver.parse(args.grammarFilename);
//    auto rules = parserDriver.getRules();
//    for (auto &rule : rules) {
//        tproc::Logger::getLogger() << rule << std::endl;
//    }


//    tproc::Grammar grammar;
//    tproc::LR0ItemSetCollection itemSet;
//////    if (grammar.initFromPlainText(ustring)) {
//    if (grammar.initFromFile(args.grammarFilename)) {
////        grammar.printFirstSet();
////        grammar.printFollowSet();

////        itemSet.build(grammar);
////        auto items = itemSet.getItemSetCollection();
////        tproc::Logger::getLogger() << "Final itemset is:" << std::endl;
////        for (auto &item : items) {
////            tproc::Logger::getLogger() << "Incoming word is ";// << item.incomingWord->getRawValue() << std::endl;
////            item.incomingWord ? tproc::Logger::getLogger() << item.incomingWord->getRawValue() :
////                                                              tproc::Logger::getLogger() << "empty";
////            tproc::Logger::getLogger() << std::endl;
////            tproc::Logger::getLogger() << "State number is:" << item.itemsetIndex << std::endl;
////            tproc::Logger::getLogger() << item << "\n\n";
////        }

//        tproc::ParserTable table;
//        table.buildTableFromGrammar(grammar);
//        table.printActionTable();
//        table.printGotoTable();

////        const UnicodeString inputText = "Сегодня Алексей Михайлович купил Новую машину. Это отличная тачка!! Больше не придется ездить на общественном транспорте.";
////        const UnicodeString inputText = "Алексей \"игрок\" в покер.";
////////        const UnicodeString inputText = "черный седан.";
//        tproc::Parser parser(grammar, table);
//        tproc::Tokenizer tokenizer(inputText);
//        auto sentences = tokenizer.getSentences();
//        for (auto &sentence : sentences) {
//            std::vector<std::pair<UnicodeString, int>> resultChains;
//            parser.tryParse(sentence, resultChains);
//            for (auto &chain : resultChains) {
//                std::cout << "Parser result:" << std::endl;
//                std::cout << chain.first << std::endl;
//            }
//        }
//////        parser.tryParse()
//    }

////    tproc::analyzeTokens({""});
////    std::vector<std::map<std::string, std::string>> result;
//    std::vector<AnalysisResult> results;
//    std::vector<std::string> tokens { "Николай", "Сергеевич", "Линецкий", "медведь", "Камни", "Река" };
//    analyzeTokens(tokens, results);
//    for (auto &result : results) {
//       tproc::Logger::getLogger() << "Tags for " << result.normalForm << std::endl;
//        for (auto &tag : result.tags) {
//            tproc::Logger::getLogger() << "Tag: " << tag << std::endl;
//        }
//        Logger::getLogger() << result.tag.partOfSpeech << std::endl;
//        Logger::getLogger() << result.tag.number << std::endl;
//        Logger::getLogger() << result.tag.animacy << std::endl;
//    }
//    const UnicodeString inputText = "Тридцатого июня был мой первый рабочий день.";

//    const UnicodeString inputText = "Красный внедорожник выехал на трассу.";

//    inputText = "Имя его Николай Сергеевич Линецкий";
//    tproc::Tokenizer tokenizer(inputText);
//    auto sentences = tokenizer.getSentences();
//    for (auto &sentence : sentences) {
//        for (auto &token : sentence) {
//            tproc::Logger::getLogger() << token << std::endl;
//        }
//    }

    Py_Finalize();

    return 0;
}
