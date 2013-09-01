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
#include <kinkaku/feature-lookup.h>
#include <kinkaku/kinkaku-util.h>
#include <kinkaku/dictionary.h>
#include <algorithm>

using namespace kinkaku;
using namespace std;

FeatureLookup::~FeatureLookup() {
    if(charDict_) delete charDict_;
    if(typeDict_) delete typeDict_;
    if(selfDict_) delete selfDict_;
    if(dictVector_) delete dictVector_;
    if(biases_) delete biases_;
    if(tagDictVector_) delete tagDictVector_;
    if(tagUnkVector_) delete tagUnkVector_;
}

void FeatureLookup::addNgramScores(const Dictionary<FeatVec> * dict, const KinkakuString & str, int window, vector<FeatSum> & score) {
    if(!dict) return;
    Dictionary<FeatVec>::MatchResult res = dict->match(str);
    for(int i = 0; i < (int)res.size(); i++) {
        const int base_pos = res[i].first - window;
        const int start = max(0, -base_pos);
        const int end = min(window*2,(int)score.size()-base_pos);
        const FeatVec & vec = *res[i].second;
        for(int j = start; j < end; j++) {
            score[base_pos+j] += vec[j];
        }
    }
}

void FeatureLookup::addTagNgrams(const KinkakuString & chars, const Dictionary<FeatVec> * dict, vector<FeatSum> & scores, int window, int startChar, int endChar) {
    if(!dict) return;
    int myStart = max(startChar-window,0);
    int myEnd = min(endChar+window,(int)chars.length());
    KinkakuString str =  chars.substr(myStart, startChar-myStart) + chars.substr(endChar, myEnd-endChar);
    Dictionary<FeatVec>::MatchResult res = dict->match(str);
    int offset = window-(startChar-myStart);
    for(int i = 0; i < (int)res.size(); i++) {
        int pos = res[i].first + offset;
        pos = (window*2 - pos - 1) * scores.size();
        FeatVal* vec = &((*res[i].second)[pos]);
        for(int j = 0; j < (int)scores.size(); j++) {
#ifdef KINKAKU_SAFE
            if(j+pos >= (int)res[i].second->size() || j+pos < 0)
                THROW_ERROR("j+pos "<<j<<"+"<<pos<<" too big for res[i].second->size() "<<res[i].second->size()<<", window="<<window);
#endif
            scores[j] += vec[j];
        }
    }
}

void FeatureLookup::addSelfWeights(const KinkakuString & word, vector<FeatSum> & scores, int featIdx) {
#ifdef KINKAKU_SAFE
    if(selfDict_ == NULL) THROW_ERROR("Trying to add self weights when no self is present");
#endif
    FeatVec * entry = selfDict_->findEntry(word);
    if(entry) {
        int base = featIdx * scores.size();
        for(int i = 0; i < (int)scores.size(); i++)
            scores[i] += (*entry)[base+i];
    }
}

void FeatureLookup::addDictionaryScores(const Dictionary<ModelTagEntry>::MatchResult & matches, int numDicts, int max, vector<FeatSum> & score) {
    if(dictVector_ == NULL || dictVector_->size() == 0 || matches.size() == 0) return;
    const int len = score.size(), dictLen = len*3*max;
    vector<char> on(numDicts*dictLen, 0);
    int end;
    ModelTagEntry* myEntry;
    for(int i = 0; i < (int)matches.size(); i++) {
        end = matches[i].first;
        myEntry = matches[i].second;
        if(myEntry->inDict == 0)
            continue;
        const int wlen = myEntry->word.length();
        const int lablen = min(wlen,max)-1;
        for(int di = 0; ((1 << di) & ~1) <= myEntry->inDict; di++) {
            if(myEntry->isInDict(di)) {
                const int dictOffset = di*dictLen;
                if(end >= wlen)
                    on[dictOffset + (end-wlen)*3*max +lablen*3 ] = 1;
                for(int k = end-wlen+1; k < end; k++)
                    on[dictOffset +     k*3*max      +lablen*3 + 1    ] = 1;
                if(end != len)
                    on[dictOffset +     end*3*max    +lablen*3 + 2    ] = 1;
            }
        }
    }
    for(int i = 0; i < len; i++) {
        FeatSum & val = score[i];
        for(int di = 0; di < numDicts; di++) {
            char* myOn = &on[di*dictLen + i*3*max];
            FeatVal* myScore = &(*dictVector_)[3*max*di];
            for(int j = 0; j < 3*max; j++) {
                val += myOn[j]*myScore[j];
            }
        }
    }
}

void FeatureLookup::addTagDictWeights(const std::vector<pair<int,int> > & exists, std::vector<FeatSum> & scores) {
    if(!exists.size()) {
        if(tagUnkVector_)
            for(int i = 0; i < (int)scores.size(); i++)
                scores[i] += (*tagUnkVector_)[i];
    } else {
        if(tagDictVector_) {
            int tags = scores.size();
            for(int j = 0; j < (int)exists.size(); j++) {
                int base = exists[j].first*tags*tags+exists[j].second*tags;
                for(int i = 0; i < (int)scores.size(); i++)
                    scores[i] += (*tagDictVector_)[base+i];
            }
        }
    }
}

void FeatureLookup::checkEqual(const FeatureLookup & rhs) const {
    checkPointerEqual(charDict_, rhs.charDict_);
    checkPointerEqual(typeDict_, rhs.typeDict_);
    checkPointerEqual(selfDict_, rhs.selfDict_);
    checkValueVecEqual(dictVector_, rhs.dictVector_);
    checkValueVecEqual(biases_, rhs.biases_);
    checkValueVecEqual(tagDictVector_, rhs.tagDictVector_);
    checkValueVecEqual(tagUnkVector_, rhs.tagUnkVector_);
}