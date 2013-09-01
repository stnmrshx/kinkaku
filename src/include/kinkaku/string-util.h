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
#ifndef STRING_UTIL_H__
#define STRING_UTIL_H__

#include <kinkaku/kinkaku-struct.h>
#include <sstream>

namespace kinkaku {

class StringUtil {

public:

    typedef char CharType;
    const static CharType KANJI    = 'K';
    const static CharType KATAKANA = 'T';
    const static CharType HIRAGANA = 'H';
    const static CharType ROMAJI   = 'R';
    const static CharType DIGIT    = 'D';
    const static CharType OTHER    = 'O';

    typedef char Encoding;
    const static Encoding ENCODING_UTF8    = 'W';
    const static Encoding ENCODING_EUC     = 'E';
    const static Encoding ENCODING_SJIS    = 'S';

    GenericMap<KinkakuChar,KinkakuChar> * normMap_;

public:

    StringUtil() : normMap_(NULL) { }

    virtual ~StringUtil() {
        if(normMap_) delete normMap_;    
    }

    virtual KinkakuChar mapChar(const std::string & str, bool add = true) = 0;
    virtual std::string showChar(KinkakuChar c) = 0;

    std::string showString(const KinkakuString & c) {
        std::ostringstream buff;
        for(unsigned i = 0; i < c.length(); i++)
            buff << showChar(c[i]);
        return buff.str();
    }

    virtual KinkakuString mapString(const std::string & str) = 0;

    virtual CharType findType(const std::string & str) = 0;
    virtual CharType findType(KinkakuChar c) = 0;

    virtual Encoding getEncoding() = 0;
    virtual const char* getEncodingString() = 0;
    
    virtual void unserialize(const std::string & str) = 0;
    virtual std::string serialize() const = 0;
    
    virtual GenericMap<KinkakuChar,KinkakuChar> * getNormMap() = 0;
    KinkakuString normalize(const KinkakuString & str);

    void checkEqual(const StringUtil & rhs) const;

    int parseInt(const char* str);
    double parseFloat(const char* str);

    std::string getTypeString(const KinkakuString& str) {
        std::ostringstream buff;
        for(unsigned i = 0; i < str.length(); i++)
            buff << findType(str[i]);
        return buff.str();
    }


};

class StringUtilUtf8 : public StringUtil {

private:
    
    const static char maskr6 = 63, maskr5 = 31, maskr4 = 15, maskr3 = 7, maskl1 = 1 << 7, maskl2 = 3 << 6, maskl3 = 7 << 5, maskl4 = 15 << 4, maskl5 = 31 << 3;

    StringCharMap charIds_;
    std::vector<std::string> charNames_;
    std::vector<CharType> charTypes_;

public:

    StringUtilUtf8();

    ~StringUtilUtf8() { }
    
    KinkakuChar mapChar(const std::string & str, bool add = true);
    std::string showChar(KinkakuChar c);

    CharType findType(KinkakuChar c);

    GenericMap<KinkakuChar,KinkakuChar> * getNormMap();

    bool badu(char val) { return ((val ^ maskl1) & maskl2); }
    KinkakuString mapString(const std::string & str);

    CharType findType(const std::string & str);

    Encoding getEncoding() { return ENCODING_UTF8; }
    const char* getEncodingString() { return "utf8"; }

    const std::vector<std::string> & getCharNames() { return charNames_; }

    void unserialize(const std::string & str);
    std::string serialize() const;

};

class StringUtilEuc : public StringUtil {

const static char maskl1 = 1 << 7;
const static KinkakuChar mask3len = 1 << 14;
    

public:
    StringUtilEuc() { };
    ~StringUtilEuc() { }

    KinkakuChar mapChar(const std::string & str, bool add = true);
    std::string showChar(KinkakuChar c);
    
    GenericMap<KinkakuChar,KinkakuChar> * getNormMap();

    KinkakuString mapString(const std::string & str);

    CharType findType(const std::string & str);
    CharType findType(KinkakuChar c);

    Encoding getEncoding();
    const char* getEncodingString();
    
    void unserialize(const std::string & str);
    std::string serialize() const;

};

class StringUtilSjis : public StringUtil {

const static char maskl1 = 1 << 7;
const static KinkakuChar mask3len = 1 << 14;
    

public:
    StringUtilSjis() { };
    ~StringUtilSjis() { }

    KinkakuChar mapChar(const std::string & str, bool add = true);
    GenericMap<KinkakuChar,KinkakuChar> * getNormMap();

    std::string showChar(KinkakuChar c);
    
    KinkakuString mapString(const std::string & str);

    CharType findType(const std::string & str);
    CharType findType(KinkakuChar c);

    Encoding getEncoding();
    const char* getEncodingString();
    
    void unserialize(const std::string & str);
    std::string serialize() const;

};

}

#endif