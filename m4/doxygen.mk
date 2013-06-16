# doxygen.mk - Provides the "html" target
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

if HAVE_DOXYGEN

DOXYGEN_DOXYFILE ?= Doxyfile

AX_V_doxygen = $(ax__v_doxygen_$(V))
ax__v_doxygen_ = $(ax__v_doxygen_$(AM_DEFAULT_VERBOSITY))
ax__v_doxygen_0 = @echo "  DOX   " $<;
ax__v_doxygen_1 =

DOXYGEN = $(DOXYGEN_PATH) $(DOXYGEN_DOXYFILE)

html-local: $(DOXYGEN_DOXYFILE)
	$(AX_V_doxygen)$(DOXYGEN)

endif
