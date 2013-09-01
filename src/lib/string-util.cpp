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
#include <kinkaku/string-util.h>
#include <kinkaku/string-util-map-utf8.h>
#include <kinkaku/string-util-map-euc.h>
#include <kinkaku/string-util-map-sjis.h>
#include <cstdlib>
#include <iostream>
#include <limits>

using namespace kinkaku;
using namespace std;


void StringUtil::checkEqual(const StringUtil & rhs) const {
    std::string me = serialize();
    std::string you = rhs.serialize();
    if(me != you) {
        THROW_ERROR("String utils don't match" << std::endl 
                    << " --- lhs --- " << std::endl << me << std::endl
                    << " --- rhs --- " << std::endl << you);
    }
}

int StringUtil::parseInt(const char* str) {
    char* endP;
    int ret = strtol(str, &endP, 10);
    if(endP == str)
        THROW_ERROR("Bad integer value '" << str << "'");
    return ret;
}
double StringUtil::parseFloat(const char* str) {
    char* endP;
    double ret = strtod(str, &endP);
    if(endP == str)
        THROW_ERROR("Bad floating-point value '" << str << "'");
    return ret;
}

StringUtilUtf8::StringUtilUtf8() {
    const char * initial[7] = { "", "K", "T", "H", "R", "D", "O" };
    for(unsigned i = 0; i < 7; i++) {
        charIds_.insert(std::pair<std::string,KinkakuChar>(initial[i], i));
        charTypes_.push_back(i==0?6:4);
        charNames_.push_back(initial[i]);
    }
}

GenericMap<KinkakuChar,KinkakuChar> * StringUtilUtf8::getNormMap() {
    if(normMap_ == NULL) {
        normMap_ = new GenericMap<KinkakuChar,KinkakuChar>;
        KinkakuString orig = mapString(STRING_UTIL_ORIG_UTF8);
        KinkakuString norm = mapString(STRING_UTIL_NORM_UTF8);
        if(orig.length() != norm.length())
            THROW_ERROR("FATAL ERROR: unmatched strings in string-util.cpp : StringUtilUtf8");
        for(int i = 0; i < (int)orig.length(); i++)
            normMap_->insert(pair<KinkakuChar,KinkakuChar>(orig[i], norm[i]));
    }
    return normMap_;
}

GenericMap<KinkakuChar,KinkakuChar> * StringUtilSjis::getNormMap() {
    if(normMap_ == NULL) {
        normMap_ = new GenericMap<KinkakuChar,KinkakuChar>;
        KinkakuString orig = mapString(STRING_UTIL_ORIG_SJIS);
        KinkakuString norm = mapString(STRING_UTIL_NORM_SJIS);
        if(orig.length() != norm.length()) {
            for(int i = 0; i < (int)min(orig.length(), norm.length()); i++)
                cerr << showChar(orig[i]) << " <-> " << showChar(norm[i]) << endl;
            THROW_ERROR("FATAL ERROR: unmatched strings in string-util.cpp : StringUtilSjis");
        }
        for(int i = 0; i < (int)orig.length(); i++)
            normMap_->insert(pair<KinkakuChar,KinkakuChar>(orig[i], norm[i]));
    }
    return normMap_;
}

GenericMap<KinkakuChar,KinkakuChar> * StringUtilEuc::getNormMap() {
    if(normMap_ == NULL) {
        normMap_ = new GenericMap<KinkakuChar,KinkakuChar>;
        KinkakuString orig = mapString(STRING_UTIL_ORIG_EUC);
        KinkakuString norm = mapString(STRING_UTIL_NORM_EUC);
        if(orig.length() != norm.length()) {
            for(int i = 0; i < (int)min(orig.length(), norm.length()); i++)
                cerr << showChar(orig[i]) << " <-> " << showChar(norm[i]) << endl;
            THROW_ERROR("FATAL ERROR: unmatched strings in string-util.cpp : StringUtilEuc");
        }
        for(int i = 0; i < (int)orig.length(); i++)
            normMap_->insert(pair<KinkakuChar,KinkakuChar>(orig[i], norm[i]));
    }
    return normMap_;
}

