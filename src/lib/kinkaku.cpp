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
#include <algorithm>
#include <set>
#include <cmath>
#include <sstream>
#include <iostream>
#include <kinkaku/config.h>
#include <kinkaku/kinkaku.h>
#include <kinkaku/dictionary.h>
#include <kinkaku/corpus-io.h>
#include <kinkaku/model-io.h>
#include <kinkaku/feature-io.h>
#include <kinkaku/dictionary.h>
#include <kinkaku/string-util.h>
#include <kinkaku/kinkaku-util.h>
#include <kinkaku/kinkaku-lm.h>
#include <kinkaku/feature-lookup.h>

using namespace kinkaku;
using namespace std;

template <class Entry>
void Kinkaku::addTag(typename Dictionary<Entry>::WordMap& allWords, const KinkakuString & word, int lev, const KinkakuString * tag, int dict) {
    typedef typename Dictionary<Entry>::WordMap WordMap;
    typename WordMap::iterator it = allWords.find(word);
    if(it == allWords.end()) {
        Entry * ent = new Entry(word);
        ent->setNumTags(lev+1);
        if(tag) {
            ent->tags[lev].push_back(*tag);
            ent->tagInDicts[lev].push_back(0);
        }
        if(dict >= 0) {
            Entry::setInDict(ent->inDict,dict);
            if(tag)
                Entry::setInDict(ent->tagInDicts[lev][0],dict);
        }
        allWords.insert(typename WordMap::value_type(word,ent));
    }
    else {
        if(tag) {
            unsigned i;
            if((int)it->second->tags.size() <= lev)
                it->second->setNumTags(lev+1);
            vector<KinkakuString> & tags = it->second->tags[lev];
            vector<unsigned char> & tagInDicts = it->second->tagInDicts[lev];
            for(i = 0; i < tags.size() && tags[i] != *tag; i++);
            if(i == tags.size()) {
                tags.push_back(*tag);
                tagInDicts.push_back(0);
            }
            if(dict >= 0)
                Entry::setInDict(tagInDicts[i],dict);
        }
        if(dict >= 0) 
            Entry::setInDict(it->second->inDict,dict);
    }
}
template <class Entry>
void Kinkaku::addTag(typename Dictionary<Entry>::WordMap& allWords, const KinkakuString & word, const KinkakuTag * tag, int dict) {
    addTag<Entry>(allWords,word,(tag?&tag->first:0),dict);
}

template <class Entry>
void Kinkaku::scanDictionaries(const vector<string> & dict, typename Dictionary<Entry>::WordMap & wordMap, KinkakuConfig * config, StringUtil * util, bool saveIds) {
    KinkakuString word;
    unsigned char numDicts = 0;
    for(vector<string>::const_iterator it = dict.begin(); it != dict.end(); it++) {
        if(config_->getDebug())
            cerr << "Reading dictionary from " << *it << " ";
        CorpusIO * io = CorpusIO::createIO(it->c_str(), CORP_FORMAT_FULL, *config, false, util);
        io->setNumTags(config_->getNumTags());
        KinkakuSentence* next;
        int lines = 0;
        while((next = io->readSentence())) {
            lines++;
            if(next->words.size() != 1) {
                ostringstream buff;
                buff << "Badly formatted dictionary entry (too many or too few words '";
                for(unsigned i = 0; i < next->words.size(); i++) {
                    if(i != 0) buff << " --- ";
                    buff << util->showString(next->words[i].surface);
                }
                buff << "')";
                THROW_ERROR(buff.str());
            }
            word = next->words[0].norm;
            for(int i = 0; i < next->words[0].getNumTags(); i++)
                if(next->words[0].hasTag(i))
                    addTag<Entry>(wordMap, word, i, &next->words[0].getTagSurf(i), (saveIds?numDicts:-1));
            if(next->words[0].getNumTags() == 0)
                addTag<Entry>(wordMap, word, 0, 0, (saveIds?numDicts:-1));
            delete next;
        }
        delete io;
        numDicts++;
        if(config_->getDebug() > 0) {
            if(lines)
                cerr << " done (" << lines  << " entries)" << endl;
            else
                cerr << " WARNING - empty training data specified."  << endl;
        }
    }
}

void Kinkaku::buildVocabulary() {

    Dictionary<ModelTagEntry>::WordMap & allWords = fio_->getWordMap();

    if(config_->getDebug() > 0)
        cerr << "Scanning dictionaries and corpora for vocabulary" << endl;
    
    vector<string> corpora = config_->getCorpusFiles();
    vector<CorpusIO::Format> corpForm = config_->getCorpusFormats();
    int maxTag = config_->getNumTags();
    for(unsigned i = 0; i < corpora.size(); i++) {
        if(config_->getDebug() > 0)
            cerr << "Reading corpus from " << corpora[i] << " ";
        CorpusIO * io = CorpusIO::createIO(corpora[i].c_str(), corpForm[i], *config_, false, util_);
        io->setNumTags(config_->getNumTags());
        KinkakuSentence* next;
        int lines = 0;
        while((next = io->readSentence())) {
            lines++;
            bool toAdd = false;
            for(unsigned i = 0; i < next->words.size(); i++) {
                if(next->words[i].isCertain) {
                    maxTag = max(next->words[i].getNumTags(),maxTag);
                    for(int j = 0; j < next->words[i].getNumTags(); j++)
                        if(next->words[i].hasTag(j))
                            addTag<ModelTagEntry>(allWords, next->words[i].norm, j, &next->words[i].getTagSurf(j), -1);
                    if(next->words[i].getNumTags() == 0)
                        addTag<ModelTagEntry>(allWords, next->words[i].norm, 0, 0, -1);
                    
                    toAdd = true;
                }
            }
            const unsigned wsSize = next->wsConfs.size();
            for(unsigned i = 0; !toAdd && i < wsSize; i++)
                toAdd = (next->wsConfs[i] != 0);
            if(toAdd)
                sentences_.push_back(next);
            else
                delete next;
        }
        if(config_->getDebug() > 0) {
            if(lines)
                cerr << " done (" << lines  << " lines)" << endl;
            else
                cerr << " WARNING - empty training data specified."  << endl;
        }
        delete io;
    }
    config_->setNumTags(maxTag);

    scanDictionaries<ModelTagEntry>(config_->getDictionaryFiles(), allWords, config_, util_, true);

    if(sentences_.size() == 0 && fio_->getFeatures().size() == 0)
        THROW_ERROR("There were no sentences in the training data. Check to make sure your training file contains sentences.");

    if(config_->getDebug() > 0)
        cerr << "Building dictionary index ";
    if(allWords.size() == 0)
        THROW_ERROR("FATAL: There were sentences in the training data, but no words were found!");
    if(dict_ != 0) delete dict_;
    dict_ = new Dictionary<ModelTagEntry>(util_);
    dict_->buildIndex(allWords);
    dict_->setNumDicts(max((int)config_->getDictionaryFiles().size(),fio_->getNumDicts()));
    if(config_->getDebug() > 0)
        cerr << "done!" << endl;

}

