# gdb.mk - Provides the "%.debug" target(s)
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

if HAVE_GDB

GDB = $(GDB_ENVIRONMENT) $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
	--mode=execute $(GDB_PATH) $(GDB_FLAGS)

%.debug: %
	$(AM_V_at)$(GDB) $*

endif
