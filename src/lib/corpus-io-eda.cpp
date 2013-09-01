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
#include <kinkaku/corpus-io-eda.h>
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

KinkakuSentence * EdaCorpusIO::readSentence() {
    THROW_ERROR("Using EDA format for input is not currently supported");
    return NULL;
}

void EdaCorpusIO::writeSentence(const KinkakuSentence * sent, double conf) {
    *str_ << "ID=" << ++id_ << endl;
    for(unsigned i = 0; i < sent->words.size(); i++) {
        const KinkakuWord & w = sent->words[i];
        string tag = "UNK";
        if(w.getNumTags() >= 1) {
            const vector< KinkakuTag > & tags = w.getTags(0);
            if(tags.size() > 0)
                tag = util_->showString(tags[0].first);
        }
        // Print
        *str_ << i+1 << " " 
              << i+2 << " "
              << util_->showString(w.surface) << " "
              << tag << " 0" << endl;
    }
    *str_ << endl;
}

EdaCorpusIO::EdaCorpusIO(StringUtil * util) : CorpusIO(util), id_(0) { }
EdaCorpusIO::EdaCorpusIO(const CorpusIO & c) : CorpusIO(c), id_(0) { }
EdaCorpusIO::EdaCorpusIO(StringUtil * util, const char* file, bool out) : CorpusIO(util,file,out), id_(0) { }
EdaCorpusIO::EdaCorpusIO(StringUtil * util, std::iostream & str, bool out) : CorpusIO(util,str,out), id_(0) { }