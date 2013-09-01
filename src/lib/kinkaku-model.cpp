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
#include <kinkaku/kinkaku-model.h>
#include <kinkaku/feature-lookup.h>
#include <kinkaku/string-util.h>
#include <kinkaku/dictionary.h>
#include "liblinear/linear.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace kinkaku;
using namespace std;

#define SIG_CUTOFF 1E-6
#define SHORT_MAX 32767

int KinkakuModel::featuresAdded_ = 0;

template <class A, class B>
class secondmore {
public:
    bool operator() (const pair<A,B> & a, const pair<A,B> & b) {
        return b.second < a.second;
    }
};

vector< pair<int,double> > KinkakuModel::runClassifier(const vector<unsigned> & feat) {
    int i, j, featSize = feat.size();
    FeatSum dec;
    vector< pair<int,double> > ret(labels_.size());
    if(numW_ == 1) {
        dec = (bias_>=0?getWeight(getBiasId()-1,0):0);
        for(i = 0; i < featSize; i++)
            dec += getWeight(feat[i]-1,0);
        double big = abs(dec)*multiplier_, small = 0;
        if(isProbabilistic(solver_)) {
            big = 1/(1+exp(-1*big));
            small = 1-big;
        }
        if(dec > 0) {
            ret[0] = pair<int,double>(labels_[0],big); 
            ret[1] = pair<int,double>(labels_[1],small); 
        } else {
            ret[0] = pair<int,double>(labels_[1],big); 
            ret[1] = pair<int,double>(labels_[0],small); 
        }
    }
    else {
        double sum = 0, max1 = -100000, max2 = -100000, weight;
        for(j = 0; j < numW_; j++) {
            dec = (bias_>=0?getWeight(getBiasId()-1,j):0);
            for(i = 0; i < featSize; i++) {
                dec += getWeight(feat[i]-1,j);
            }
            weight = dec*multiplier_;
            if(isProbabilistic(solver_)) {
                weight = 1/(1+exp(-1*weight));
                sum += weight;
            }
            else if(weight > max1) {
                max2 = max1; max1 = weight;
            }
            else if(weight > max2)
                max2 = weight;
            ret[j] = pair<int,double>(labels_[j], weight);
        }
        if(isProbabilistic(solver_))
            for(j = 0; j < numW_; j++)
                ret[j].second /= sum;
        else 
            for(j = 0; j < numW_; j++)
                ret[j].second -= max2;
        sort(ret.begin(),ret.end(),secondmore<int,double>());
    }
    return ret;
}


void KinkakuModel::printClassifier(const vector<unsigned> & feat, StringUtil * util, ostream & out) {
    int i, j, featSize = feat.size();
    FeatSum weight, tot;
    vector< pair<string,double> > idxs;
    vector<double> sums(numW_,0);
    if(numW_ == 1) {
        if(bias_>=0) {
            sums[0] = getWeight(getBiasId()-1,0);
            ostringstream buff; buff << "BIAS=" << sums[0];
            idxs.push_back(pair<string,double>(buff.str(),abs(sums[0])));
        }
        for(i = 0; i < featSize; i++) {
            weight = getWeight(feat[i]-1,0);
            ostringstream buff; buff << util->showString(showFeat(feat[i])) << "=" << weight;
            idxs.push_back(pair<string,double>(buff.str(),abs(weight)));
            sums[0] += weight;
        }
    }
    else {
        if(bias_>=0) {
            tot = 0;
            ostringstream buff; buff << "BIAS=";
            for(j = 0; j < numW_; j++) {
                weight = getWeight(getBiasId()-1,j);
                sums[j] += weight;
                tot += abs(weight);
                if(j != 0) buff << "/";
                buff << weight;
            }
            idxs.push_back(pair<string,double>(buff.str(),tot));
        }
        for(i = 0; i < featSize; i++) {
            tot = 0;
            ostringstream buff; buff << util->showString(showFeat(feat[i])) << "=";
            for(j = 0; j < numW_; j++) {
                weight = getWeight(feat[i]-1,j);
                sums[j] += weight;
                tot += abs(weight);
                if(j != 0) buff << "/";
                buff << weight;
            }
            idxs.push_back(pair<string,double>(buff.str(),tot));
        }
    }
    sort(idxs.begin(),idxs.end(),secondmore<string,double>());
    for(i = 0; i < (int)idxs.size(); i++) {
        if(i != 0) out << " ";
        out << idxs[i].first;
    }
    out << " --- TOTAL=";
    for(i = 0; i < numW_; i++) {
        if(i != 0) out << "/";
        out << sums[i];
    }
   
}

