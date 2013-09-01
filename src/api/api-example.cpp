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
#include <iostream>
#include <kinkaku/kinkaku.h>
#include <kinkaku/kinkaku-struct.h>
#include <kinkaku/string-util.h>

using namespace std;
using namespace kinkaku;

int main(int argc, char** argv) {
    Kinkaku kinkaku;    
    kinkaku.readModel("../../data/model.bin");
    StringUtil* util = kinkaku.getStringUtil(); 
    KinkakuConfig* config = kinkaku.getConfig();
    KinkakuString surface_string = util->mapString("これはテストです。");
    KinkakuSentence sentence(surface_string, util->normalize(surface_string));
    kinkaku.calculateWS(sentence);
    for(int i = 0; i < config->getNumTags(); i++)
        kinkaku.calculateTags(sentence,i);

    const KinkakuSentence::Words & words =  sentence.words;
    for(int i = 0; i < (int)words.size(); i++) {
        cout << util->showString(words[i].surface);
        for(int j = 0; j < (int)words[i].tags.size(); j++) {
            cout << "\t";
            for(int k = 0; k < (int)words[i].tags[j].size(); k++) {
                cout << " " << util->showString(words[i].tags[j][k].first) << 
                        "/" << words[i].tags[j][k].second;
            }
        }
        cout << endl;
    }
    cout << endl;
}