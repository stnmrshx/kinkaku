KNKH = 

AM_CPPFLAGS = -I$(srcdir)/../include -DPKGDATADIR='"$(pkgdatadir)"'

bin_PROGRAMS = kinkaku train-kinkaku

kinkaku_SOURCES = run-kinkaku.cpp ${KNKH}
kinkaku_LDADD = ../lib/libkinkaku.la

train_kinkaku_SOURCES = train-kinkaku.cpp ${KNKH}
train_kinkaku_LDADD = ../lib/libkinkaku.la
