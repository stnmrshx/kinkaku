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
#include <kinkaku/kinkaku-util.h>
#include <kinkaku/kinkaku-config.h>
#include <kinkaku/corpus-io.h>
#include <kinkaku/corpus-io-full.h>
#include <kinkaku/corpus-io-eda.h>
#include <kinkaku/corpus-io-tokenized.h>
#include <kinkaku/corpus-io-part.h>
#include <kinkaku/corpus-io-prob.h>
#include <kinkaku/corpus-io-raw.h>
#include <cmath>
#include "config.h"

#define PROB_TRUE    100.0
#define PROB_FALSE   -100.0
#define PROB_UNKNOWN 0.0

using namespace kinkaku;
using namespace std;

CorpusIO * CorpusIO::createIO(const char* file, Format form, const KinkakuConfig & conf, bool output, StringUtil* util) {
    if(form == CORP_FORMAT_FULL)      { return new FullCorpusIO(util,file,output,conf.getWordBound(),conf.getTagBound(),conf.getElemBound(),conf.getEscape()); }
    else if(form == CORP_FORMAT_TAGS)      { 
        FullCorpusIO * io = new FullCorpusIO(util,file,output,conf.getWordBound(),conf.getTagBound(),conf.getElemBound(),conf.getEscape());
        io->setPrintWords(false);
        return io;
    }
    else if(form == CORP_FORMAT_TOK)      { return new TokenizedCorpusIO(util,file,output,conf.getWordBound()); }
    else if(form == CORP_FORMAT_PART) { return new PartCorpusIO(util,file,output,conf.getUnkBound(),conf.getSkipBound(),conf.getNoBound(),conf.getHasBound(),conf.getTagBound(),conf.getElemBound(),conf.getEscape()); }
    else if(form == CORP_FORMAT_PROB) { return new ProbCorpusIO(util,file,output,conf.getWordBound(),conf.getTagBound(),conf.getElemBound(),conf.getEscape()); }
    else if(form == CORP_FORMAT_RAW)  { return new RawCorpusIO(util,file,output);  }
    else if(form == CORP_FORMAT_EDA)  { return new EdaCorpusIO(util,file,output);  }
    else
        THROW_ERROR("Illegal Output Format");
}

CorpusIO * CorpusIO::createIO(iostream & file, Format form, const KinkakuConfig & conf, bool output, StringUtil* util) {
    if(form == CORP_FORMAT_FULL)      { return new FullCorpusIO(util,file,output,conf.getWordBound(),conf.getTagBound(),conf.getElemBound(),conf.getEscape()); }
    else if(form == CORP_FORMAT_TAGS)      { 
        FullCorpusIO * io = new FullCorpusIO(util,file,output,conf.getWordBound(),conf.getTagBound(),conf.getElemBound(),conf.getEscape());
        io->setPrintWords(false);
        return io;
    }
    else if(form == CORP_FORMAT_TOK)      { return new TokenizedCorpusIO(util,file,output,conf.getWordBound()); }
    else if(form == CORP_FORMAT_PART) { return new PartCorpusIO(util,file,output,conf.getUnkBound(),conf.getSkipBound(),conf.getNoBound(),conf.getHasBound(),conf.getTagBound(),conf.getElemBound(),conf.getEscape()); }
    else if(form == CORP_FORMAT_PROB) { return new ProbCorpusIO(util,file,output,conf.getWordBound(),conf.getTagBound(),conf.getElemBound(),conf.getEscape()); }
    else if(form == CORP_FORMAT_RAW)  { return new RawCorpusIO(util,file,output);  }
    else if(form == CORP_FORMAT_EDA)  { return new EdaCorpusIO(util,file,output);  }
    else 
        THROW_ERROR("Illegal Output Format");
}