#                                        -*- Automake -*-
# Process this file with automake to produce Makefile.in.

lib_LTLIBRARIES = libbsafe.la libbstring.la libbstring++.la

# BSafe library
libbsafe_la_SOURCES = \
	bsafe.c
libbsafe_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

# C library
libbstring_la_SOURCES = \
	bstraux.c \
	bstrlib.c
libbstring_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

# C++ wrapper
libbstring___la_SOURCES = \
	bstrwrap.cpp
libbstring___la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
libbstring___la_LIBADD = \
	libbstring.la

# install development headers
pkgincludedir = $(includedir)/bstring
pkginclude_HEADERS = \
	bsafe.h \
	bstraux.h \
	bstrlib.h \
	bstrwrap.h
