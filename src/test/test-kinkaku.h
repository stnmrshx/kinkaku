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
#ifndef TEST_KYTEA__
#define TEST_KYTEA__

#include <algorithm>

using namespace std;

namespace kinkaku {

class KinkakuTest {

public:

    int testGetTypeString() {
        StringUtilUtf8 util;
        KinkakuString str = util.mapString("漢カひ。１A");
        string act = util.getTypeString(str);
        string exp = "KTHODR";
        if(act != exp) {
            cout << "testGetTypeString::Expected "<<exp << " but got "<<act <<endl;
            return 0;
        }
        return 1;
    }
    
    int compareFeatures(vector<KinkakuString> & exp, vector<KinkakuString> & act, StringUtilUtf8 & util) {
        sort(exp.begin(), exp.end());
        sort(act.begin(), act.end());
        bool bad = false;
        if(exp.size() != act.size()) {
            bad = true;
            cout << "Sizes exp.size()=="<<exp.size()<<", act.size()=="<<act.size()<<endl;
        }
        for(int i = 0; !bad && i < (int)exp.size(); i++) 
            if(exp[i] != act[i])
                bad = true;
        if(bad) {
            cout << "EXP:";
            for(int i = 0; i < (int)exp.size(); i++)
                cout << " " << util.showString(exp[i]);
            cout << endl << "ACT:";
            for(int i = 0; i < (int)act.size(); i++)
                cout << " " << util.showString(act[i]);
            cout << endl; 
        }
        return bad ? 0 : 1;
    }

    int testWSNgramFeatures() {
        StringUtilUtf8 util;
        Kinkaku kinkaku;
        kinkaku.setWSModel(new KinkakuModel());
        KinkakuString str = util.mapString("漢カひ。１A");
        vector<KinkakuString> exp, act;
        exp.push_back(util.mapString("X-2漢"));
        exp.push_back(util.mapString("X-1カ"));
        exp.push_back(util.mapString("X0ひ"));
        exp.push_back(util.mapString("X1。"));
        exp.push_back(util.mapString("X2１"));
        exp.push_back(util.mapString("X3A"));
        exp.push_back(util.mapString("X-2漢カ"));
        exp.push_back(util.mapString("X-1カひ"));
        exp.push_back(util.mapString("X0ひ。"));
        exp.push_back(util.mapString("X1。１"));
        exp.push_back(util.mapString("X2１A"));
        Kinkaku::SentenceFeatures sentFeats(5);
        vector<KinkakuString> charPrefixes_;
        for(int i = -2; i <= 3; i++) {
            ostringstream oss; oss << "X" << i;
            charPrefixes_.push_back(util.mapString(oss.str()));
        }
        kinkaku.wsNgramFeatures(str, sentFeats, charPrefixes_, 2);
        if((int)sentFeats.size() != 5)
            THROW_ERROR("sentFeats.size() == " << sentFeats.size());
        for(int i = 0; i < (int)sentFeats[2].size(); i++)
            act.push_back(kinkaku.getWSModel()->showFeat(sentFeats[2][i]));
        return compareFeatures(exp, act, util);
    }

    int testTagNgramFeatures() {
        StringUtilUtf8 util;
        Kinkaku kinkaku;
        kinkaku.setWSModel(new KinkakuModel());
        KinkakuString str = util.mapString("漢カひ単語。１A");
        vector<KinkakuString> exp, act;
        exp.push_back(util.mapString("X-2漢"));
        exp.push_back(util.mapString("X-1カ"));
        exp.push_back(util.mapString("X0ひ"));
        exp.push_back(util.mapString("X1。"));
        exp.push_back(util.mapString("X2１"));
        exp.push_back(util.mapString("X3A"));
        exp.push_back(util.mapString("X-2漢カ"));
        exp.push_back(util.mapString("X-1カひ"));
        exp.push_back(util.mapString("X0ひ。"));
        exp.push_back(util.mapString("X1。１"));
        exp.push_back(util.mapString("X2１A"));
        vector<KinkakuString> charPrefixes_;
        for(int i = -2; i <= 3; i++) {
            ostringstream oss; oss << "X" << i;
            charPrefixes_.push_back(util.mapString(oss.str()));
        }
        vector<unsigned> act_feats;
        kinkaku.tagNgramFeatures(str, act_feats, charPrefixes_, kinkaku.getWSModel(), 2, 2, 5);
        for(int i = 0; i < (int)act_feats.size(); i++)
            act.push_back(kinkaku.getWSModel()->showFeat(act_feats[i]));
        return compareFeatures(exp, act, util);
    }

