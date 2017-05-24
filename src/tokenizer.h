#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <unicode/unistr.h>
#include <vector>
#include <iostream>

namespace tproc {

enum MorphProperty {
    FIRST_NAME = 01,
    SECOND_NAME = 02,
    PATR = 04,
    INIT = 010,
    GEOX = 020,
    NUMB = 040,
    MONTH = 0100
};

//std::ostream &operator<<(std::ostream &os, MorphProperty nameChar);

struct Token {
    UnicodeString word;
    UnicodeString normalForm;
//    std::vector<UnicodeString> tags;
//    unsigned morphPropsMask;
    UnicodeString partOfSpeech;
    // normal form without quotes and other unwanted symbols (for hint words matching)
    UnicodeString pureTokenNormal;
    UnicodeString pureToken;
    unsigned propMask = 0;

    Token(UnicodeString &&word, UnicodeString &&norm) :
        word { std::move (word) }, normalForm { std::move(norm) } {}
    Token(const UnicodeString &word, const UnicodeString &norm) :
        word { word }, normalForm { norm } {}
    Token() = default;

    unsigned long hash() const;

    bool operator==(const Token &other);
    bool operator!=(const Token &other);
    friend std::ostream &operator<<(std::ostream &os, const Token &token);
};

class Tokenizer {
public:
    using Sentence = std::vector<Token>;

    Tokenizer(const UnicodeString &plainText);
    std::vector<Sentence> getSentences() { return sentences; }
private:
    std::vector<Sentence> sentences;
};

}

#endif //TOKENIZER_H_

