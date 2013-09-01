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
#include <sstream>
#include <cmath>
#include <kinkaku/corpus-io-prob.h>
#include <kinkaku/kinkaku-struct.h>
#include <kinkaku/config.h>
#include <kinkaku/string-util.h>
#include <kinkaku/kinkaku-util.h>

#define PROB_TRUE    100.0
#define PROB_FALSE   -100.0
#define PROB_UNKNOWN 0.0

using namespace kinkaku;
using namespace std;

KinkakuSentence * ProbCorpusIO::readSentence() {
#ifdef KINKAKU_SAFE
    if(out_ || !str_) 
        THROW_ERROR("Attempted to read a sentence from an closed or output object");
#endif
    KinkakuSentence* ret = FullCorpusIO::readSentence();
    if(ret == 0)
        return 0;
    string s;
    getline(*str_, s);
    istringstream wsiss(s);
    KinkakuSentence::Floats::iterator wsit = ret->wsConfs.begin();
    while((wsiss >> s) && (wsit != ret->wsConfs.end())) {
        *wsit = util_->parseFloat(s.c_str());
        wsit++;
    }
    if(wsiss.good() || wsit != ret->wsConfs.end()) {
        THROW_ERROR("Bad number of WS confidences in a probability file");
    }
    for(int i = 0; i < getNumTags(); i++) {
        getline(*str_, s);
        istringstream peiss(s);
        KinkakuSentence::Words::iterator peit = ret->words.begin();
        while((peiss >> s) && (peit != ret->words.end())) {
            if(peit->getTag(i))
                peit->setTagConf(i,util_->parseFloat(s.c_str()));
            peit++;
        }
        if(peiss.good() || peit != ret->words.end()) {
            THROW_ERROR("Bad number of PE confidences in a probability file");
        }
    }
    getline(*str_, s);
    if(s.length())
        THROW_ERROR("Badly formatted probability file (no white-space between sentences)");

    return ret;
}

void ProbCorpusIO::writeSentence(const KinkakuSentence * sent, double conf)  {
    FullCorpusIO::writeSentence(sent, conf);
    const string & space = util_->showChar(bounds_[0]), &amp = util_->showChar(bounds_[2]);
    for(unsigned i = 0; i < sent->wsConfs.size(); i++) {
        if(i != 0) *str_ << space;
        *str_ << abs(sent->wsConfs[i]);
    }
    *str_ << endl;
    for(int k = 0; k < getNumTags(); k++) {
        if(getDoTag(k)) {
            for(unsigned i = 0; i < sent->words.size(); i++) {
                if(i != 0) *str_ << space;
                const vector< KinkakuTag > & tags = sent->words[i].getTags(k);
                if(tags.size() > 0) {
                    *str_ << tags[0].second;
                    if(allTags_)
                        for(unsigned j = 1; j < tags.size(); j++)
                            *str_ << amp << tags[j].second;
                } else
                    *str_ << 0;
            }
            *str_ << endl;
        }
    }
    *str_ << endl;
}