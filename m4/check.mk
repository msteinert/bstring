# check.mk - Provides the "%.check" target
#
# Copyright 1994-2012 Free Software Foundation, Inc.
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

%.check: %
	$(AM_V_at)$(TESTS_ENVIRONMENT) $*
