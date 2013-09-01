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
#include <kinkaku/general-io.h>
#include <kinkaku/kinkaku-util.h>
#include <kinkaku/kinkaku-string.h>
#include <fstream>
#include <stdint.h>

using namespace std;
using namespace kinkaku;

void GeneralIO::openFile(const char* file, bool out, bool bin) {
    fstream::openmode mode = (out?fstream::out:fstream::in);
    if(bin) mode = mode | fstream::binary;
    fstream * str = new fstream(file, mode);
    if(str->fail()) 
        THROW_ERROR("Couldn't open file '"<<file<<"' for "<<(out?"output":"input"));
    setStream(*str, out, bin);
    owns_ = true;
}

void GeneralIO::setStream(iostream & str, bool out, bool bin) {
    if(str_ && owns_)
        delete str_;
    str_ = &str;
    str_->precision(DECIMAL_PRECISION);
    bin_ = bin;
    out_ = out;
    owns_ = false;
}

void GeneralIO::writeString(const KinkakuString & str) {
    writeBinary((uint32_t)str.length());
    for(unsigned i = 0; i < str.length(); i++)
        writeBinary(str[i]);
}

template <class T>
T GeneralIO::readBinary() {
    T v;
    str_->read(reinterpret_cast<char *>(&v),sizeof(T));
    return v;
} 

template bool GeneralIO::readBinary<bool>();
template char GeneralIO::readBinary<char>();
template short GeneralIO::readBinary<short>();
template int GeneralIO::readBinary<int>();
template double GeneralIO::readBinary<double>();
template unsigned short GeneralIO::readBinary<unsigned short>();
template unsigned int GeneralIO::readBinary<unsigned int>();
template unsigned char GeneralIO::readBinary<unsigned char>();

std::string GeneralIO::readString() {
    std::string str;
    getline(*str_, str, (char)0);
    return str;
}
KinkakuString GeneralIO::readKinkakuString() {
    KinkakuString ret(readBinary<uint32_t>());
    for(unsigned i = 0; i < ret.length(); i++)
        ret[i] = readBinary<KinkakuChar>();
    return ret;
}