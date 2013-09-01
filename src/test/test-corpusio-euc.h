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
#ifndef TEST_CORPUSIO_EUC__
#define TEST_CORPUSIO_EUC__

#include <kinkaku/corpus-io.h>
#include "test-base.h"

namespace kinkaku {

class TestCorpusIOEuc : public TestBase {

private:

    StringUtilEuc * util;

public:

    TestCorpusIOEuc() {
        util = new StringUtilEuc;
    }

    ~TestCorpusIOEuc() {
        delete util;    
    }

    int testWordSegConf() {
        stringstream instr;
        instr << "\xa4\xb3\x7c\xa4\xec\x2d\xa4\xcf\x20\xa5\xc7\x20\xa1\xbc\x20\xa5\xbf\x20\xa4\xc7\x2d\xa4\xb9\x20\xa1\xa3" << endl;
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
        instr << "\xa4\xb3\x2d\xa4\xec\x2f\x2f\xa4\xb3\xa4\xec" << endl;
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
        instr << "\xa4\xb3\x2d\xa4\xec\x2f\xcc\xbe\xbb\xec\x20\xa4\xcf\x2f\xbd\xf5\xbb\xec\x20\xa5\xc7\xa1\xbc\xa5\xbf\x2f\xcc\xbe\xbb\xec\x20\xa4\xc7\x2f\xbd\xf5\xc6\xb0\xbb\xec\x20\xa4\xb9\x2f\xb8\xec\xc8\xf8\x20\xa1\xa3\x2f\xca\xe4\xbd\xf5\xb5\xad\xb9\xe6" << endl;
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
        string confident_text = "\xa4\xb3\xa4\xec\x2f\xc2\xe5\xcc\xbe\xbb\xec\x2f\xa4\xb3\xa4\xec\x20\xa4\xcf\x2f\xbd\xf5\xbb\xec\x2f\xa4\xcf\x20\xbf\xae\xcd\xea\x2f\xcc\xbe\xbb\xec\x2f\xa4\xb7\xa4\xf3\xa4\xe9\xa4\xa4\x20\xc5\xd9\x2f\xc0\xdc\xc8\xf8\xbc\xad\x2f\xa4\xc9\x20\xa4\xce\x2f\xbd\xf5\xbb\xec\x2f\xa4\xce\x20\xb9\xe2\x2f\xb7\xc1\xcd\xc6\xbb\xec\x2f\xa4\xbf\xa4\xab\x20\xa4\xa4\x2f\xb8\xec\xc8\xf8\x2f\xa4\xa4\x20\xc6\xfe\xce\xcf\x2f\xcc\xbe\xbb\xec\x2f\xa4\xcb\xa4\xe5\xa4\xa6\xa4\xea\xa4\xe7\xa4\xaf\x20\xa4\xc7\x2f\xbd\xf5\xc6\xb0\xbb\xec\x2f\xa4\xc7\x20\xa4\xb9\x2f\xb8\xec\xc8\xf8\x2f\xa4\xb9\x20\xa1\xa3\x2f\xca\xe4\xbd\xf5\xb5\xad\xb9\xe6\x2f\xa1\xa3\n";
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
        string input = "\xa4\xb3\xa4\xec\x2f\xc2\xe5\xcc\xbe\xbb\xec\x2f\xa4\xb3\xa4\xec\x20\xa4\xcf\x2f\xbd\xf5\xbb\xec\x2f\xa4\xcf\x20\xcc\xa4\xc3\xce\x2f\xcc\xbe\xbb\xec\x2f\xa4\xdf\xa4\xc1\n";
        stringstream instr;
        instr << input;
        FullCorpusIO infcio(util, instr, false);
        KinkakuSentence * sent = infcio.readSentence();
        sent->words[2].setUnknown(true);
        string exp = "\xa4\xb3\xa4\xec\x2f\xc2\xe5\xcc\xbe\xbb\xec\x2f\xa4\xb3\xa4\xec\x20\xa4\xcf\x2f\xbd\xf5\xbb\xec\x2f\xa4\xcf\x20\xcc\xa4\xc3\xce\x2f\xcc\xbe\xbb\xec\x2f\xa4\xdf\xa4\xc1\x2f\x55\x4e\x4b\n";
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
        cout << "#### TestCorpusIOEuc Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

};

}

#endif