    int testTagSelfFeatures() {
        StringUtilUtf8 util;
        vector<KinkakuString> exp, act;
        Kinkaku kinkaku;
        kinkaku.setWSModel(new KinkakuModel());
        exp.push_back(util.mapString("SX単語"));
        exp.push_back(util.mapString("STKK"));
        vector<unsigned> feats;
        kinkaku.tagSelfFeatures(util.mapString("単語"), feats, util.mapString("SX"), kinkaku.getWSModel());
        kinkaku.tagSelfFeatures(util.mapString("KK"), feats, util.mapString("ST"), kinkaku.getWSModel());
        for(int i = 0; i < (int)feats.size(); i++)
            act.push_back(kinkaku.getWSModel()->showFeat(feats[i]));
        return compareFeatures(exp, act, util);
    }

    void makeDictionary(Kinkaku & kinkaku, StringUtil & util) {
        // Make the dictionary entry
        Dictionary<ModelTagEntry>::WordMap allWords;
        KinkakuString noun = util.mapString("名詞");
        KinkakuString verb = util.mapString("動詞");
        kinkaku.addTag<ModelTagEntry>(allWords, util.mapString("単語"), 0, &noun, 0);
        kinkaku.addTag<ModelTagEntry>(allWords, util.mapString("単語"), 0, &verb, 1);
        Dictionary<ModelTagEntry> * dict = new Dictionary<ModelTagEntry>(&util);
        dict->setNumDicts(2);
        dict->buildIndex(allWords);
        kinkaku.setDictionary(dict);
    }

    int testTagDictFeatures() {
        StringUtilUtf8 util;
        vector<KinkakuString> exp, act;
        Kinkaku kinkaku;
        kinkaku.setWSModel(new KinkakuModel());
        exp.push_back(util.mapString("D0T0"));
        exp.push_back(util.mapString("D1T1"));
        makeDictionary(kinkaku, util);
        // Test
        vector<unsigned> feats;
        kinkaku.tagDictFeatures(util.mapString("単語"), 0, feats, kinkaku.getWSModel());
        for(int i = 0; i < (int)feats.size(); i++)
            act.push_back(kinkaku.getWSModel()->showFeat(feats[i]));
        return compareFeatures(exp, act, util);
    }

    KinkakuModel * makeFeatureLookup(StringUtil * util, int classes) {
        // Make the feature values
        const int SIZE = 18;
        const char* featStrs[SIZE] = 
            { "X-2漢", "X-1カ", "X0ひ", "X1。", "X2１", "X3A",
              "X-2漢カ", "X-1カひ", "X0ひ。", "X1。１", "X2１A",
              "D0L1", "D0I5", "D1R5",
              "SX単語", "STKK", "D0T0", "D1T1" };
        KinkakuModel * mod = new KinkakuModel;
        mod->setNumClasses(classes);
        mod->setLabel(0, 1);
        mod->setLabel(1, -1);
        int lastFeat = -1;
        for(int i = 0; i < SIZE; i++)
            lastFeat = mod->mapFeat(util->mapString(featStrs[i]));
        int actual_classes = (classes == 2 ? 1 : classes);
        mod->initializeWeights(actual_classes, lastFeat+1);
        for(int i = 0; i < lastFeat; i++)
            for(int j = 0; j < actual_classes; j++)
                mod->setWeight(i, j, (i+1)*(j+1));
        mod->buildFeatureLookup(util, 3, 3, 2, 5);
        return mod;
    }

