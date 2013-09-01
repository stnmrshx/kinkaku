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
#ifndef TEST_ANALYSIS__
#define TEST_ANALYSIS__

#include <cmath>
#include "test-base.h"

namespace kinkaku {

class TestAnalysis : public TestBase {

private:

    Kinkaku *kinkaku, *kinkakuLogist, *kinkakuMCSVM, *kinkakuNoWS;
    StringUtil *util, *utilLogist, *utilMCSVM, *utilNoWS;

public:

    TestAnalysis() {
        const char* toy_text = 
"これ/代名詞/これ は/助詞/は 学習/名詞/がくしゅう データ/名詞/でーた で/助動詞/で す/語尾/す 。/補助記号/。\n"
"大変/形状詞/でーた で/助動詞/で す/語尾/す 。/補助記号/。\n"
"\n"
"どうぞ/副詞/どうぞ モデル/名詞/もでる を/助詞/を K i n k a k u/名詞/きゅーてぃー で/助詞/で 学習/名詞/がくしゅう し/動詞/し て/助詞/て くださ/動詞/くださ い/語尾/い ！/補助記号/！\n"
"処理/名詞/しょり を/助詞/を 行/動詞/おこな っ/語尾/っ た/助動詞/た ．/補助記号/。\n"
"京都/名詞/きょうと に/助詞/に 行/動詞/い っ/語尾/っ た/助動詞/た ．/補助記号/。\n";
        ofstream ofs("/tmp/kinkaku-toy-corpus.txt"); 
        ofs << toy_text; ofs.close();
        const char* toyCmd[7] = {"", "-model", "/tmp/kinkaku-svm-model.bin", "-full", "/tmp/kinkaku-toy-corpus.txt", "-global", "1"};
        KinkakuConfig * config = new KinkakuConfig;
        config->setDebug(0);
        config->setOnTraining(true);
        config->parseTrainCommandLine(7, toyCmd);
        kinkaku = new Kinkaku(config);
        kinkaku->trainAll();
        util = kinkaku->getStringUtil();
        config->setOnTraining(false);
        const char* toyCmdLogist[9] = {"", "-model", "/tmp/kinkaku-logist-model.bin", "-full", "/tmp/kinkaku-toy-corpus.txt", "-global", "1", "-solver", "0"};
        KinkakuConfig * configLogist = new KinkakuConfig;
        configLogist->setDebug(0);
        configLogist->setTagMax(0);
        configLogist->setOnTraining(true);
        configLogist->parseTrainCommandLine(9, toyCmdLogist);
        kinkakuLogist = new Kinkaku(configLogist);
        kinkakuLogist->trainAll();
        utilLogist = kinkakuLogist->getStringUtil();
        configLogist->setOnTraining(false);
        const char* toyCmdMCSVM[9] = {"", "-model", "/tmp/kinkaku-logist-model.bin", "-full", "/tmp/kinkaku-toy-corpus.txt", "-global", "1", "-solver", "4"};
        KinkakuConfig * configMCSVM = new KinkakuConfig;
        configMCSVM->setDebug(0);
        configMCSVM->setTagMax(0);
        configMCSVM->setOnTraining(true);
        configMCSVM->parseTrainCommandLine(9, toyCmdMCSVM);
        kinkakuMCSVM = new Kinkaku(configMCSVM);
        kinkakuMCSVM->trainAll();
        utilMCSVM = kinkakuMCSVM->getStringUtil();
        configMCSVM->setOnTraining(false);
        const char* toyCmdNoWS[8] = {"", "-model", "/tmp/kinkaku-logist-model.bin", "-full", "/tmp/kinkaku-toy-corpus.txt", "-global", "1", "-nows"};
        KinkakuConfig * configNoWS = new KinkakuConfig;
        configNoWS->setDebug(0);
        configNoWS->setTagMax(0);
        configNoWS->setOnTraining(true);
        configNoWS->parseTrainCommandLine(8, toyCmdNoWS);
        kinkakuNoWS = new Kinkaku(configNoWS);
        kinkakuNoWS->trainAll();
        utilNoWS = kinkakuNoWS->getStringUtil();
        configNoWS->setOnTraining(false);
    }

    ~TestAnalysis() {
        delete kinkaku;
        delete kinkakuLogist;
        delete kinkakuMCSVM;
        delete kinkakuNoWS;
    }

    int testWordSegmentationEmpty() {
        KinkakuString str = util->mapString("");
        KinkakuSentence sentence(str, util->normalize(str));
        kinkaku->calculateWS(sentence);
        KinkakuString::Tokens toks = util->mapString("").tokenize(util->mapString(" "));
        return checkWordSeg(sentence,toks,util);
    }

