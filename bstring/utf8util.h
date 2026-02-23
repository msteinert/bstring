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
 * \brief Interface for low-level UTF-8 utility functions.
 *
 * This module is standalone and does not depend on bstrlib.
 */

#ifndef UTF8_UNICODE_UTILITIES
#define UTF8_UNICODE_UTILITIES

#include <limits.h>

/* If bstrlib.h has not been included, define the visibility attribute here.
   The #ifndef guard ensures we don't conflict if bstrlib.h came first. */
#ifndef BSTR_PUBLIC
# if __GNUC__ >= 4
#  define BSTR_PUBLIC __attribute__ ((visibility ("default")))
# else
#  define BSTR_PUBLIC
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if INT_MAX >= 0x7fffffffUL
typedef int cpUcs4;
#elif LONG_MAX >= 0x7fffffffUL
typedef long cpUcs4;
#else
#error This compiler is not supported
#endif

#if UINT_MAX == 0xFFFF
typedef unsigned int cpUcs2;
#elif USHRT_MAX == 0xFFFF
typedef unsigned short cpUcs2;
#elif UCHAR_MAX == 0xFFFF
typedef unsigned char cpUcs2;
#else
#error This compiler is not supported
#endif

#define isLegalUnicodeCodePoint(v) \
	((((v) < 0xD800L) || ((v) > 0xDFFFL)) && \
	 (((unsigned long)(v)) <= 0x0010FFFFL) && \
	 (((v)|0x1F0001) != 0x1FFFFFL))

struct utf8Iterator {
	unsigned char *data;
	int slen;
	int start, next;
	int error;
};

#define utf8IteratorNoMore(it) (!(it) || (it)->next >= (it)->slen)

BSTR_PUBLIC void utf8IteratorInit(struct utf8Iterator *iter,
                                  unsigned char *data, int slen);
BSTR_PUBLIC void utf8IteratorUninit(struct utf8Iterator *iter);
BSTR_PUBLIC cpUcs4 utf8IteratorGetNextCodePoint(struct utf8Iterator *iter,
                                                cpUcs4 errCh);
BSTR_PUBLIC cpUcs4 utf8IteratorGetCurrCodePoint(struct utf8Iterator *iter,
                                                cpUcs4 errCh);
BSTR_PUBLIC int utf8ScanBackwardsForCodePoint(unsigned char *msg, int len,
                                              int pos, cpUcs4 *out);

#ifdef __cplusplus
}
#endif

#endif /* UTF8_UNICODE_UTILITIES */
