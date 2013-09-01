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
#ifndef KINKAKU_H__
#define KINKAKU_H__

#include <kinkaku/kinkaku-config.h>
#include <kinkaku/kinkaku-struct.h>
#include <vector>

namespace kinkaku  {

class KinkakuTest;
class StringUtil;
class KinkakuConfig;
template <class T> class Dictionary;
class ModelTagEntry;
class ProbTagEntry;
class KinkakuModel;
class KinkakuLM;
class FeatureIO;

class Kinkaku {

private:
    friend class KinkakuTest;
    typedef unsigned FeatureId;
    typedef std::vector<KinkakuSentence*> Sentences;
    typedef std::vector< std::vector< FeatureId > > SentenceFeatures;

    StringUtil* util_;
    KinkakuConfig* config_;
    Dictionary<ModelTagEntry> * dict_;
    Sentences sentences_;

    KinkakuModel* wsModel_;

    Dictionary<ProbTagEntry>* subwordDict_;
    std::vector<KinkakuLM*> subwordModels_;

    std::vector<KinkakuModel*> globalMods_;
    std::vector< std::vector<KinkakuString> > globalTags_;

    std::vector<unsigned> dictFeats_;
    std::vector<KinkakuString> charPrefixes_, typePrefixes_;

    FeatureIO* fio_;

public:

    void readModel(const char* fileName);

    void writeModel(const char* fileName);

    void calculateWS(KinkakuSentence & sent);
    
    void calculateTags(KinkakuSentence & sent, int lev);

    void calculateUnknownTag(KinkakuWord & str, int lev);

    StringUtil* getStringUtil() { return config_->getStringUtil(); }

    KinkakuConfig* getConfig() { return config_; }

    void trainAll();

    void analyze();

    void init();

    Kinkaku() : config_(new KinkakuConfig()) { init(); }
    Kinkaku(KinkakuConfig * config) : config_(config) { init(); }
    
    ~Kinkaku();

    KinkakuModel* getWSModel() { return wsModel_; }

    void setWSModel(KinkakuModel* model) { wsModel_ = model; }

    template <class Entry>
    void setDictionary(Dictionary<Entry> * dict);

public:

    void checkEqual(const Kinkaku & rhs);

private:

    void buildVocabulary();
    void trainSanityCheck();

    void trainWS();
    void preparePrefixes();
    unsigned wsDictionaryFeatures(const KinkakuString & sent, SentenceFeatures & feat);
    unsigned wsNgramFeatures(const KinkakuString & sent, SentenceFeatures & feat, const std::vector<KinkakuString> & prefixes, int n);

    void trainLocalTags(int lev);
    void trainGlobalTags(int lev);
    unsigned tagNgramFeatures(const KinkakuString & chars, std::vector<unsigned> & feat, const std::vector<KinkakuString> & prefixes, KinkakuModel * model, int n, int sc, int ec);
    unsigned tagSelfFeatures(const KinkakuString & self, std::vector<unsigned> & feat, const KinkakuString & pref, KinkakuModel * model);
    unsigned tagDictFeatures(const KinkakuString & surf, int lev, std::vector<unsigned> & myFeats, KinkakuModel * model);

    std::vector<std::pair<int,int> > getDictionaryMatches(const KinkakuString & str, int lev);

    template <class Entry>
    void addTag(typename Dictionary<Entry>::WordMap& allWords, const KinkakuString & word, int lev, const KinkakuString * tag, int dict);
    template <class Entry>
    void addTag(typename Dictionary<Entry>::WordMap& allWords, const KinkakuString & word, const KinkakuTag * tag, int dict);
    template <class Entry>
    void scanDictionaries(const std::vector<std::string> & dict, typename Dictionary<Entry>::WordMap & wordMap, KinkakuConfig * config, StringUtil * util, bool saveIds = true);

    void trainUnk(int lev);
    void buildFeatureLookups();

    void analyzeInput();
    
    std::vector<KinkakuTag> generateTagCandidates(const KinkakuString & str, int lev);

};

}

#endif