    int testWordSegmentationUnk() {
        KinkakuString str = util->mapString("これは学習デエタです。");
        KinkakuSentence sentence(str, util->normalize(str));
        kinkaku->calculateWS(sentence);
        KinkakuString::Tokens toks = util->mapString("これ は 学習 デエタ で す 。").tokenize(util->mapString(" "));
        if(!checkWordSeg(sentence,toks,util)) { return 0; }
        vector<bool> unk_exp(6, false), unk_act(6);
        unk_exp[3] = true;
        for(int i = 0; i < 6; i++)
            unk_act[i] = sentence.words[i].getUnknown();
        return checkVector(unk_exp, unk_act);
    }
    
    int testNormalizationUnk() {
        KinkakuString str = util->mapString("これはKinkakuです.");
        KinkakuSentence sentence(str, util->normalize(str));
        kinkaku->calculateWS(sentence);
        KinkakuString::Tokens toks = util->mapString("これ は Kinkaku で す .").tokenize(util->mapString(" "));
        if(!checkWordSeg(sentence,toks,util)) { return 0; }
        vector<bool> unk_exp(6, false), unk_act(6);
        for(int i = 0; i < 6; i++)
            unk_act[i] = sentence.words[i].getUnknown();
        return checkVector(unk_exp, unk_act);
    }

    int testWordSegmentationSVM() {
        KinkakuString str = util->mapString("これは学習データです。");
        KinkakuSentence sentence(str, util->normalize(str));
        kinkaku->calculateWS(sentence);
        KinkakuString::Tokens toks = util->mapString("これ は 学習 データ で す 。").tokenize(util->mapString(" "));
        return checkWordSeg(sentence,toks,util);
    }

    int testWordSegmentationMCSVM() {
        KinkakuString str = utilMCSVM->mapString("これは学習データです。");
        KinkakuSentence sentence(str, utilMCSVM->normalize(str));
        kinkakuMCSVM->calculateWS(sentence);
        KinkakuString::Tokens toks = utilMCSVM->mapString("これ は 学習 データ で す 。").tokenize(utilMCSVM->mapString(" "));
        return checkWordSeg(sentence,toks,utilMCSVM);
    }

    int testWordSegmentationLogistic() {
        KinkakuString str = utilLogist->mapString("これは学習データです。");
        KinkakuSentence sentence(str, utilLogist->normalize(str));
        kinkakuLogist->calculateWS(sentence);
        KinkakuString::Tokens toks = utilLogist->mapString("これ は 学習 データ で す 。").tokenize(utilLogist->mapString(" "));
        int correct = checkWordSeg(sentence,toks,utilLogist);
        if(correct) {
            for(int i = 0; i < (int)sentence.wsConfs.size(); i++) {
                if(sentence.wsConfs[i] < 0.0 || sentence.wsConfs[i] > 1.0) {
                    cerr << "Confidience for logistic WS "<<i<<" is not probability: " << sentence.wsConfs[i] << endl;
                    correct = 0;
                }
            }
        }
        return correct;
    }

    int testGlobalTaggingSVM() {
        KinkakuString str = util->mapString("これは学習データです。");
        KinkakuSentence sentence(str, util->normalize(str));
        kinkaku->calculateWS(sentence);
        kinkaku->calculateTags(sentence,0);
        KinkakuString::Tokens toks = util->mapString("代名詞 助詞 名詞 名詞 助動詞 語尾 補助記号").tokenize(util->mapString(" "));
        int correct = checkTags(sentence,toks,0,util);
        if(correct) {
            for(int i = 0; i < (int)sentence.words.size(); i++) {
                if(sentence.words[i].tags[0][1].second != 0.0) {
                    cerr << "Margin on word "<<i<<" is not 0.0 (== "<<sentence.words[i].tags[0][1].second<<")"<<endl;
                    correct = false;
                }
            }
        }
        return correct;
    }

    int testGlobalTaggingMCSVM() {
        KinkakuString str = utilMCSVM->mapString("これは学習データです。");
        KinkakuSentence sentence(str, utilMCSVM->normalize(str));
        kinkakuMCSVM->calculateWS(sentence);
        kinkakuMCSVM->calculateTags(sentence,0);
        KinkakuString::Tokens toks = utilMCSVM->mapString("代名詞 助詞 名詞 名詞 助動詞 語尾 補助記号").tokenize(utilMCSVM->mapString(" "));
        int correct = checkTags(sentence,toks,0,utilMCSVM);
        if(correct) {
            for(int i = 0; i < (int)sentence.words.size(); i++) {
                if(sentence.words[i].tags[0][1].second != 0.0) {
                    cerr << "Margin on word "<<i<<" is not 0.0 (== "<<sentence.words[i].tags[0][1].second<<")"<<endl;
                    correct = false;
                }
            }
        }
        return correct;
    }

