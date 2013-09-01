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
#ifndef FEATURE_LOOKUP__
#define FEATURE_LOOKUP__

#include <vector>
#include <cstddef>
#include <kinkaku/feature-vector.h>
#include <kinkaku/dictionary.h>

namespace kinkaku {

class KinkakuString;
class ModelTagEntry;

class FeatureLookup {

protected:

	Dictionary<FeatVec> *charDict_, *typeDict_, *selfDict_;
	FeatVec *dictVector_, *biases_, *tagDictVector_, *tagUnkVector_;

public:

	FeatureLookup() : charDict_(NULL), typeDict_(NULL), selfDict_(NULL), dictVector_(NULL), biases_(NULL), tagDictVector_(NULL), tagUnkVector_(NULL) { }
	~FeatureLookup();

	void checkEqual(const FeatureLookup & rhs) const;

	const Dictionary<FeatVec> * getCharDict() const { return charDict_; }
	const Dictionary<FeatVec> * getTypeDict() const { return typeDict_; }
	const Dictionary<FeatVec> * getSelfDict() const { return selfDict_; }
	const FeatVec * getDictVector() const { return dictVector_; }
	const FeatVal getBias(int id) const { return (*biases_)[id]; }
	const std::vector<FeatVal> * getBiases() const { return biases_; }
	const FeatVal getTagUnkFeat(int tag) const { return (*tagUnkVector_)[tag]; }
	const std::vector<FeatVal> * getTagDictVector() const { return tagDictVector_; }
	const std::vector<FeatVal> * getTagUnkVector() const { return tagUnkVector_; }

	void addNgramScores(const Dictionary<FeatVec> * dict, const KinkakuString & str, int window, std::vector<FeatSum> & score);
	void addDictionaryScores(const Dictionary<ModelTagEntry>::MatchResult & matches, int numDicts, int max, std::vector<FeatSum> & score);
	void addTagNgrams(const KinkakuString & chars, const Dictionary<FeatVec> * dict, std::vector<FeatSum> & scores, int window, int startChar, int endChar);
	void addSelfWeights(const KinkakuString & chars, std::vector<FeatSum> & scores, int isType);
	void addTagDictWeights(const std::vector<std::pair<int,int> > & exists, std::vector<FeatSum> & scores);
	void setCharDict(Dictionary<FeatVec> * charDict) { charDict_ = charDict; }
	void setTypeDict(Dictionary<FeatVec> * typeDict) { typeDict_ = typeDict; }
	void setSelfDict(Dictionary<FeatVec> * selfDict) { selfDict_ = selfDict; }
	void setDictVector(FeatVec * dictVector) { dictVector_ = dictVector; }
	void setBias(FeatVal bias, int id) {
		if(biases_ == NULL)
			biases_ = new FeatVec(id+1, 0);
		else if((int)biases_->size() <= id)
			biases_->resize(id+1, 0);
		(*biases_)[id] = bias;
	}
	void setBiases(FeatVec * biases) { biases_ = biases; }
	void setTagDictVector(FeatVec * tagDictVector) { tagDictVector_ = tagDictVector; }
	void setTagUnkVector(FeatVec * tagUnkVector) { tagUnkVector_ = tagUnkVector; }

};

}

#endif