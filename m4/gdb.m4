# gdb.m4 - Check for GDB.
#
# Copyright 2012 Michael Steinert
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

# AX_PROG_GDB
# -----------
# Setup gdb and the "%.debug" target.
AC_DEFUN([AX_PROG_GDB],
[dnl
	AC_ARG_VAR([GDB_PATH], [Path the GDB executable.])
	AC_ARG_VAR([GDB_FLAGS], [Flags for valgrind.])
	AC_ARG_VAR([GDB_ENVIRONMENT],
		   [Environment variables for valgrind.])
	AC_CHECK_PROGS([GDB_PATH], [gdb], [:])
	AM_CONDITIONAL([HAVE_GDB], [test "x$GDB_PATH" != "x:"])
])dnl
