#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <unicode/unistr.h>
#include <vector>
#include <iostream>

namespace tproc {

struct Token {
    UnicodeString word;
    UnicodeString normalForm;
    std::vector<UnicodeString> tags;

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

