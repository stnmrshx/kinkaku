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
#include <kinkaku/corpus-io-part.h>
#include <kinkaku/kinkaku-struct.h>
#include <kinkaku/config.h>
#include <kinkaku/string-util.h>
#include <kinkaku/kinkaku-util.h>
#include <kinkaku/config.h>

#define PROB_TRUE    100.0
#define PROB_FALSE   -100.0
#define PROB_UNKNOWN 0.0

using namespace kinkaku;
using namespace std;

KinkakuSentence * PartCorpusIO::readSentence() {
#ifdef KINKAKU_SAFE
    if(out_ || !str_) 
        THROW_ERROR("Attempted to read a sentence from an closed or output object");
#endif
    string s;
    getline(*str_, s);
    if(str_->eof())
        return 0;
    KinkakuString ks = util_->mapString(s), buff(ks.length());
    KinkakuChar ukBound = bounds_[0], skipBound = bounds_[1], noBound = bounds_[2], 
        hasBound = bounds_[3], slashChar = bounds_[4], elemChar = bounds_[5], 
        escapeChar = bounds_[6];
    KinkakuSentence * ret = new KinkakuSentence();

    int len = ks.length(), charLen = 0;
    for(int j = 0; j < len; j++) {
        int bpos = 0;
        bool cert = true;
        for( ; j < len; j++) {
            if(ks[j] == ukBound || ks[j] == skipBound || ks[j] == noBound || ks[j] == hasBound || ks[j] == slashChar || ks[j] == elemChar)
                THROW_ERROR("Misplaced character '"<<util_->showChar(ks[j])<<"' in "<<s);
            if(ks[j] == escapeChar && ++j >= len)
                THROW_ERROR("Misplaced escape at the end of "<<s);
            buff[bpos++] = ks[j++];
            if(j >= len || ks[j] == slashChar || ks[j] == hasBound) 
                break;
            else if(ks[j] == ukBound || ks[j] == skipBound) {
                ret->wsConfs.push_back(PROB_UNKNOWN);
                cert = false;
            } else if(ks[j] != noBound) {
                THROW_ERROR("Misplaced character '"<<util_->showChar(ks[j])<<"' in "<<s);
            } else
                ret->wsConfs.push_back(PROB_FALSE);
        }
        KinkakuString word_str = buff.substr(0,bpos);
        KinkakuWord word(word_str, util_->normalize(word_str));
        charLen += bpos;
        word.isCertain = cert;
        bpos = 0;
        int lev = -1;
        while(j < len && ks[j] != hasBound) {
            if(ks[j] == slashChar) lev++;
            bpos = 0;
            for(++j ; j < len && ks[j] != hasBound && ks[j] != slashChar && ks[j] != elemChar; j++) {
                if(ks[j] == escapeChar && ++j == len)
                    THROW_ERROR("Illegal trailing escape character at "<<s);
                buff[bpos++] = ks[j];
            }
            if(bpos != 0)
                word.addTag(lev,KinkakuTag(buff.substr(0,bpos),PROB_TRUE));
        }
        if(j != len)
            ret->wsConfs.push_back(PROB_TRUE);
        ret->words.push_back(word);
    }

    ret->surface = KinkakuString(charLen);
    ret->norm = KinkakuString(charLen);
    unsigned pos = 0;
    for(KinkakuSentence::Words::const_iterator tit = ret->words.begin(); tit != ret->words.end(); tit++) {
        ret->surface.splice(tit->surface, pos);
        ret->norm.splice(tit->norm, pos);
        pos += tit->surface.length();
    }

    return ret;
}

