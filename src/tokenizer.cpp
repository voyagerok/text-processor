#include <Python.h>
#include <iostream>
#include <unicode/ustream.h>
#include <map>
#include <memory>
#include <unordered_set>

#include "tokenizer.h"
#include "utils/string-helper.h"
#include "morph-analyzer.h"
#include "utils/converter.h"
#include "utils/logger.h"
#include "unicode/regex.h"

namespace tproc {

//static const UnicodeString sentenceDelims = "[\\?+,\\.+,\\!+]";

//static Token buildTokenFromAnalysisResult(const AnalysisResult &result) {
//    Token token;
//    token.normalForm = UnicodeString(result.normalForm.c_str());
//    for (auto &tag : result.tags) {
//        token.tags.push_back(UnicodeString(tag.c_str()));
//    }

//    return token;
//}

//std::ostream &operator<<(std::ostream &os, MorphProperty propMa) {
////    switch (nameChar) {
////    case MorphNameChar::FIRST_NAME:
////        os << "First name";
////        break;
////    case MorphNameChar::INIT:
////        os << "Initial";
////        break;
////    case MorphNameChar::NONE:
////        os << "None";
////        break;
////    case MorphNameChar::PATR:
////        os << "Patr";
////        break;
////    case MorphNameChar::SECOND_NAME:
////        os << "Second name";
////        break;
////    }
//    if ((name))

//    return os;
//}

bool Token::operator==(const Token &other) {
    if (this->word != other.word) {
        return false;
    }
    if (this->normalForm != other.normalForm) {
        return false;
    }
//    if (this->tags.size() != other.tags.size()) {
//        return false;
//    }
//    for (int i = 0; i < this->tags.size(); ++i) {
//        if (this->tags[i] != other.tags[i]) {
//            return false;
//        }
//    }
    if (this->partOfSpeech != other.partOfSpeech) {
        return false;
    }
    if (this->propMask != other.propMask) {
        return false;
    }

    return true;
}

bool Token::operator!=(const Token &other) {
    return !(*this == other);
}


std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << "Original word: " << token.word << ", normal form: " << token.normalForm
       << "Part of speech: " << token.partOfSpeech << ", morph char: "; //<< token.nameCharacteristic;

    if ((token.propMask & MorphProperty::FIRST_NAME)) {
        os << "FirstName ";
    }
    if ((token.propMask & MorphProperty::INIT)) {
        os << "Init ";
    }
    if ((token.propMask & MorphProperty::PATR)) {
        os << "Patr ";
    }
    if ((token.propMask & MorphProperty::SECOND_NAME)) {
        os << "SecondName ";
    }
    if ((token.propMask & MorphProperty::GEOX)) {
        os << "GEOX";
    }
    if ((token.propMask & MorphProperty::NUMB)) {
        os << "Number";
    }

    return os;
}

struct DictChecker {
    DictChecker() {
        UErrorCode status = U_ZERO_ERROR;
        monthRegexMatcher = new RegexMatcher(monthRegexPattern, UREGEX_CASE_INSENSITIVE, status);
        if (U_FAILURE(status)) {
            throw std::runtime_error("Failed to initialize month matching regex");
        }

        numbeRegexMatcher = new RegexMatcher(numberRegexPattern, 0, status);
        if (U_FAILURE(status)) {
            throw std::runtime_error("Failed to initialize number matching regex");
        }
    }

    ~DictChecker() {
        delete monthRegexMatcher;
    }

    enum MatchFlags {
        MONTH = 01,
        NUMBER = 02
    };

    unsigned int check(const UnicodeString &token) {
        unsigned result = 0;
        UErrorCode status = U_ZERO_ERROR;

        monthRegexMatcher->reset(token);
        if (monthRegexMatcher->matches(status)) {
            result |= MatchFlags::MONTH;
        }

        numbeRegexMatcher->reset(token);
        if (numbeRegexMatcher->matches(status)) {
            result |= MatchFlags::NUMBER;
        }

        return result;
    }

    DictChecker(const DictChecker&) = delete;
    DictChecker &operator=(const DictChecker&) = delete;

    static DictChecker &sharedInstance() {
        static DictChecker checker;
        return checker;
    }

private:
    const char *monthRegexPattern = "январь|февраль|март|апрель|май|июнь|июль|август|сентябрь|октябрь|ноябрь|декабрь";
    RegexMatcher *monthRegexMatcher;

    const char *numberRegexPattern = "[0-9]+";
    RegexMatcher *numbeRegexMatcher = nullptr;
};

