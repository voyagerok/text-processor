# -*- coding: utf-8 -*-

from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp.map cimport map

import pymorphy2

cdef public cppclass Tag:
    string partOfSpeech
    string animacy
    string morphCase
    string gender
    string number

cdef public cppclass AnalysisResult:
    Tag tag
    string normalForm

cdef public analyzeTokens(const vector[string] &tokens, vector[AnalysisResult] &analysis_results):
    morph = pymorphy2.MorphAnalyzer()
    cdef map[string,string] token_results
    cdef AnalysisResult result
    for i in xrange(tokens.size()):
        morph_result = morph.parse((tokens[i].c_str()).decode('UTF-8'))
        #print(morph_result)
        for res in morph_result:
            #print(res.tag.number)
            result.normalForm = res.normal_form.encode('UTF-8')
            if res.tag.number is None:
                result.tag.number = ""
            else:
                result.tag.number = res.tag.number
            if res.tag.POS is None:
                result.tag.partOfSpeech = ""
            else:
                result.tag.partOfSpeech = res.tag.POS
            if res.tag.animacy is None:
                result.tag.animacy = ""
            else:
                result.tag.animacy = res.tag.animacy
            if res.tag.case is None:
                result.tag.morphCase = ""
            else:
                result.tag.morphCase = res.tag.case
            if res.tag.gender is None:
                result.tag.gender = ""
            else:
                result.tag.gender = res.tag.gender
            if res.tag.number is None:
                result.tag.number
            else:
                result.tag.number = res.tag.number
            analysis_results.push_back(result)
