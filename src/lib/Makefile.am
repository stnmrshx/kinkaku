LLLIBS = liblinear/liblinear.la
KNKCPP =  kinkaku.cpp general-io.cpp corpus-io-prob.cpp corpus-io-eda.cpp corpus-io-full.cpp corpus-io-part.cpp corpus-io-tokenized.cpp corpus-io-raw.cpp corpus-io.cpp model-io.cpp string-util.cpp kinkaku-model.cpp kinkaku-config.cpp kinkaku-lm.cpp feature-io.cpp dictionary.cpp feature-lookup.cpp kinkaku-util.cpp kinkaku-string.cpp kinkaku-struct.cpp
# KNKH = kinkaku.h corpus-io.h model-io.h string-util.h \
#        kinkaku-model.h kinkaku-string.h kinkaku-struct.h dictionary.h general-io.h \
#        kinkaku-config.h

AM_CPPFLAGS = -I$(srcdir)/../include -DPKGDATADIR='"$(pkgdatadir)"'

SUBDIRS = liblinear

lib_LTLIBRARIES = libkinkaku.la

libkinkaku_la_SOURCES = ${KNKCPP}
libkinkaku_la_LIBADD = ${LLLIBS}
libkinkaku_la_LDFLAGS = -version-info 0:0:0
