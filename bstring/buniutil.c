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

/*
 * buniutil.c
 *
 * This file is not necessarily part of the core bstring library itself, but
 * is an implementation of basic UTF-8 processing for bstrings.  This module
 * depends on bstrlib.c and utf8util.c.
 */

#include "bstrlib.h"
#include "buniutil.h"

#define UNICODE__CODE_POINT__REPLACEMENT_CHARACTER (0xFFFDL)

/*  int buIsUTF8Content (const bstring bu)
 *
 *  Scan string and return 1 if its entire contents is entirely UTF-8 code
 *  points.  Otherwise return 0.
 */
int
buIsUTF8Content(const bstring bu)
{
	struct utf8Iterator iter;

	if (NULL == bdata(bu)) return 0;
	for (utf8IteratorInit(&iter, bu->data, bu->slen);
	     iter.next < iter.slen;) {
		if (0 >= utf8IteratorGetNextCodePoint(&iter, -1)) return 0;
	}
	return 1;
}

/*  int buGetBlkUTF16 (cpUcs2 *ucs2, int len, cpUcs4 errCh,
 *                     const bstring bu, int pos)
 *
 *  Convert a string of UTF-8 code points (bu) skipping the first pos code
 *  points, into a sequence of UTF-16 encoded code points.  Returns the
 *  number of UCS-2 16-bit words written to the output.  No more than len
 *  words are written to the target array ucs2.  If any code point in bu is
 *  unparsable, it will be translated to errCh.
 */
int
buGetBlkUTF16(/* @out */ cpUcs2 *ucs2, int len, cpUcs4 errCh,
              const bstring bu, int pos)
{
	struct tagbstring t;
	struct utf8Iterator iter;
	cpUcs4 ucs4;
	int i;
	int j;

	if (!isLegalUnicodeCodePoint(errCh))
		errCh = UNICODE__CODE_POINT__REPLACEMENT_CHARACTER;
	if (NULL == ucs2 || 0 >= len || NULL == bdata(bu) || 0 > pos)
		return BSTR_ERR;

	for (j=0, i=0; j < bu->slen; j++) {
		if (0x80 != (0xC0 & bu->data[j])) {
			if (i >= pos) break;
			i++;
		}
	}

	t.mlen = -1;
	t.data = bu->data + j;
	t.slen = bu->slen - j;

	utf8IteratorInit(&iter, t.data, t.slen);

	ucs4 = BSTR_ERR;
	for (i=0; 0 < len && iter.next < iter.slen; i++) {
		ucs4 = utf8IteratorGetNextCodePoint(&iter, errCh);
		if (0 > ucs4) break;

		if (ucs4 < 0x10000) {
			*ucs2++ = (cpUcs2) ucs4;
			len--;
		} else {
			if (len < 2) {
				*ucs2++ = UNICODE__CODE_POINT__REPLACEMENT_CHARACTER;
				len--;
			} else {
				long y = ucs4 - 0x10000;
				ucs2[0] = (cpUcs2) (0xD800 | (y >> 10));
				ucs2[1] = (cpUcs2) (0xDC00 | (y & 0x03FF));
				len -= 2;
				ucs2 += 2;
				i++;
			}
		}
	}
	while (0 < len) {
		*ucs2++ = 0;
		len--;
	}

	utf8IteratorUninit(&iter);
	if (0 > ucs4) return BSTR_ERR;
	return i;
}

/*

Unicode                   UTF-8
-------                   -----
U-00000000 - U-0000007F:  0xxxxxxx
U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

UTF-32: U-000000 - U-10FFFF

*/

/*  int buAppendBlkUcs4 (bstring b, const cpUcs4 *bu, int len, cpUcs4 errCh)
 *
 *  Convert an array of UCS-4 code points (bu) to UTF-8 code points and
 *  append to b.  Any invalid code point is replaced by errCh.  If errCh is
 *  itself not a valid code point, then this translation will halt upon the
 *  first error and return BSTR_ERR.  Otherwise BSTR_OK is returned.
 */