feature_node * allocateFeatures(const vector<unsigned> & feats, int biasId, double biasVal) {
    feature_node * nodes = (feature_node*)malloc((feats.size()+(biasVal>=0?2:1))*sizeof(feature_node));
    unsigned i;
    for(i = 0; i < feats.size(); i++) {
        nodes[i].index=feats[i];
        nodes[i].value = 1;
    }
    if(biasVal >= 0) {
        nodes[i].index = biasId;
        nodes[i++].value = biasVal;
    }
    nodes[i].index = -1;
    return nodes;
}
void KinkakuModel::trainModel(const vector< vector<unsigned> > & xs, vector<int> & ys, double bias, int solver, double epsilon, double cost) {
    if(xs.size() == 0) return;
    solver_ = solver;
    if(weights_.size()>0)
        weights_.clear();
    setBias(bias);
    struct problem   prob;
    struct parameter param;
    prob.l = xs.size();
    prob.y = &ys.front();

    feature_node** myXs = (feature_node**)malloc(sizeof(feature_node*)*xs.size());
    int biasId = getBiasId();
    for(int i = 0; i < prob.l; i++)
        myXs[i] = allocateFeatures(xs[i], biasId, bias);
    prob.x = myXs;

    prob.bias = bias;
    prob.n = names_.size()+(bias>=0?1:0);

    param.solver_type = solver;
    param.C = cost;
    param.eps = epsilon;
    param.nr_weight = 0;
    param.weight_label = NULL;
    param.weight = NULL;
    if(param.eps == HUGE_VAL) {
    	if(param.solver_type == L2R_LR || param.solver_type == L2R_L2LOSS_SVC)
    		param.eps = 0.01;
    	else if(param.solver_type == L2R_L2LOSS_SVC_DUAL || param.solver_type == L2R_L1LOSS_SVC_DUAL || param.solver_type == MCSVM_CS || param.solver_type == L2R_LR_DUAL)
    		param.eps = 0.1;
    	else if(param.solver_type == L1R_L2LOSS_SVC || param.solver_type == L1R_LR)
    		param.eps = 0.01;
    }
    model* mod_ = train(&prob, &param);

    for(int i = 0; i < prob.l; i++)
        free(myXs[i]);
    free(myXs);

    int i, j;

    labels_.resize(mod_->nr_class);
    for(int i = 0; i < mod_->nr_class; i++)
        labels_[i] = mod_->label[i];

    numW_ = (labels_.size()==2 && solver_ != MCSVM_CS?1:labels_.size());
    
#if DISABLE_QUANTIZE
    multiplier_ = 1;
#else
    const unsigned wSize = numW_*names_.size();
    multiplier_ = 0;
    double val;
    for(unsigned i = 0; i < wSize; i++) {
        val = abs(mod_->w[i]);
        if(val > multiplier_)
            multiplier_ = val;
    }
    multiplier_ /= SHORT_MAX;
#endif

    oldNames_ = names_;
    names_.clear();
    ids_.clear();
    KinkakuString empty;
    mapFeat(empty);
    weights_.clear();
    for(i=0; i<(int)oldNames_.size()-1; i++) {
        double myMax = 0.0;
    	for(j=0; j<numW_; j++) 
            myMax = max(abs(mod_->w[i*numW_+j]),myMax);
        if(myMax>SIG_CUTOFF) {
            mapFeat(oldNames_[i+1]);
            if(numW_ == 2) {
                weights_.push_back((FeatVal)
                        ((mod_->w[i*numW_]-mod_->w[i*numW_+1])/multiplier_));
            } else {
                for(j = 0; j < numW_; j++)
                    weights_.push_back((FeatVal)(mod_->w[i*numW_+j]/multiplier_));
            }
        }
    }
    if(bias_>=0) {
        if(numW_ == 2) {
            weights_.push_back((FeatVal)
                    ((mod_->w[i*numW_]-mod_->w[i*numW_+1])/multiplier_));
        } else {
            for(j = 0; j < numW_; j++)
                weights_.push_back((FeatVal)(mod_->w[i*numW_+j]/multiplier_));
        }
    }

    if(numW_ == 2) numW_ = 1;

    free_and_destroy_model(&mod_);
    addFeat_ = false;

}

