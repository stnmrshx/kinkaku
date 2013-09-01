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
#ifndef KINKAKU_STRING_H__
#define KINKAKU_STRING_H__

#include <vector>
#include <cstddef>

namespace kinkaku {

typedef unsigned short KinkakuChar;

class StringUtil;

class KinkakuStringImpl {

public:
    unsigned length_;
    unsigned count_;
    KinkakuChar* chars_;

    KinkakuStringImpl() : length_(0), count_(1), chars_(0) { }
    KinkakuStringImpl(unsigned length);
    KinkakuStringImpl(const KinkakuStringImpl & impl);
    ~KinkakuStringImpl() {
        if(chars_)
            delete [] chars_;
    }

    unsigned dec() { return --count_; }
    unsigned inc() { return ++count_; }

};

class KinkakuString {

public:
    friend class StringUtil;
    
private:
    KinkakuStringImpl* impl_;

public:

    typedef std::vector<KinkakuString> Tokens;

    KinkakuString() : impl_(0) { }
    KinkakuString(const KinkakuString & str) { 
        impl_ = str.impl_;
        if(impl_) impl_->inc();
    }
    KinkakuString(unsigned length) { impl_ = new KinkakuStringImpl(length); }
    
    ~KinkakuString() {
        if(impl_ && !impl_->dec())
            delete impl_;
    }

    Tokens tokenize(const KinkakuString & delim, bool includeDelim = false) const;

    void splice(const KinkakuString& str, unsigned pos);

    KinkakuString substr(unsigned s) const;
    KinkakuString substr(unsigned s, unsigned l) const;
    
    inline KinkakuChar & operator[](int i) {
#ifdef KINKAKU_SAFE
        if(impl_ == 0 || i < 0 || (unsigned)i >= impl_->length_)
            throw std::runtime_error("string index out of bounds");
#endif
        return getImpl()->chars_[i];
    }
    
    inline const KinkakuChar & operator[](int i) const {
#ifdef KINKAKU_SAFE
        if(impl_ == 0 || i < 0 || (unsigned)i >= impl_->length_)
            throw std::runtime_error("string index out of bounds");
#endif
        return impl_->chars_[i];
    }
    
    KinkakuString & operator= (const KinkakuString &str) {
        if(impl_ && !impl_->dec())
            delete impl_;
        impl_ = str.impl_;
        if(impl_) impl_->inc();
        return *this;
    }


    inline unsigned length() const {
        return (impl_?impl_->length_:0); 
    }

    size_t getHash() const;

    inline const KinkakuStringImpl * getImpl() const {
        return impl_;
    } 
    KinkakuStringImpl * getImpl();

    bool beginsWith(const KinkakuString & s) const;

};

inline KinkakuString operator+(const KinkakuString& a, const KinkakuChar& b) {
    const KinkakuStringImpl * aimp = a.getImpl();
    if(aimp == 0) {
        KinkakuString ret(1);
        ret[0] = b;
        return ret;
    }
    KinkakuString ret(aimp->length_+1);
    ret.splice(a,0);
    ret[aimp->length_]=b;
    return ret;
}

inline KinkakuString operator+(const KinkakuString& a, const KinkakuString& b) {
    const KinkakuStringImpl * aimp = a.getImpl();
    if(aimp == 0)
        return b;
    const KinkakuStringImpl * bimp = b.getImpl();
    if(bimp == 0)
        return a;
    KinkakuString ret(aimp->length_+bimp->length_);
    ret.splice(a,0);
    ret.splice(b,aimp->length_);
    return ret;
}

inline bool operator<(const KinkakuString & a, const KinkakuString & b) {
    unsigned i;
    const unsigned al = a.length(), bl = b.length(), ml=std::min(al,bl);
    for(i = 0; i < ml; i++) {
        if(a[i] < b[i]) return true;
        else if(b[i] < a[i]) return false;
    }
    return (bl != i);
}

inline bool operator==(const KinkakuString & a, const KinkakuString & b) {
    unsigned i;
    const unsigned al = a.length();
    if(al!=b.length())
        return false;
    for(i = 0; i < al; i++)
        if(a[i] != b[i]) return false;
    return true;
}

inline bool operator!=(const KinkakuString & a, const KinkakuString & b) {
    return !(a==b);
}

class KinkakuStringHash {
public:
    size_t operator()(const KinkakuString & x) const {
        return x.getHash();
    }
};

}

#endif