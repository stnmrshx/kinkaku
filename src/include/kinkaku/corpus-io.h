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
#ifndef CORPUS_IO_H__ 
#define CORPUS_IO_H__ 

namespace kinkaku {
class CorpusIO;
const static char CORP_FORMAT_RAW  = 0;
const static char CORP_FORMAT_FULL = 1;
const static char CORP_FORMAT_PART = 2;
const static char CORP_FORMAT_PROB = 3;
const static char CORP_FORMAT_TOK = 4;
const static char CORP_FORMAT_DEFAULT = 5;
const static char CORP_FORMAT_EDA = 6;
const static char CORP_FORMAT_TAGS = 7;
}

#include <kinkaku/general-io.h>
#include <vector>

namespace kinkaku {

class KinkakuConfig;
class StringUtil;
class KinkakuSentence;

class CorpusIO : public GeneralIO {

protected:

	std::string unkTag_;
	int numTags_;
	std::vector<bool> doTag_;

public:

	typedef char Format;

	CorpusIO(StringUtil * util) : GeneralIO(util), unkTag_(), numTags_(0), doTag_() { }
	CorpusIO(StringUtil * util, const char* file, bool out) : GeneralIO(util,file,out,false), numTags_(0), doTag_() { } 
	CorpusIO(StringUtil * util, std::iostream & str, bool out) : GeneralIO(util,str,out,false), numTags_(0), doTag_() { }

	int getNumTags() { return numTags_; }
	void setNumTags(int numTags) { numTags_ = numTags; }
	void setDoTag(int i, bool v) { 
		if(i >= (int)doTag_.size()) doTag_.resize(i+1,true);
		doTag_[i] = v;
	}
	bool getDoTag(int i) { return i >= (int)doTag_.size() || doTag_[i]; }

	virtual ~CorpusIO() { }

	static CorpusIO* createIO(const char* file, Format form, const KinkakuConfig & conf, bool output, StringUtil* util);
	static CorpusIO* createIO(std::iostream & str, Format form, const KinkakuConfig & conf, bool output, StringUtil* util);

	virtual KinkakuSentence * readSentence() = 0;
	virtual void writeSentence(const KinkakuSentence * sent, double conf = 0.0) = 0;

	void setUnkTag(const std::string & tag) { unkTag_ = tag; }

};

}

#endif