void KinkakuModel::setNumClasses(unsigned v) {
    if(v == 1) 
        THROW_ERROR("Trying to set the number of classes to 1");
    labels_.resize(v);
    numW_ = (v==2 && solver_ != MCSVM_CS?1:v);
}

Dictionary<vector<FeatVal> > * KinkakuModel::makeDictionaryFromPrefixes(const vector<KinkakuString> & prefs, StringUtil* util, bool adjustPos) {
    typedef Dictionary<vector<FeatVal> >::WordMap WordMap;
    WordMap wm;
    int pos;
    for(int i = 0; i < (int)names_.size(); i++) {
        const KinkakuString & str = names_[i];
        for(pos = 0; pos < (int)prefs.size() && !str.beginsWith(prefs[pos]); pos++);
        if(pos != (int)prefs.size()) {
            featuresAdded_++;
            KinkakuString name = str.substr(prefs[pos].length());
            WordMap::iterator it = wm.find(name);
            if(it == wm.end()) {
                pair<WordMap::iterator, bool> p = wm.insert(WordMap::value_type(name,new vector<FeatVal>(prefs.size()*numW_)));
                it = p.first;
            }
            int id = (adjustPos ?
                (prefs.size()-pos-name.length())*numW_ :
                pos*numW_
            );
            for(int j = 0; j < numW_; j++) {
                (*it->second)[id+j] = getWeight(i-1,j) * labels_[0];
            }
        }
    }
    if(wm.size() > 0) {
        Dictionary<vector<FeatVal> > * ret = new Dictionary<vector<FeatVal> >(util);
        ret->buildIndex(wm);
        return ret;
    }
    return NULL;
}

