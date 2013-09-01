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
#ifndef MODEL_IO_H__ 
#define MODEL_IO_H__ 

#include <kinkaku/dictionary.h>
#include <kinkaku/general-io.h>
#include <kinkaku/feature-vector.h>
#include <vector>

#if DISABLE_QUANTIZE
#   define MODEL_IO_VERSION "1.0.0NQ"
#else
#   define MODEL_IO_VERSION "1.0.0"
#endif

namespace kinkaku {

class FeatureLookup;
class KinkakuModel;
class KinkakuLM;
class ModelTagEntry;
class ProbTagEntry;

class ModelIO : public GeneralIO {

public:

    typedef char Format;
    const static Format FORMAT_BINARY = 'B';
    const static Format FORMAT_TEXT = 'T';
    const static Format FORMAT_UNKNOWN = 'U';

    int numTags_;

public:

    ModelIO(StringUtil* util) : GeneralIO(util) { }
    ModelIO(StringUtil* util, const char* file, bool out, bool bin) : GeneralIO(util,file,out,bin) { }
    ModelIO(StringUtil* util, std::iostream & str, bool out, bool bin) : GeneralIO(util,str,out,bin) { }

    virtual ~ModelIO() { }

    static ModelIO* createIO(const char* file, Format form, bool output, KinkakuConfig & config);
    static ModelIO* createIO(std::iostream & str, Format form, bool output, KinkakuConfig & config);

    virtual void writeConfig(const KinkakuConfig & conf) = 0;
    virtual void writeModel(const KinkakuModel * mod) = 0;
    virtual void writeWordList(const std::vector<KinkakuString> & list) = 0;
    virtual void writeLM(const KinkakuLM * mod) = 0;
    virtual void writeFeatVec(const FeatVec * vec) = 0;

    virtual void readConfig(KinkakuConfig & conf) = 0;
    virtual KinkakuModel * readModel() = 0;
    virtual std::vector<KinkakuString> readWordList() = 0;
    virtual KinkakuLM * readLM() = 0;
    virtual FeatVec * readFeatVec() = 0;

    virtual void writeModelDictionary(const Dictionary<ModelTagEntry> * dict) = 0;
    virtual void writeProbDictionary(const Dictionary<ProbTagEntry> * dict) = 0;
    virtual void writeVectorDictionary(const Dictionary<FeatVec > * dict) = 0;
    virtual Dictionary<ModelTagEntry> * readModelDictionary() = 0;
    virtual Dictionary<ProbTagEntry> * readProbDictionary() = 0;
    virtual Dictionary<FeatVec > * readVectorDictionary() = 0;

    virtual void writeFeatureLookup(const FeatureLookup * featLookup) = 0;
    virtual FeatureLookup * readFeatureLookup() = 0;

};

}

#endif