unsigned Kinkaku::wsDictionaryFeatures(const KinkakuString & chars, SentenceFeatures & features) {
    ModelTagEntry* myEntry;
    const unsigned len = features.size(), max=config_->getDictionaryN(), dictLen = len*3*max;
    vector<char> on(dict_->getNumDicts()*dictLen, 0);
    unsigned ret = 0, end;
    Dictionary<ModelTagEntry>::MatchResult matches = dict_->match(chars);
    for(unsigned i = 0; i < matches.size(); i++) {
        end = matches[i].first;
        myEntry = matches[i].second;
        if(myEntry->inDict == 0)
            continue;
        const unsigned wlen = myEntry->word.length();
        const unsigned lablen = min(wlen,max)-1;
        for(unsigned di = 0; ((1 << di) & ~1) <= myEntry->inDict; di++) {
            if(myEntry->isInDict(di)) {
                const unsigned dictOffset = di*dictLen;
                if(end >= wlen)
                    on[dictOffset + (end-wlen)*3*max +/*0*max*/+ lablen] = 1;
                if(end != len)
                    on[dictOffset +     end*3*max    +  2*max  + lablen] = 1;
                for(unsigned k = end-wlen+1; k < end; k++)
                    on[dictOffset +     k*3*max    +  1*max  + lablen] = 1;
            }
        }        
    }
    for(unsigned i = 0; i < len; i++) {
        for(unsigned di = 0; di < dict_->getNumDicts(); di++) {
            char* myOn = &on[di*dictLen + i*3*max];
            for(unsigned j = 0; j < 3*max; j++) {
                unsigned featId = 3*max*di+j;
                if(myOn[j] && dictFeats_[featId]) {
                    features[i].push_back(dictFeats_[featId]);
                    ret++;
                }
            }
        }
    }
    return ret;
}
unsigned Kinkaku::wsNgramFeatures(const KinkakuString & chars, SentenceFeatures & features, const vector<KinkakuString> & prefixes, int n) {
    const int featSize = (int)features.size(), charLength = (int)chars.length(), w = (int)prefixes.size()/2;
    unsigned ret = 0, thisFeat;
    for(int i = 0; i < featSize; i++) {
        const int rightBound=min(i+w+1,charLength);
        vector<FeatureId> & myFeats = features[i];
        for(int j = i-w+1; j < rightBound; j++) {
            if(j < 0) continue;
            KinkakuString str = prefixes[j-i+w-1];
            const int nextRight = min(j+n, rightBound);
            for(int k = j; k<nextRight; k++) {
                str = str+chars[k];
                thisFeat = wsModel_->mapFeat(str);
                if(thisFeat) {
                    myFeats.push_back(thisFeat);
                    ret++;
                }
            }
        }
    }
    return ret;
}



void Kinkaku::preparePrefixes() {
    if(config_->getDoWS() && wsModel_) {
        const char cs[3] = { 'L', 'I', 'R' };
        dictFeats_.resize(0);
        for(unsigned di = 0; di < dict_->getNumDicts(); di++) {
            for(unsigned i = 0; i < 3; i++) {
                for(unsigned j = 0; j < (unsigned)config_->getDictionaryN(); j++) {
                    ostringstream buff;
                    buff << "D" << di << cs[i] << (j+1);
                    dictFeats_.push_back(wsModel_->mapFeat(util_->mapString(buff.str())));
                }
            }
        }
    }
    charPrefixes_.resize(0);
    for(int i = 1; i <= 2*(int)config_->getCharWindow(); i++) {
        ostringstream buff;
        buff << "X" << i-(int)config_->getCharWindow();
        charPrefixes_.push_back(util_->mapString(buff.str()));
    }
    typePrefixes_.resize(0);
    for(int i = 1; i <= 2*(int)config_->getTypeWindow(); i++) {
        ostringstream buff;
        buff << "T" << i-(int)config_->getTypeWindow();
        typePrefixes_.push_back(util_->mapString(buff.str()));
    }
}

void Kinkaku::trainWS() {
    if(wsModel_)
        delete wsModel_;
    TagTriplet * trip = fio_->getFeatures(util_->mapString("WS"),true);
    if(trip->third)
        wsModel_ = trip->third;
    else 
        trip->third = wsModel_ = new KinkakuModel();

    if(config_->getDebug() > 0)
        cerr << "Creating word segmentation features ";
    vector<unsigned> dictFeats;
    bool hasDictionary = (dict_->getNumDicts() > 0 && dict_->getStates().size() > 0);
    preparePrefixes();
    unsigned scount = 0;
    vector< vector<unsigned> > & xs = trip->first;
    vector<int> & ys = trip->second;
    for(Sentences::const_iterator it = sentences_.begin(); it != sentences_.end(); it++) {
        if(++scount % 1000 == 0)
            cerr << ".";
        KinkakuSentence * sent = *it;
        SentenceFeatures feats(sent->wsConfs.size());
        unsigned fts = 0;
        if(hasDictionary)
            fts += wsDictionaryFeatures(sent->norm, feats);
        fts += wsNgramFeatures(sent->norm, feats, charPrefixes_, config_->getCharN());
        string str = util_->getTypeString(sent->norm);
        fts += wsNgramFeatures(util_->mapString(str), feats, typePrefixes_, config_->getTypeN());
        for(unsigned i = 0; i < feats.size(); i++) {
            if(abs(sent->wsConfs[i]) > config_->getConfidence()) {
                xs.push_back(feats[i]);
                ys.push_back(sent->wsConfs[i]>1?1:-1);
            }
        }
    }
    if(config_->getDebug() > 0)
        cerr << " done!" << endl << "Building classifier ";

    wsModel_->trainModel(xs,ys,config_->getBias(),config_->getSolverType(),config_->getEpsilon(),config_->getCost());

    if(config_->getDebug() > 0)
        cerr << " done!" << endl;

    fio_->printFeatures(util_->mapString("WS"),util_);

}


