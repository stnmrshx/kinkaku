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
#ifndef MODEL_IO_BINARY_H__
#define MODEL_IO_BINARY_H__

#include <kinkaku/model-io.h>
#include <kinkaku/dictionary.h>

namespace kinkaku {

class BinaryModelIO : public ModelIO {

public:

    BinaryModelIO(StringUtil* util) : ModelIO(util) { }
    BinaryModelIO(StringUtil* util, const char* file, bool out) : ModelIO(util,file,out,true) { }
    BinaryModelIO(StringUtil* util, std::iostream & str, bool out) : ModelIO(util,str,out,true) { }

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
            writeBinary((unsigned char)0);
            writeBinary((uint32_t)0);
            return;
        }
        if(dict->getNumDicts() > 8)
            THROW_ERROR("Only 8 dictionaries can be stored in a binary file.");
        writeBinary(dict->getNumDicts());
        const std::vector<DictionaryState*> & states = dict->getStates();
        writeBinary((uint32_t)states.size());
        for(unsigned i = 0; i < states.size(); i++) {
            const DictionaryState * state = states[i];
            writeBinary((uint32_t)state->failure);
            writeBinary((uint32_t)state->gotos.size());
            for(unsigned j = 0; j < state->gotos.size(); j++) {
                writeBinary((KinkakuChar)state->gotos[j].first);
                writeBinary((uint32_t)state->gotos[j].second);
            }
            writeBinary((uint32_t)state->output.size());
            for(unsigned j = 0; j < state->output.size(); j++) 
                writeBinary((uint32_t)state->output[j]);
            writeBinary(state->isBranch);
        }
        const std::vector<Entry*> & entries = dict->getEntries();
        writeBinary((uint32_t)entries.size());
        for(unsigned i = 0; i < entries.size(); i++)
            writeEntry(entries[i]);

    }

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
        unsigned numDicts = readBinary<unsigned char>();
        dict->setNumDicts(numDicts);
        std::vector<DictionaryState*> & states = dict->getStates();
        states.resize(readBinary<uint32_t>());
        if(states.size() == 0) {
            delete dict;
            return 0;
        }
        for(unsigned i = 0; i < states.size(); i++) {
            DictionaryState * state = new DictionaryState();
            state->failure = readBinary<uint32_t>();
            state->gotos.resize(readBinary<uint32_t>());
            for(unsigned j = 0; j < state->gotos.size(); j++) {
                state->gotos[j].first = readBinary<KinkakuChar>();
                state->gotos[j].second = readBinary<uint32_t>();
            }
            state->output.resize(readBinary<uint32_t>());
            for(unsigned j = 0; j < state->output.size(); j++) 
                state->output[j] = readBinary<uint32_t>();
            state->isBranch = readBinary<bool>();
            states[i] = state;
        }
        std::vector<Entry*> & entries = dict->getEntries();
        entries.resize(readBinary<uint32_t>());
        for(unsigned i = 0; i < entries.size(); i++) 
            entries[i] = readEntry<Entry>();
        return dict;
    }

};

}

#endif
