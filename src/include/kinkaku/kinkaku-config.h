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
#ifndef KINKAKU_CONFIG_H__
#define KINKAKU_CONFIG_H__

namespace kinkaku {
class KinkakuConfig;
}

#include <string>
#include <vector>

namespace kinkaku {

class StringUtil;

class KinkakuConfig {

private:

    typedef char CorpForm;
    bool onTraining_;

    unsigned debug_;

    StringUtil * util_; 

    std::vector<std::string> corpora_;
    std::vector<CorpForm> corpusFormats_; 
    
    std::vector<std::string> dicts_; 

    std::vector<std::string> subwordDicts_; 

    std::string model_; 
    char modelForm_; 

    std::string input_, output_; 
    CorpForm inputForm_, outputForm_; 

    std::string featIn_, featOut_;
    std::ostream* featStr_;

    bool doWS_, doTags_, doUnk_;
    std::vector<bool> doTag_;

    bool addFeat_;
    double confidence_;
    char charW_, charN_, typeW_, typeN_, dictN_;
    char unkN_;
    unsigned unkBeam_;
    std::string defTag_;
    std::string unkTag_;

    double bias_; 
    double eps_; 
    double cost_; 
    int solverType_; 

    std::vector<std::string> args_;

    void setIOFormat(const char* str, CorpForm & cf);
    
    std::string wordBound_, tagBound_, elemBound_, unkBound_, noBound_, hasBound_, skipBound_, escape_;

    std::string wsConstraint_;

    int numTags_;
    std::vector<bool> global_;
    unsigned tagMax_;

    void ch(const char * n, const char* v);

public:

    KinkakuConfig();
    KinkakuConfig(const KinkakuConfig & rhs);
    ~KinkakuConfig();
    void addCorpus(const std::string & corp, CorpForm format);
    void addDictionary(const std::string & corp);
    void addSubwordDict(const std::string & corp);

    void parseTrainCommandLine(int argc, const char ** argv);
    void parseRunCommandLine(int argc, const char ** argv);

    void printUsage();
    void printVersion();

    unsigned parseTrainArg(const char * n, const char * v);

    unsigned parseRunArg(const char * n, const char * v);

    const std::vector<std::string> & getCorpusFiles() const { return corpora_; }
    const std::vector<CorpForm> & getCorpusFormats() const { return corpusFormats_; }
    const std::vector<std::string> & getDictionaryFiles() const { return dicts_; }
    const std::vector<std::string> & getSubwordDictFiles() const { return subwordDicts_; }
    const std::string & getModelFile();
    const char getModelFormat() const { return modelForm_; }
    const unsigned getDebug() const { return debug_; }
    StringUtil * getStringUtil() { return util_; }
    const StringUtil * getStringUtil() const { return util_; }
    const CorpForm getInputFormat() const { return inputForm_; }
    const CorpForm getOutputFormat() const { return outputForm_; }
    const std::string & getFeatureIn() const { return featIn_; }
    const std::string & getFeatureOut() const { return featOut_; }
    const bool getWriteFeatures() const { return featOut_.length() > 0; }
    
    const char getCharN() const { return charN_; }
    const char getCharWindow() const { return charW_; }
    const char getTypeN() const { return typeN_; }
    const char getTypeWindow() const { return typeW_; }
    const char getDictionaryN() const { return dictN_; }
    const char getUnkN() const { return unkN_; }
    const unsigned getTagMax() const { return tagMax_; }
    const unsigned getUnkBeam() const { return unkBeam_; }
    const std::string & getUnkTag() const { return unkTag_; }
    const std::string & getDefaultTag() const { return defTag_; }
    const std::string & getWsConstraint() const { return wsConstraint_; }

