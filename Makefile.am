#                                        -*- Automake -*-
# Process this file with automake to produce Makefile.in.

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -I$(top_srcdir)/bstring

lib_LTLIBRARIES = bstring/libbstring.la

bstring_libbstring_la_SOURCES = \
	bstring/bstraux.c \
	bstring/bstrlib.c
bstring_libbstring_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

# install development headers
pkgincludedir = $(includedir)/bstring
pkginclude_HEADERS = \
	bstring/bstraux.h \
	bstring/bstrlib.h

if HAVE_CHECK
TESTS = $(check_PROGRAMS)
check_PROGRAMS = \
	tests/bstest \
	tests/testaux

tests_bstest_SOURCES = tests/bstest.c
tests_bstest_CFLAGS = -I$(top_srcdir)/bstring $(CHECK_CFLAGS)
tests_bstest_LDFLAGS = $(CHECK_LIBS)
tests_bstest_LDADD = bstring/libbstring.la

tests_testaux_SOURCES = tests/testaux.c
tests_testaux_CFLAGS = -I$(top_srcdir)/bstring $(CHECK_CFLAGS)
tests_testaux_LDFLAGS = $(CHECK_LIBS)
tests_testaux_LDADD = bstring/libbstring.la
endif

# Documentation setup
DOXYGEN_DOXYFILE = $(top_srcdir)/doc/Doxyfile
include $(top_srcdir)/m4/doxygen.mk

# Test targets
include $(top_srcdir)/m4/valgrind.mk
include $(top_srcdir)/m4/check.mk
include $(top_srcdir)/m4/gdb.mk

# Install pkg-config scripts
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = bstring.pc

release: dist
	$(AM_V_GEN)sha256sum $(DIST_ARCHIVES) > $(PACKAGE)-$(VERSION).sha256sum

EXTRA_DIST = doc README.md
