#                                        -*- Automake -*-
# Process this file with automake to produce Makefile.in.

INCLUDES = -I$(top_srcdir)/bstring

check_PROGRAMS = bstest cpptest test testaux
TESTS = $(check_PROGRAMS)

bstest_SOURCES = bstest.c
bstest_LDADD = $(top_builddir)/bstring/libbstring.la

cpptest_SOURCES = cpptest.cpp
cpptest_LDADD = \
	$(top_builddir)/bstring/libbstring++.la \
	$(top_builddir)/bstring/libbstring.la

test_SOURCES = test.cpp
test_LDADD = \
	$(top_builddir)/bstring/libbstring++.la \
	$(top_builddir)/bstring/libbstring.la

testaux_SOURCES = testaux.c
testaux_LDADD = $(top_builddir)/bstring/libbstring.la