void PartCorpusIO::writeSentence(const KinkakuSentence * sent, double conf)  {
    unsigned curr = 0;
    const string & ukBound = util_->showChar(bounds_[0]), & skipBound = util_->showChar(bounds_[1]), 
        &noBound = util_->showChar(bounds_[2]), &hasBound = util_->showChar(bounds_[3]), 
        &slashChar = util_->showChar(bounds_[4]), &elemChar = util_->showChar(bounds_[5]);
    for(unsigned i = 0; i < sent->words.size(); i++) {
        const KinkakuWord & w = sent->words[i];
        string sepType = ukBound;
        for(unsigned j = 0; j < w.surface.length(); ) {
            *str_ << util_->showChar(sent->surface[curr]);
            if(curr == sent->wsConfs.size()) sepType = skipBound;
            else if(sent->wsConfs[curr] > conf) sepType = hasBound;
            else if(sent->wsConfs[curr] < conf*-1) sepType = noBound;
            else sepType = ukBound;
            if(++j != w.surface.length())
                *str_ << sepType;
            curr++;
        }
        for(int j = 0; j < w.getNumTags(); j++) {
            const vector<KinkakuTag> & tags = w.getTags(j);
            for(int k = 0; k < (int)tags.size(); k++)
                if(tags[k].second > conf)
                    *str_ << (k==0?slashChar:elemChar) << util_->showString(tags[k].first);
        }
        if(w.getUnknown())
            *str_ << unkTag_;
        if(sepType != skipBound)
            *str_ << sepType;
    }
    *str_ << endl;
}

PartCorpusIO::PartCorpusIO(StringUtil * util, const char* unkBound, const char* skipBound, const char* noBound, const char* hasBound, const char* tagBound, const char* elemBound, const char* escape) : CorpusIO(util), bounds_(7) { 
    bounds_[0] = util_->mapChar(unkBound);
    bounds_[1] = util_->mapChar(skipBound);
    bounds_[2] = util_->mapChar(noBound);
    bounds_[3] = util_->mapChar(hasBound);
    bounds_[4] = util_->mapChar(tagBound);
    bounds_[5] = util_->mapChar(elemBound);
    bounds_[6] = util_->mapChar(escape);
}
PartCorpusIO::PartCorpusIO(const CorpusIO & c, const char* unkBound, const char* skipBound, const char* noBound, const char* hasBound, const char* tagBound, const char* elemBound, const char* escape) : CorpusIO(c), bounds_(7) { 
    bounds_[0] = util_->mapChar(unkBound);
    bounds_[1] = util_->mapChar(skipBound);
    bounds_[2] = util_->mapChar(noBound);
    bounds_[3] = util_->mapChar(hasBound);
    bounds_[4] = util_->mapChar(tagBound);
    bounds_[5] = util_->mapChar(elemBound);
    bounds_[6] = util_->mapChar(escape);
}
PartCorpusIO::PartCorpusIO(StringUtil * util, std::iostream & str, bool out, const char* unkBound, const char* skipBound, const char* noBound, const char* hasBound, const char* tagBound, const char* elemBound, const char* escape) : CorpusIO(util,str,out), bounds_(7) { 
    bounds_[0] = util_->mapChar(unkBound);
    bounds_[1] = util_->mapChar(skipBound);
    bounds_[2] = util_->mapChar(noBound);
    bounds_[3] = util_->mapChar(hasBound);
    bounds_[4] = util_->mapChar(tagBound);
    bounds_[5] = util_->mapChar(elemBound);
    bounds_[6] = util_->mapChar(escape);
}
PartCorpusIO::PartCorpusIO(StringUtil * util, const char* file, bool out, const char* unkBound, const char* skipBound, const char* noBound, const char* hasBound, const char* tagBound, const char* elemBound, const char* escape) : CorpusIO(util,file,out), bounds_(7) { 
    bounds_[0] = util_->mapChar(unkBound);
    bounds_[1] = util_->mapChar(skipBound);
    bounds_[2] = util_->mapChar(noBound);
    bounds_[3] = util_->mapChar(hasBound);
    bounds_[4] = util_->mapChar(tagBound);
    bounds_[5] = util_->mapChar(elemBound);
    bounds_[6] = util_->mapChar(escape);
}
