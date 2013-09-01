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
#include <kinkaku/kinkaku-struct.h>
#include <kinkaku/kinkaku-util.h>

using namespace kinkaku;

namespace kinkaku {

template <class T>
void checkMapEqual(const KinkakuStringMap<T> & a, const KinkakuStringMap<T> & b) {
    if(a.size() != b.size())
        THROW_ERROR("checkMapEqual a.size() != b.size() ("<<a.size()<<", "<<b.size());
    for(typename KinkakuStringMap<T>::const_iterator ait = a.begin();
        ait != a.end();
        ait++) {
        typename KinkakuStringMap<T>::const_iterator bit = b.find(ait->first);
        if(bit == b.end() || ait->second != bit->second)
            THROW_ERROR("Values don't match in map");
    }
}

template void checkMapEqual(const KinkakuStringMap<double> & a, const KinkakuStringMap<double> & b);
template void checkMapEqual(const KinkakuStringMap<unsigned int> & a, const KinkakuStringMap<unsigned int> & b);

}


void KinkakuSentence::refreshWS(double confidence) {
    Words newWords;
    int nextWord = 0, nextEnd = 0, nextStart = -1;
    if(surface.length() != 0) {
        int last = 0, i;
        for(i = 0; i <= (int)wsConfs.size(); i++) {
            double myConf = (i == (int)wsConfs.size()) ? 100.0 : wsConfs[i];
            if(myConf > confidence) {
                while(nextWord < (int)words.size() && nextEnd < i+1) {
                    nextStart = nextEnd;
                    nextEnd += words[nextWord].surface.length();
                    nextWord++;
                }
                if(last == nextStart && i+1 == nextEnd)
                    newWords.push_back(words[nextWord-1]);
                else {
                    KinkakuWord w(surface.substr(last, i-last+1), norm.substr(last, i-last+1));
                    newWords.push_back(w);
                }
                last = i+1;
            }
        }
    }
    words = newWords;
}