    int testGlobalTaggingNoWS() {
        KinkakuString str = utilNoWS->mapString("これは学習データです。");
        KinkakuSentence sentence(str, utilNoWS->normalize(str));
        sentence.wsConfs[0] = -1; sentence.wsConfs[1] = 1; sentence.wsConfs[2] = 1;
        sentence.wsConfs[3] = -1; sentence.wsConfs[4] = 1; sentence.wsConfs[5] = -1;
        sentence.wsConfs[6] = -1; sentence.wsConfs[7] = 1; sentence.wsConfs[8] = 1; sentence.wsConfs[9] = 1;
        sentence.refreshWS(0);
        kinkakuNoWS->calculateTags(sentence,0);
        KinkakuString::Tokens toks = utilNoWS->mapString("代名詞 助詞 名詞 名詞 助動詞 語尾 補助記号").tokenize(utilNoWS->mapString(" "));
        int correct = checkTags(sentence,toks,0,utilNoWS);
        if(correct) {
            for(int i = 0; i < (int)sentence.words.size(); i++) {
                if(sentence.words[i].tags[0][1].second != 0.0) {
                    cerr << "Margin on word "<<i<<" is not 0.0 (== "<<sentence.words[i].tags[0][1].second<<")"<<endl;
                    correct = false;
                }
            }
        }
        return correct;
    }

    int testNoWSUnk() {
        KinkakuString str = utilNoWS->mapString("これは学習デエタです。");
        KinkakuSentence sentence(str, utilNoWS->normalize(str));
        sentence.wsConfs[0] = -1; sentence.wsConfs[1] = 1;
        sentence.wsConfs[2] = 1; sentence.wsConfs[3] = -1; 
        sentence.wsConfs[4] = 1; sentence.wsConfs[5] = -1;
        sentence.wsConfs[6] = -1; sentence.wsConfs[7] = 1;
        sentence.wsConfs[8] = 1; sentence.wsConfs[9] = 1;
        sentence.refreshWS(0);
        for(int i = 0; i < 7; i++)
            sentence.words[i].setUnknown(false);
        kinkakuNoWS->calculateTags(sentence,0);
        vector<bool> unk_exp(7, false), unk_act(7);
        unk_exp[3] = true;
        for(int i = 0; i < 7; i++)
            unk_act[i] = sentence.words[i].getUnknown();
        return checkVector(unk_exp, unk_act);
    }

    int testGlobalTaggingLogistic() {
        KinkakuString str = utilLogist->mapString("これは学習データです。");
        KinkakuSentence sentence(str, utilLogist->normalize(str));
        kinkakuLogist->calculateWS(sentence);
        kinkakuLogist->calculateTags(sentence,0);
        KinkakuString::Tokens toks = utilLogist->mapString("代名詞 助詞 名詞 名詞 助動詞 語尾 補助記号").tokenize(util->mapString(" "));
        int correct = checkTags(sentence,toks,0,utilLogist);
        if(correct) {
            for(int i = 0; i < (int)sentence.words.size(); i++) {
                double sum = 0.0;
                for(int j = 0; j < (int)sentence.words[i].tags[0].size(); j++)
                    sum += sentence.words[i].tags[0][j].second;
                if(fabs(1.0-sum) > 0.01) {
                    cerr << "Probability on word "<<i<<" is not close to 1 (== "<<sum<<")"<<endl;
                    correct = false;
                }
            }
        }
        return correct;
    }

    int testGlobalSelf() {
        KinkakuString::Tokens words = util->mapString("これ 京都 学習 データ どうぞ 。").tokenize(util->mapString(" "));
        KinkakuString::Tokens tags = util->mapString("代名詞 名詞 名詞 名詞 副詞 補助記号").tokenize(util->mapString(" "));
        KinkakuString::Tokens singleTag(1);
        if(words.size() != tags.size()) THROW_ERROR("words.size() != tags.size() in testGlobalSelf");
        int ok = 1;
        for(int i = 0; i < (int)words.size(); i++) {
            KinkakuSentence sent(words[i], util->normalize(words[i]));
            sent.refreshWS(1);
            if(sent.words.size() != 1) THROW_ERROR("Bad segmentation in testGlobalSelf");
            kinkaku->calculateTags(sent,0);
            singleTag[0] = tags[i];
            ok = (checkTags(sent,singleTag,0,util) ? ok : 0);
        }
        return ok;
    }