int
buAppendBlkUcs4(bstring b, const cpUcs4 *bu, int len, cpUcs4 errCh)
{
	int oldSlen;

	if (NULL == bu || NULL == b || 0 > len) return BSTR_ERR;
	oldSlen = blengthe(b, -1);
	if (0 > oldSlen) return BSTR_ERR;
	if (!isLegalUnicodeCodePoint(errCh)) errCh = ~0;

	for (int i=0; i < len; i++) {
		unsigned char c[6];
		cpUcs4 v = bu[i];

		if (!isLegalUnicodeCodePoint(v)) {
			if (~0 == errCh) {
				b->slen = oldSlen;
				return BSTR_ERR;
			}
			v = errCh;
		}

		if (v < 0x80) {
			if (BSTR_OK != bconchar(b, (char) v)) {
				b->slen = oldSlen;
				return BSTR_ERR;
			}
		} else if (v < 0x800) {
			c[0] = (unsigned char) ( (v >>  6)         + 0xc0);
			c[1] = (unsigned char) ((        v & 0x3f) + 0x80);
			if (BSTR_OK != bcatblk(b, c, 2)) {
				b->slen = oldSlen;
				return BSTR_ERR;
			}
		} else if (v < 0x10000) {
			c[0] = (unsigned char) ( (v >> 12)         + 0xe0);
			c[1] = (unsigned char) (((v >>  6) & 0x3f) + 0x80);
			c[2] = (unsigned char) ((        v & 0x3f) + 0x80);
			if (BSTR_OK != bcatblk(b, c, 3)) {
				b->slen = oldSlen;
				return BSTR_ERR;
			}
		} else
#if 0
			if (v < 0x200000)
#endif
		{
			c[0] = (unsigned char) ( (v >> 18)         + 0xf0);
			c[1] = (unsigned char) (((v >> 12) & 0x3f) + 0x80);
			c[2] = (unsigned char) (((v >>  6) & 0x3f) + 0x80);
			c[3] = (unsigned char) ((        v & 0x3f) + 0x80);
			if (BSTR_OK != bcatblk(b, c, 4)) {
				b->slen = oldSlen;
				return BSTR_ERR;
			}
		}
#if 0
		else if (v < 0x4000000) {
			c[0] = (unsigned char) ( (v >> 24)         + 0xf8);
			c[1] = (unsigned char) (((v >> 18) & 0x3f) + 0x80);
			c[2] = (unsigned char) (((v >> 12) & 0x3f) + 0x80);
			c[3] = (unsigned char) (((v >>  6) & 0x3f) + 0x80);
			c[4] = (unsigned char) ((        v & 0x3f) + 0x80);
			if (BSTR_OK != bcatblk(b, c, 5)) {
				b->slen = oldSlen;
				return BSTR_ERR;
			}
		} else {
			c[0] = (unsigned char) ( (v >> 30)         + 0xfc);
			c[1] = (unsigned char) (((v >> 24) & 0x3f) + 0x80);
			c[2] = (unsigned char) (((v >> 18) & 0x3f) + 0x80);
			c[3] = (unsigned char) (((v >> 12) & 0x3f) + 0x80);
			c[4] = (unsigned char) (((v >>  6) & 0x3f) + 0x80);
			c[5] = (unsigned char) ((        v & 0x3f) + 0x80);
			if (BSTR_OK != bcatblk(b, c, 6)) {
				b->slen = oldSlen;
				return BSTR_ERR;
			}
		}
#endif
	}
	return BSTR_OK;
}

#define endSwap(cs, mode) \
	((mode) ? ((((cs) & 0xFF) << 8) | (((cs) >> 8) & 0xFF)) : (cs))
#define TEMP_UCS4_BUFFER_SIZE (64)

/*  int buAppendBlkUTF16 (bstring bu, const cpUcs2 *utf16, int len,
 *                        cpUcs2 *bom, cpUcs4 errCh)
 *
 *  Append an array of UCS-2 code units (utf16) as UTF-8 to bstring bu.
 *  Any invalid code point is replaced by errCh.  If errCh is itself not a
 *  valid code point, then this translation will halt upon the first error
 *  and return BSTR_ERR.  Otherwise BSTR_OK is returned.  If a byte order
 *  mark has been previously read, it may be passed in as bom, otherwise if
 *  *bom is set to 0, it will be filled in with the BOM as read from the
 *  first character if it is a BOM.
 */
int
buAppendBlkUTF16(bstring bu, const cpUcs2 *utf16, int len, cpUcs2 *bom,
                 cpUcs4 errCh)
{
	cpUcs4 buff[TEMP_UCS4_BUFFER_SIZE];
	int cc;
	int i;
	int sm;
	int oldSlen;

	if (NULL == bdata(bu) || NULL == utf16 || len < 0) return BSTR_ERR;
	if (!isLegalUnicodeCodePoint(errCh)) errCh = ~0;
	if (len == 0) return BSTR_OK;

	oldSlen = bu->slen;
	i = 0;

	/* Check for BOM character and select endianness.  Also remove the
	   BOM from the stream, since there is no need for it in UTF-8. */
	if (bom && (cpUcs2) 0xFFFE == *bom) {
		sm = 8;
	} else if (bom && (cpUcs2) 0xFEFF == *bom) {
		sm = 0;
	} else if (utf16[i] == (cpUcs2) 0xFFFE) {
		if (bom) *bom = utf16[i];
		sm = 8;
		i++;
	} else if (utf16[i] == (cpUcs2) 0xFEFF) {
		if (bom) *bom = utf16[i];
		sm = 0;
		i++;
	} else {
		sm = 0; /* Assume local endianness. */
	}

	cc = 0;
	for (; i < len; i++) {
		cpUcs4 c;
		cpUcs4 v;
		v = endSwap(utf16[i], sm);

		if ((v | 0x7FF) == 0xDFFF) { /* Deal with surrogate pairs */
			if (v >= 0xDC00 || i >= len) {
			ErrMode:;
				if (~0 == errCh) {
				ErrReturn:;
					bu->slen = oldSlen;
					return BSTR_ERR;
				}
				v = errCh;
			} else {
				i++;
				if ((c = endSwap(utf16[i], sm) - 0xDC00) > 0x3FF)
					goto ErrMode;
				v = ((v - 0xD800) << 10) + c + 0x10000;
			}
		}
		buff[cc] = v;
		cc++;
		if (cc >= TEMP_UCS4_BUFFER_SIZE) {
			if (0 > buAppendBlkUcs4(bu, buff, cc, errCh))
				goto ErrReturn;
			cc = 0;
		}
	}
	if (cc > 0 && 0 > buAppendBlkUcs4(bu, buff, cc, errCh)) goto ErrReturn;

	return BSTR_OK;
}
