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
#ifndef CORPUS_IO_PART_H__ 
#define CORPUS_IO_PART_H__ 

#include <kinkaku/corpus-io.h>
#include <kinkaku/kinkaku-string.h>

namespace kinkaku {

class PartCorpusIO : public CorpusIO {
    
private:

	KinkakuString bounds_;

public:

	PartCorpusIO(StringUtil * util, const char* unkBound = " ", const char* skipBound = "?", const char* noBound = "-", const char* hasBound = "|", const char* tagBound = "/", const char* elemBound = "&", const char* escape = "\\");
	PartCorpusIO(const CorpusIO & c, const char* unkBound = " ", const char* skipBound = "?", const char* noBound = "-", const char* hasBound = "|", const char* tagBound = "/", const char* elemBound = "&", const char* escape = "\\");
	PartCorpusIO(StringUtil * util, std::iostream & str, bool out, const char* unkBound = " ", const char* skipBound = "?", const char* noBound = "-", const char* hasBound = "|", const char* tagBound = "/", const char* elemBound = "&", const char* escape = "\\");
	PartCorpusIO(StringUtil * util, const char* file, bool out, const char* unkBound = " ", const char* skipBound = "?", const char* noBound = "-", const char* hasBound = "|", const char* tagBound = "/", const char* elemBound = "&", const char* escape = "\\");

	KinkakuSentence * readSentence();
	void writeSentence(const KinkakuSentence * sent, double conf = 0.0);

};

}

#endif