KinkakuChar StringUtilUtf8::mapChar(const string & str, bool add) {
    StringCharMap::iterator it = charIds_.find(str);
    KinkakuChar ret = 0;
    if(it != charIds_.end())
        ret = it->second;
    else if (add) {
        if (charTypes_.size() > std::numeric_limits<KinkakuChar>::max())
          THROW_ERROR("FATAL ERROR: id exceeds numerical limit in string-util.cpp : StringUtilUtf8");
        ret = charTypes_.size();
        charIds_.insert(pair<string, KinkakuChar>(str,ret));
        charTypes_.push_back(findType(str));
        charNames_.push_back(str);
    }
    return ret;
}

string StringUtilUtf8::showChar(KinkakuChar c) {
#ifdef KINKAKU_SAFE
    if(c >= charNames_.size())
        THROW_ERROR("FATAL: Index out of bounds in showChar");
#endif 
    return charNames_[c];
}

StringUtil::CharType StringUtilUtf8::findType(KinkakuChar c) {
    return charTypes_[c];
}

KinkakuString StringUtilUtf8::mapString(const string & str) {
    unsigned pos = 0, len = str.length();
    vector<KinkakuChar> ret;
    while(pos < len) {
        if(!(maskl1 & str[pos]))
            ret.push_back(mapChar(str.substr(pos++, 1)));
        else if((maskl5 & str[pos]) == maskl5) {
            THROW_ERROR("Expected UTF8 file but found non-UTF8 string (specify the proper encoding with -encode utf8/euc/sjis): "<<str);
        }
        else if((maskl4 & str[pos]) == maskl4) {
            if(pos + 3 >= len || badu(str[pos+1]) || badu(str[pos+2]) || badu(str[pos+3]))
                THROW_ERROR("Expected UTF8 file but found non-UTF8 string (specify the proper encoding with -encode utf8/euc/sjis): "<<str);
            ret.push_back(mapChar(str.substr(pos, 4)));
            pos += 4;
        }
        else if((maskl3 & str[pos]) == maskl3) {
            if(pos + 2 >= len || badu(str[pos+1]) || badu(str[pos+2]))
                THROW_ERROR("Expected UTF8 file but found non-UTF8 string (specify the proper encoding with -encode utf8/euc/sjis): "<<str);
            ret.push_back(mapChar(str.substr(pos, 3)));
            pos += 3;
        }
        else {
            if(pos + 1 >= len || badu(str[pos+1]))
                THROW_ERROR("Expected UTF8 file but found non-UTF8 string (specify the proper encoding with -encode utf8/euc/sjis): "<<str);
            ret.push_back(mapChar(str.substr(pos, 2)));
            pos += 2;
        }
    }
    KinkakuString retstr(ret.size());
    for(unsigned i = 0; i < ret.size(); i++)
        retstr[i] = ret[i];
    return retstr;
}

StringUtil::CharType StringUtilUtf8::findType(const string & str) {
    if(str.length() == 0)
        return OTHER;
    if(str.length()>4)
        THROW_ERROR("Malformed utf8 character in findType");
    unsigned val = 0;
    if(str.length() == 1) val = str[0];
    else if(str.length() == 2) val = ((str[0]&maskr5)<<6) | (maskr6&str[1]);
    else if(str.length() == 3) val = ((str[0]&maskr4)<<12) | ((maskr6&str[1])<<6) | (maskr6&str[2]);
    else val = ((str[0]&maskr3)<<18) | ((maskr6&str[1])<<12) | ((maskr6&str[2])<<18) | (maskr6&str[3]);
    
    if((val >= 0x41 && val <= 0x5A) || (val >= 0x61 && val <= 0x7A)
        || (val >= 0xFF21 && val <= 0xFF3A) || (val >= 0xFF41 && val <= 0xFF5A)) {
        return ROMAJI;
    }
    else if((val >= 0x3040 && val <= 0x3096)) {
        return HIRAGANA;
    }
    else if((val >= 0x30A0 && val <= 0x30FF && val != 0x30FB) || (val >= 0xFF66 && val <= 0xFF9F)) {
        return KATAKANA;
    }
    else if((val >= 0x30 && val <= 0x39) || (val >= 0xFF10 && val <= 0xFF19)) {
        return DIGIT;
    }
    else if((val >= 0x3400 && val <= 0x4DBF) 
           || (val >= 0x4E00 && val <= 0x9FFF) 
           || (val >= 0xF900 && val <= 0xFAFF) 
           || (val >= 0x20000 && val <= 0x2A6DF) 
           || (val >= 0x2A700 && val <= 0x2B73F) 
           || (val >= 0x2B740 && val <= 0x2B81F) 
           || (val >= 0x2F800 && val <= 0x2FA1F)) { 
        return KANJI;
    }
    return OTHER;
}


