#ifndef GRAMMAR_WORDS_FACTORY_HPP_
#define GRAMMAR_WORDS_FACTORY_HPP_

#include <map>
#include <algorithm>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <unicode/unistr.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include "predicate.hpp"
//#include "grammar-rule.h"

// http://stackoverflow.com/questions/15858141/conveniently-declaring-compile-time-strings-in-c/15859077#15859077
//#define GET_STR_AUX(_, i, str) (sizeof(str) > (i) ? str[(i)] : 0),
//#define GET_STR(str) BOOST_PP_REPEAT(64,GET_STR_AUX,str) 0

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

namespace tproc {

class GRuleWord;
using GRuleWordPtr = std::shared_ptr<GRuleWord>;

enum class ReservedWord {
    NAME,
    SURNAME,
    PATR,
    INIT,
    ANYWORD,
    NOUN,
    ADJF,
    COMP,
    VERB,
    INFN,
    NUMR,
    GRND,
    PRTS,
    PRTF,
    ADVB,
    NPRO,
    PREP,
    CONJ,
    PRCL,
    INTJ,
    NUM,
    GEOX,
    MONTH,
    EMPTY,
    END_OF_INPUT,
    NONE
};

class GWordStorage {
public:

    static GRuleWordPtr getNonTerminal(const UnicodeString &name);
    static GRuleWordPtr getTerminal(const UnicodeString &name);
    static GRuleWordPtr getTerminal(const UnicodeString &name, const std::vector<PredicatePtr> &predicates);
    static GRuleWordPtr getEmptyTerminal();
    static GRuleWordPtr getEOITerminal();
    static GRuleWordPtr getNumTerminal();

    static UnicodeString &getReservedWord(ReservedWord reservedWord) { return reservedWords[reservedWord]; }
private:
    static std::map<UnicodeString, GRuleWordPtr> nterms;
    static std::map<UnicodeString, std::vector<GRuleWordPtr>> terms;
    static std::unordered_map<ReservedWord, UnicodeString> reservedWords;
};

//namespace GWordFactory {

////template<class StdStringType>
////GRuleWordPtr produceNonTerminal(const StdStringType &str) {
////    return produceNonTerminal<GET_STR(str.c_str())>();
////}

////template<UnicodeString>
//GRuleWordPtr produceNonTerminal(const UnicodeString &str) {
//    std::string utf8_str;
//    str.toUTF8String(utf8_str);
//    return produceNonTerminal<GET_STR(utf8_str.c_str())>();
//}

//template<char...name>
//GRuleWordPtr produceNonTerminal() {
//    static GRuleWordPtr result = std::make_shared<NonTerminal>(UnicodeString { name... });
//    return result;
//}

//template<char...name>
//GRuleWordPtr produceTerminal() {
//    static GRuleWordPtr result = std::make_shared<Terminal>(UnicodeString { name... });
//    return result;
//}

//template <char...name>
//GRuleWordPtr produceTerminal(const std::vector<PredicatePtr> &predicates) {
//    static std::set<GRuleWordPtr> terminals;
//    auto foundTerm = std::find_if(terminals.begin(), terminals.end(), [&predicates](const GRuleWordPtr &term){
//        if (term->getPredicates().size() != predicates.size()) {
//            return false;
//        }
//        unsigned long hash_sum_term, hash_sum_pred;
//        for (int i = 0; i < predicates.size(); ++i) {
//            hash_sum_pred += predicates[i]->hash();
//            hash_sum_term += predicates[i]->hash();
//        }
//        if (hash_sum_pred != hash_sum_term) {
//            return false;
//        }
//        return true;
//    });
//    if (foundTerm != terminals.end()) {
//        return  *foundTerm;
//    } else {
//        auto result = std::make_shared<Terminal>(UnicodeString {name...}, predicates);
//        terminals.insert(result);
//        return result;
//    }
//}

//}

}

#endif //GRAMMAR_WORDS_FACTORY_HPP_