unsigned Kinkaku::tagNgramFeatures(const KinkakuString & chars, vector<unsigned> & feat, const vector<KinkakuString> & prefixes, KinkakuModel * model, int n, int sc, int ec) {
    int w = (int)prefixes.size()/2;
    vector<KinkakuChar> wind(prefixes.size());
    for(int i = w-1; i >= 0; i--)
        wind[w-i-1] = (sc-i<0?0:chars[sc-i]);
    for(int i = 0; i < w; i++)
        wind[w+i] = (ec+i>=(int)chars.length()?0:chars[ec+i]);
    unsigned ret = 0, thisFeat = 0;
    for(unsigned i = 0; i < wind.size(); i++) {
        if(wind[i] == 0) continue; 
        KinkakuString str = prefixes[i];
        for(int k = 0; k < n && i+k < wind.size() && wind[i+k] != 0; k++) {
            str = str+wind[i+k];
            thisFeat = model->mapFeat(str);
            if(thisFeat) {
                feat.push_back(thisFeat);
                ret++;
            }
        }
    }
    return ret;
}

unsigned Kinkaku::tagSelfFeatures(const KinkakuString & self, vector<unsigned> & feat, const KinkakuString & pref, KinkakuModel * model) {
    unsigned thisFeat = model->mapFeat(pref+self), ret = 0;
    if(thisFeat) {
        feat.push_back(thisFeat);
        ret++;
    }
    return ret;
}

void Kinkaku::trainGlobalTags(int lev) {
    if(dict_ == 0)
        return;
    if(config_->getDebug() > 0)
        cerr << "Creating tagging features (tag "<<lev+1<<") ";

    bool wsAdd = false;
    if(wsModel_) {
        wsAdd = wsModel_->getAddFeatures();
        wsModel_->setAddFeatures(false);
    }
    preparePrefixes();
    if(wsModel_)
        wsModel_->setAddFeatures(wsAdd);

    ostringstream oss; oss << "T "<<lev<<" G";
    KinkakuString featId = util_->mapString(oss.str());
    TagTriplet * trip = fio_->getFeatures(featId,true);
    globalMods_[lev] = (trip->third?trip->third:new KinkakuModel());
    trip->third = globalMods_[lev];
    KinkakuString kssx = util_->mapString("SX"), ksst = util_->mapString("ST");
    
    for(Sentences::const_iterator it = sentences_.begin(); it != sentences_.end(); it++) {
        int startPos = 0, finPos=0;
        KinkakuString charStr = (*it)->norm;
        KinkakuString typeStr = util_->mapString(util_->getTypeString(charStr));
        for(unsigned j = 0; j < (*it)->words.size(); j++) {
            startPos = finPos;
            KinkakuWord & word = (*it)->words[j];
            finPos = startPos+word.norm.length();
            if(!word.getTag(lev) || word.getTagConf(lev) <= config_->getConfidence())
                continue;
            unsigned myTag;
            KinkakuString tagSurf = word.getTagSurf(lev);
            for(myTag = 0; myTag < trip->fourth.size() && tagSurf != trip->fourth[myTag]; myTag++);
            if(myTag == trip->fourth.size()) 
                trip->fourth.push_back(tagSurf);
            myTag++;
            vector<unsigned> feat;
            tagNgramFeatures(charStr, feat, charPrefixes_, trip->third, config_->getCharN(), startPos-1, finPos);
            tagNgramFeatures(typeStr, feat, typePrefixes_, trip->third, config_->getTypeN(), startPos-1, finPos);
            tagSelfFeatures(word.norm, feat, kssx, trip->third);
            tagSelfFeatures(util_->mapString(util_->getTypeString(word.norm)), feat, ksst, trip->third);
            tagDictFeatures(word.norm, lev, feat, trip->third);
            trip->first.push_back(feat);
            trip->second.push_back(myTag);
        }
    }
    if(config_->getDebug() > 0)
        cerr << "done!" << endl << "Training global tag classifiers ";


    trip->third->trainModel(trip->first,trip->second,config_->getBias(),config_->getSolverType(),config_->getEpsilon(),config_->getCost()); 

    globalTags_[lev] = trip->fourth;
    if(config_->getDebug() > 0)
        cerr << "done with " << globalTags_[lev].size() << " labels and " << 
                trip->third->getNumFeatures() << " features!" << endl;

    fio_->printFeatures(featId,util_);
}

template <class T>
T max(const vector<int> & vec) {
    T myMax = 0;
    for(unsigned i = 0; i < vec.size(); i++)
        if(myMax < vec[i])
            myMax = vec[i];
    return myMax;
}