void StringUtilUtf8::unserialize(const string & str) {
    charIds_.clear(); charNames_.clear(); charTypes_.clear();
    mapChar("");
    KinkakuString ret = mapString(str);
}

string StringUtilUtf8::serialize() const {
    ostringstream buff;
    for(unsigned i = 1; i < charNames_.size(); i++)
        buff << charNames_[i];
    return buff.str();
}

inline KinkakuChar eucm(char a, char b) {
    KinkakuChar ret = a & 0xFF;
    ret = ret << 8;
    ret = ret | (b&0xFF);
    return ret;
}
inline unsigned char euc1(KinkakuChar a) {
    return (a & 0xFF00) >> 8;
}
inline unsigned char euc2(KinkakuChar a) {
    return (a & 0xFF);
}

KinkakuChar StringUtilEuc::mapChar(const string & str, bool add) {
    unsigned len = str.length();
    KinkakuChar ret;
    if(len == 1) {
#ifdef KINKAKU_SAFE
        if(str[0] & maskl1)
            THROW_ERROR("Expected EUC file but found non-EUC string (specify the proper encoding with -encode utf8/euc/sjis): "<<str);
#endif
        ret = eucm(0,str[0]);
    }
    else if(len == 2) {
#ifdef KINKAKU_SAFE
        if(!(maskl1 & str[0] & str[1]))
            THROW_ERROR("Expected EUC file but found non-EUC string (specify the proper encoding with -encode utf8/euc/sjis): "<<str);
#endif
        ret = eucm(str[0],str[1]);
    } 
    else
        THROW_ERROR("Expected EUC file but found non-EUC string (specify the proper encoding with -encode utf8/euc/sjis): "<<str);
    return ret;
}

string StringUtilEuc::showChar(KinkakuChar c) {
    if(c < 0x8E) {
        char arr[2] = { c, 0 };
        string ret(arr);
        return ret;
    }
    else {
        char arr[3] = { euc1(c), euc2(c), 0 }; 
        string ret(arr);
        return ret;
    }
}

KinkakuString StringUtilEuc::mapString(const string & str) {
    unsigned pos = 0, len = str.length();
    vector<KinkakuChar> ret;
    while(pos < len) {
        if(!(maskl1 & str[pos]))
            ret.push_back(mapChar(str.substr(pos++, 1)));
        else {
            ret.push_back(mapChar(str.substr(pos,2)));
            pos += 2;
        }
    }
    KinkakuString retstr(ret.size());
    for(unsigned i = 0; i < ret.size(); i++)
        retstr[i] = ret[i];
    return retstr;
}

StringUtil::CharType StringUtilEuc::findType(const string & str) {
    return findType(mapChar(str));
}
StringUtil::CharType StringUtilEuc::findType(KinkakuChar c) {
    unsigned char c1 = euc1(c), c2 = euc2(c);
    if((c2 >= 0x30 && c2 <= 0x39) || (c1 == 0xA3 && c2 >= 0xB0 && c2 <= 0xB9))
        return DIGIT;
    else if((c2 >= 0x41 && c2 <= 0x5A) || (c2 >= 0x61 && c2 <= 0x7A)
        || (c1 == 0xA3 && ((c2 >= 0xC1 && c2 <= 0xDA) || (c2 >= 0xE1 && c2 <= 0xFA)))) {
        return ROMAJI;
    }
    else if(c1 == 0xA4 && c2 >= 0xA1 && c2 <= 0xF3) {
        return HIRAGANA;
    }
    else if((c1 == 0xA5 && c2 >= 0xA1 && c2 <= 0xF6) || 
        (c1 == 0xA1 && c2 == 0xBC) || 
        (c1 == 0x8E) 
        ) {
        return KATAKANA;
    }
    else if(c1 >= 0xB0 && c1 <= 0xF4) {
        return KANJI;
    }
    return OTHER;
}

StringUtil::Encoding StringUtilEuc::getEncoding() { return StringUtil::ENCODING_EUC; } 
const char* StringUtilEuc::getEncodingString() { return "euc"; }

void StringUtilEuc::unserialize(const string & str) {  }
string StringUtilEuc::serialize() const { string ret; return ret; } 


inline KinkakuChar sjism(char a, char b) {
    KinkakuChar ret = a & 0xFF;
    ret = ret << 8;
    ret = ret | (b&0xFF);
    return ret;
}
inline unsigned char sjis1(KinkakuChar a) {
    return (a & 0xFF00) >> 8;
}
inline unsigned char sjis2(KinkakuChar a) {
    return (a & 0xFF);
}