    int testModelToLookup() {
        // Make the expected dictionary
        StringUtilUtf8 util;
        const int SIZE = 11;
        const char* wordStrs[SIZE] = { "漢", "カ", "ひ", "。", "１", "A",
                                       "漢カ", "カひ", "ひ。", "。１", "１A"};
        const int wordPoss[SIZE] = { 5, 4, 3, 2, 1, 0, 4, 3, 2, 1, 0 };
        typedef Dictionary<vector<FeatVal> >::WordMap WordMap;
        WordMap wm;
        for(int i = 0; i < SIZE; i++) {
            pair<WordMap::iterator, bool> it = wm.insert(WordMap::value_type(util.mapString(wordStrs[i]),new vector<FeatVal>(6,0)));
            (*it.first->second)[wordPoss[i]] = i+1;
        }
        Dictionary<vector<FeatVal> > exp(&util);
        exp.buildIndex(wm);
        KinkakuModel * mod = makeFeatureLookup(&util, 2);
        FeatureLookup * look = mod->getFeatureLookup();
        const Dictionary<vector<FeatVal> > * act = look->getCharDict();
        int ret = 1;
        if((int)act->getEntries().size() != SIZE) {
            cerr << "act->getEntries().size() == "<<act->getEntries().size()<<endl;
            ret = 0;
        } else {
            for(int i = 0; i < SIZE; i++) {
                const vector<FeatVal> * actVec = act->findEntry(util.mapString(wordStrs[i]));
                vector<FeatVal> * expVec = exp.findEntry(util.mapString(wordStrs[i]));
                if(actVec == NULL) {
                    cerr << "actVec["<<i<<"] == NULL"<<endl;
                    ret = 0;
                }
                else if(expVec == NULL) {
                    cerr << "expVec["<<i<<"] == NULL"<<endl;
                    ret = 0;
                }
                else if(*actVec != *expVec) {
                    cerr << "expVec["<<i<<"] != actVec["<<i<<"]"<<endl;
                    for(int j = 0; j < (int)expVec->size(); j++) cerr << (*expVec)[j] << " "; cerr << endl;
                    for(int j = 0; j < (int)actVec->size(); j++) cerr << (*actVec)[j] << " "; cerr << endl;
                    ret = 0;
                }
            }
        }
        vector<FeatVal> dictExp(2*5*3, 0);
        dictExp[0*15+0*3+2] = SIZE+1;
        dictExp[0*15+4*3+1] = SIZE+2;
        dictExp[1*15+4*3+0] = SIZE+3;
        const vector<FeatVal> & dictAct = *look->getDictVector();
        if(dictExp.size() != dictAct.size()) {
            cerr << "dictExp.size() == "<<dictExp.size()
                 << " dictAct.size() == "<<dictAct.size() <<endl;
            ret = 0;
        } else {
            for(int i = 0; i < (int)dictAct.size(); i++) {
                if(dictExp[i] != dictAct[i]) {
                    cerr 
                        << "dictExp["<<i<<"]="<<dictExp[i]
                        << " dictAct["<<i<<"]="<<dictAct[i] << endl;
                    ret = 0;
                }
            }
        }
        
        delete mod;
        return ret;
    }

    int testFeatureLookup() {
        StringUtilUtf8 util;
        KinkakuModel * mod = makeFeatureLookup(&util, 2);
        FeatureLookup * feat = mod->getFeatureLookup();
        KinkakuString str = util.mapString("漢カひ。１A");
        vector<FeatSum> act(5,0);
        feat->addNgramScores(feat->getCharDict(), str, 3, act);
        vector<FeatSum> exp(5,0);
        exp[2] = 11*(11+1)/2;
        int ret = 1;
        for(int i = 0; i < 5; i++) {
            if(act[i] != exp[i]) {
                cerr 
                    << "act["<<i<<"]="<<act[i]
                    << " exp["<<i<<"]="<<exp[i] <<endl;
                ret = 0;
            }
        }
        delete mod;
        return ret;
    }

    void makePrefixes(vector<KinkakuString> & charPrefixes, vector<KinkakuString> & typePrefixes, StringUtil * util) {
        charPrefixes.resize(0);
        for(int i = 1; i <= 2*3; i++) {
            ostringstream buff;
            buff << "X" << i-3;
            charPrefixes.push_back(util->mapString(buff.str()));
        }
        typePrefixes.resize(0);
        for(int i = 1; i <= 2*3; i++) {
            ostringstream buff;
            buff << "T" << i-3;
            typePrefixes.push_back(util->mapString(buff.str()));
        }
    }

