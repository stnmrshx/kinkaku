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
#include <kinkaku/kinkaku-string.h>
#include <cstring>

using namespace kinkaku;
using namespace std;

KinkakuStringImpl::KinkakuStringImpl(unsigned length) : length_(length), count_(1) {
    chars_ = new KinkakuChar[length];
}
KinkakuStringImpl::KinkakuStringImpl(const KinkakuStringImpl & impl) : length_(impl.length_), count_(1) {
    chars_ = new KinkakuChar[length_];
    memcpy(chars_, impl.chars_, sizeof(KinkakuChar)*length_);
}

KinkakuString::Tokens KinkakuString::tokenize(const KinkakuString & delim, bool includeDelim) const {
    unsigned i,j,s=0;
    const unsigned l=length(),dl=delim.length();
    vector<KinkakuString> ret;
    for(i = 0; i < l; i++) {
        for(j = 0; j < dl && delim[j] != impl_->chars_[i]; j++);
        if(j != dl) {
            if(s != i)
                ret.push_back(substr(s,i-s));
            if(includeDelim)
                ret.push_back(substr(i,1));
            s = i+1;
        }
    }
    if(s != i)
        ret.push_back(substr(s,i-s));
    return ret;
}

void KinkakuString::splice(const KinkakuString& str, unsigned pos) {
    const unsigned l = str.length();
    if(!l) 
        return;
#ifdef KINKAKU_SAFE
    if(pos+l > length())
        throw runtime_error("KinkakuString splice index out of bounds");
#endif
    memcpy(impl_->chars_+pos, str.getImpl()->chars_, sizeof(KinkakuChar)*l);
}

KinkakuString KinkakuString::substr(unsigned s) const {
    const unsigned l = length()-s;
#ifdef KINKAKU_SAFE
    if(s+l > length())
        throw runtime_error("KinkakuString substr index out of bounds");
#endif
    KinkakuString ret(l);
    memcpy(ret.getImpl()->chars_, impl_->chars_+s, sizeof(KinkakuChar)*l);
    return ret;
}


KinkakuString KinkakuString::substr(unsigned s, unsigned l) const {
#ifdef KINKAKU_SAFE
    if(s+l > length())
        throw runtime_error("substr out of bounds");
#endif
    KinkakuString ret(l);
    memcpy(ret.getImpl()->chars_, impl_->chars_+s, sizeof(KinkakuChar)*l);
    return ret;
}

size_t KinkakuString::getHash() const {
    size_t hash = 5381;
    if(impl_==0)
        return hash;
    const unsigned l = impl_->length_;
    const KinkakuChar* cs = impl_->chars_;
    for(unsigned i = 0; i < l; i++)
        hash = ((hash << 5) + hash) + cs[i];
    return hash;
}

KinkakuStringImpl * KinkakuString::getImpl() {
    if(impl_->count_ != 1) {
        impl_->dec();
        impl_ = new KinkakuStringImpl(*impl_);
    }
    return impl_;
}
bool KinkakuString::beginsWith(const KinkakuString & s) const {
    if(s.length() > this->length()) return 0;
    for(int i = s.length()-1; i >= 0; i--) {
        if((*this)[i] != s[i])
            return 0;
    }
    return 1;
}