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
#ifndef MODEL_IO_TEXT_H__
#define MODEL_IO_TEXT_H__

#include <kinkaku/model-io.h>
#include <algorithm>

namespace kinkaku {

class CorpusIO;

class TextModelIO : public ModelIO {

public:

    TextModelIO(StringUtil* util) : ModelIO(util) { }
    TextModelIO(StringUtil* util, const char* file, bool out) : ModelIO(util,file,out,false) { }
    TextModelIO(StringUtil* util, std::iostream & str, bool out) : ModelIO(util,str,out,false) { }

    void writeConfig(const KinkakuConfig & conf);
    void writeModel(const KinkakuModel * mod);
    void writeWordList(const std::vector<KinkakuString> & list);
    void writeModelDictionary(const Dictionary<ModelTagEntry> * dict) { writeDictionary(dict); }
    void writeProbDictionary(const Dictionary<ProbTagEntry> * dict) { writeDictionary(dict); }
    void writeVectorDictionary(const Dictionary<FeatVec > * dict) { writeDictionary(dict); }
    void writeLM(const KinkakuLM * mod);
    void writeFeatVec(const FeatVec * vec);
    void writeFeatureLookup(const FeatureLookup * featLookup);

    template <class Entry>
    void writeEntry(const Entry * entry);

    template <class Entry>
    void writeDictionary(const Dictionary<Entry> * dict) {
        if(dict == 0) {
            *str_ << "0" << std::endl << "0" << std::endl;
            return;
        }
        *str_ << (unsigned)dict->getNumDicts() << std::endl;
        const std::vector<DictionaryState*> & states = dict->getStates();
        *str_ << states.size() << std::endl;
        if(states.size() == 0)
            return;
        for(unsigned i = 0; i < states.size(); i++) {
            *str_ << states[i]->failure;
            for(unsigned j = 0; j < states[i]->gotos.size(); j++)
                *str_ << " " << util_->showChar(states[i]->gotos[j].first) << " " << states[i]->gotos[j].second;
            *str_ << std::endl;
            for(unsigned j = 0; j < states[i]->output.size(); j++) {
                if(j!=0) *str_ << " ";
                *str_ << states[i]->output[j];
            }
            *str_ << std::endl;
            *str_ << (states[i]->isBranch?'b':'n') << std::endl;
        }
        const std::vector<Entry*> & entries = dict->getEntries();
        *str_ << entries.size() << std::endl;
        for(unsigned i = 0; i < entries.size(); i++)
            writeEntry((Entry*)entries[i]);
    }

    static CorpusIO* createIO(const char* file, Format form, bool output, StringUtil* util);
    static CorpusIO* createIO(std::iostream & str, Format form, bool output, StringUtil* util);

    void readConfig(KinkakuConfig & conf);
    KinkakuModel * readModel();
    std::vector<KinkakuString> readWordList();
    Dictionary<ModelTagEntry> * readModelDictionary() { return readDictionary<ModelTagEntry>(); }
    Dictionary<ProbTagEntry> * readProbDictionary()  { return readDictionary<ProbTagEntry>(); }
    Dictionary<FeatVec > * readVectorDictionary()  { return readDictionary<FeatVec >(); }
    KinkakuLM * readLM();
    FeatVec * readFeatVec();
    FeatureLookup * readFeatureLookup();

    template <class Entry>
    Entry * readEntry();

    template <class Entry>
    Dictionary<Entry> * readDictionary() {
        Dictionary<Entry> * dict = new Dictionary<Entry>(util_);
        std::string line, buff;
        std::getline(*str_, line);
        dict->setNumDicts(util_->parseInt(line.c_str()));
        std::vector<DictionaryState*> & states = dict->getStates();
        getline(*str_, line);
        states.resize(util_->parseInt(line.c_str()));
        if(states.size() == 0) {
            delete dict;
            return 0;
        }
        for(unsigned i = 0; i < states.size(); i++) {
            DictionaryState * state = new DictionaryState();
            getline(*str_, line);
            std::istringstream iss(line);
            iss >> buff;
            state->failure = util_->parseInt(buff.c_str());
            while(iss >> buff) {
                std::pair<KinkakuChar,unsigned> p;
                p.first = util_->mapChar(buff.c_str());
                if(!(iss >> buff))
                    THROW_ERROR("Bad form model (goto character without a destination)");
                p.second = util_->parseInt(buff.c_str());
                state->gotos.push_back(p);
            }
            sort(state->gotos.begin(), state->gotos.end());
            getline(*str_, line);
            std::istringstream iss2(line);
            while(iss2 >> buff)
                state->output.push_back(util_->parseInt(buff.c_str()));
            getline(*str_, line);
            if(line.length() != 1)
                THROW_ERROR("Bad form model (branch indicator not found)");
            state->isBranch = (line[0] == 'b');
            states[i] = state;
        }
        std::vector<Entry*> & entries = dict->getEntries();
        getline(*str_, line);
        entries.resize(util_->parseInt(line.c_str()));
        for(unsigned i = 0; i < entries.size(); i++) {
            entries[i] = readEntry<Entry>();
        }
        return dict;
    }

};

}

#endif