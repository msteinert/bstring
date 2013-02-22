#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Initialize autoconf
AC_PREREQ([2.59])

# Library versioning
# Making releases:
#	micro_version += 1
#	interface_age += 1
# If functions have been added or backward-compatibility has been broken:
#	interface_age = 0
m4_define([major_version], [0])
m4_define([minor_version], [1])
m4_define([micro_version], [0])
m4_define([interface_age], [0])

# Initialize autoconf & automake
AC_INIT([Bstring], [major_version.minor_version.micro_version],
    [https://github.com/msteinert/bstring/issues], [bstring])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.9 subdir-objects foreign -Wno-portability])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_SRCDIR([bstring/bstrlib.h])

# Initialize Libtool
LT_PREREQ([2.2])
LT_INIT

# Checks for programs
m4_ifdef([AC_PROG_CC_C99], [AC_PROG_CC_C99],
	[AC_MSG_WARN([Attemping c99 workaround due to old version of autoconf])
	AC_PROG_CC
	AX_TRY_CFLAGS([-std=c99], [AX_CFLAGS([-std=c99])])])

# Checks for programs
AC_PROG_CXX
AC_LANG([C++])

# Enable C compiler flags
AX_TRY_CFLAGS([-Wall], [AX_CFLAGS([-Wall])])
AX_TRY_CFLAGS([-Wstrict-prototypes], [AX_CFLAGS([-Wstrict-prototypes])])
AX_TRY_CFLAGS([-Wcast-align], [AX_CFLAGS([-Wcast-align])])
AX_TRY_CFLAGS([-fno-common], [AX_CFLAGS([-fno-common])])

# Enable C++ compiler flags
AX_TRY_CXXFLAGS([-Wall], [AX_CXXFLAGS([-Wall])])
AX_TRY_CXXFLAGS([-Wnon-virtual-dtor], [AX_CXXFLAGS([-Wnon-virtual-dtor])])
AX_TRY_CXXFLAGS([-fno-rtti], [AX_CXXFLAGS([-fno-rtti])])

# Pkg-config versioning
series_major=major_version
series_minor=minor_version
series=$series_major.$series_minor
AC_SUBST([series_major])
AC_SUBST([series_minor])
AC_SUBST([series])

# Libtool versioning
LT_CURRENT=m4_eval(100 * minor_version + micro_version - interface_age)
LT_REVISION=interface_age
LT_AGE=m4_eval(m4_eval(100 * minor_version + micro_version) - interface_age)
AC_SUBST([LT_CURRENT])
AC_SUBST([LT_REVISION])
AC_SUBST([LT_AGE])

# Debug option
AC_ARG_ENABLE([memory-debug],
	[AS_HELP_STRING([--enable-memory-debug],
		[enable memory debugging features @<:@default=no@:>@])],,
	[enable_memory_debug=no])
AS_IF([test "x$enable_memory_debug" = "xyes"],
	[AC_DEFINE([BSTRLIB_MEMORY_DEBUG], [1],
		[Define to 1 to enable memory debugging])])

# Output files
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	bsafe.pc
	bstring.pc
	bstring++.pc
	Makefile
])
AC_OUTPUT
AC_MSG_RESULT([
$PACKAGE_NAME build options:
	Host: $host
	Version: $VERSION
	Memory Debugging: $enable_memory_debug
])