void Kinkaku::trainLocalTags(int lev) {
    if(config_->getDebug() > 0)
        cerr << "Creating tagging features (tag "<<lev+1<<") ";
    if(dict_ == 0)
        return;
    ostringstream oss; oss << "T "<<lev<<" L ";
    KinkakuString featId = util_->mapString(oss.str());
    bool wsAdd = false;
    if(wsModel_) {
        wsAdd = wsModel_->getAddFeatures();
        wsModel_->setAddFeatures(false);
    }
    preparePrefixes();
    if(wsModel_)
        wsModel_->setAddFeatures(wsAdd);
    vector<ModelTagEntry*> & entries = dict_->getEntries();
    ModelTagEntry* myEntry = 0;
    for(unsigned i = 0; i < entries.size(); i++) {
        myEntry = entries[i];
        if((int)myEntry->tags.size() > lev && (myEntry->tags[lev].size() > 1 || config_->getWriteFeatures())) {
            TagTriplet * trip = fio_->getFeatures(featId+myEntry->word,true);
            if((int)myEntry->tagMods.size() <= lev)
                myEntry->tagMods.resize(lev+1,0);
            myEntry->tagMods[lev] = (trip->third ? trip->third : new KinkakuModel());
            trip->third = myEntry->tagMods[lev];
            trip->fourth = myEntry->tags[lev];
        }
    }
    for(Sentences::const_iterator it = sentences_.begin(); it != sentences_.end(); it++) {
        int startPos = 0, finPos=0;
        KinkakuString charStr = (*it)->norm;
        KinkakuString typeStr = util_->mapString(util_->getTypeString(charStr));
        for(unsigned j = 0; j < (*it)->words.size(); j++) {
            startPos = finPos;
            KinkakuWord & word = (*it)->words[j];
            finPos = startPos+word.norm.length();
            if(!word.getTag(lev) || word.getTagConf(lev) <= config_->getConfidence())
                continue;
            TagTriplet * trip = fio_->getFeatures(featId+word.norm,false);
            if(trip) {
                unsigned myTag = dict_->getTagID(word.norm,word.getTagSurf(lev),lev);
                if(myTag != 0) {
                    vector<unsigned> feat;
                    tagNgramFeatures(charStr, feat, charPrefixes_, trip->third, config_->getCharN(), startPos-1, finPos);
                    tagNgramFeatures(typeStr, feat, typePrefixes_, trip->third, config_->getTypeN(), startPos-1, finPos);
                    trip->first.push_back(feat);
                    trip->second.push_back(myTag);
                }
            }
        }
    }
    if(config_->getDebug() > 0)
        cerr << "done!" << endl << "Training local tag classifiers ";
    for(unsigned i = 0; i < entries.size(); i++) {
        myEntry = entries[i];
        if((int)myEntry->tags.size() > lev && (myEntry->tags[lev].size() > 1 || config_->getWriteFeatures())) {
            TagTriplet * trip = fio_->getFeatures(featId+myEntry->word,false);
            if(!trip) THROW_ERROR("FATAL: Unbuilt model in entry table");
            vector< vector<unsigned> > & xs = trip->first;
            vector<int> & ys = trip->second;
            
            trip->third->trainModel(xs,ys,config_->getBias(),config_->getSolverType(),config_->getEpsilon(),config_->getCost());
            if(trip->third->getNumClasses() == 1) {
                int myLab = trip->third->getLabel(0)-1;
                KinkakuString tmpString = myEntry->tags[lev][0]; myEntry->tags[lev][0] = myEntry->tags[lev][myLab]; myEntry->tags[lev][myLab] = tmpString;
                char tmpDict = myEntry->tagInDicts[lev][0]; myEntry->tagInDicts[lev][0] = myEntry->tagInDicts[lev][myLab]; myEntry->tagInDicts[lev][myLab] = tmpDict;
            }
        }
    }

    fio_->printFeatures(featId,util_);

    if(config_->getDebug() > 0)
        cerr << "done!" << endl;
}

vector<pair<int,int> > Kinkaku::getDictionaryMatches(const KinkakuString & surf, int lev) {
    vector<pair<int,int> > ret;
    if(!dict_) return ret;
    const ModelTagEntry* ent = dict_->findEntry(surf);
    if(ent == 0 || ent->inDict == 0 || (int)ent->tagInDicts.size() <= lev)
        return ret;
    const vector<unsigned char> & tid = ent->tagInDicts[lev];
    for(int i = 0; i < (int)tid.size(); i++) {
        for(int j = 0; j < dict_->getNumDicts(); j++)
            if(ModelTagEntry::isInDict(tid[i],j)) 
                ret.push_back(pair<int,int>(j,i));
    }
    return ret;
}

unsigned Kinkaku::tagDictFeatures(const KinkakuString & surf, int lev, vector<unsigned> & myFeats, KinkakuModel * model) {
    vector<pair<int,int> > matches = getDictionaryMatches(surf,lev);
    if(matches.size() == 0) {
        unsigned thisFeat = model->mapFeat(util_->mapString("UNK"));
        if(thisFeat) { myFeats.push_back(thisFeat); return 1; }
        return 0;
    }
    int ret = 0;
    for(int i = 0; i < (int)matches.size(); i++) {
        ostringstream oss; oss << "D" << matches[i].first << "T" << matches[i].second;
        unsigned thisFeat = model->mapFeat(util_->mapString(oss.str()));
        if(thisFeat != 0) {
            myFeats.push_back(thisFeat);
            ret++;
        } 
    }
    return ret;
}

void Kinkaku::trainSanityCheck() {
    if(config_->getCorpusFiles().size() == 0 && config_->getFeatureIn().length() == 0) {
        THROW_ERROR("At least one input corpus must be specified (-part/-full/-prob)");
    } else if(config_->getDictionaryFiles().size() > 8) {
        THROW_ERROR("The maximum number of dictionaries that can be specified is 8.");
    } else if(config_->getModelFile().length() == 0) {
        THROW_ERROR("An output model file must be specified when training (-model)");
    }
    ModelIO * modout = ModelIO::createIO(config_->getModelFile().c_str(),config_->getModelFormat(), true, *config_);
    delete modout;
}


