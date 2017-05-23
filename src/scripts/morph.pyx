# -*- coding: utf-8 -*-

from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp.map cimport map
from libcpp cimport bool
from libcpp.memory cimport shared_ptr
from libcpp.memory cimport make_shared
from libcpp.utility cimport pair

from cython.operator cimport dereference as deref

import pymorphy2

cdef extern from "<utility>" namespace "std":
    cdef T&& std_move "std::move" [T](T&& t)

cdef extern from "<unicode/unistr.h>":
    cdef cppclass UnicodeString:
        UnicodeString(const char *codepageData)
        T& toUTF8String[T](T &result) const

cdef extern from "tokenizer.h" namespace "tproc":
    cdef enum MorphProperty:
        FIRST_NAME,
        SECOND_NAME,
        PATR,
        INIT,
        GEOX,
        NUMB

cdef extern from "<utility>" namespace "std":
    cdef pair[T1,T2] make_pair[T1,T2](T1 t1, T2 t2)

cdef public cppclass AnalysisResult:
    UnicodeString normalForm
    UnicodeString partOfSpeech
    unsigned int nameCharMask

morph = pymorphy2.MorphAnalyzer()

cdef analyzeToken(const UnicodeString tok, vector[pair[UnicodeString, vector[shared_ptr[AnalysisResult]]]] &analysis_results):
    cdef vector[shared_ptr[AnalysisResult]] res_for_tok    
    cdef shared_ptr[AnalysisResult] analysis_res
   
    cdef string utf8_str
    tok.toUTF8String(utf8_str)
    morph_results = morph.parse((utf8_str.c_str()).decode('UTF-8'))

    #print(morph_results)

    cdef unsigned propMask
    for morph_result in morph_results:
        if morph_result.score < 0.1:
            continue
        analysis_res = make_shared[AnalysisResult]()
        deref(analysis_res).normalForm = UnicodeString(morph_result.normal_form.encode('UTF-8'))
        propMask = 0 
        if morph_result.tag.POS is not None:
            deref(analysis_res).partOfSpeech = UnicodeString(morph_result.tag.POS.encode('UTF-8'))
        if 'Name' in morph_result.tag:
            propMask |= FIRST_NAME
        if 'Patr' in morph_result.tag:
            propMask |= PATR
        if 'Surn' in morph_result.tag:
            propMask |= SECOND_NAME
        if 'Init' in morph_result.tag:
            propMask |= INIT
        if 'Geox' in morph_result.tag:
            propMask |= GEOX
        if 'NUMB' in morph_result.tag:
            propMask |= NUMB
        deref(analysis_res).nameCharMask = propMask
        res_for_tok.push_back(analysis_res)
    #analysis_results[tok] = res_for_tok
    analysis_results.push_back(pair[UnicodeString, vector[shared_ptr[AnalysisResult]]](tok, res_for_tok))

cdef public analyzeTokens(const vector[UnicodeString] &tokens, vector[pair[UnicodeString, vector[shared_ptr[AnalysisResult]]]] &analysis_results):
    for i in xrange(tokens.size()):
        analyzeToken(tokens[i], analysis_results)