    int testWSLookupMatchesModel() {
        StringUtilUtf8 util;
        Kinkaku kinkaku;
        kinkaku.setWSModel(new KinkakuModel());
        KinkakuModel & mod = *kinkaku.getWSModel();
        mod.setNumClasses(2);
        mod.setLabel(0, 1);
        mod.setLabel(1, -1);
        mod.setNumWeights(1);
        int id = 0;
        KinkakuString str = util.mapString("漢カひ。１A");
        KinkakuString typeStr = util.mapString(util.getTypeString(str));
        for(int i = 0; i < (int)str.length(); i++) {
            for(int j = 1; j <= 3; j++) {
                if(i+j > (int)str.length()) break;
                for(int k = -2; k <= 4-j; k++) {
                    ostringstream oss1; oss1 << "X" << k << util.showString(str.substr(i,j));
                    id = mod.mapFeat(util.mapString(oss1.str()));
                    ostringstream oss2; oss2 << "T" << k << util.showString(typeStr.substr(i,j));
                    id = mod.mapFeat(util.mapString(oss2.str()));
                }
            }
        }
        mod.initializeWeights(1, id+1);
        for(int i = 0; i <= id; i++) {
            mod.setWeight(i, 0, i);
        }
        mod.buildFeatureLookup(&util, 3, 3, 2, 5);
        FeatureLookup * feat = mod.getFeatureLookup();
        vector<FeatSum> act(5,0);
        feat->addNgramScores(feat->getCharDict(), str, 3, act);
        feat->addNgramScores(feat->getTypeDict(), typeStr, 3, act);
        for(int i = 0; i < 5; i++)
            act[i] += feat->getBias(0);
        Kinkaku::SentenceFeatures sentFeats(5);
        vector<KinkakuString> charPrefixes, typePrefixes;
        makePrefixes(charPrefixes, typePrefixes, &util);
        kinkaku.wsNgramFeatures(str, sentFeats, charPrefixes, 3);
        kinkaku.wsNgramFeatures(typeStr, sentFeats, typePrefixes, 3);
        int ret = 1;
        for(int i = 0; i < 5; i++) {
            pair<int,double> answer = mod.runClassifier(sentFeats[i])[0];
            if(answer.second != act[i]) {
                cerr << "model["<<i<<"]="<<answer.second<<", act["<<i<<"]="<<act[i]<<endl;
                ret = 0;
            }
        }
        return ret;
    }
    
    int testTagLookupMatchesModel() {
        Kinkaku kinkaku;
        StringUtil * util = kinkaku.getStringUtil();
        kinkaku.setWSModel(new KinkakuModel());
        makeDictionary(kinkaku, *util);
        KinkakuModel & mod = *kinkaku.getWSModel();
        mod.setNumClasses(3);
        mod.setLabel(0, 1);
        mod.setLabel(1, 2);
        mod.setLabel(2, 3);
        KinkakuString kssx = util->mapString("SX"), ksst = util->mapString("ST");
        int id = 0;
        KinkakuString str = util->mapString("漢カひ単語。１A");
        KinkakuString typeStr = util->mapString(util->getTypeString(str));
        for(int i = 0; i < (int)str.length(); i++) {
            for(int j = 1; j <= 3; j++) {
                if(i+j > (int)str.length()) break;
                for(int k = -2; k <= 4-j; k++) {
                    ostringstream oss1; oss1 << "X" << k << util->showString(str.substr(i,j));
                    id = mod.mapFeat(util->mapString(oss1.str()));
                    ostringstream oss2; oss2 << "T" << k << util->showString(typeStr.substr(i,j));
                    id = mod.mapFeat(util->mapString(oss2.str()));
                }
                ostringstream oss3; oss3 << "SX" << util->showString(str.substr(i,j));
                id = mod.mapFeat(util->mapString(oss3.str()));
                ostringstream oss4; oss4 << "ST" << util->showString(typeStr.substr(i,j));
                id = mod.mapFeat(util->mapString(oss4.str()));
            }
        }
        id = mod.mapFeat(util->mapString("D0T0"));
        id = mod.mapFeat(util->mapString("D1T1"));
        id = mod.mapFeat(util->mapString("UNK"));
        mod.initializeWeights(3, id+1);
        for(int i = 0; i <= id; i++)
            for(int j = 0; j < 3; j++)
                mod.setWeight(i, j, i*3+j);
        mod.setAddFeatures(false);
        mod.buildFeatureLookup(util, 3, 3, 2, 5);
        FeatureLookup * feat = mod.getFeatureLookup();
        Kinkaku::SentenceFeatures sentFeats(5);
        vector<KinkakuString> charPrefixes, typePrefixes;
        makePrefixes(charPrefixes, typePrefixes, util);
        int ret = 1;
        for(int i = 0; i < 5; i++) {
            vector<FeatSum> act(3,0);
            feat->addTagNgrams(str, feat->getCharDict(), act, 3, i, i+2);
            feat->addTagNgrams(typeStr, feat->getTypeDict(), act, 3, i, i+2);
            for(int j = 0; j < 3; j++) 
                act[j] += feat->getBias(j);
            feat->addSelfWeights(str.substr(i,2), act, 0);
            feat->addSelfWeights(typeStr.substr(i,2), act, 1);
            feat->addTagDictWeights(kinkaku.getDictionaryMatches(str.substr(i,2), 0), act);
            vector<unsigned> feats;
            kinkaku.tagNgramFeatures(str, feats, charPrefixes, &mod, 3, i-1, i+2);
            kinkaku.tagNgramFeatures(typeStr, feats, typePrefixes, &mod, 3, i-1, i+2);
            kinkaku.tagSelfFeatures(str.substr(i,2), feats, kssx, &mod);
            kinkaku.tagSelfFeatures(typeStr.substr(i,2), feats, ksst, &mod);
            kinkaku.tagDictFeatures(str.substr(i,2), 0, feats, &mod);
            vector< pair<int,double> > answers = mod.runClassifier(feats);
            FeatSum secondBest = act[answers[1].first-1];
            for(int j = 0; j < (int)answers.size(); j++) {
                if(answers[j].first-1 >= (int)act.size())
                    THROW_ERROR("answers[j].first too big "<<answers[j].first-1<<", act.size() == "<<act.size());
                act[answers[j].first-1] -= secondBest;
                if(answers[j].second != act[answers[j].first-1]) {
                    cerr << "model["<<i<<"]["<<answers[j].first-1<<"]="<<answers[j].second<<", act["<<i<<"]["<<answers[j].first-1<<"]="<<act[answers[j].first-1]<<endl;
                    ret = 0;
                }
            }
        }
        return ret;
    }

