/* Copyright 2002-2015 Paul Hsieh
 * This file is part of Bstrlib.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *    3. Neither the name of bstrlib nor the names of its contributors may be
 *       used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * GNU General Public License Version 2 (the "GPL").
 */

/**
 * \file
 * \brief Interface for basic Unicode utility functions for bstrings.
 *
 * Depends on bstrlib.h and utf8util.h.
 */

#ifndef BSTRLIB_UNICODE_UTILITIES
#define BSTRLIB_UNICODE_UTILITIES

#include "bstrlib.h"
#include "utf8util.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Scan a bstring and return 1 if its entire content consists of valid UTF-8
 * encoded code points, otherwise return 0.
 */
BSTR_PUBLIC int
buIsUTF8Content(const bstring bu);

/**
 * Convert an array of UCS-4 code points (bu, len elements) to UTF-8 and
 * append the result to the bstring b.
 *
 * Any invalid code point is replaced by errCh. If errCh is itself not a
 * valid code point, translation halts on the first error and BSTR_ERR is
 * returned. Otherwise BSTR_OK is returned.
 */
BSTR_PUBLIC int
buAppendBlkUcs4(bstring b, const cpUcs4 *bu, int len, cpUcs4 errCh);

/* For those unfortunate enough to be stuck supporting UTF-16. */

/**
 * Convert the UTF-8 bstring bu (starting at code-point offset pos) to a
 * sequence of UTF-16 encoded code units written to ucs2 (at most len units).
 *
 * Returns the number of UCS-2 16-bit words written. Any unparsable code
 * point is translated to errCh.
 */
BSTR_PUBLIC int
buGetBlkUTF16(/* @out */ cpUcs2 *ucs2, int len, cpUcs4 errCh,
              const bstring bu, int pos);

/**
 * Append an array of UTF-16 code units (utf16, len elements) to the UTF-8
 * bstring bu.
 *
 * Any invalid code point is replaced by errCh. If errCh is itself not a
 * valid code point, translation halts on the first error and BSTR_ERR is
 * returned. Otherwise BSTR_OK is returned. If a byte order mark has been
 * previously read it may be passed in via bom; if *bom is 0 it will be
 * filled in from the first character if it is a BOM.
 */
BSTR_PUBLIC int
buAppendBlkUTF16(bstring bu, const cpUcs2 *utf16, int len, cpUcs2 *bom,
                 cpUcs4 errCh);

#ifdef __cplusplus
}
#endif

#endif /* BSTRLIB_UNICODE_UTILITIES */
