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
#include <kinkaku/corpus-io-full.h>
#include <kinkaku/kinkaku-struct.h>
#include <kinkaku/config.h>
#include <kinkaku/string-util.h>
#include <kinkaku/kinkaku-util.h>
#include "config.h"

#define PROB_TRUE    100.0
#define PROB_FALSE   -100.0
#define PROB_UNKNOWN 0.0

using namespace kinkaku;
using namespace std;

KinkakuSentence * FullCorpusIO::readSentence() {
#ifdef KINKAKU_SAFE
    if(out_ || !str_) 
        THROW_ERROR("Attempted to read a sentence from an closed or output object");
#endif
    string s;
    getline(*str_, s);
    if(str_->eof())
        return 0;

    KinkakuChar spaceChar = bounds_[0], slashChar = bounds_[1], ampChar = bounds_[2], bsChar = bounds_[3];
    KinkakuString ks = util_->mapString(s), buff(ks.length());
    int len = ks.length();
    KinkakuSentence * ret = new KinkakuSentence();
    int charLen = 0;

    int j = 0, bpos, lev;
    for(j = 0; j < len; j++) {
        bpos = 0;
        for( ; j < len && ks[j] != spaceChar && ks[j] != slashChar; j++) {
            if(ks[j] == ampChar) {
                THROW_ERROR("Illegal tag separator in word position at "<<s);
            } else if(ks[j] == bsChar && ++j == len) {
                THROW_ERROR("Illegal trailing escape character at "<<s);
            }
            buff[bpos++] = ks[j];
        }
        if(bpos == 0) {
            if(ks[j] == spaceChar)
                continue;
            else
                THROW_ERROR("Empty word at position "<<j<<" in "<<s);
        }
        KinkakuString word_str = buff.substr(0,bpos);
        KinkakuWord word(word_str, util_->normalize(word_str));
        charLen += bpos;
        lev = -1;
        while(j < len && ks[j] != spaceChar) {
            if(ks[j] == slashChar) lev++;
            bpos = 0;
            for(++j ; j < len && ks[j] != spaceChar && ks[j] != slashChar && ks[j] != ampChar; j++) {
                if(ks[j] == bsChar && ++j == len)
                    THROW_ERROR("Illegal trailing escape character at "<<s);
                buff[bpos++] = ks[j];
            }
            if(bpos != 0)
                word.addTag(lev,KinkakuTag(buff.substr(0,bpos),PROB_TRUE));
        }
        ret->words.push_back(word);
    }
     
    ret->surface = KinkakuString(charLen);
    ret->norm = KinkakuString(charLen);
    unsigned pos = 0;
    for(KinkakuSentence::Words::const_iterator tit = ret->words.begin(); tit != ret->words.end(); tit++) {
        ret->surface.splice(tit->surface, pos);
        ret->norm.splice(tit->norm, pos);
        unsigned nextPos = pos + tit->surface.length() - 1;
        while(pos++ < nextPos)
            ret->wsConfs.push_back(PROB_FALSE);
        ret->wsConfs.push_back(PROB_TRUE); 
    }
    if(ret->wsConfs.size() > 0)
        ret->wsConfs.pop_back();
    return ret;
}

void FullCorpusIO::writeSentence(const KinkakuSentence * sent, double conf) {
    const string & wb = util_->showChar(bounds_[0]), tb = util_->showChar(bounds_[1]), eb = util_->showChar(bounds_[2]);
    for(unsigned i = 0; i < sent->words.size(); i++) {
        if(i != 0) *str_ << wb;
        const KinkakuWord & w = sent->words[i];
        if(printWords_) *str_ << util_->showString(w.surface);
        int printed = 0;
        for(int j = 0; j < w.getNumTags(); j++) {
            const vector< KinkakuTag > & tags = w.getTags(j);
            if(tags.size() > 0) {
                *str_ << ((printWords_ || printed++ > 0) ? tb : "") << util_->showString(tags[0].first);
                if(allTags_) 
                    for(unsigned k = 1; k < tags.size(); k++) 
                        *str_ << eb << util_->showString(tags[k].first);
            }
        }
        if(w.getUnknown())
            *str_ << unkTag_;
    }
    *str_ << endl;
}

FullCorpusIO::FullCorpusIO(StringUtil * util, const char* wordBound, const char* tagBound, const char* elemBound, const char* escape) : CorpusIO(util), allTags_(false), bounds_(4), printWords_(true) { 
    bounds_[0] = util_->mapChar(wordBound);
    bounds_[1] = util_->mapChar(tagBound);
    bounds_[2] = util_->mapChar(elemBound);
    bounds_[3] = util_->mapChar(escape);
}
FullCorpusIO::FullCorpusIO(const CorpusIO & c, const char* wordBound, const char* tagBound, const char* elemBound, const char* escape) : CorpusIO(c), allTags_(false), bounds_(4), printWords_(true) { 
    bounds_[0] = util_->mapChar(wordBound);
    bounds_[1] = util_->mapChar(tagBound);
    bounds_[2] = util_->mapChar(elemBound);
    bounds_[3] = util_->mapChar(escape);
}
FullCorpusIO::FullCorpusIO(StringUtil * util, const char* file, bool out, const char* wordBound, const char* tagBound, const char* elemBound, const char* escape) : CorpusIO(util,file,out), allTags_(false), bounds_(4), printWords_(true) { 
    bounds_[0] = util_->mapChar(wordBound);
    bounds_[1] = util_->mapChar(tagBound);
    bounds_[2] = util_->mapChar(elemBound);
    bounds_[3] = util_->mapChar(escape);
} 
FullCorpusIO::FullCorpusIO(StringUtil * util, std::iostream & str, bool out, const char* wordBound, const char* tagBound, const char* elemBound, const char* escape) : CorpusIO(util,str,out), allTags_(false), bounds_(4), printWords_(true) { 
    bounds_[0] = util_->mapChar(wordBound);
    bounds_[1] = util_->mapChar(tagBound);
    bounds_[2] = util_->mapChar(elemBound);
    bounds_[3] = util_->mapChar(escape);
}
