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
#ifndef FEATURE_IO_H__
#define FEATURE_IO_H__

#include <kinkaku/kinkaku-model.h>
#include <kinkaku/dictionary.h>
#include <map>

namespace kinkaku {

class FeatureIO {

protected:

	std::ofstream * out_;

	TagHash feats_;
	typedef std::map<KinkakuString, ModelTagEntry*> WordMap;
	WordMap wm_;
	int numTags_, numDicts_;

public:

	FeatureIO() : out_(0), numTags_(0), numDicts_(0) { }
	~FeatureIO();

	int getNumTags() { return numTags_; }
	int getNumDicts() { return numDicts_; }
	void setNumTags(int numTags) { numTags_ = numTags; }

	void load(const std::string& fileName,StringUtil* util);

	void openOut(const std::string& fileName);
	void closeOut();

	WordMap & getWordMap() { return wm_; }

	TagHash & getFeatures() { return feats_; }
	TagTriplet * getFeatures(const KinkakuString & str, bool add);

	void printFeatures(const KinkakuString & featId, TagTriplet* trip, StringUtil * util);
	void printFeatures(const KinkakuString & featId, StringUtil * util);
	void printWordMap(StringUtil * util);

};

}

#endif