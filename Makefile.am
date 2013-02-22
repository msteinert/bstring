#                                        -*- Automake -*-
# Process this file with automake to produce Makefile.in.

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = \
	bstring/libbsafe.la \
	bstring/libbstring.la \
	bstring/libbstring++.la

# BSafe library
bstring_libbsafe_la_SOURCES = \
	bstring/bsafe.c
bstring_libbsafe_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

# C library
bstring_libbstring_la_SOURCES = \
	bstring/bstraux.c \
	bstring/bstrlib.c
bstring_libbstring_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

# C++ wrapper
bstring_libbstring___la_SOURCES = \
	bstring/bstrwrap.cpp
bstring_libbstring___la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
bstring_libbstring___la_LIBADD = \
	bstring/libbstring.la

# install development headers
pkgincludedir = $(includedir)/bstring
pkginclude_HEADERS = \
	bstring/bsafe.h \
	bstring/bstraux.h \
	bstring/bstrlib.h \
	bstring/bstrwrap.h

check_PROGRAMS = \
	tests/bstest \
	tests/cpptest \
	tests/test \
	tests/testaux
TESTS = $(check_PROGRAMS)

tests_bstest_SOURCES = tests/bstest.c
tests_bstest_CPPFLAGS = -I$(top_srcdir)/bstring
tests_bstest_LDADD = bstring/libbstring.la

tests_cpptest_SOURCES = tests/cpptest.cpp
tests_cpptest_CPPFLAGS = -I$(top_srcdir)/bstring
tests_cpptest_LDADD = bstring/libbstring++.la bstring/libbstring.la

tests_test_SOURCES = tests/test.cpp
tests_test_CPPFLAGS = -I$(top_srcdir)/bstring
tests_test_LDADD = bstring/libbstring++.la bstring/libbstring.la

tests_testaux_SOURCES = tests/testaux.c
tests_testaux_CPPFLAGS = -I$(top_srcdir)/bstring
tests_testaux_LDADD = bstring/libbstring.la

# install pkg-config scripts
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = \
	bsafe.pc \
	bstring.pc \
	bstring++.pc