KinkakuChar StringUtilSjis::mapChar(const string & str, bool add) {
    unsigned len = str.length();
    KinkakuChar ret;
    if(len == 1) {
#ifdef KINKAKU_SAFE
        const unsigned char first = (unsigned char)str[0];
        if((first & maskl1) && !(first >= 0xA0 && first <= 0xDF))
            THROW_ERROR("Expected SJIS file but found non-SJIS string (specify the proper encoding with -encode utf8/euc/sjis): "<<str);
#endif
        ret = sjism(0,str[0]);
    }
    else if(len == 2) {
#ifdef KINKAKU_SAFE
        const unsigned char first = (unsigned char)str[0];
        if(!(first & maskl1) || (first >= 0xA0 && first <= 0xDF))
            THROW_ERROR("Expected SJIS file but found non-SJIS string (specify the proper encoding with -encode utf8/euc/sjis): "<<str);
#endif
        ret = sjism(str[0],str[1]);
    } 
    else
        THROW_ERROR("Expected SJIS file but found non-SJIS string (specify the proper encoding with -encode utf8/euc/sjis): "<<str);
    return ret;
}

string StringUtilSjis::showChar(KinkakuChar c) {
    if(c < 0xFF) {
        char arr[2] = { c, 0 };
        string ret(arr);
        return ret;
    }
    else {
        char arr[3] = { sjis1(c), sjis2(c), 0 }; 
        string ret(arr);
        return ret;
    }
}

KinkakuString StringUtilSjis::mapString(const string & str) {
    unsigned pos = 0, len = str.length();
    vector<KinkakuChar> ret;
    while(pos < len) {
        const unsigned char first = (unsigned char)str[pos];
        if(!(first & maskl1) || (first >= 0xA0 && first <= 0xDF))
            ret.push_back(mapChar(str.substr(pos++, 1)));
        else {
            ret.push_back(mapChar(str.substr(pos,2)));
            pos += 2;
        }
    }
    KinkakuString retstr(ret.size());
    for(unsigned i = 0; i < ret.size(); i++)
        retstr[i] = ret[i];
    return retstr;
}

StringUtil::CharType StringUtilSjis::findType(const string & str) {
    return findType(mapChar(str));
}
StringUtil::CharType StringUtilSjis::findType(KinkakuChar c) {
    unsigned char c1 = sjis1(c), c2 = sjis2(c);
    if((c1 == 0 && c2 >= 0x30 && c2 <= 0x39) || (c1 == 0x82 && c2 >= 0x4F && c2 <= 0x58))
        return DIGIT;
    else if((c1 == 0 && ((c2 >= 0x41 && c2 <= 0x5A) || (c2 >= 0x61 && c2 <= 0x7A)))
        || (c1 == 0x82 && ((c2 >= 0x60 && c2 <= 0x79) || (c2 >= 0x81 && c2 <= 0x9A)))) {
        return ROMAJI;
    }
    else if(c1 == 0x82 && c2 >= 0x9F && c2 <= 0xF1) {
        return HIRAGANA;
    }
    else if((c1 == 0x83 && c2 >= 0x40 && c2 <= 0x96) || 
        (c1 == 0x81 && c2 == 0x5B) || 
        (c1 == 0 && c2 >= 0xA6 && c2 <= 0xDF) 
        ) {
        return KATAKANA;
    }
    else if(
        (c1 >= 0x88 && c1 <= 0x9F) ||
        (c1 >= 0xE0 && c1 <= 0xEA)
    ) {
        return KANJI;
    }
    return OTHER;
}

KinkakuString StringUtil::normalize(const KinkakuString & str) {
    KinkakuString ret(str.length());
    GenericMap<KinkakuChar,KinkakuChar> * normMap = getNormMap();
    for(int i = 0; i < (int)str.length(); i++) {
        GenericMap<KinkakuChar,KinkakuChar>::const_iterator it = normMap->find(str[i]);
        ret[i] = (it == normMap->end()) ? str[i] : it->second;
    }
    return ret;
}

StringUtil::Encoding StringUtilSjis::getEncoding() { return StringUtil::ENCODING_SJIS; } 
const char* StringUtilSjis::getEncodingString() { return "sjis"; }

void StringUtilSjis::unserialize(const string & str) {  }
string StringUtilSjis::serialize() const { string ret; return ret; } 