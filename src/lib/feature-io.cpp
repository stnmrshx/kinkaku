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
#include <kinkaku/string-util.h>
#include <kinkaku/kinkaku-util.h>
#include <kinkaku/feature-io.h>
#include <kinkaku/dictionary.h>
#include <fstream>

using namespace kinkaku;
using namespace std;

FeatureIO::~FeatureIO() {
    if(out_) delete out_;
}

void FeatureIO::load(const string& fileName,StringUtil* util) {
    ifstream in(fileName.c_str());
    if(in.fail())
        THROW_ERROR("Failed to open feature file "<<fileName);
    string line, str, str2;
    unsigned char maxDict = 0;
    getline(in,line); 
    numTags_ = util->parseInt(line.c_str());
    getline(in,line); 
    int numWords = util->parseInt(line.c_str());
    for(int i = 0; i < numWords; i++) {
        getline(in,line); istringstream iss(line);
        iss >> str >> str2;
        int inDict = util->parseInt(str2.c_str());
        ModelTagEntry* ent = new ModelTagEntry(util->mapString(str));
        ent->inDict = (char)inDict;
        maxDict = max(maxDict,(unsigned char)inDict);
        ent->setNumTags(numTags_);
        for(int j = 0; j < numTags_; j++) {
            getline(in,line); istringstream tiss(line);
            tiss >> str; int numTypes = util->parseInt(str.c_str());
            for(int k = 0; k < numTypes; k++) {
                tiss >> str >> str2;
                inDict = util->parseInt(str2.c_str());
                ent->tags[j].push_back(util->mapString(str.c_str()));
                ent->tagInDicts[j].push_back((char)inDict);
            }
        }
        wm_.insert(pair<KinkakuString,ModelTagEntry*>(ent->word,ent));
    }
    getline(in,line); 
    if(line.length()) {
        THROW_ERROR("Expected empty line in feature file, but instead got '"<<line<<"'");
    }

    for(unsigned i = 0; i < 8; i++)
        if((1 << i) & maxDict)
            numDicts_ = i+1;

    while(getline(in,line)) {
        istringstream titless(line);
        titless >> str;
        if(str == "T") {
            titless >> str;
            numTags_ = max(numTags_,util->parseInt(str.c_str())+1);
        }
        TagTriplet * trip = new TagTriplet();
        trip->first = vector< vector<unsigned> >();
        trip->second = vector<int>();
        trip->third = new KinkakuModel();
        feats_.insert(pair<KinkakuString,TagTriplet*>(util->mapString(line),trip));
        getline(in,line); istringstream iss(line);
        while(iss >> str) 
            trip->fourth.push_back(util->mapString(str));
        getline(in,line); int numFeats = util->parseInt(line.c_str());
        vector<KinkakuString> featNames(numFeats);
        for(int i = 0; i < numFeats; i++) {
            getline(in,line);
            trip->third->mapFeat(util->mapString(line));
        }
        while(getline(in,line) && line.length() > 0) {
            istringstream iss(line);
            iss >> str; trip->second.push_back(util->parseInt(str.c_str()));
            vector<unsigned> x;
            while(iss >> str) {
                x.push_back(util->parseInt(str.c_str()));
            }
            trip->first.push_back(x);
        }
    }
}

void FeatureIO::openOut(const string& fileName) {
    if(out_) delete out_;
    out_ = new ofstream(fileName.c_str());
}
void FeatureIO::closeOut() {
    delete out_; out_ = 0;
}

TagTriplet * FeatureIO::getFeatures(const KinkakuString & str, bool add) {
    TagHash::iterator it = feats_.find(str);
    TagTriplet * ret = 0;
    if(it != feats_.end())
        ret = it->second;
    else if(add) {
        ret = new TagTriplet();
        feats_.insert(pair<KinkakuString,TagTriplet*>(str,ret));
    }
    return ret;
}

void FeatureIO::printFeatures(const KinkakuString & featId, TagTriplet * trip, StringUtil * util) {    
    if(!out_ || trip->first.size() == 0) return;
    *out_ << util->showString(featId) << endl;
    for(unsigned i = 0; i < trip->fourth.size(); i++) {
        if(i != 0) *out_ << " ";
        *out_ << util->showString(trip->fourth[i]);
    }
    const FeatNameVec & names = trip->third->getOldNames();
    *out_ << endl << names.size() << endl;
    for(int i = 0; i < (int)names.size(); i++) {
        *out_ << util->showString(names[i]) << endl;
    }
    for(int i = 0; i < (int)trip->first.size(); i++) {
        *out_ << trip->second[i];
        for(int j = 0; j < (int)trip->first[i].size(); j++) {
            *out_ << " " << trip->first[i][j];
        }
        *out_ << endl;
    }
    *out_ << endl;
}

void FeatureIO::printFeatures(const KinkakuString & featId, StringUtil * util) { 
    vector<KinkakuString> gc;
    for(TagHash::iterator it = feats_.begin(); it != feats_.end(); it++) {
        if(it->first.beginsWith(featId)) {
            gc.push_back(it->first);
            printFeatures(it->first,it->second,util);
            delete it->second;
        }
    }
    for(unsigned i = 0; i < gc.size(); i++) {
        feats_.erase(gc[i]);
    }
}

void FeatureIO::printWordMap(StringUtil * util) {
    if(!out_) return;
    *out_ << numTags_ << endl;
    *out_ << wm_.size() << endl;
    for(Dictionary<ModelTagEntry>::WordMap::const_iterator it = wm_.begin(); it != wm_.end(); it++) {
        const TagEntry * te = it->second;
        *out_ << util->showString(te->word) << " " << (int)te->inDict << endl;
        for(int i = 0; i < numTags_; i++) {
            if(i >= (int)te->tags.size()) {
                *out_ << "0" << endl;
            } else {
                *out_ << te->tags[i].size();
                for(unsigned j = 0; j < te->tags[i].size(); j++)
                    *out_ << " " << util->showString(te->tags[i][j]) << " " << (int)te->tagInDicts[i][j];
                *out_ << endl;
            }
        }
    }
    *out_ << endl;
}