# -*- coding: utf-8 -*-

from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp.map cimport map

import pymorphy2

'''
cdef public cppclass Tag:
    string partOfSpeech
    string animacy
    string morphCase
    string gender
    string number
'''

cdef public cppclass AnalysisResult:
    vector[string] tags
    string normalForm

cdef public analyzeTokens(const vector[string] &tokens, vector[AnalysisResult] &analysis_results):
    morph = pymorphy2.MorphAnalyzer()
    #cdef map[string,string] token_results
    cdef AnalysisResult result
    for i in xrange(tokens.size()):
        morph_result = morph.parse((tokens[i].c_str()).decode('UTF-8'))
        #print(morph_result)
        for res in morph_result:
            #print(res.tag.number)
            result.normalForm = res.normal_form.encode('UTF-8')
            result.tags.clear()
            '''
            if res.tag.number is not None:
                result.tags.push_back(res.tag.number)
            '''
            if res.tag.POS is not None:
                result.tags.push_back(res.tag.POS)
            '''
            if res.tag.animacy is not None:
                result.tags.push_back(res.tag.animacy)
            if res.tag.case is not None:
                result.tags.push_back(res.tag.case)
            if res.tag.gender is not None:
                result.tags.push_back(res.tag.gender)
            if res.tag.number is not None:
                result.tags.push_back(res.tag.number)
            '''
            analysis_results.push_back(result)