void KinkakuModel::buildFeatureLookup(StringUtil * util, int charw, int typew, int numDicts, int maxLen) {
    if(featLookup_) {
        delete featLookup_;
        featLookup_ = 0;
    }
    if(names_.size() == 0 || getNumClasses() < 2)
        return;
    featLookup_ = new FeatureLookup;
    featuresAdded_ = 0;
    vector<KinkakuString> charPref, typePref, selfPref, dictPref;
    for(int i = 1-charw; i <= charw; i++) {
        ostringstream oss; oss << "X" << i;
        charPref.push_back(util->mapString(oss.str()));
    }
    featLookup_->setCharDict(makeDictionaryFromPrefixes(charPref, util, true));
    for(int i = 1-typew; i <= typew; i++) {
        ostringstream oss; oss << "T" << i;
        typePref.push_back(util->mapString(oss.str()));
    }
    featLookup_->setTypeDict(makeDictionaryFromPrefixes(typePref, util, true));
    selfPref.push_back(util->mapString("SX"));
    selfPref.push_back(util->mapString("ST"));
    featLookup_->setSelfDict(makeDictionaryFromPrefixes(selfPref, util, false));
    int bias = getBiasId();
    if(bias != -1) {
        featuresAdded_++;
        for(int j = 0; j < numW_; j++)
            featLookup_->setBias(getWeight(bias-1, j) * labels_[0], j);
    }    
    bool prevAddFeat = addFeat_;
    addFeat_ = false;
    if(numDicts*maxLen > 0) {
        vector<FeatVal> * dictFeats = new vector<FeatVal>(numDicts*maxLen*3,0);
        int id = 0;
        for(int i = 0; i < numDicts; i++) {
            for(int j = 1; j <= maxLen; j++) {
                ostringstream oss1; oss1 << "D" << i << "R" << j;
                unsigned id1 = mapFeat(util->mapString(oss1.str()));
                if(id1 != 0) {
                    (*dictFeats)[id] = getWeight(id1-1, 0) * labels_[0];
                    featuresAdded_++;
                }
                id++;
                ostringstream oss2; oss2 << "D" << i << "I" << j;
                unsigned id2 = mapFeat(util->mapString(oss2.str()));
                if(id2 != 0) {
                    featuresAdded_++;
                    (*dictFeats)[id] = getWeight(id2-1, 0) * labels_[0];
                }
                id++;
                ostringstream oss3; oss3 << "D" << i << "L" << j;
                unsigned id3 = mapFeat(util->mapString(oss3.str()));
                if(id3 != 0) {
                    featuresAdded_++;
                    (*dictFeats)[id] = getWeight(id3-1, 0) * labels_[0];
                }
                id++;
            }
        }
        featLookup_->setDictVector(dictFeats);
    }
    if(numDicts > 0) {
        vector<FeatVal> * tagDictFeats = new vector<FeatVal>(numDicts*labels_.size()*labels_.size(),0);
        int id = 0;
        for(int i = 0; i <= numDicts; i++) {
            for(int j = 0; j < (int)labels_.size(); j++) {
                ostringstream oss1; oss1 << "D" << i << "T" << j;
                unsigned id1 = mapFeat(util->mapString(oss1.str()));
                if(id1 != 0) {
                    for(int k = 0; k < (int)labels_.size(); k++)
                        (*tagDictFeats)[id+k] = getWeight(id1-1, k) * labels_[0];
                    featuresAdded_++;
                }
                id += labels_.size();
            }
        }
        featLookup_->setTagDictVector(tagDictFeats);
    }
    unsigned id1 = mapFeat(util->mapString("UNK"));
    if(id1 != 0) {
        vector<FeatVal> * tagUnkFeats = new vector<FeatVal>(labels_.size(),0);
        featuresAdded_++;
        for(int k = 0; k < (int)labels_.size(); k++)
            (*tagUnkFeats)[k] = getWeight(id1-1, k) * labels_[0];
        featLookup_->setTagUnkVector(tagUnkFeats);
    }
    addFeat_ = prevAddFeat;
    if(featuresAdded_ != (int)names_.size())
        THROW_ERROR("Did not add all the features to the feature lookup ("<<featuresAdded_<<" != "<<names_.size()<<")");
}


void KinkakuModel::checkEqual(const KinkakuModel & rhs) const {
    if(featLookup_ == NULL) {
        checkMapEqual(ids_, rhs.ids_);
        checkValueVecEqual(names_, rhs.names_);
        checkValueVecEqual(weights_, rhs.weights_);
    }
    checkValueVecEqual(labels_, rhs.labels_);
    if(abs((double)(multiplier_ - rhs.multiplier_)/multiplier_) > 0.01) THROW_ERROR("multipliers don't match: "<<multiplier_ << " != " << rhs.multiplier_);
    if(bias_ != rhs.bias_) THROW_ERROR("biases don't match: "<<bias_ << " != " << rhs.bias_);
    if(solver_ != rhs.solver_) THROW_ERROR("solvers don't match: "<<solver_ << " != " << rhs.solver_);
    if(numW_ != rhs.numW_) THROW_ERROR("numWs don't match: "<<numW_ << " != " << rhs.numW_);
    if(addFeat_ != rhs.addFeat_) THROW_ERROR("addFeats don't match: "<<addFeat_ << " != " << rhs.addFeat_);
    checkPointerEqual(featLookup_, rhs.featLookup_);
}


KinkakuModel::~KinkakuModel() {
    if(featLookup_) delete featLookup_;
}


void KinkakuModel::setNumFeatures(unsigned i) {
    if(i != getNumFeatures()) 
        THROW_ERROR("setting the number of features to a different value is not allowed ("<<i<<" != "<<getNumFeatures()<<")");
}