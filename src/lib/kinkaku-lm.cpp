/*
** Kinkaku - Text Mining Analysis Tools
**
** Copyright (c) 2013, stnmrshx (stnmrshx@gmail.com)
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification, 
** are permitted provided that the following conditions are met: 
** 
** 1. Redistributions of source code must retain the above copyright notice, this
**    list of conditions and the following disclaimer. 
** 2. Redistributions in binary form must reproduce the above copyright notice,
**    this list of conditions and the following disclaimer in the documentation
**    and/or other materials provided with the distribution. 
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
** ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**/
#include <kinkaku/kinkaku-lm.h>
#include <kinkaku/kinkaku-util.h>
#include <iostream>
#include <cmath>

using namespace kinkaku;
using namespace std;

bool addCount(KinkakuDoubleMap & myMap, const KinkakuString & str) {
    KinkakuDoubleMap::iterator it = myMap.find(str);
    if(it == myMap.end()) {
        myMap.insert(pair<KinkakuString,double>(str,1.0));
        return false;
    } 
    it->second++;
    return true;
}

void KinkakuLM::train(const std::vector<KinkakuString> & corpus) {
    KinkakuDoubleMap denominators;
    for(unsigned i = 0; i < corpus.size(); i++) {
        KinkakuString trainString(corpus[i].length()+n_);
        for(unsigned j = 0; j < n_-1; j++)
            trainString[j] = 0;
        trainString[trainString.length()-1] = 0;
        trainString.splice(corpus[i],n_-1);
        for(unsigned j = n_; j < trainString.length(); j++) {
            for(unsigned len = n_; len > 0; len--) {
                KinkakuString fbString = trainString.substr(j-len,len-1);
                addCount(denominators,fbString);
                if(addCount(probs_, trainString.substr(j-len, len)))
                    break;
                else
                    addCount(fallbacks_,fbString);
            }
        }
    }
    vector<unsigned> oneCounts(n_,0), twoCounts(n_,0), allCounts(n_,0);
    for(KinkakuDoubleMap::const_iterator it = probs_.begin(); it != probs_.end(); it++) {
        unsigned n = it->first.length()-1;
        if(it->second == 1.0)
            oneCounts[n]++;
        else if(it->second == 2.0)
            twoCounts[n]++;
        allCounts[n]++;
    }
    vector<double> discounts(n_,0);
    for(unsigned i = 0; i < n_; i++) {
        if(oneCounts[i] * twoCounts[i] == 0) {
            cerr << "WARNING: Setting discount["<<i<<"] to 0.5 for lack of training data" << endl;
            discounts[i] = 0.5;
        }
        else
            discounts[i] = (double)oneCounts[i]/(oneCounts[i]+twoCounts[i]*2.0);
    }
    for(KinkakuDoubleMap::iterator it = probs_.begin(); it != probs_.end(); it++) {
        unsigned n = it->first.length()-1;
        it->second = log((it->second-discounts[n])/denominators[it->first.substr(0,n)]);
    }
    for(KinkakuDoubleMap::iterator it = fallbacks_.begin(); it != fallbacks_.end(); it++)
        it->second = log((it->second*discounts[it->first.length()])/denominators[it->first]);
}

double KinkakuLM::scoreSingle(const KinkakuString & val, int pos) {
    KinkakuString ngram(n_);
    for(unsigned i = 0; i < n_; i++) ngram[i] = 0;
    int npos = n_;
    if((int)val.length() == pos) {
        npos--;
        pos--;
    }
    while(--npos >= 0 && pos >= 0)
        ngram[npos] = val[pos--];
    double prob = 0;
    for(npos = 0; npos < (int)n_; npos++) {
        KinkakuDoubleMap::const_iterator it = probs_.find(ngram.substr(npos));
        if(it != probs_.end()) {
            prob += it->second;
            return prob;
        } else {
            it = fallbacks_.find(ngram.substr(npos, n_-npos-1));
            if(it != fallbacks_.end())
                prob += it->second;
        }
    }
    return prob + log(1.0/vocabSize_);
}

double KinkakuLM::score(const KinkakuString& val) const {
    unsigned j, len;
    double prob = 0;
    KinkakuString testString(val.length()+n_);
    for(j = 0; j < n_-1; j++)
        testString[j] = 0;
    testString[testString.length()-1] = 0;
    testString.splice(val,n_-1);
    for(j = n_; j < testString.length(); j++) {
        for(len = n_; len > 0; len--) {
            KinkakuDoubleMap::const_iterator it = const_cast<KinkakuDoubleMap*>(&probs_)->find(testString.substr(j-len, len));
            if(it != probs_.end()) {
                prob += it->second;
                break;
            } else {
                it = const_cast<KinkakuDoubleMap*>(&fallbacks_)->find(testString.substr(j-len, len-1));
                if(it != fallbacks_.end())
                    prob += it->second;
            }
        }
        if(n_ == 0)
            prob += log(1.0/vocabSize_);
    }
    return prob;
}

void KinkakuLM::checkEqual(const KinkakuLM & rhs) const {
    if(n_ != rhs.n_)
        THROW_ERROR("KinkakuLM n_ don't match: " << n_ << " != " << rhs.n_);
    if(vocabSize_ != rhs.vocabSize_)
        THROW_ERROR("KinkakuLM vocabSize_ don't match: " << vocabSize_ << " != " << rhs.vocabSize_);
    checkMapEqual(probs_, rhs.probs_);
    checkMapEqual(fallbacks_, rhs.fallbacks_);
}