inline void collectCounts(vector<unsigned> & vec, unsigned pos) {
    for(unsigned i = 0; i < pos; i++) {
        if(vec.size() <= i) vec.push_back(1);
        else                 vec[i]++;
    }
}
void Kinkaku::trainUnk(int lev) {
    
    if(!subwordDict_) {
        Dictionary<ProbTagEntry>::WordMap subwordMap;
        scanDictionaries<ProbTagEntry>(config_->getSubwordDictFiles(), subwordMap, config_, util_, false);
        subwordDict_ = new Dictionary<ProbTagEntry>(util_);
        subwordDict_->buildIndex(subwordMap);
    }

    if(config_->getDebug() > 0)
        cerr << " Aligning pronunciation strings" << endl;
    typedef vector< pair<unsigned,unsigned> > AlignHyp;
    const vector<ModelTagEntry*> & dictEntries = dict_->getEntries();
    Dictionary<ProbTagEntry>::WordMap tagMap;
    vector<KinkakuString> tagCorpus;
    for(unsigned w = 0; w < dictEntries.size(); w++) {
        const TagEntry* myDictEntry = dictEntries[w];
        const KinkakuString & word = myDictEntry->word;
        const unsigned wordLen = word.length();
        if((int)myDictEntry->tags.size() <= lev) continue;
        for(unsigned p = 0; p < myDictEntry->tags[lev].size(); p++) {
            const KinkakuString & tag = myDictEntry->tags[lev][p];
            vector< vector< AlignHyp > > stacks(wordLen+1, vector< AlignHyp >());
            stacks[0].push_back(AlignHyp(1,pair<unsigned,unsigned>(0,0)));
            Dictionary<ProbTagEntry>::MatchResult matches = subwordDict_->match(word);
            for(unsigned i = 0; i < matches.size(); i++) {
                const ProbTagEntry* mySubEntry = matches[i].second;
                const unsigned cend = matches[i].first+1;
                const unsigned cstart = cend-mySubEntry->word.length();
                for(unsigned j = 0; j < stacks[cstart].size(); j++) {
                    const AlignHyp & myHyp = stacks[cstart][j];
                    const unsigned pstart = myHyp[myHyp.size()-1].second;
                    if((int)mySubEntry->tags.size() <= lev) continue;
                    for(unsigned k = 0; k < mySubEntry->tags[lev].size(); k++) {
                        const KinkakuString & pstr = mySubEntry->tags[lev][k];
                        const unsigned pend = pstart+pstr.length();
                        if(pend <= tag.length() && tag.substr(pstart,pend-pstart) == pstr) {
                            AlignHyp nextHyp = myHyp;
                            nextHyp.push_back( pair<unsigned,unsigned>(cend,pend) );
                            stacks[cend].push_back(nextHyp);
                        }
                    }
                }
            }
            for(unsigned i = 0; i < stacks[wordLen].size(); i++) {
                const AlignHyp & myHyp = stacks[wordLen][i];
                if(myHyp[myHyp.size()-1].second == tag.length()) {
                    tagCorpus.push_back(tag);
                    for(unsigned j = 1; j < myHyp.size(); j++) {
                        KinkakuString subChar = word.substr(myHyp[j-1].first,myHyp[j].first-myHyp[j-1].first);
                        KinkakuString subTag = tag.substr(myHyp[j-1].second,myHyp[j].second-myHyp[j-1].second);
                        ProbTagEntry* mySubEntry = subwordDict_->findEntry(subChar);
                        mySubEntry->incrementProb(subTag,lev);
                        addTag<ProbTagEntry>(tagMap,subTag,lev,&subTag,0);
                    }
                    break;
                }
            }
        }
    }
    if(tagMap.size() == 0) {
        cerr << " No words found! Aborting unknown model for level "<<lev<<endl;
        return;
    }
    if(config_->getDebug() > 0)
        cerr << " Building index" << endl;
    Dictionary<ProbTagEntry> tagDict(util_);
    tagDict.buildIndex(tagMap);

    if(config_->getDebug() > 0)
        cerr << " Calculating alpha" << endl;
    TwoCountHash tagCounts;
    const vector<ProbTagEntry*> & subEntries = subwordDict_->getEntries();
    vector<unsigned> numerCounts;
    for(unsigned w = 0; w < subEntries.size(); w++) {
        const ProbTagEntry* mySubEntry = subEntries[w];
        if((int)mySubEntry->tags.size() <= lev) continue;
        for(unsigned p = 0; p < mySubEntry->tags[lev].size(); p++) {
            const KinkakuString& tag = mySubEntry->tags[lev][p];
            TwoCountHash::iterator pcit = tagCounts.find(tag);
            if(pcit == tagCounts.end())
                pcit = tagCounts.insert(TwoCountHash::value_type(tag,TwoCountHash::mapped_type(0,0))).first;
            unsigned totalTagCounts = (unsigned)(mySubEntry->probs[lev].size()>p?mySubEntry->probs[lev][p]:0);
            pcit->second.first++; 
            pcit->second.second += totalTagCounts; 
            collectCounts(numerCounts,totalTagCounts);
        }
    }
    vector< vector<unsigned> > denomCounts;
    for(TwoCountHash::const_iterator it = tagCounts.begin(); it != tagCounts.end(); it++) {
        if(denomCounts.size() < it->second.first) 
            denomCounts.resize(it->second.first,vector<unsigned>());
        collectCounts(denomCounts[it->second.first-1],it->second.second);
    }
    double alpha = 0.0001, maxAlpha = 100, changeCutoff = 0.0000001, change = 1;
    while(abs(change) > changeCutoff && alpha < maxAlpha) {
        double der1 = 0, der2 = 0, lik = 0, den = 0;
        for(unsigned i = 0; i < numerCounts.size(); i++) {
            den = alpha+i;
            der1 += numerCounts[i]/den;
            der2 -= numerCounts[i]/den/den;
            lik += numerCounts[i]*log(den);
        }
        for(unsigned i = 0; i < denomCounts.size(); i++) {
            for(unsigned j = 0; j < denomCounts[i].size(); j++) {
                den = (i+1)*alpha+j;
                der1 -= denomCounts[i][j]*(i+1)/den;
                der2 += denomCounts[i][j]*(i+1)*(i+1)/den/den;
                lik -= denomCounts[i][j]*log(den);
            }
        }
        change = -1*der1/der2;
        alpha += change;
    }
    if(alpha > maxAlpha) {
        alpha = 1;
        if(config_->getDebug() > 0)
            cerr << "WARNING: Alpha maximization exploded, reverting to alpha="<<alpha<<endl;
    }
    
    for(unsigned p = 0; p < tagCorpus.size(); p++) {
        Dictionary<ProbTagEntry>::MatchResult matches = tagDict.match(tagCorpus[p]);
        for(unsigned m = 0; m < matches.size(); m++)
            matches[m].second->incrementProb(matches[m].second->word,lev);
    }

    for(unsigned w = 0; w < subEntries.size(); w++) {
        ProbTagEntry* mySubEntry = subEntries[w];
        if(mySubEntry->probs[lev].size() != mySubEntry->tags[lev].size())
            mySubEntry->probs[lev].resize(mySubEntry->tags[lev].size(),0);
        for(unsigned p = 0; p < mySubEntry->tags[lev].size(); p++) {
            const KinkakuString & tag = mySubEntry->tags[lev][p];
            ProbTagEntry* myTagEntry = tagDict.findEntry(tag);
            double origCount = mySubEntry->probs[lev][p];
            pair<unsigned,unsigned> myTagCounts = tagCounts[tag];
            mySubEntry->probs[lev][p] = (mySubEntry->probs[lev][p]+alpha) / (myTagCounts.second+alpha*myTagCounts.first);
            if(myTagEntry)
                mySubEntry->probs[lev][p] *= myTagCounts.second/myTagEntry->probs[lev][0];
            else if (origCount != 0.0) 
                THROW_ERROR("FATAL: Numerator found but denominator not in TM calculation");
            mySubEntry->probs[lev][p] = log(mySubEntry->probs[lev][p]);
        }
    }
    
    if(config_->getDebug() > 0)
        cerr << " Calculating LM" << endl;
    if((int)subwordModels_.size() <= lev) subwordModels_.resize(lev+1,0);
    subwordModels_[lev] = new KinkakuLM(config_->getUnkN());
    subwordModels_[lev]->train(tagCorpus);

}