    int testFeatureLookupDictionary() {
        StringUtilUtf8 util;
        KinkakuModel * mod = makeFeatureLookup(&util, 2);
        FeatureLookup * look = mod->getFeatureLookup();
        KinkakuString str = util.mapString("漢カひ。１A");
        Kinkaku kinkaku;
        Dictionary<ModelTagEntry>::WordMap dictMap;
        kinkaku.addTag<ModelTagEntry>(dictMap, util.mapString("１"), 0, NULL, 0);
        kinkaku.addTag<ModelTagEntry>(dictMap, util.mapString("漢カひ。１A"), 0, NULL, 0);
        kinkaku.addTag<ModelTagEntry>(dictMap, util.mapString("漢カひ。１"), 0, NULL, 0);
        kinkaku.addTag<ModelTagEntry>(dictMap, util.mapString("カひ。１A"), 0, NULL, 1);
        Dictionary<ModelTagEntry> dict(&util);
        dict.buildIndex(dictMap);
        vector<FeatSum> exp(5,13); 
        exp[4] += 12; 
        exp[0] += 14; 
        vector<FeatSum> act(5,0);
        look->addDictionaryScores(dict.match(str), 2, 5, act);
        int ret = 1;
        for(int i = 0; i < 5; i++) {
            if(act[i] != exp[i]) {
                cerr 
                    << "act["<<i<<"]="<<act[i]
                    << " exp["<<i<<"]="<<exp[i] <<endl;
                ret = 0;
            }
        }
        delete mod;
        return ret;
    }

    bool runTest() {
        int done = 0, succeeded = 0;
        done++; cout << "testGetTypeString()" << endl; if(testGetTypeString()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testWSNgramFeatures()" << endl; if(testWSNgramFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testTagNgramFeatures()" << endl; if(testTagNgramFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testTagSelfFeatures()" << endl; if(testTagSelfFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testTagDictFeatures()" << endl; if(testTagDictFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testModelToLookup()" << endl; if(testModelToLookup()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testFeatureLookup()" << endl; if(testFeatureLookup()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testWSLookupMatchesModel()" << endl; if(testWSLookupMatchesModel()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testTagLookupMatchesModel()" << endl; if(testTagLookupMatchesModel()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testFeatureLookupDictionary()" << endl; if(testFeatureLookupDictionary()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestKinkaku Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return (done == succeeded);
    }

};

}

#endif