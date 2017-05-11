#include "g-parser-driver.hpp"

#include <fstream>
#include <sstream>

#include "gparser.tab.hh"
#include "g-scanner.hpp"

namespace tproc {

void GParserDriver::appendRule(ComplexGrammarRule &rule) {
    this->rules.push_back(std::move(rule));
}

GRuleWord GParserDriver::makeRuleWord(UnicodeString &rawValue, std::vector<std::shared_ptr<Property>> &props) {
    unsigned mask = 0;
    int minRep = 1;
    int maxRep = 1;
    for (auto &prop : props) {
        std::shared_ptr<SimpleProperty> simpleProp { nullptr };
        std::shared_ptr<ComplexNumProperty> complexNumProp { nullptr };
        if ((simpleProp  = std::dynamic_pointer_cast<SimpleProperty>(prop)) != nullptr) {
            mask |= static_cast<unsigned>(simpleProp->propType);
        } else if ((complexNumProp = std::dynamic_pointer_cast<ComplexNumProperty>(prop)) != nullptr) {
            mask |= static_cast<unsigned>(complexNumProp->propType);
            switch (complexNumProp->propType) {
            case GRuleWordPropType::MAX_REP:
                maxRep = complexNumProp->propValue;
                break;
            case GRuleWordPropType::MIN_REP:
                minRep = complexNumProp->propValue;
                break;
            default:
                break;
            }
        }
    }

    return GRuleWord { rawValue, mask, minRep, maxRep };
}

bool GParserDriver::parse(const std::string &fname) {
    std::ifstream ifs { fname };
    return parseHelper(ifs);
}

bool GParserDriver::parse(const char *const filename) {
    std::ifstream ifs { filename };
    return parseHelper(ifs);
}

bool GParserDriver::parse(std::istream &iss) {
    return parseHelper(iss);
}

bool GParserDriver::parseHelper(std::istream &iss) {

    this->scanner = std::make_shared<GScanner>( &iss );

    this->parser = std::make_shared<GParser>(*scanner, *this);

    const int accept( 0 );
    if( parser->parse() != accept )
    {
       std::cerr << "Parse failed!!\n";
       return false;
    }

    return true;
}

}

