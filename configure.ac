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
m4_define([major_version], [1])
m4_define([minor_version], [0])
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

# Check the build environment
m4_ifdef([AC_PROG_CC_C99], [AC_PROG_CC_C99],
	 [AC_MSG_WARN([Attemping c99 workaround due to old version of autoconf])
	  AC_PROG_CC
	  AX_TRY_CFLAGS([-std=c99], [AX_CFLAGS([-std=c99])])])
AX_PROG_VALGRIND
AX_PROG_DOXYGEN
AX_PROG_GDB

# OS-specific settings
AS_CASE([$host_os],
[darwin*],
	[VALGRIND_FLAGS="--dsymutil=yes"]
)

# Enable C compiler flags
AX_TRY_CFLAGS([-Wall], [AX_CFLAGS([-Wall])])
AX_TRY_CFLAGS([-Wextra], [AX_CFLAGS([-Wextra])])
AX_TRY_CFLAGS([-pedantic], [AX_CFLAGS([-pedantic])])
AX_TRY_CFLAGS([-Wstrict-prototypes], [AX_CFLAGS([-Wstrict-prototypes])])
AX_TRY_CFLAGS([-Wcast-align], [AX_CFLAGS([-Wcast-align])])
AX_TRY_CFLAGS([-fno-common], [AX_CFLAGS([-fno-common])])
AX_TRY_CFLAGS([-fvisibility=hidden], [AX_CFLAGS([-fvisibility=hidden])])
AX_TRY_LDFLAGS([-Wl,--no-undefined], [AX_LDFLAGS([-Wl,--no-undefined])])

# Libtool versioning
LT_CURRENT=m4_eval(100 * minor_version + micro_version - interface_age)
LT_REVISION=interface_age
LT_AGE=m4_eval(m4_eval(100 * minor_version + micro_version) - interface_age)
AC_SUBST([LT_CURRENT])
AC_SUBST([LT_REVISION])
AC_SUBST([LT_AGE])

# Setup unit-test environment
PKG_CHECK_MODULES([CHECK], [check >= 0.9],
		  [have_check=yes
		   VALGRIND_ENVIRONMENT="CK_FORK=no"
		   GDB_ENVIRONMENT="CK_FORK=no"
		   # Some versions of the Check header file use
		   # GNU-style token pasting.
		   AX_TRY_CFLAGS([-Wno-gnu],
				 [CHECK_CFLAGS="$CHECK_CFLAGS -Wno-gnu"])],
		  [have_check=no
		   AC_MSG_WARN([Unable to run unit tests.])])
AM_CONDITIONAL([HAVE_CHECK], [test "x$have_check" = "xyes"])

# Output files & status
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	bstring.pc
	doc/Doxyfile
	Makefile
])
AC_OUTPUT
AC_MSG_RESULT([
$PACKAGE_NAME build options:
	Host: $host
	Version: $VERSION
])
