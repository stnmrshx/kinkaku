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
#ifndef KINKAKU_STRUCT_H__
#define KINKAKU_STRUCT_H__

#include <kinkaku/config.h>
#include <kinkaku/kinkaku-string.h>
#include <string>
#include <map>

#ifdef HAVE_BOOST_TR1_UNORDERED_MAP_HPP
#   include <boost/tr1/unordered_map.hpp>
    template <class Key, class T>
    class GenericMap : public std::tr1::unordered_map<Key,T> { };
    template <class T>
    class StringMap : public std::tr1::unordered_map<std::string,T> { };
    template <class T>
    class KinkakuStringMap : public std::tr1::unordered_map<kinkaku::KinkakuString,T,kinkaku::KinkakuStringHash> { };
#elif HAVE_TR1_UNORDERED_MAP
#if _MSC_VER >=1600
#   include <unordered_map>
#else
#   include <tr1/unordered_map>
#endif
    template <class Key, class T>
    class GenericMap : public std::tr1::unordered_map<Key,T> { };
    template <class T>
    class StringMap : public std::tr1::unordered_map<std::string,T> { };
    template <class T>
    class KinkakuStringMap : public std::tr1::unordered_map<kinkaku::KinkakuString,T,kinkaku::KinkakuStringHash> { };
#elif HAVE_EXT_HASH_MAP
#   include <ext/hash_map>
    namespace __gnu_cxx {
    template <>
    struct hash<std::string> {
        size_t operator() (const std::string& x) const { return hash<const char*>()(x.c_str()); }
    };
    }
    template <class Key, class T>
    class GenericMap : public __gnu_cxx::hash_map<Key,T> { };
    template <class T>
    class StringMap : public __gnu_cxx::hash_map<std::string,T> { };
    template <class T>
    class KinkakuStringMap : public __gnu_cxx::hash_map<kinkaku::KinkakuString,T,kinkaku::KinkakuStringHash> { };
#else
#   include <map>
    template <class Key, class T>
    class GenericMap : public std::map<Key,T> { };
    template <class T>
    class StringMap : public std::map<std::string,T> { };
    template <class T>
    class KinkakuStringMap : public std::map<kinkaku::KinkakuString,T> { };
#endif

namespace kinkaku  {

template <class T>
void checkMapEqual(const KinkakuStringMap<T> & a, const KinkakuStringMap<T> & b);

typedef std::pair<KinkakuString,double> KinkakuTag;
inline bool operator<(const KinkakuTag & a, const KinkakuTag & b) {
    if(a.second < b.second) return false;
    if(b.second < a.second) return true;
    return a.first < b.first;
}

class KinkakuWord {
public:
    KinkakuWord(const KinkakuString & s, const KinkakuString & n) : surface(s), norm(n), isCertain(true), unknown(false) { }

    KinkakuString surface;
    KinkakuString norm;
    std::vector< std::vector< KinkakuTag > > tags;
    bool isCertain;
    bool unknown;

    void limitTags(unsigned lev, unsigned lim) {
        if(tags.size() > lev && tags[lev].size() > lim)
            tags[lev].resize(lim);
    }
    const int getNumTags() const { return tags.size(); }
    const KinkakuTag * getTag(int lev) const { return (lev<(int)tags.size()&&tags[lev].size()>0) ? &tags[lev][0] : 0; }
    const std::vector< KinkakuTag > & getTags(int lev) const { return tags[lev]; }
    const KinkakuString & getTagSurf(int lev) const { return tags[lev][0].first; }
    double getTagConf(int lev) const { return tags[lev][0].second; }
    void setTag(int lev, const KinkakuTag & tag) { 
        if(lev >= (int)tags.size()) tags.resize(lev+1);
        tags[lev].resize(1);
        tags[lev][0] = tag;
    }
    void setTagConf(int lev, double conf) { tags[lev][0].second = conf; }
    void clearTags(int lev) { if((int)tags.size() > lev) tags[lev].clear(); }
    void addTag(int lev, const KinkakuTag & tag) { 
        if(lev >= (int)tags.size()) tags.resize(lev+1);
        tags[lev].push_back(tag);
    }
    void setUnknown(bool val) { unknown = val; }
    bool getUnknown() const { return unknown; }
    bool hasTag(int lev) const { return (int)tags.size() > lev && tags[lev].size() > 0; }

};

class KinkakuSentence {

public:

    typedef std::vector<KinkakuWord> Words;
    typedef std::vector<double> Floats;

    KinkakuString surface;
    KinkakuString norm;
    Floats wsConfs;

    Words words;

    KinkakuSentence() : surface(), wsConfs(0) {
    }
    KinkakuSentence(const KinkakuString & str, const KinkakuString & norm_str) : surface(str), norm(norm_str), wsConfs(std::max(str.length(),(unsigned)1)-1,0) { }

    void refreshWS(double confidence);

};

}

typedef StringMap<kinkaku::KinkakuChar> StringCharMap;
typedef KinkakuStringMap<unsigned> KinkakuUnsignedMap;
typedef KinkakuStringMap<double>   KinkakuDoubleMap;
typedef KinkakuStringMap<std::pair<unsigned,unsigned> > TwoCountHash;

#endif