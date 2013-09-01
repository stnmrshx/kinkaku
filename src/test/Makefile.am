KNKH = test-kinkaku.h test-analysis.h test-base.h test-corpusio.h \
       test-sentence.h \
       test-corpusio-euc.h \
       test-corpusio-sjis.h

AM_CPPFLAGS = -I$(srcdir)/../include -DPKGDATADIR='"$(pkgdatadir)"'

noinst_PROGRAMS = test-kinkaku

test_kinkaku_SOURCES = test-kinkaku.cpp ${KNKH}
test_kinkaku_LDADD = ../lib/libkinkaku.la
