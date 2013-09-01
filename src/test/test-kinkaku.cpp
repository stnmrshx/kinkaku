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
#include <kinkaku/feature-lookup.h>
#include <kinkaku/kinkaku-model.h>
#include <kinkaku/corpus-io.h>
#include <kinkaku/corpus-io-part.h>
#include <kinkaku/corpus-io-full.h>
#include <kinkaku/corpus-io-tokenized.h>
#include <kinkaku/corpus-io-raw.h>
#include <kinkaku/model-io.h>
#include <kinkaku/kinkaku-util.h>
#include <fstream>
#include <iostream>
#include <kinkaku/kinkaku-config.h>
#include <kinkaku/kinkaku.h>
#include "test-kinkaku.h"
#include "test-analysis.h"
#include "test-corpusio.h"
#include "test-corpusio-euc.h"
#include "test-corpusio-sjis.h"
#include "test-sentence.h"

using namespace std;

int main(int argv, char **argc) {
    kinkaku::KinkakuTest test_kinkaku;
    kinkaku::TestAnalysis test_analysis;
    kinkaku::TestCorpusIO test_corpusio;
    kinkaku::TestCorpusIOEuc test_corpusio_euc;
    kinkaku::TestCorpusIOSjis test_corpusio_sjis;
    kinkaku::TestSentence test_sentence;
    if(!(
        test_kinkaku.runTest() &&
        test_analysis.runTest() &&
        test_sentence.runTest() &&
        test_corpusio.runTest() &&
        test_corpusio_euc.runTest() &&
        test_corpusio_sjis.runTest()
        )) {
        cout << "**** FAILED!!! ****" << endl;
    } else {
        cout << "**** passed ****" << endl;
    }
}