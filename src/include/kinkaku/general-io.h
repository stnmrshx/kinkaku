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
#ifndef GENERAL_IO_H__ 
#define GENERAL_IO_H__ 

#include <iostream>
#include <cstddef>

#if DISABLE_QUANTIZE
#	define DECIMAL_PRECISION 8
#else
#	define DECIMAL_PRECISION 6
#endif

namespace kinkaku {

class StringUtil;
class KinkakuString;

class GeneralIO {

protected:

	StringUtil* util_;
	std::iostream* str_;
	bool out_;
	bool bin_;
	bool owns_;

	template <class T>
	
	void writeBinary(T v) {
		str_->write(reinterpret_cast<char *>(&v), sizeof(T));
	}

	void writeString(const char* str, size_t size) {
		str_->write(str, size+1);
	} 
    
    void writeString(const std::string & str) {
    	str_->write(str.c_str(), str.length()+1);
    }

    void writeString(const KinkakuString & str);

    template <class T>
    T readBinary();

    std::string readString();
    KinkakuString readKinkakuString();

public:

	GeneralIO(StringUtil* util) : 
		util_(util), str_(0), out_(true), bin_(false), owns_(false) { }

	GeneralIO(StringUtil* util, std::iostream & str, bool out, bool bin) : 
		util_(util), str_(&str), out_(out), bin_(false), owns_(false) 
			{ setStream(str, out, bin); }

	GeneralIO(StringUtil* util, const char* file, bool out, bool bin) : 
		util_(util), str_(0), bin_(false), owns_(true) 
			{ openFile(file,out,bin); }

	~GeneralIO() {
		if(str_ && owns_)
			delete str_;
	}

	void openFile(const char* file, bool out, bool bin);
	void setStream(std::iostream & str, bool out, bool bin);

};

}

#endif