void Kinkaku::buildFeatureLookups() {
    if(wsModel_) {
        wsModel_->buildFeatureLookup(util_, config_->getCharWindow(), config_->getTypeWindow(), dict_->getNumDicts(), config_->getDictionaryN());
    }
    for(int i = 0; i < (int)globalMods_.size(); i++)
        if(globalMods_[i])
            globalMods_[i]->buildFeatureLookup(util_, config_->getCharWindow(), config_->getTypeWindow(), dict_->getNumDicts(), config_->getDictionaryN());
    vector<ModelTagEntry*> & localEntries = dict_->getEntries();
    for(int i = 0; i < (int)localEntries.size(); i++) {
        if(localEntries[i]) {
            for(int j = 0; j < (int)localEntries[i]->tagMods.size(); j++) {
                if(localEntries[i]->tagMods[j]) {
                    localEntries[i]->tagMods[j]->buildFeatureLookup(util_, config_->getCharWindow(), config_->getTypeWindow(), dict_->getNumDicts(), config_->getDictionaryN());    
                }
            }
        }
    }

}

void Kinkaku::writeModel(const char* fileName) {

    if(config_->getDebug() > 0)    
        cerr << "Printing model to " << fileName;
    buildFeatureLookups();

    ModelIO * modout = ModelIO::createIO(fileName,config_->getModelFormat(), true, *config_);
    modout->writeConfig(*config_);
    modout->writeModel(wsModel_);
    for(int i = 0; i < config_->getNumTags(); i++) {
        modout->writeWordList(i >= (int)globalTags_.size()?vector<KinkakuString>():globalTags_[i]);
        modout->writeModel(i >= (int)globalMods_.size()?0:globalMods_[i]);
    }
    modout->writeModelDictionary(dict_);
    modout->writeProbDictionary(subwordDict_);
    for(int i = 0; i < config_->getNumTags(); i++)
        modout->writeLM(i>=(int)subwordModels_.size()?0:subwordModels_[i]);

    delete modout;

    if(config_->getDebug() > 0)    
        cerr << " done!" << endl;

}

void Kinkaku::readModel(const char* fileName) {
    
    if(config_->getDebug() > 0)
        cerr << "Reading model from " << fileName;

    
    ModelIO * modin = ModelIO::createIO(fileName,ModelIO::FORMAT_UNKNOWN, false, *config_);
    util_ = config_->getStringUtil();

    modin->readConfig(*config_);
    wsModel_ = modin->readModel();

    globalMods_.resize(config_->getNumTags(),0);
    globalTags_.resize(config_->getNumTags(), vector<KinkakuString>());
    for(int i = 0; i < config_->getNumTags(); i++) {
        globalTags_[i] = modin->readWordList();
        globalMods_[i] = modin->readModel();
    }
    dict_ = modin->readModelDictionary();
    subwordDict_ = modin->readProbDictionary();
    subwordModels_.resize(config_->getNumTags(),0);
    for(int i = 0; i < config_->getNumTags(); i++)
        subwordModels_[i] = modin->readLM();

    delete modin;
    
    preparePrefixes();

    if(config_->getDebug() > 0)    
        cerr << " done!" << endl;
}

void Kinkaku::calculateWS(KinkakuSentence & sent) {
    if(!wsModel_)
        THROW_ERROR("This model cannot be used for word segmentation.");
    
    if(sent.norm.length() == 0)
        return;

    FeatureLookup * featLookup = wsModel_->getFeatureLookup();
    vector<FeatSum> scores(sent.norm.length()-1, featLookup->getBias(0));
    featLookup->addNgramScores(featLookup->getCharDict(), sent.norm, config_->getCharWindow(), scores);
    const string & type_str = util_->getTypeString(sent.norm);
    featLookup->addNgramScores(featLookup->getTypeDict(), util_->mapString(type_str), config_->getTypeWindow(), scores);
    if(featLookup->getDictVector())
        featLookup->addDictionaryScores(
            dict_->match(sent.norm),
            dict_->getNumDicts(), config_->getDictionaryN(),
            scores);
    
    const string & wsc = config_->getWsConstraint();
    if(wsc.size())
        for(unsigned i = 0; i < scores.size(); i++)
            if(type_str[i]==type_str[i+1] && wsc.find(type_str[i]) != std::string::npos)
                scores[i] = KinkakuModel::isProbabilistic(config_->getSolverType())?0:-100;

    for(unsigned i = 0; i < sent.wsConfs.size(); i++)
        if(abs(sent.wsConfs[i]) <= config_->getConfidence())
            sent.wsConfs[i] = scores[i]*wsModel_->getMultiplier();
    sent.refreshWS(config_->getConfidence());
    for(int i = 0; i < (int)sent.words.size(); i++) {
        KinkakuWord & word = sent.words[i];
        word.setUnknown(dict_->findEntry(word.norm) == 0);
    }
    if(KinkakuModel::isProbabilistic(config_->getSolverType())) {
        for(unsigned i = 0; i < sent.wsConfs.size(); i++)
            sent.wsConfs[i] = 1/(1.0+exp(-abs(sent.wsConfs[i])));
    }
}

bool kinkakuTagMore(const KinkakuTag a, const KinkakuTag b) {
    return a.second > b.second;
}

