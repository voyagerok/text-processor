#include <Python.h>
#include <iostream>
#include <unicode/ustream.h>

#include "tokenizer.h"
#include "utils/string-helper.h"
#include "morph-analyzer.h"
#include "utils/converter.h"

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

bool Token::operator==(const Token &other) {
    if (this->word != other.word) {
        return false;
    }
    if (this->normalForm != other.normalForm) {
        return false;
    }
    if (this->tags.size() != other.tags.size()) {
        return false;
    }
    for (int i = 0; i < this->tags.size(); ++i) {
        if (this->tags[i] != other.tags[i]) {
            return false;
        }
    }

    return true;
}

bool Token::operator!=(const Token &other) {
    return !(*this == other);
}


std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << "Original word: " << token.word << ", normal form: " << token.normalForm << ", tags: ";
    for (int i = 0; i < token.tags.size(); ++i) {
        os << token.tags[i];
        if (i < token.tags.size() - 1) {
            os << ", ";
        }
    }

    return os;
}

Tokenizer::Tokenizer(const UnicodeString &plainText) {
    std::vector<UnicodeString> plainSentences;
    split_unistring(plainText, {"\\?+","\\.+","\\!+"}, plainSentences);
    for (auto &plainSentence : plainSentences) {
//        std::cout << plainSentence << std::endl;
        std::vector<UnicodeString> plainTokens;
        split_unistring(plainSentence, {"\\s+","\\;+","\\:+","\\,+"}, plainTokens);
        Sentence currentSentence;
        for (auto &plainToken : plainTokens) {
//            std::cout << plainToken << std::endl;
            std::vector<AnalysisResult> analysisResults;
            analyzeTokens({stdStringFromUnistr(plainToken)}, analysisResults);
            if (analysisResults.size() > 0) {
                AnalysisResult &firstResult = analysisResults[0];
                Token currentToken;
                currentToken.normalForm = UnicodeString(firstResult.normalForm.c_str());
                for (auto &tag : firstResult.tags) {
                    currentToken.tags.push_back(UnicodeString(tag.c_str()).toLower());
                }
                currentToken.word = plainToken;
                currentSentence.push_back(std::move(currentToken));
            }
        }

        this->sentences.push_back(std::move(currentSentence));
    }
}

}