    int testLocalTagging() {
        KinkakuString str = util->mapString("東京に行った。");
        KinkakuSentence sentence(str, util->normalize(str));
        kinkaku->calculateWS(sentence);
        kinkaku->calculateTags(sentence,1);
        KinkakuString::Tokens toks = util->mapString("UNK に い っ た 。").tokenize(util->mapString(" "));
        return checkTags(sentence,toks,1,util);
    }

    int testPartialSegmentation() {
        stringstream instr;
        instr << "こ|れ-は デ ー タ で-す 。" << endl;
        PartCorpusIO io(util, instr, false);
        KinkakuSentence * sent = io.readSentence();
        kinkaku->calculateWS(*sent);
        KinkakuString::Tokens toks = util->mapString("こ れは データ です 。").tokenize(util->mapString(" "));
        int ok = checkWordSeg(*sent,toks,util);
        delete sent;
        return ok;
    }

    int testConfidentInput() {
        string confident_text = "これ/代名詞/これ は/助詞/は 信頼/名詞/しんらい 度/接尾辞/ど の/助詞/の 高/形容詞/たか い/語尾/い 入力/名詞/にゅうりょく で/助動詞/で す/語尾/す 。/補助記号/。\n";
        stringstream instr;
        instr << confident_text;
        FullCorpusIO infcio(util, instr, false);
        KinkakuSentence * sent = infcio.readSentence();
        kinkaku->calculateWS(*sent);
        stringstream outstr1;
        FullCorpusIO outfcio1(util, outstr1, true);
        outfcio1.writeSentence(sent);
        string actual_text = outstr1.str();
        if(actual_text != confident_text) {
            cout << "WS: actual_text != confident_text"<<endl<<" "<<actual_text<<endl<<" "<<confident_text<<endl;
            return 0;
        }
        kinkaku->calculateTags(*sent,0);
        kinkaku->calculateTags(*sent,1);
        stringstream outstr2;
        FullCorpusIO outfcio2(util, outstr2, true);
        outfcio2.writeSentence(sent);
        actual_text = outstr2.str();
        delete sent;
        if(actual_text != confident_text) {
            cout << "Tag: actual_text != confident_text"<<endl<<" "<<actual_text<<endl<<" "<<confident_text<<endl;
            return 0;
        } else {
            return 1;
        } 
    }

    int testTextIO() {
        kinkaku->getConfig()->setModelFormat(ModelIO::FORMAT_TEXT);
        kinkaku->writeModel("/tmp/kinkaku-model.txt");
        Kinkaku actKinkaku;
        actKinkaku.readModel("/tmp/kinkaku-model.txt");
        kinkaku->checkEqual(actKinkaku);
        return 1;
    }

    int testBinaryIO() {
        kinkaku->getConfig()->setModelFormat(ModelIO::FORMAT_BINARY);
        kinkaku->writeModel("/tmp/kinkaku-model.bin");
        Kinkaku actKinkaku;
        actKinkaku.readModel("/tmp/kinkaku-model.bin");
        kinkaku->checkEqual(actKinkaku);
        return 1;
    }

    bool runTest() {
        int done = 0, succeeded = 0;
        done++; cout << "testWordSegmentationSVM()" << endl; if(testWordSegmentationSVM()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testWordSegmentationEmpty()" << endl; if(testWordSegmentationEmpty()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testWordSegmentationUnk()" << endl; if(testWordSegmentationUnk()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testWordSegmentationLogistic()" << endl; if(testWordSegmentationLogistic()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testGlobalTaggingSVM()" << endl; if(testGlobalTaggingSVM()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testGlobalTaggingLogistic()" << endl; if(testGlobalTaggingLogistic()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testGlobalTaggingNoWS()" << endl; if(testGlobalTaggingNoWS()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testNoWSUnk()" << endl; if(testNoWSUnk()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testGlobalSelf()" << endl; if(testGlobalSelf()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testNormalizationUnk()" << endl; if(testNormalizationUnk()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testLocalTagging()" << endl; if(testLocalTagging()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testPartialSegmentation()" << endl; if(testPartialSegmentation()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testTextIO()" << endl; if(testTextIO()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testBinaryIO()" << endl; if(testBinaryIO()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "testConfidentInput()" << endl; if(testConfidentInput()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestAnalysis Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

};

}

#endif