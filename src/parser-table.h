#ifndef PARSER_TABLE_H_
#define PARSER_TABLE_H_

#include <unicode/unistr.h>
#include <map>
#include <vector>

namespace tproc {

class Grammar;

enum class ActionName {SHIFT, REDUCE, ACCEPT, ERROR};

struct ParserAction {
    ActionName actionName;
    int actionStep;
};

class ParserTable {
public:
    bool buildTableFromGrammar(const Grammar &grammar);
private:
    std::vector<std::map<UnicodeString, ParserAction>> actionTable;
    std::vector<std::map<UnicodeString, int>> gotoTable;
};

}

#endif //PARSER_TABLE_H_

