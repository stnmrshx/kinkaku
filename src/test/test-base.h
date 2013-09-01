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
#ifndef TEST_BASE__
#define TEST_BASE__

namespace kinkaku {

class TestBase {

protected:

    TestBase() : passed_(false) { }

    bool passed_;

    int checkWordSeg(const KinkakuSentence & sent, const vector<KinkakuString> & toks, StringUtil * util) {
        const KinkakuSentence::Words & words =  sent.words;
        int ok = 1;
        for(int i = 0; i < (int)max(words.size(), toks.size()); i++) {
            if(i >= (int)words.size() || i >= (int)toks.size() || words[i].surface != toks[i]) {
                ok = 0;
                cout << "words["<<i<<"] != toks["<<i<<"] ("<<
                    (i >= (int)words.size() ? "NULL" : util->showString(words[i].surface))
                    <<" != "<<
                    (i >= (int)toks.size() ? "NULL" : util->showString(toks[i]))
                    <<")"<<endl;
            }
        }
        return ok;
    }
    
    int checkTags(const KinkakuSentence & sent, const vector<KinkakuString> & toks, int pos, StringUtil * util) {
        const KinkakuSentence::Words & words =  sent.words;
        int ok = (words.size() == toks.size() ? 1 : 0);
        KinkakuString noneString = util->mapString("NONE");
        for(int i = 0; i < (int)max(words.size(), toks.size()); i++) {
            KinkakuString myTag;
            if(i >= (int)words.size())
                myTag = util->mapString("NULL");
            else if(pos >= (int)words[i].tags.size() || 0 == (int)words[i].tags[pos].size())
                myTag = util->mapString("NONE");
            else
                myTag = words[i].tags[pos][0].first;
            if(i >= (int)toks.size() || myTag != toks[i]) {
                ok = 0;
                cout << "words["<<i<<"] != toks["<<i<<"] ("<<
                    util->showString(myTag)
                    <<" != "<<
                    (i >= (int)toks.size() ? "NULL" : util->showString(toks[i]))
                    <<")"<<endl;
            }
        }
        return ok;
    }


    template<class T>
    int checkVector(const vector<T> & exp, const vector<T> & act) {
        int ok = 1;
        for(int i = 0; i < (int)max(exp.size(), act.size()); i++) {
            if(i >= (int)exp.size() || i >= (int)act.size() || exp[i] != act[i]) {
                ok = 0;
                cout << "exp["<<i<<"] != act["<<i<<"] (";
                if(i >= (int)exp.size()) cout << "NULL"; else cout << exp[i];
                cout <<" != ";
                if(i >= (int)act.size()) cout << "NULL"; else cout << act[i];
                cout << ")" << endl;
            }
        }
        return ok;
    }

};

}

#endif