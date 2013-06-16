# gdb.m4 - Check for Doxygen.
#
# Copyright 2013 Michael Steinert
#
# This file is free software; the copyright holder gives unlimited
# permission to copy and/or distribute it, with or without modifications,
# as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.

#serial 1

# AX_PROG_DOXYGEN
# ---------------
# Setup Doxygen and the "html" target.
AC_DEFUN([AX_PROG_DOXYGEN],
[dnl
	AC_ARG_VAR([DOXYGEN_PATH], [Path to the Doxygen executable.])
	AC_CHECK_PROGS([DOXYGEN_PATH], [doxygen], [:])
	AM_CONDITIONAL([HAVE_DOXYGEN], [test "x$DOXYGEN_PATH" != "x:"])
])dnl
