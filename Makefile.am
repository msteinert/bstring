#                                        -*- Automake -*-
# Process this file with automake to produce Makefile.in.

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -I$(top_srcdir)/bstring

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

TESTS = $(check_PROGRAMS)
check_PROGRAMS =

if HAVE_CHECK
check_PROGRAMS += tests/testaux
tests_testaux_SOURCES = tests/testaux.c
tests_testaux_CFLAGS = -I$(top_srcdir)/bstring $(CHECK_FLAGS)
tests_testaux_LDFLAGS = $(CHECK_LIBS)
tests_testaux_LDADD = bstring/libbstring.la
endif

check_PROGRAMS += tests/bstest
tests_bstest_SOURCES = tests/bstest.c
tests_bstest_CFLAGS = -I$(top_srcdir)/bstring
tests_bstest_LDADD = bstring/libbstring.la

check_PROGRAMS += tests/cpptest
tests_cpptest_SOURCES = tests/cpptest.cpp
tests_cpptest_CXXFLAGS = -I$(top_srcdir)/bstring
tests_cpptest_LDADD = bstring/libbstring++.la bstring/libbstring.la

check_PROGRAMS += tests/test
tests_test_SOURCES = tests/test.cpp
tests_test_CXXFLAGS = -I$(top_srcdir)/bstring
tests_test_LDADD = bstring/libbstring++.la bstring/libbstring.la

# Test targets
include $(top_srcdir)/m4/valgrind.mk
include $(top_srcdir)/m4/check.mk
include $(top_srcdir)/m4/gdb.mk

# Install pkg-config scripts
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = \
	bsafe.pc \
	bstring.pc \
	bstring++.pc
