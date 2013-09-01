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
#ifndef KINKAKU_DICTIONARY_H_
#define KINKAKU_DICTIONARY_H_

#include <kinkaku/kinkaku-string.h>
#include <kinkaku/string-util.h>
#include <map>
#include <deque>

namespace kinkaku  {

class KinkakuModel;
class KinkakuString;

class TagEntry {
public:

	TagEntry(const KinkakuString & str) : word(str), tags(), inDict(0) { }
	virtual ~TagEntry() { }

	KinkakuString word;
	std::vector< std::vector<KinkakuString> > tags;
	std::vector< std::vector<unsigned char> > tagInDicts;
	unsigned char inDict;

	virtual void setNumTags(int i) {
		tags.resize(i);
		tagInDicts.resize(i);
	}

	inline static bool isInDict(unsigned char in, unsigned char test) {
		return (1 << test) & in;
    }

    inline bool isInDict(unsigned char test) const {
    	return isInDict(inDict,test);
    }

    inline static void setInDict(unsigned char & in, unsigned char test) {
    	in |= (1 << test);
    }

    inline void setInDict(char test) {
    	setInDict(inDict,test);
    }
};

class ModelTagEntry : public TagEntry {

public:

	ModelTagEntry(const KinkakuString & str) : TagEntry(str) { }
	~ModelTagEntry();

	void setNumTags(int i) {
		TagEntry::setNumTags(i);
		tagMods.resize(i,0);
	}

	std::vector<KinkakuModel *> tagMods;

};

class ProbTagEntry : public TagEntry {

public:

	ProbTagEntry(const KinkakuString & str) : TagEntry(str), probs() { }
	~ProbTagEntry() { }

	double incrementProb(const KinkakuString & str, int lev);

	void setNumTags(int i) {
		TagEntry::setNumTags(i);
		probs.resize(i);
	}

	std::vector< std::vector< double > > probs;

};

class DictionaryState {

public:

	DictionaryState() : failure(0), gotos(), output(), isBranch(false) { }
	typedef std::vector< std::pair< KinkakuChar, unsigned> > Gotos;
	unsigned failure;
	Gotos gotos;
	std::vector< unsigned > output;
	bool isBranch;

	inline unsigned step(KinkakuChar input) {
		Gotos::const_iterator l=gotos.begin(), r=gotos.end(), m;
		KinkakuChar check;
		while(r != l) {
			m = l+std::distance(l,r)/2;
			check = m->first;
			if(input<check) r=m;
			else if(input>check) l=m+1;
			else return m->second;
		}
		return 0;
	}

};

template <class Entry>
class Dictionary {

public:

	typedef std::map<KinkakuString, Entry*> WordMap;
	typedef typename WordMap::const_iterator wm_const_iterator;
	typedef std::vector< std::pair<unsigned,Entry*> > MatchResult;

private:

	StringUtil * util_;
	std::vector<DictionaryState*> states_;
	std::vector<Entry*> entries_;
	unsigned char numDicts_;

	void buildGoto(wm_const_iterator start, wm_const_iterator end, unsigned lev, unsigned nid);
	void buildFailures();

public:

	Dictionary(StringUtil * util) : util_(util), numDicts_(0) { };
	void clearData();

	~Dictionary() {
		clearData();
	};

	void buildIndex(const WordMap & input);
	void print();

	const Entry * findEntry(KinkakuString str) const;
	Entry * findEntry(KinkakuString str);
	unsigned getTagID(KinkakuString str, KinkakuString tag, int lev);

	MatchResult match( const KinkakuString & chars ) const;

	std::vector<Entry*> & getEntries() { return entries_; }
	std::vector<DictionaryState*> & getStates() { return states_; }
	const std::vector<Entry*> & getEntries() const { return entries_; }
	const std::vector<DictionaryState*> & getStates() const { return states_; }
	unsigned char getNumDicts() const { return numDicts_; }
	void setNumDicts(unsigned char numDicts) { numDicts_ = numDicts; }
	void checkEqual(const Dictionary<Entry> & rhs) const;

};

}

#endif