# define BEAM_SIZE 50
vector< KinkakuTag > Kinkaku::generateTagCandidates(const KinkakuString & str, int lev) {
    Dictionary<ProbTagEntry>::MatchResult matches = subwordDict_->match(str);
    vector< vector< KinkakuTag > > stack(str.length()+1);
    stack[0].push_back(KinkakuTag(KinkakuString(),0));
    unsigned end, start, lastEnd = 0;
    for(unsigned i = 0; i < matches.size(); i++) {
        ProbTagEntry* entry = matches[i].second;
        end = matches[i].first+1;
        start = end-entry->word.length();
        if(end != lastEnd && config_->getUnkBeam() > 0 && stack[lastEnd].size() > config_->getUnkBeam()) {
            sort(stack[lastEnd].begin(), stack[lastEnd].end(), kinkakuTagMore);
            stack[lastEnd].resize(config_->getUnkBeam());
        }
        lastEnd = end;
        for(unsigned j = 0; j < entry->tags[lev].size(); j++) {
            for(unsigned k = 0; k < stack[start].size(); k++) {
                KinkakuTag nextPair(
                    stack[start][k].first+entry->tags[lev][j],
                    stack[start][k].second+entry->probs[lev][j]
                );
                for(unsigned pos = stack[start][k].first.length(); pos < nextPair.first.length(); pos++) {
                    nextPair.second += subwordModels_[lev]->scoreSingle(nextPair.first,pos);
                }
                stack[end].push_back(nextPair);
            }
        }
    }
    vector<KinkakuTag> ret = stack[stack.size()-1];
    for(unsigned i = 0; i < ret.size(); i++)
        ret[i].second += subwordModels_[lev]->scoreSingle(ret[i].first,ret[i].first.length());
    return ret;
}
void Kinkaku::calculateUnknownTag(KinkakuWord & word, int lev) {
    if(lev >= (int)subwordModels_.size() || subwordModels_[lev] == 0) return;
    if(word.norm.length() > 256) {
        cerr << "WARNING: skipping pronunciation estimation for extremely long unknown word of length "
            <<word.norm.length()<<" starting with '"
            <<util_->showString(word.norm.substr(0,20))<<"'"<<endl;
        word.addTag(lev, KinkakuTag(util_->mapString("<NULL>"),0));
        return;
    }
    if((int)word.tags.size() <= lev) word.tags.resize(lev+1);
    word.tags[lev] = generateTagCandidates(word.norm, lev);
    vector<KinkakuTag> & tags = word.tags[lev];
    double maxProb = -1e20, totalProb = 0;
    for(unsigned i = 0; i < tags.size(); i++)
        maxProb = max(maxProb,tags[i].second);
    for(unsigned i = 0; i < tags.size(); i++) {
        tags[i].second = exp(tags[i].second-maxProb);
        totalProb += tags[i].second;
    }
    for(unsigned i = 0; i < tags.size(); i++)
        tags[i].second /= totalProb;
    sort(tags.begin(), tags.end());
    if(config_->getTagMax() != 0 && config_->getTagMax() < tags.size())
        tags.resize(config_->getTagMax());

}
void Kinkaku::calculateTags(KinkakuSentence & sent, int lev) {
    int startPos = 0, finPos=0;
    KinkakuString charStr = sent.norm;
    KinkakuString typeStr = util_->mapString(util_->getTypeString(charStr));
    KinkakuString kssx = util_->mapString("SX"), ksst = util_->mapString("ST");
    const string & defTag = config_->getDefaultTag();
    for(unsigned i = 0; i < sent.words.size(); i++) {
        KinkakuWord & word = sent.words[i];
        if((int)word.tags.size() > lev
            && (int)word.tags[lev].size() > 0
            && abs(word.tags[lev][0].second) > config_->getConfidence())
                continue;
        startPos = finPos;
        finPos = startPos+word.norm.length();
        ModelTagEntry* ent = dict_->findEntry(word.norm);
        word.setUnknown(ent == 0);
        vector<KinkakuString> * tags = 0;
        KinkakuModel * tagMod = 0;
        bool useSelf = false;
        if(lev < (int)globalMods_.size() && globalMods_[lev] != 0) {
            tagMod = globalMods_[lev];
            tags = &globalTags_[lev];
            useSelf = true;
        }
        else if(ent != 0 && (int)ent->tags.size() > lev) {
            tagMod = ent->tagMods[lev];
            tags = &(ent->tags[lev]);
        }
        if(tags == 0 || tags->size() == 0) {
            if(config_->getDoUnk()) {
                calculateUnknownTag(word,lev);
                if(config_->getDebug() >= 2)
                    cerr << "Tag "<<i+1<<" ("<<util_->showString(sent.words[i].surface)<<"->UNK)"<<endl;
            }
        }
        else {
            vector<unsigned> feat;
            FeatureLookup * look;
            if(tagMod == 0 || (look = tagMod->getFeatureLookup()) == NULL)
                word.setTag(lev, KinkakuTag((*tags)[0],(KinkakuModel::isProbabilistic(config_->getSolverType())?1:100)));
            else {        
#ifdef KINKAKU_SAFE
                if(look == NULL) THROW_ERROR("null lookure lookup during analysis");
#endif
                vector<FeatSum> scores(tagMod->getNumWeights(), 0);
                look->addTagNgrams(charStr, look->getCharDict(), scores, config_->getCharN(), startPos, finPos);
                look->addTagNgrams(typeStr, look->getTypeDict(), scores, config_->getTypeN(), startPos, finPos);
                if(useSelf) {
                    look->addSelfWeights(charStr.substr(startPos,finPos-startPos), scores, 0);
                    look->addSelfWeights(typeStr.substr(startPos,finPos-startPos), scores, 1);
                    look->addTagDictWeights(getDictionaryMatches(charStr.substr(startPos,finPos-startPos), 0), scores);
                }
                for(int j = 0; j < (int)scores.size(); j++) 
                    scores[j] += look->getBias(j);
                if(scores.size() == 1)
                    scores.push_back(KinkakuModel::isProbabilistic(config_->getSolverType())?-1*scores[0]:0);
                word.clearTags(lev);
                for(int i = 0; i < (int)scores.size(); i++)
                    word.addTag(lev, KinkakuTag((*tags)[i],scores[i]*tagMod->getMultiplier()));
                sort(word.tags[lev].begin(), word.tags[lev].end(), kinkakuTagMore);
                if(KinkakuModel::isProbabilistic(config_->getSolverType())) {
                    double sum = 0;
                    for(int i = 0; i < (int)word.tags[lev].size(); i++) {
                        word.tags[lev][i].second = exp(word.tags[lev][i].second);
                        sum += word.tags[lev][i].second;
                    }
                    for(int i = 0; i < (int)word.tags[lev].size(); i++) {
                        word.tags[lev][i].second /= sum;
                    }
                } else {
                    double secondBest = word.tags[lev][1].second;
                    for(int i = 0; i < (int)word.tags[lev].size(); i++)
                        word.tags[lev][i].second -= secondBest;
                }
            }
        }
        if(!word.hasTag(lev) && defTag.length())
            word.addTag(lev,KinkakuTag(util_->mapString(defTag),0));
        if(config_->getTagMax() > 0)
            word.limitTags(lev,config_->getTagMax());
    }
}

