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
#ifndef KINKAKU_MODEL_H__
#define KINKAKU_MODEL_H__

#include <iostream>
#include <kinkaku/feature-vector.h>
#include <kinkaku/kinkaku-string.h>
#include <kinkaku/kinkaku-struct.h>

#define SIG_CUTOFF 1E-6

namespace kinkaku {

typedef std::vector<KinkakuString> FeatNameVec;

class FeatureLookup;
template <class Entry>
class Dictionary;

class KinkakuModel {
public:

    static inline bool isProbabilistic(int solver) {
        return solver == 0 || solver == 6 || solver == 7;
    }

    static int featuresAdded_;

protected:

    KinkakuUnsignedMap ids_;
    FeatNameVec names_;
    FeatNameVec oldNames_;
    std::vector<int> labels_;
    std::vector<FeatVal> weights_;
    double multiplier_;
    double bias_;
    int solver_, numW_;
    bool addFeat_;
    FeatureLookup * featLookup_;

public:
    KinkakuModel() : multiplier_(1.0f), bias_(1.0f), solver_(1), addFeat_(true), featLookup_(NULL) {
        KinkakuString str;
        mapFeat(str);
    }
    ~KinkakuModel();

    void checkEqual(const KinkakuModel & rhs) const;

    inline unsigned mapFeat(const KinkakuString & str) {
        KinkakuUnsignedMap::const_iterator it = ids_.find(str);
        unsigned ret = 0;
        if(it != ids_.end())
            ret = it->second;
        else if(addFeat_) {
            ret = names_.size();
            ids_[str] = ret;
            names_.push_back(str);
        }
        return ret;
    }
    inline KinkakuString showFeat(unsigned val) {
#ifdef KINKAKU_SAFE
        if(val >= names_.size())
            THROW_ERROR("FATAL: Array index out of bounds in showFeat ("<<val<<" >= "<<names_.size()<<")");
#endif
        return names_[val];
    }

    int getBiasId() { return (bias_?(int)names_.size():-1); }

    void setAddFeatures(bool addFeat) { addFeat_ = addFeat; }
    bool getAddFeatures() { return addFeat_; }

    const FeatNameVec & getNames() const { return names_; }
    const FeatNameVec & getOldNames() const { return oldNames_; }

    std::vector< std::pair<int,double> > runClassifier(const std::vector<unsigned> & feat);
    void printClassifier(const std::vector<unsigned> & feat, StringUtil * util, std::ostream & out = std::cerr);

    void trainModel(const std::vector< std::vector<unsigned> > & xs, std::vector<int> & ys, double bias, int solver, double epsilon, double cost);
    void trimModel();

    inline const KinkakuUnsignedMap & getIds() const { return ids_; }
    inline const unsigned getNumFeatures() const { return names_.size()-1; }
    inline const double getBias() const { return bias_; }
    inline const unsigned getNumWeights() const { return numW_; }
    inline const int getSolver() const { return solver_; }
    inline const unsigned getNumClasses() const { return labels_.size(); }
    inline const int getLabel(unsigned idx) const { return labels_[idx]; }
    inline FeatureLookup * getFeatureLookup() const { return featLookup_; }
    inline const FeatVal getWeight(unsigned i, unsigned j) const {
        int id = i*numW_+j;
#ifdef KINKAKU_SAFE
        if(id >= (int)weights_.size())
            THROW_ERROR("weight out of bounds: size="<<weights_.size()<<" id="<<id);
#endif
        return weights_[id];
    }
    const double getMultiplier() const { return multiplier_; }
    void setBias(double bias) { bias_ = bias; }
    void setLabel(unsigned i, int lab) { labels_[i] = lab; }
    void setSolver(int i) { solver_ = i; }
    void setNumWeights(int i) { numW_ = i; }
    void setFeatureLookup(FeatureLookup * featLookup) { featLookup_ = featLookup; }
    void setNumFeatures(unsigned i);
    void setNumClasses(unsigned i);

    void initializeWeights(unsigned i, unsigned j) { weights_.resize(i*j,0); }
    void setWeight(unsigned i, unsigned j, FeatVal w) { 
        int id = i*numW_+j;
#ifdef KINKAKU_SAFE
        if(id >= (int)weights_.size())
            THROW_ERROR("weight out of bounds: size="<<weights_.size()<<" id="<<id);
#endif
        weights_[id] = w;
    }
    void setMultiplier(double m) { multiplier_ = m; }

    void buildFeatureLookup(StringUtil * util, int charw, int typew, int numDicts, int maxLen);
    Dictionary<std::vector<FeatVal> > * 
        makeDictionaryFromPrefixes(const std::vector<KinkakuString> & prefs, StringUtil* util, bool adjustPos);
    

};

class TagTriplet {
public:
    std::vector< std::vector<unsigned> > first;
    std::vector<int> second;
    KinkakuModel * third;
    std::vector<KinkakuString> fourth;

    TagTriplet() : first(), second(), third(0), fourth() { }
};

typedef KinkakuStringMap<TagTriplet*> TagHash;

}

#endif