    const double getBias() const { return bias_; }
    const double getEpsilon() const { return eps_; }
    const double getCost() const { return cost_; }
    const int getSolverType() const { return solverType_; }
    const bool getDoWS() const { return doWS_; }
    const bool getDoUnk() const { return doUnk_; }
    const bool getDoTags() const { return doTags_; }
    const bool getDoTag(int i) const { return doTags_ && (i >= (int)doTag_.size() || doTag_[i]); }
    const char* getWordBound() const { return wordBound_.c_str(); } 
    const char* getTagBound() const { return tagBound_.c_str(); } 
    const char* getElemBound() const { return elemBound_.c_str(); } 
    const char* getUnkBound() const { return unkBound_.c_str(); } 
    const char* getNoBound() const { return noBound_.c_str(); } 
    const char* getHasBound() const { return hasBound_.c_str(); } 
    const char* getSkipBound() const { return skipBound_.c_str(); } 
    const char* getEscape() const { return escape_.c_str(); } 

    const double getConfidence() const { return confidence_; }
    const char getEncoding() const;
    const char* getEncodingString() const;
    int getNumTags() const { return numTags_; }
    bool getGlobal(int i) const { return i < (int)global_.size() && global_[i]; }

    const std::vector<std::string> & getArguments() const { return args_; }
    
    void setDebug(unsigned debug) { debug_ = debug; }
    void setModelFile(const char* file) { model_ = file; }
    void setModelFormat(char mf) { modelForm_ = mf; }
    void setEpsilon(double v) { eps_ = v; }
    void setCost(double v) { cost_ = v; }
    void setBias(bool v) { bias_ = (v?1.0f:-1.0f); }
    void setSolverType(int v) { solverType_ = v; }
    void setCharWindow(char v) { charW_ = v; }
    void setCharN(char v) { charN_ = v; }
    void setTypeWindow(char v) { typeW_ = v; }
    void setTypeN(char v) { typeN_ = v; }
    void setDictionaryN(char v) { dictN_ = v; }
    void setUnkN(char v) { unkN_ = v; }
    void setTagMax(unsigned v) { tagMax_ = v; }
    void setUnkBeam(unsigned v) { unkBeam_ = v; }
    void setUnkTag(const std::string & v) { unkTag_ = v; }
    void setUnkTag(const char* v) { unkTag_ = v; }
    void setDefaultTag(const std::string & v) { defTag_ = v; }
    void setDefaultTag(const char* v) { defTag_ = v; }
    void setOnTraining(bool v) { onTraining_ = v; }
    void setDoWS(bool v) { doWS_ = v; }
    void setDoUnk(bool v) { doUnk_ = v; }
    void setDoTags(bool v) { doTags_ = v; } 
    void setDoTag(int i, bool v)  { 
        if(i >= (int)doTag_.size()) doTag_.resize(i+1,true); 
        doTag_[i] = v;
    } 
    void setInputFormat(CorpForm v) { inputForm_ = v; }
    void setWordBound(const char* v) { wordBound_ = v; } 
    void setTagBound(const char* v) { tagBound_ = v; } 
    void setElemBound(const char* v) { elemBound_ = v; } 
    void setUnkBound(const char* v) { unkBound_ = v; } 
    void setNoBound(const char* v) { noBound_ = v; } 
    void setHasBound(const char* v) { hasBound_ = v; } 
    void setSkipBound(const char* v) { skipBound_ = v; } 
    void setEscape(const char* v) { escape_ = v; } 
    void setNumTags(int v) { numTags_ = v; } 
    void setGlobal(int v) { if((int)global_.size() <= v) global_.resize(v+1,false); global_[v] = true; } 
    void setFeatureIn(const std::string & featIn) { featIn_ = featIn; }
    void setFeatureOut(const std::string & featOut) { featOut_ = featOut; }
    void setWsConstraint(const std::string & wsConstraint) { wsConstraint_ = wsConstraint; }

    std::ostream * getFeatureOutStream();
    void closeFeatureOutStream();

    void setEncoding(const char* str);

};

}

#endif