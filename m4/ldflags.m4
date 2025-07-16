# ldflags.m4 - Test and set linker flags
#
# Copyright 2011, 2012 Michael Steinert
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

# AX_TRY_LDFLAGS(FLAG, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
# ---------------------------------------------------------
# Test if a linker flag is supported.
# FLAG: a linker flag to try
# ACTION-IF-TRUE: commands to execute if FLAG is supported
# ACTION-IF-FALSE: commands to execute if FLAG is not supported
AC_DEFUN([AX_TRY_LDFLAGS],
[dnl
_ax_ldflags=$LDFLAGS
LDFLAGS="$1 $LDFLAGS"
AC_MSG_CHECKING([if linker accepts '$1'])
AC_LINK_IFELSE(
	    [AC_LANG_SOURCE([int main(int,char**){return 0;}])],
	    [AC_MSG_RESULT([yes])
	     LDFLAGS=$_ax_ldflags
	     $2],
	    [AC_MSG_RESULT([no])
	     LDFLAGS=$_ax_ldflags
	     $3])
])dnl

# AX_LDFLAGS(flags)
# -----------------
# Enable linker flags.
# FLAGS: a whitespace-separated list of linker flags to set
AC_DEFUN([AX_LDFLAGS],
[dnl
m4_foreach_w([_ax_flag], [$1],
	     [AS_CASE([" $LDFLAGS "],
		      [*[[\ \	]]_ax_flag[[\ \	]]*],
		      [],
		      [*],
		      [LDFLAGS="$LDFLAGS _ax_flag"])])
])dnl