void Kinkaku::trainAll() {
    
    trainSanityCheck();
    
    if(config_->getFeatureIn().length()) {
        if(config_->getDebug() > 0)
            cerr << "Loading features from "<<config_->getFeatureIn() << "...";
        fio_->load(config_->getFeatureIn(),util_);
        if(config_->getDebug() > 0)
            cerr << " done!" << endl;
    }
    config_->setNumTags(max(config_->getNumTags(),fio_->getNumTags()));
    if(config_->getFeatureOut().length())
        fio_->openOut(config_->getFeatureOut());

    buildVocabulary();
    fio_->setNumTags(config_->getNumTags());
    fio_->printWordMap(util_);

    if(config_->getDoWS())
        trainWS();
    
    if(config_->getDoTags()) {
        if((int)globalMods_.size() <= config_->getNumTags()) {
            globalMods_.resize(config_->getNumTags(),0);
            globalTags_.resize(config_->getNumTags(), vector<KinkakuString>());
        }
        for(int i = 0; i < config_->getNumTags(); i++) {
            if(config_->getGlobal(i))
                trainGlobalTags(i);
            else {
                trainLocalTags(i);
                if(config_->getSubwordDictFiles().size() > 0)
                    trainUnk(i);
            }
        }
    }

    fio_->closeOut();

    writeModel(config_->getModelFile().c_str());

}

void Kinkaku::analyze() {
    
    if(config_->getInputFormat() == CORP_FORMAT_FULL || config_->getInputFormat() == CORP_FORMAT_TOK)
        config_->setDoWS(false);

    std::ostringstream buff;
    if(config_->getModelFile().length() == 0)
        throw std::runtime_error("A model file must be specified to run Kinkaku (-model)");
    
    readModel(config_->getModelFile().c_str());
    if(!config_->getDoWS() && !config_->getDoTags()) {
        buff << "Both word segmentation and tagging are disabled." << std::endl
             << "At least one must be selected to perform processing." << std::endl;
        throw std::runtime_error(buff.str());
    }
    if(config_->getDoWS()) {
        if(config_->getInputFormat() == CORP_FORMAT_DEFAULT)
           config_->setInputFormat(CORP_FORMAT_RAW);
    } else {
        if(config_->getInputFormat() == CORP_FORMAT_DEFAULT)
             config_->setInputFormat(CORP_FORMAT_TOK);
        else if(config_->getInputFormat() == CORP_FORMAT_RAW) {
            buff << "In order to handle raw corpus input, word segmentation must be turned on." << std::endl
                 << "Either specify -in {full,part,prob}, stop using -nows, or train a new " << std::endl
                 << "model that has word segmentation included." << std::endl;
            throw std::runtime_error(buff.str());
        }
    }
    if(config_->getDoWS() && wsModel_ == NULL)
        THROW_ERROR("Word segmentation cannot be performed with this model. A new model must be retrained without the -nows option.");

    if(config_->getDebug() > 0)    
        cerr << "Analyzing input ";

    CorpusIO *in, *out;
    iostream *inStr = 0, *outStr = 0;
    const vector<string> & args = config_->getArguments();
    if(args.size() > 0) {
        in  = CorpusIO::createIO(args[0].c_str(),config_->getInputFormat(), *config_, false, util_);
    } else {
        inStr = new iostream(cin.rdbuf());
        in  = CorpusIO::createIO(*inStr, config_->getInputFormat(), *config_, false, util_);
    }
    if(args.size() > 1) {
        out  = CorpusIO::createIO(args[1].c_str(),config_->getOutputFormat(), *config_, true, util_);
    } else {
        outStr = new iostream(cout.rdbuf());
        out = CorpusIO::createIO(*outStr, config_->getOutputFormat(), *config_, true, util_);
    }
    out->setUnkTag(config_->getUnkTag());
    out->setNumTags(config_->getNumTags());
    for(int i = 0; i < config_->getNumTags(); i++)
        out->setDoTag(i,config_->getDoTag(i));

    KinkakuSentence* next;
    while((next = in->readSentence()) != 0) {
        if(config_->getDoWS())
            calculateWS(*next);
        if(config_->getDoTags())
            for(int i = 0; i < config_->getNumTags(); i++)
                if(config_->getDoTag(i))
                    calculateTags(*next, i);
        out->writeSentence(next);
        delete next;
    }

    delete in;
    delete out;
    if(inStr) delete inStr;
    if(outStr) delete outStr;

    if(config_->getDebug() > 0)    
        cerr << "done!" << endl;

}

void Kinkaku::checkEqual(const Kinkaku & rhs) {
    checkPointerEqual(util_, rhs.util_);
    checkPointerEqual(dict_, rhs.dict_);
    checkPointerEqual(wsModel_, rhs.wsModel_);
    checkPointerEqual(subwordDict_, rhs.subwordDict_);
    checkPointerVecEqual(subwordModels_, rhs.subwordModels_);
    checkPointerVecEqual(globalMods_, rhs.globalMods_);
    checkValueVecEqual(globalTags_, rhs.globalTags_);
    checkValueVecEqual(dictFeats_, rhs.dictFeats_);
}

Kinkaku::~Kinkaku() {
    if(dict_) delete dict_;
    if(subwordDict_) delete subwordDict_;
    if(wsModel_) delete wsModel_;
    if(config_) delete config_;
    if(fio_) delete fio_;
    for(int i = 0; i < (int)subwordModels_.size(); i++) {
        if(subwordModels_[i] != 0) delete subwordModels_[i];
    }
    for(int i = 0; i < (int)globalMods_.size(); i++)
        if(globalMods_[i] != 0) delete globalMods_[i];
    for(Sentences::iterator it = sentences_.begin(); it != sentences_.end(); it++)
        delete *it;
    
}
void Kinkaku::init() { 
    util_ = config_->getStringUtil();
    dict_ = NULL;
    wsModel_ = NULL;
    subwordDict_ = NULL;
    fio_ = new FeatureIO;
}

template <class Entry>
void Kinkaku::setDictionary(Dictionary<Entry> * dict) {
    if(dict_ != 0) delete dict_;
    dict_ = dict;
}
template void Kinkaku::setDictionary(Dictionary<ModelTagEntry> * dict);