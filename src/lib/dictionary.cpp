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
#include <kinkaku/dictionary.h>
#include <kinkaku/kinkaku-string.h>
#include <kinkaku/kinkaku-util.h>
#include <kinkaku/kinkaku-model.h>
#include <kinkaku/string-util.h>
#include <kinkaku/feature-vector.h>
#include <iostream>

using namespace kinkaku;
using namespace std;

namespace kinkaku {

ModelTagEntry::~ModelTagEntry() {
    for(int i = 0; i < (int)tagMods.size(); i++)
        if(tagMods[i]) 
            delete tagMods[i];
}

double ProbTagEntry::incrementProb(const KinkakuString & str, int lev) {
    if(probs.size() != tags.size())
        probs.resize(tags.size());
    if(probs[lev].size() != tags[lev].size())
        probs[lev].resize(tags[lev].size(), 0.0);
    for(unsigned i = 0; i < tags[lev].size(); i++)
        if(tags[lev][i] == str) 
            return ++probs[lev][i];
    THROW_ERROR("Attempt to increment a non-existent tag string");
}

template <class Entry>
void Dictionary<Entry>::checkEqual(const Dictionary<Entry> & rhs) const {
    if(states_.size() != rhs.states_.size())
        THROW_ERROR("states_.size() != rhs.states_.size() ("<<states_.size()<<" != "<<rhs.states_.size());
    if(entries_.size() != rhs.entries_.size())
        THROW_ERROR("entries_.size() != rhs.entries_.size() ("<<entries_.size()<<" != "<<rhs.entries_.size());
    if(numDicts_ != rhs.numDicts_)
        THROW_ERROR("numDicts_ != rhs.numDicts_ ("<<numDicts_<<" != "<<rhs.numDicts_);
}

template <class Entry>
void Dictionary<Entry>::buildGoto(wm_const_iterator start, wm_const_iterator end, unsigned lev, unsigned nid) {
#ifdef KINKAKU_SAFE
    if(start == end) return;
    if(nid >= states_.size())
        THROW_ERROR("Out of bounds node in buildGoto ("<<nid<<" >= "<<states_.size()<<")");
#endif
    wm_const_iterator startCopy = start;
    DictionaryState & node = *states_[nid];
    if(startCopy->first.length() == lev) {
        node.output.push_back(entries_.size());
        node.isBranch = true;
        entries_.push_back(startCopy->second);
        startCopy++;
    }
    if(startCopy == end) return;
    wm_const_iterator binEnd = startCopy, binStart;
    unsigned numBins = 0;
    KinkakuChar lastChar = binEnd->first[lev];
    do {
        binEnd++;
        KinkakuChar nextChar = (binEnd == end?0:binEnd->first[lev]);
        if(nextChar != lastChar) {
            numBins++;
            lastChar = nextChar;
        }
    } while(binEnd != end);
    node.gotos.reserve(numBins);
    binStart = startCopy, binEnd = startCopy;
    lastChar = binStart->first[lev];
    do {
        binEnd++;
        KinkakuChar nextChar = (binEnd == end?0:binEnd->first[lev]);
        if(nextChar != lastChar) {
            unsigned nextNode = states_.size();
            states_.push_back(new DictionaryState());
            node.gotos.push_back(std::pair<KinkakuChar,unsigned>(lastChar,nextNode));
            buildGoto(binStart,binEnd,lev+1,nextNode);
            binStart = binEnd;
            lastChar = nextChar;
        }
    } while(binEnd != end);
}

template <class Entry>
void Dictionary<Entry>::buildFailures() {
    if(states_.size() == 0)
        return;
    std::deque<unsigned> sq;
    DictionaryState::Gotos & g0 = states_[0]->gotos;
    for(unsigned i = 0; i < g0.size(); i++)
        sq.push_back(g0[i].second);
    while(sq.size() != 0) {
        unsigned r = sq.front();
        sq.pop_front();
        DictionaryState::Gotos & gr = states_[r]->gotos;
        for(unsigned i = 0; i < gr.size(); i++) {
            KinkakuChar a = gr[i].first;
            unsigned s = gr[i].second;
            sq.push_back(s);
            unsigned state = states_[r]->failure;
            unsigned trans = 0;
            while((trans = states_[state]->step(a)) == 0 && (state != 0))
                state = states_[state]->failure;
            states_[s]->failure = trans;
            for(unsigned j = 0; j < states_[trans]->output.size(); j++)
                states_[s]->output.push_back(states_[trans]->output[j]);
        }
    }

}

template <class Entry>
void Dictionary<Entry>::clearData() {
    for(unsigned i = 0; i < states_.size(); i++)
        delete states_[i];
    for(unsigned i = 0; i < entries_.size(); i++)
        delete entries_[i];
    entries_.clear();
    states_.clear();
}

template <class Entry>
void Dictionary<Entry>::buildIndex(const WordMap & input) {
    if(input.size() == 0)
        THROW_ERROR("Cannot build dictionary for no input");
    clearData();
    states_.push_back(new DictionaryState());
    buildGoto(input.begin(), input.end(), 0, 0);
    buildFailures();
}

inline string showWord(StringUtil * util, const ModelTagEntry * entry) {
    return util->showString(entry->word);
}
inline string showWord(StringUtil * util, const ProbTagEntry * entry) {
    return util->showString(entry->word);
}
inline string showWord(StringUtil * util, const FeatVec * entry) {
    ostringstream oss;
    for(int i = 0; i < (int)entry->size(); i++) {
        if(i != 0) oss << ",";
        oss << (*entry)[i];
    }
    return oss.str();
}

template <class Entry>
void Dictionary<Entry>::print() {
    for(unsigned i = 0; i < states_.size(); i++) {
        std::cout << "s="<<i<<", f="<<states_[i]->failure<<", o='";
        for(unsigned j = 0; j < states_[i]->output.size(); j++) {
            if(j!=0) std::cout << " ";
            // std::cout << util_->showString(entries_[states_[i]->output[j]]->word);
            std::cout << showWord(util_, entries_[states_[i]->output[j]]);
        }
        std::cout << "' g='";
        for(unsigned j = 0; j < states_[i]->gotos.size(); j++) {
            if(j!=0) std::cout << " ";
            std::cout << util_->showChar(states_[i]->gotos[j].first) << "->" << states_[i]->gotos[j].second;
        }
        std::cout << "'" << std::endl;
    }
}

template <class Entry>
Entry * Dictionary<Entry>::findEntry(KinkakuString str) {
    if(str.length() == 0) return 0;
    unsigned state = 0, lev = 0;
    do {
#ifdef KINKAKU_SAFE
        if(state >= states_.size())
            THROW_ERROR("Accessing state "<<state<<" that is larger than states_ ("<<states_.size()<<")");
        if(states_[state] == 0)
            THROW_ERROR("Accessing null state "<<state);
#endif
        state = states_[state]->step(str[lev++]);
    } while (state != 0 && lev < str.length());
    if(states_[state]->output.size() == 0) return 0;
    if(!states_[state]->isBranch) return 0;
    return entries_[states_[state]->output[0]];
}
template <class Entry>
const Entry * Dictionary<Entry>::findEntry(KinkakuString str) const {
    if(str.length() == 0) return 0;
    unsigned state = 0, lev = 0;
    do {
        state = states_[state]->step(str[lev++]);
    } while (state != 0 && lev < str.length());
    if(states_[state]->output.size() == 0) return 0;
    if(!states_[state]->isBranch) return 0;
    return entries_[states_[state]->output[0]];
}

template <>
unsigned Dictionary<FeatVec>::getTagID(KinkakuString str, KinkakuString tag, int lev) {
    return 0;
}
template <class Entry>
unsigned Dictionary<Entry>::getTagID(KinkakuString str, KinkakuString tag, int lev) {
    const Entry * ent = findEntry(str);
    if(ent == 0) return 0;
    for(unsigned i = 0; i < ent->tags[lev].size(); i++)
        if(ent->tags[lev][i] == tag)
            return i+1;
    return 0;
}

template <class Entry>
typename Dictionary<Entry>::MatchResult Dictionary<Entry>::match( const KinkakuString & chars ) const {
    const unsigned len = chars.length();
    unsigned currState = 0, nextState;
    MatchResult ret;
    for(unsigned i = 0; i < len; i++) {
        KinkakuChar c = chars[i];
        while((nextState = states_[currState]->step(c)) == 0 && currState != 0)
            currState = states_[currState]->failure;
        currState = nextState;
        std::vector<unsigned> & output = states_[currState]->output;
        for(unsigned j = 0; j < output.size(); j++) 
            ret.push_back( std::pair<unsigned, Entry*>(i, entries_[output[j]]) );
    }
    return ret;
}

template class Dictionary<ModelTagEntry>;
template class Dictionary<ProbTagEntry>;
template class Dictionary<FeatVec>;

}