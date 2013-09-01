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
#ifndef TEST_CORPUSIO_SJIS__
#define TEST_CORPUSIO_SJIS__

#include <kinkaku/corpus-io.h>
#include "test-base.h"

namespace kinkaku {

class TestCorpusIOSjis : public TestBase {

private:

    StringUtilSjis * util;

public:

    TestCorpusIOSjis() {
        util = new StringUtilSjis;
    }

    ~TestCorpusIOSjis() {
        delete util;    
    }

    int testWordSegConf() {
        stringstream instr;
        instr << "\x82\xb1\x7c\x82\xea\x2d\x82\xcd\x20\x83\x66\x20\x81\x5b\x20\x83\x5e\x20\x82\xc5\x2d\x82\xb7\x20\x81\x42" << endl;
        PartCorpusIO io(util, instr, false);
        KinkakuSentence * sent = io.readSentence();
        vector<double> exp(8,0.0);
        exp[0] = 100; exp[1] = -100; exp[6] = -100;
        bool ret = checkVector(exp, sent->wsConfs); 
        delete sent;
        return ret;
    }

    int testPartEmptyLines() {
        stringstream instr;
        instr << "" << endl;
        PartCorpusIO io(util, instr, false);
        KinkakuSentence * sent = io.readSentence();
        vector<double> exp(0,0.0);
        bool ret = checkVector(exp, sent->wsConfs); 
        delete sent;
        return ret;
    }

    int testPartEmptyTag() {
        stringstream instr;
        instr << "\x82\xb1\x2d\x82\xea\x2f\x2f\x82\xb1\x82\xea" << endl;
        PartCorpusIO io(util, instr, false);
        KinkakuSentence * sent = io.readSentence();
        int ret = 1;
        if(sent->words.size() != 1) {
            cerr << "Sentence size " << sent->words.size() << " != 1" << endl;
            ret = 0;
        }
        delete sent;
        return ret;
    }

    int testFullTagConf() {
        stringstream instr;
        instr << "\x82\xb1\x2d\x82\xea\x2f\x96\xbc\x8e\x8c\x20\x82\xcd\x2f\x8f\x95\x8e\x8c\x20\x83\x66\x81\x5b\x83\x5e\x2f\x96\xbc\x8e\x8c\x20\x82\xc5\x2f\x8f\x95\x93\xae\x8e\x8c\x20\x82\xb7\x2f\x8c\xea\x94\xf6\x20\x81\x42\x2f\x95\xe2\x8f\x95\x8b\x4c\x8d\x86" << endl;
        FullCorpusIO io(util, instr, false);
        KinkakuSentence * sent = io.readSentence();
        if(sent->words.size() != 6)
            THROW_ERROR("sent->words size doesn't match 5 " << sent->words.size());
        bool ret = true;
        for(int i = 0; i < 6; i ++) {
            if(sent->words[i].tags[0][0].second != 100.0) {
                cerr << "Bad confidence for tag " << i << ": " << sent->words[i].tags[0][0].second << endl;
                ret = false;
            }
        }
        delete sent;
        return ret;
    }

    int testLastValue() {
        string confident_text = "\x82\xb1\x82\xea\x2f\x91\xe3\x96\xbc\x8e\x8c\x2f\x82\xb1\x82\xea\x20\x82\xcd\x2f\x8f\x95\x8e\x8c\x2f\x82\xcd\x20\x90\x4d\x97\x8a\x2f\x96\xbc\x8e\x8c\x2f\x82\xb5\x82\xf1\x82\xe7\x82\xa2\x20\x93\x78\x2f\x90\xda\x94\xf6\x8e\xab\x2f\x82\xc7\x20\x82\xcc\x2f\x8f\x95\x8e\x8c\x2f\x82\xcc\x20\x8d\x82\x2f\x8c\x60\x97\x65\x8e\x8c\x2f\x82\xbd\x82\xa9\x20\x82\xa2\x2f\x8c\xea\x94\xf6\x2f\x82\xa2\x20\x93\xfc\x97\xcd\x2f\x96\xbc\x8e\x8c\x2f\x82\xc9\x82\xe3\x82\xa4\x82\xe8\x82\xe5\x82\xad\x20\x82\xc5\x2f\x8f\x95\x93\xae\x8e\x8c\x2f\x82\xc5\x20\x82\xb7\x2f\x8c\xea\x94\xf6\x2f\x82\xb7\x20\x81\x42\x2f\x95\xe2\x8f\x95\x8b\x4c\x8d\x86\x2f\x81\x42\n";
        stringstream instr;
        instr << confident_text;
        FullCorpusIO infcio(util, instr, false);
        KinkakuSentence * sent = infcio.readSentence();
        int ret = 1;
        if(sent->words.size() != 11) {
            cerr << "Did not get expected sentence size of 11: " << sent->words.size() << endl;
            ret = 0;
        } else if(sent->words[10].tags.size() != 2) {
            cerr << "Did not get two levels of tags for final word: " << sent->words[10].tags.size() << endl;
            ret = 0;
        }
        delete sent;
        return ret;
    }
    
    int testUnkIO() {
        string input = "\x82\xb1\x82\xea\x2f\x91\xe3\x96\xbc\x8e\x8c\x2f\x82\xb1\x82\xea\x20\x82\xcd\x2f\x8f\x95\x8e\x8c\x2f\x82\xcd\x20\x96\xa2\x92\x6d\x2f\x96\xbc\x8e\x8c\x2f\x82\xdd\x82\xbf\n";
        stringstream instr;
        instr << input;
        FullCorpusIO infcio(util, instr, false);
        KinkakuSentence * sent = infcio.readSentence();
        sent->words[2].setUnknown(true);
        string exp = "\x82\xb1\x82\xea\x2f\x91\xe3\x96\xbc\x8e\x8c\x2f\x82\xb1\x82\xea\x20\x82\xcd\x2f\x8f\x95\x8e\x8c\x2f\x82\xcd\x20\x96\xa2\x92\x6d\x2f\x96\xbc\x8e\x8c\x2f\x82\xdd\x82\xbf\x2f\x55\x4e\x4b\n";
        stringstream outstr;
        FullCorpusIO outfcio(util, outstr, true);
        outfcio.setUnkTag("/UNK");
        outfcio.writeSentence(sent);
        string act = outstr.str();
        if(exp != act) {
            cerr << "exp: "<<exp<<endl<<"act: "<<act<<endl;
            return 0;
        }
        return 1;
    }

    bool runTest() {
        int done = 0, succeeded = 0;
        done++; cout << "testWordSegConf()" << endl; if(testWordSegConf()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testPartEmptyLines()" << endl; if(testPartEmptyLines()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testPartEmptyTag()" << endl; if(testPartEmptyTag()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testFullTagConf()" << endl; if(testFullTagConf()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testLastValue()" << endl; if(testLastValue()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testUnkIO()" << endl; if(testUnkIO()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestCorpusIOSjis Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

};

}

#endif