Tokenizer::Tokenizer(const UnicodeString &plainText) {

    std::vector<UnicodeString> plainSentences;
//    split_unistring(plainText, {"\\?","\\.","\\!"}, plainSentences);
    split_unistring(plainText, {"\n"}, plainSentences);
    Token endOfInput {"$", "$" };
    for (auto &plainSentence : plainSentences) {
        Logger::getLogger() << plainSentence << std::endl;
        std::vector<UnicodeString> plainTokens;
        split_unistring(plainSentence, {"\\s",
                                        "\\;",
                                        "\\:",
                                        "\\,",
                                        "\\(",
                                        "\\)",
                                        "\\<",
                                        "\\>",
                                        "\\{",
                                        "\\}"},
                        plainTokens);
        auto leftAngleBracket = UnicodeString::fromUTF8("\xc2\xab");
        auto rightAngleBracket = UnicodeString::fromUTF8("\xc2\xbb");
        for (auto &token : plainTokens) {
            token.findAndReplace(leftAngleBracket, "\"");
            token.findAndReplace(rightAngleBracket, "\"");
        }

        Logger::getLogger() << "PlainTokens Count: " << plainTokens.size() << std::endl;
//        split_unistring(plainSentence, {"\\s"}, plainTokens);
        Sentence currentSentence;

        std::vector<std::pair<UnicodeString, std::vector<std::shared_ptr<AnalysisResult>>>> analysisResults;
        analyzeTokens(plainTokens, analysisResults);

        Logger::getLogger() << "Analysis result Count: " << analysisResults.size() << std::endl;

        for (auto &resultsForWord : analysisResults) {
            if (resultsForWord.second.empty()) {
                continue;
            }
            Token currentToken;
            for (auto &result : resultsForWord.second) {
//                if ((result->nameCharMask & MorphProperty::FIRST_NAME)) {
//                    currentToken.propMask |= MorphProperty::FIRST_NAME;
//                    break;
//                } else if (result->isName) {
//                    currentToken.nameCharacteristic = MorphNameChar::FIRST_NAME;
//                    break;
//                } else if (result->isPatr) {
//                    currentToken.nameCharacteristic = MorphNameChar::PATR;
//                    break;
//                } else if (result->isSurname) {
//                    currentToken.nameCharacteristic = MorphNameChar::SECOND_NAME;
//                    break;
//                }
                currentToken.propMask |= result->nameCharMask;
            }

//            unsigned checkResult = DictChecker::sharedInstance().check(resultsForWord)

            currentToken.word = std::move(resultsForWord.first);
            currentToken.normalForm = std::move(resultsForWord.second[0]->normalForm.toLower());
            currentToken.partOfSpeech = std::move(resultsForWord.second[0]->partOfSpeech.toLower());
            // for hint words matching
            currentToken.pureTokenNormal = currentToken.normalForm;
            currentToken.pureTokenNormal.findAndReplace("\"", "");
            currentToken.pureToken = currentToken.word;
            currentToken.pureToken.findAndReplace("\"", "");

            unsigned checkResult = DictChecker::sharedInstance().check(currentToken.pureTokenNormal);
            if ((checkResult & DictChecker::MatchFlags::MONTH)) {
                currentToken.propMask |= MorphProperty::MONTH;
            }
            if ((checkResult & DictChecker::MatchFlags::NUMBER)) {
                currentToken.propMask |= MorphProperty::NUMB;
            }

            Logger::getLogger() << "Current token: " << currentToken << std::endl;

            currentSentence.push_back(currentToken);

        }
//        for (auto &plainToken : plainTokens) {
////            Logger::getLogger() << plainToken << std::endl;
////            std::vector<AnalysisResult> analysisResults;
////            analyzeTokens({stdStringFromUnistr(plainToken)}, analysisResults);
////            if (analysisResults.size() > 0) {
////                AnalysisResult &firstResult = analysisResults[0];
////                Token currentToken;
////                currentToken.normalForm = UnicodeString(firstResult.normalForm.c_str()).findAndReplace("ё","е");
////                for (auto &tag : firstResult.tags) {
////                    currentToken.tags.push_back(UnicodeString(tag.c_str()).toLower());
////                }
////                Logger::getLogger() << "Token normal form is " << currentToken.normalForm << std::endl;
////                if (currentToken.tags.size() > 0) {
////                    Logger::getLogger() << "Token tag is " << currentToken.tags.at(0) << std::endl;
////                }
////                currentToken.word = plainToken;
////                currentSentence.push_back(std::move(currentToken));
////            }
//        }
        //currentSentence.push_back({"$","$",{"none"}});
        currentSentence.push_back(endOfInput);

        this->sentences.push_back(std::move(currentSentence));
    }
}

}

