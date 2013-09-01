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
#include <kinkaku/dictionary.h>
#include <kinkaku/string-util.h>
#include <kinkaku/feature-lookup.h>
#include <kinkaku/kinkaku-model.h>
#include <kinkaku/kinkaku-lm.h>

using namespace std;
using namespace kinkaku;

namespace kinkaku {

template <class T>
void checkPointerEqual(const T* lhs, const T* rhs) {
    if(lhs == NULL) {
        if(rhs != NULL)
            THROW_ERROR("lhs == NULL, rhs != NULL");
    } else {
        if(rhs == NULL)
            THROW_ERROR("lhs != NULL, rhs == NULL");
        lhs->checkEqual(*rhs);
    }
}

template void checkPointerEqual(const StringUtil* lhs, const StringUtil* rhs);
template void checkPointerEqual(const KinkakuModel* lhs, const KinkakuModel* rhs);
template void checkPointerEqual(const FeatureLookup* lhs, const FeatureLookup* rhs);
template void checkPointerEqual(const Dictionary<ModelTagEntry>* lhs, const Dictionary<ModelTagEntry>* rhs);
template void checkPointerEqual(const Dictionary<ProbTagEntry>* lhs, const Dictionary<ProbTagEntry>* rhs);
template void checkPointerEqual(const Dictionary<FeatVec>* lhs, const Dictionary<FeatVec>* rhs);

template <class T>
void checkValueVecEqual(const std::vector<T> & a, const std::vector<T> & b) {
    if(a.size() != b.size()) THROW_ERROR("Vector sizes don't match: "<<a.size()<<" != "<<b.size());
    for(int i = 0; i < (int)a.size(); i++)
        if(a[i] != b[i]) THROW_ERROR("Vectors don't match at "<<i);
}

template <class T>
void checkValueVecEqual(const std::vector<T> * a, const std::vector<T> * b) {
    if((a == NULL || a->size() == 0) != (b == NULL || b->size() == 0)) {
        THROW_ERROR("only one dictVector_ is NULL");
    } else if(a != NULL) {
        checkValueVecEqual(*a, *b);
    }
}

template void checkValueVecEqual(const std::vector<unsigned int> * a, const std::vector<unsigned int> * b);
template void checkValueVecEqual(const std::vector<short> * a, const std::vector<short> * b);
template void checkValueVecEqual(const std::vector<vector<KinkakuString> > * a, const std::vector<vector<KinkakuString> > * b);
template void checkValueVecEqual(const std::vector<int> * a, const std::vector<int> * b);
template void checkValueVecEqual(const std::vector<KinkakuString> * a, const std::vector<KinkakuString> * b);

template <class T>
void checkPointerVecEqual(const std::vector<T*> & a, const std::vector<T*> & b) {
    if(a.size() > b.size()) {
        for(int i = b.size(); i < (int)a.size(); i++)
            if(a[i] != 0)
                THROW_ERROR("Vector sizes don't match: "<<a.size()<<" != "<<b.size());
    } else if(b.size() > a.size()) {
        for(int i = a.size(); i < (int)b.size(); i++)
            if(b[i] != 0)
                THROW_ERROR("Vector sizes don't match: "<<a.size()<<" != "<<b.size());
    } else {
        for(int i = 0; i < (int)a.size(); i++)
            checkPointerEqual(a[i], b[i]);
    }
}

template void checkPointerVecEqual(const std::vector<KinkakuModel*> & a, const std::vector<KinkakuModel*> & b);
template void checkPointerVecEqual(const std::vector<KinkakuLM*> & a, const std::vector<KinkakuLM*> & b);

template <class T>
void checkPointerVecEqual(const std::vector<T*> * a, const std::vector<T*> * b) {
    if((a == NULL || a->size() == 0) != (b == NULL || b->size() == 0)) {
        THROW_ERROR("only one dictVector_ is NULL");
    } else if(a != NULL) {
        checkPointerVecEqual(*a, *b);
    }
}

}