#ifndef G_PARSER_H_
#define G_PARSER_H_

#include <string>
#include <cstddef>
#include <istream>
#include <unicode/unistr.h>
#include <memory>

#include "grammar-rule.h"

namespace tproc {

class GParser;
class GScanner;

//enum class PropertyType {
//    PROP_QUOTED,
//    PROP_START_UPPER,
//    PROP_MIN_REP,
//    PROP_MAX_REP
//};

struct Property {
    virtual ~Property() {}
};

struct SimpleProperty: public Property {
    SimpleProperty(GRuleWordPropType propType): propType { propType } {}
    GRuleWordPropType propType;
};

struct ComplexNumProperty: public Property {
    ComplexNumProperty(GRuleWordPropType propType, int propValue): propType { propType }, propValue { propValue } {}
    GRuleWordPropType propType;
    int propValue;
};

class GParserDriver {
public:
    GParserDriver() = default;
    virtual ~GParserDriver() = default;

    std::vector<ComplexGrammarRule> &getRules() { return this->rules; }

    bool parse( const char * const filename );
    bool parse( std::istream &iss );
    bool parse(const std::string &fname);

    void appendRule(ComplexGrammarRule &rule);
    GRuleWord makeRuleWord(UnicodeString &rawValue, std::vector<std::shared_ptr<Property>> &props);
private:
    bool parseHelper(std::istream &iss);

    std::vector<ComplexGrammarRule> rules;

    std::shared_ptr<GParser> parser { nullptr };
    std::shared_ptr<GScanner> scanner { nullptr };
};

}

#endif //G_PARSER_H_
