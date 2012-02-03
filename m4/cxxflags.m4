# cxxflags.m4 - Test and set compiler flags
#
# Copyright 2011 Michael Steinert
#
# This file is free software; the copyright holder(s) give unlimited
# permission to copy and/or distribute it, with or without modifications,
# as long as this notice is preserved.

#serial 1

# AX_TRY_CXXFLAGS(FLAG, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
# ----------------------------------------------------------
# Test a compiler flag is supported.
# FLAG: a compiler flag to try
# ACTION-IF-TRUE: commands to execute if FLAG is supported
# ACTION-IF-FALSE: commands to execute if FLAG is not supported
AC_DEFUN([AX_TRY_CXXFLAGS],
[dnl
AC_REQUIRE([AC_PROG_CC])
_ax_cxxflags=$CXXFLAGS
CXXFLAGS="$1 $CXXFLAGS"
AC_MSG_CHECKING([if compiler accepts '$1'])
AC_TRY_COMPILE([], [],
	[AC_MSG_RESULT([yes])
	CXXFLAGS=$_ax_cxxflags
	$2],
	[AC_MSG_RESULT([no])
	CXXFLAGS=$_ax_cxxflags
	$3])
])dnl

# AX_CXXFLAGS(FLAGS)
# ------------------
# Enable compiler flags.
# FLAGS: a whitespace-separated list of compiler flags to set
AC_DEFUN([AX_CXXFLAGS],
[dnl
m4_foreach_w([_ax_flag], [$1],
	[AS_CASE([" $CXXFLAGS "],
		[*[[\ \	]]_ax_flag[[\ \	]]*],
			[],
		[*],
			[CXXFLAGS="$CXXFLAGS _ax_flag"])
	])dnl
])dnl
