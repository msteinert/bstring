/* Copyright (C) 2026 Daniel Markstedt <daniel@mindani.net>
 * UTF-8 unit tests for the Better String Library
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
 * This file is the C unit test for the UTF-8 modules (utf8util, buniutil).
 *
 * Test data quick reference:
 *   U+0041 'A'      = 0x41                   (1-byte ASCII)
 *   U+00A9 '©'      = 0xC2 0xA9              (2-byte)
 *   U+20AC '€'      = 0xE2 0x82 0xAC         (3-byte)
 *   U+1F600 '😀'   = 0xF0 0x9F 0x98 0x80    (4-byte)
 *   UTF-16 U+1F600  = { 0xD83D, 0xDE00 }     (surrogate pair)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "buniutil.h"
#include "bstrlib.h"
#include "utf8util.h"
#include <check.h>
#include <stdlib.h>
#include <string.h>

/* -----------------------------------------------------------------------
 * core_000: utf8IteratorInit — valid inputs and error inputs
 * ----------------------------------------------------------------------- */
START_TEST(core_000)
{
	struct utf8Iterator iter;
	unsigned char data[] = "Hello";

	/* NULL iter pointer must not crash */
	utf8IteratorInit(NULL, data, 5);

	/* Valid initialisation */
	utf8IteratorInit(&iter, data, 5);
	ck_assert_int_eq(iter.slen,  5);
	ck_assert_int_eq(iter.next,  0);
	ck_assert_int_eq(iter.start, -1);
	ck_assert_int_eq(iter.error, 0);
	ck_assert(iter.data == data);

	/* NULL data → sentinel values */
	utf8IteratorInit(&iter, NULL, 5);
	ck_assert_int_eq(iter.slen, -1);
	ck_assert_int_eq(iter.next, -1);
	ck_assert_int_eq(iter.error, 1);

	/* Negative slen → sentinel values */
	utf8IteratorInit(&iter, data, -1);
	ck_assert_int_eq(iter.slen, -1);
	ck_assert_int_eq(iter.next, -1);
	ck_assert_int_eq(iter.error, 1);

	/* Zero-length string is valid */
	utf8IteratorInit(&iter, data, 0);
	ck_assert_int_eq(iter.slen,  0);
	ck_assert_int_eq(iter.next,  0);
	ck_assert_int_eq(iter.error, 0);
}
END_TEST

/* -----------------------------------------------------------------------
 * core_001: utf8IteratorUninit — clears all fields; handles NULL gracefully
 * ----------------------------------------------------------------------- */
START_TEST(core_001)
{
	struct utf8Iterator iter;
	unsigned char data[] = "Hello";

	utf8IteratorInit(&iter, data, 5);
	utf8IteratorUninit(&iter);
	ck_assert(iter.data  == NULL);
	ck_assert_int_eq(iter.slen,  -1);
	ck_assert_int_eq(iter.start, -1);
	ck_assert_int_eq(iter.next,  -1);

	/* NULL pointer must not crash */
	utf8IteratorUninit(NULL);
}
END_TEST

/* -----------------------------------------------------------------------
 * core_002: utf8IteratorGetNextCodePoint — ASCII string iteration
 * ----------------------------------------------------------------------- */
START_TEST(core_002)
{
	struct utf8Iterator iter;
	unsigned char data[] = "ABC";
	cpUcs4 cp;

	utf8IteratorInit(&iter, data, 3);

	cp = utf8IteratorGetNextCodePoint(&iter, '?');
	ck_assert_int_eq(cp, 'A');
	ck_assert_int_eq(iter.error, 0);
	ck_assert_int_eq(iter.start, 0);
	ck_assert_int_eq(iter.next,  1);

	cp = utf8IteratorGetNextCodePoint(&iter, '?');
	ck_assert_int_eq(cp, 'B');
	ck_assert_int_eq(iter.start, 1);
	ck_assert_int_eq(iter.next,  2);

	cp = utf8IteratorGetNextCodePoint(&iter, '?');
	ck_assert_int_eq(cp, 'C');
	ck_assert_int_eq(iter.start, 2);
	ck_assert_int_eq(iter.next,  3);

	/* Past end → errCh */
	cp = utf8IteratorGetNextCodePoint(&iter, '?');
	ck_assert_int_eq(cp, '?');

	/* NULL iterator → errCh */
	cp = utf8IteratorGetNextCodePoint(NULL, '?');
	ck_assert_int_eq(cp, '?');
}
END_TEST

/* -----------------------------------------------------------------------
 * core_003: utf8IteratorGetNextCodePoint — multi-byte sequences
 *
 *   Sequence: © (U+00A9, 2-byte) € (U+20AC, 3-byte) 😀 (U+1F600, 4-byte)
 *   Bytes:    C2 A9                E2 82 AC              F0 9F 98 80
 * ----------------------------------------------------------------------- */
START_TEST(core_003)
{
	struct utf8Iterator iter;
	/* © € 😀 */
	unsigned char data[] = {
		0xC2, 0xA9,             /* U+00A9 © */
		0xE2, 0x82, 0xAC,       /* U+20AC € */
		0xF0, 0x9F, 0x98, 0x80  /* U+1F600 😀 */
	};
	cpUcs4 cp;

	utf8IteratorInit(&iter, data, sizeof(data));

	cp = utf8IteratorGetNextCodePoint(&iter, '?');
	ck_assert_int_eq(cp, 0x00A9);
	ck_assert_int_eq(iter.error, 0);
	ck_assert_int_eq(iter.start, 0);
	ck_assert_int_eq(iter.next,  2);

	cp = utf8IteratorGetNextCodePoint(&iter, '?');
	ck_assert_int_eq(cp, 0x20AC);
	ck_assert_int_eq(iter.error, 0);
	ck_assert_int_eq(iter.start, 2);
	ck_assert_int_eq(iter.next,  5);

	cp = utf8IteratorGetNextCodePoint(&iter, '?');
	ck_assert_int_eq(cp, 0x1F600);
	ck_assert_int_eq(iter.error, 0);
	ck_assert_int_eq(iter.start, 5);
	ck_assert_int_eq(iter.next,  9);

	/* Exhausted */
	cp = utf8IteratorGetNextCodePoint(&iter, '?');
	ck_assert_int_eq(cp, '?');
}
END_TEST

/* -----------------------------------------------------------------------
 * core_004: utf8IteratorGetNextCodePoint — invalid byte sequences
 *
 *   0x80 alone is a stray continuation byte (invalid lead).
 *   0xFF is never valid in UTF-8.
 * ----------------------------------------------------------------------- */
START_TEST(core_004)
{
	struct utf8Iterator iter;
	/* stray continuation, then a valid ASCII char */
	unsigned char data_cont[] = { 0x80, 0x41 };
	/* 0xFF is always invalid */
	unsigned char data_ff[]   = { 0xFF, 0x41 };
	cpUcs4 cp;

	/* Stray continuation byte → error, iterator skips to next valid lead */
	utf8IteratorInit(&iter, data_cont, sizeof(data_cont));
	cp = utf8IteratorGetNextCodePoint(&iter, '?');
	ck_assert_int_eq(cp, '?');
	ck_assert_int_eq(iter.error, 1);
	/* After error the iterator should have advanced past the bad byte(s) */
	ck_assert(iter.next > 0);

	/* 0xFF lead byte → error */
	utf8IteratorInit(&iter, data_ff, sizeof(data_ff));
	cp = utf8IteratorGetNextCodePoint(&iter, '?');
	ck_assert_int_eq(cp, '?');
	ck_assert_int_eq(iter.error, 1);
}
END_TEST

/* -----------------------------------------------------------------------
 * core_005: utf8IteratorGetNextCodePoint — truncated sequence bounds checks
 *
 * The backing arrays contain full valid code points, but slen is set so the
 * sequence is truncated at the end. Iterator must treat each as invalid and
 * return errCh instead of decoding bytes past slen.
 * ----------------------------------------------------------------------- */
START_TEST(core_005)
{
	struct utf8Iterator iter;
	cpUcs4 cp;

	{
		unsigned char data[] = { 0xC2, 0xA9 };
		utf8IteratorInit(&iter, data, 1);
		cp = utf8IteratorGetNextCodePoint(&iter, '?');
		ck_assert_int_eq(cp, '?');
		ck_assert_int_eq(iter.error, 1);
		ck_assert_int_eq(iter.start, 0);
		ck_assert_int_eq(iter.next, 1);
	}

	{
		unsigned char data[] = { 0xE2, 0x82, 0xAC };
		utf8IteratorInit(&iter, data, 2);
		cp = utf8IteratorGetNextCodePoint(&iter, '?');
		ck_assert_int_eq(cp, '?');
		ck_assert_int_eq(iter.error, 1);
		ck_assert_int_eq(iter.start, 0);
		ck_assert_int_eq(iter.next, 2);
	}

	{
		unsigned char data[] = { 0xF0, 0x9F, 0x98, 0x80 };
		utf8IteratorInit(&iter, data, 3);
		cp = utf8IteratorGetNextCodePoint(&iter, '?');
		ck_assert_int_eq(cp, '?');
		ck_assert_int_eq(iter.error, 1);
		ck_assert_int_eq(iter.start, 0);
		ck_assert_int_eq(iter.next, 3);
	}
}
END_TEST

/* -----------------------------------------------------------------------
 * core_006: utf8IteratorGetCurrCodePoint — truncated sequence bounds checks
 *
 * Peek must never decode bytes beyond slen. For each truncated sequence, it
 * should return errCh, set iter.error, and leave iter.next unchanged.
 * ----------------------------------------------------------------------- */
START_TEST(core_006)
{
	struct utf8Iterator iter;
	cpUcs4 cp;

	{
		unsigned char data[] = { 0xC2, 0xA9 };
		utf8IteratorInit(&iter, data, 1);
		cp = utf8IteratorGetCurrCodePoint(&iter, '?');
		ck_assert_int_eq(cp, '?');
		ck_assert_int_eq(iter.error, 1);
		ck_assert_int_eq(iter.next, 0);
	}

	{
		unsigned char data[] = { 0xE2, 0x82, 0xAC };
		utf8IteratorInit(&iter, data, 2);
		cp = utf8IteratorGetCurrCodePoint(&iter, '?');
		ck_assert_int_eq(cp, '?');
		ck_assert_int_eq(iter.error, 1);
		ck_assert_int_eq(iter.next, 0);
	}

	{
		unsigned char data[] = { 0xF0, 0x9F, 0x98, 0x80 };
		utf8IteratorInit(&iter, data, 3);
		cp = utf8IteratorGetCurrCodePoint(&iter, '?');
		ck_assert_int_eq(cp, '?');
		ck_assert_int_eq(iter.error, 1);
		ck_assert_int_eq(iter.next, 0);
	}
}
END_TEST

/* -----------------------------------------------------------------------
 * core_007: utf8IteratorGetCurrCodePoint — peek without advancing
 * ----------------------------------------------------------------------- */
START_TEST(core_007)
{
	struct utf8Iterator iter;
	unsigned char data[] = { 0xC2, 0xA9, 0x41 }; /* © A */
	cpUcs4 cp;

	utf8IteratorInit(&iter, data, sizeof(data));

	/* Peek twice at the same position — must not advance */
	cp = utf8IteratorGetCurrCodePoint(&iter, '?');
	ck_assert_int_eq(cp, 0x00A9);
	ck_assert_int_eq(iter.next, 0); /* still at start */

	cp = utf8IteratorGetCurrCodePoint(&iter, '?');
	ck_assert_int_eq(cp, 0x00A9);
	ck_assert_int_eq(iter.next, 0);

	/* Now advance with GetNext, then peek the next char */
	utf8IteratorGetNextCodePoint(&iter, '?');
	ck_assert_int_eq(iter.next, 2);

	cp = utf8IteratorGetCurrCodePoint(&iter, '?');
	ck_assert_int_eq(cp, 0x41); /* 'A' */
	ck_assert_int_eq(iter.next, 2); /* still not advanced */

	/* NULL iterator → errCh */
	cp = utf8IteratorGetCurrCodePoint(NULL, '?');
	ck_assert_int_eq(cp, '?');
}
END_TEST

/* -----------------------------------------------------------------------
 * core_008: utf8ScanBackwardsForCodePoint — various positions
 *
 *   Data: © (0xC2 0xA9) at bytes 0-1, then 'A' (0x41) at byte 2
 * ----------------------------------------------------------------------- */
START_TEST(core_008)
{
	unsigned char data[] = { 0xC2, 0xA9, 0x41 }; /* © A */
	cpUcs4 out;
	int ret;

	/* pos=0 is the lead byte of © — ret=0, out=0xA9 */
	ret = utf8ScanBackwardsForCodePoint(data, 3, 0, &out);
	ck_assert_int_eq(ret, 0);
	ck_assert_int_eq(out, 0x00A9);

	/* pos=1 is the continuation byte — ret=1 (1 byte back to lead), out=0xA9 */
	ret = utf8ScanBackwardsForCodePoint(data, 3, 1, &out);
	ck_assert_int_eq(ret, 1);
	ck_assert_int_eq(out, 0x00A9);

	/* pos=2 is ASCII 'A' — ret=0, out='A' */
	ret = utf8ScanBackwardsForCodePoint(data, 3, 2, &out);
	ck_assert_int_eq(ret, 0);
	ck_assert_int_eq(out, 0x41);

	/* NULL msg → error (negative) */
	ret = utf8ScanBackwardsForCodePoint(NULL, 3, 0, &out);
	ck_assert(ret < 0);

	/* pos out of range → error */
	ret = utf8ScanBackwardsForCodePoint(data, 3, 3, &out);
	ck_assert(ret < 0);

	/* out=NULL is accepted; return value indicates success/failure */
	ret = utf8ScanBackwardsForCodePoint(data, 3, 2, NULL);
	ck_assert_int_eq(ret, 0);
}
END_TEST

/* -----------------------------------------------------------------------
 * core_009: buIsUTF8Content
 * ----------------------------------------------------------------------- */
START_TEST(core_009)
{
	bstring b;
	int ret;

	/* NULL bstring → 0 */
	ret = buIsUTF8Content(NULL);
	ck_assert_int_eq(ret, 0);

	/* Empty string → 1 (vacuously true) */
	b = bfromcstr("");
	ck_assert(b != NULL);
	ret = buIsUTF8Content(b);
	ck_assert_int_eq(ret, 1);
	bdestroy(b);

	/* Pure ASCII → 1 */
	b = bfromcstr("Hello, world!");
	ck_assert(b != NULL);
	ret = buIsUTF8Content(b);
	ck_assert_int_eq(ret, 1);
	bdestroy(b);

	/* Valid multi-byte UTF-8: © € 😀 */
	{
		unsigned char utf8[] = {
			0xC2, 0xA9,
			0xE2, 0x82, 0xAC,
			0xF0, 0x9F, 0x98, 0x80
		};
		b = blk2bstr(utf8, sizeof(utf8));
		ck_assert(b != NULL);
		ret = buIsUTF8Content(b);
		ck_assert_int_eq(ret, 1);
		bdestroy(b);
	}

	/* Invalid: stray 0x80 continuation byte → 0 */
	{
		unsigned char bad[] = { 0x41, 0x80, 0x41 };
		b = blk2bstr(bad, sizeof(bad));
		ck_assert(b != NULL);
		ret = buIsUTF8Content(b);
		ck_assert_int_eq(ret, 0);
		bdestroy(b);
	}

	/* Invalid: truncated 2-byte sequence → 0 */
	{
		unsigned char bad[] = { 0xC2 }; /* lead without continuation */
		b = blk2bstr(bad, sizeof(bad));
		ck_assert(b != NULL);
		ret = buIsUTF8Content(b);
		ck_assert_int_eq(ret, 0);
		bdestroy(b);
	}
}
END_TEST

/* -----------------------------------------------------------------------
 * core_010: buAppendBlkUcs4 — UCS-4 array → UTF-8 bstring
 * ----------------------------------------------------------------------- */
START_TEST(core_010)
{
	bstring b;
	int ret;

	/* NULL arguments → BSTR_ERR */
	b = bfromcstr("");
	ck_assert(b != NULL);
	ret = buAppendBlkUcs4(NULL, NULL, 0, '?');
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = buAppendBlkUcs4(b, NULL, 1, '?');
	ck_assert_int_eq(ret, BSTR_ERR);
	bdestroy(b);

	/* ASCII code points */
	{
		cpUcs4 pts[] = { 'H', 'i' };
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUcs4(b, pts, 2, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 2);
		ck_assert_int_eq(b->data[0], 'H');
		ck_assert_int_eq(b->data[1], 'i');
		bdestroy(b);
	}

	/* Mixed: © (U+00A9) and € (U+20AC) */
	{
		cpUcs4 pts[] = { 0x00A9, 0x20AC };
		unsigned char expected[] = {
			0xC2, 0xA9,       /* © */
			0xE2, 0x82, 0xAC  /* € */
		};
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUcs4(b, pts, 2, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 5);
		ret = memcmp(b->data, expected, 5);
		ck_assert_int_eq(ret, 0);
		bdestroy(b);
	}

	/* 4-byte: 😀 (U+1F600) */
	{
		cpUcs4 pts[] = { 0x1F600 };
		unsigned char expected[] = { 0xF0, 0x9F, 0x98, 0x80 };
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUcs4(b, pts, 1, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 4);
		ret = memcmp(b->data, expected, 4);
		ck_assert_int_eq(ret, 0);
		bdestroy(b);
	}

	/* Invalid code point with valid errCh → substituted */
	{
		cpUcs4 pts[] = { 0xD800 }; /* surrogates are illegal */
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUcs4(b, pts, 1, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 1);
		ck_assert_int_eq(b->data[0], '?');
		bdestroy(b);
	}

	/* Invalid code point with invalid errCh → BSTR_ERR, bstring unchanged */
	{
		cpUcs4 pts[] = { 0xD800 };
		b = bfromcstr("pre");
		ck_assert(b != NULL);
		ret = buAppendBlkUcs4(b, pts, 1, 0xD800); /* errCh also invalid */
		ck_assert_int_eq(ret, BSTR_ERR);
		/* slen must be rolled back */
		ck_assert_int_eq(b->slen, 3);
		bdestroy(b);
	}

	/* Zero-length array → BSTR_OK, nothing appended */
	{
		cpUcs4 pts[] = { 'X' };
		b = bfromcstr("pre");
		ck_assert(b != NULL);
		ret = buAppendBlkUcs4(b, pts, 0, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 3);
		bdestroy(b);
	}
}
END_TEST

/* -----------------------------------------------------------------------
 * core_011: buGetBlkUTF16 — UTF-8 bstring → UTF-16 array
 * ----------------------------------------------------------------------- */
START_TEST(core_011)
{
	cpUcs2 buf[16];
	int ret;

	/* NULL arguments → BSTR_ERR */
	{
		unsigned char raw[] = { 0x41 };
		bstring b = blk2bstr(raw, 1);
		ck_assert(b != NULL);
		ret = buGetBlkUTF16(NULL, 4, '?', b, 0);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = buGetBlkUTF16(buf, 0, '?', b, 0);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = buGetBlkUTF16(buf, 4, '?', NULL, 0);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = buGetBlkUTF16(buf, 4, '?', b, -1);
		ck_assert_int_eq(ret, BSTR_ERR);
		bdestroy(b);
	}

	/* ASCII "AB" → UTF-16 { 0x0041, 0x0042, 0, ... } */
	{
		bstring b = bfromcstr("AB");
		ck_assert(b != NULL);
		memset(buf, 0xFF, sizeof(buf));
		ret = buGetBlkUTF16(buf, 4, '?', b, 0);
		ck_assert_int_eq(ret, 2);
		ck_assert_int_eq(buf[0], 0x0041);
		ck_assert_int_eq(buf[1], 0x0042);
		ck_assert_int_eq(buf[2], 0); /* null-padded */
		bdestroy(b);
	}

	/* © € → UTF-16 BMP values (U+00A9, U+20AC) */
	{
		unsigned char raw[] = {
			0xC2, 0xA9,
			0xE2, 0x82, 0xAC
		};
		bstring b = blk2bstr(raw, sizeof(raw));
		ck_assert(b != NULL);
		memset(buf, 0xFF, sizeof(buf));
		ret = buGetBlkUTF16(buf, 4, '?', b, 0);
		ck_assert_int_eq(ret, 2);
		ck_assert_int_eq(buf[0], 0x00A9);
		ck_assert_int_eq(buf[1], 0x20AC);
		bdestroy(b);
	}

	/* pos=1 skips first code point */
	{
		bstring b = bfromcstr("AB");
		ck_assert(b != NULL);
		memset(buf, 0, sizeof(buf));
		ret = buGetBlkUTF16(buf, 4, '?', b, 1);
		ck_assert_int_eq(ret, 1);
		ck_assert_int_eq(buf[0], 0x0042);
		bdestroy(b);
	}

	/* Supplementary character 😀 (U+1F600) → surrogate pair */
	{
		unsigned char raw[] = { 0xF0, 0x9F, 0x98, 0x80 };
		bstring b = blk2bstr(raw, sizeof(raw));
		ck_assert(b != NULL);
		memset(buf, 0, sizeof(buf));
		ret = buGetBlkUTF16(buf, 4, '?', b, 0);
		ck_assert_int_eq(ret, 2); /* one code point → two UTF-16 units */
		ck_assert_int_eq(buf[0], 0xD83D); /* high surrogate */
		ck_assert_int_eq(buf[1], 0xDE00); /* low surrogate */
		bdestroy(b);
	}
}
END_TEST

/* -----------------------------------------------------------------------
 * core_012: buAppendBlkUTF16 — UTF-16 array → UTF-8 bstring
 * ----------------------------------------------------------------------- */
START_TEST(core_012)
{
	bstring b;
	int ret;

	/* NULL / bad arguments → BSTR_ERR */
	b = bfromcstr("");
	ck_assert(b != NULL);
	{
		cpUcs2 u[] = { 0x0041 };
		ret = buAppendBlkUTF16(NULL, u, 1, NULL, '?');
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = buAppendBlkUTF16(b, NULL, 1, NULL, '?');
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = buAppendBlkUTF16(b, u, -1, NULL, '?');
		ck_assert_int_eq(ret, BSTR_ERR);
	}
	bdestroy(b);

	/* Zero-length input → BSTR_OK, nothing appended */
	{
		cpUcs2 u[] = { 0x0041 };
		b = bfromcstr("pre");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 0, NULL, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 3);
		bdestroy(b);
	}

	/* ASCII "AB" in UTF-16 → "AB" in UTF-8 */
	{
		cpUcs2 u[] = { 0x0041, 0x0042 };
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 2, NULL, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 2);
		ck_assert_int_eq(b->data[0], 'A');
		ck_assert_int_eq(b->data[1], 'B');
		bdestroy(b);
	}

	/* BMP characters: U+00A9 © and U+20AC € */
	{
		cpUcs2 u[] = { 0x00A9, 0x20AC };
		unsigned char expected[] = {
			0xC2, 0xA9,
			0xE2, 0x82, 0xAC
		};
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 2, NULL, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 5);
		ret = memcmp(b->data, expected, 5);
		ck_assert_int_eq(ret, 0);
		bdestroy(b);
	}

	/* Surrogate pair: 😀 (U+1F600) = { 0xD83D, 0xDE00 } */
	{
		cpUcs2 u[] = { 0xD83D, 0xDE00 };
		unsigned char expected[] = { 0xF0, 0x9F, 0x98, 0x80 };
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 2, NULL, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 4);
		ret = memcmp(b->data, expected, 4);
		ck_assert_int_eq(ret, 0);
		bdestroy(b);
	}

	/* Little-endian BOM (0xFFFE) → byte-swapped input */
	{
		/* 'A' (0x0041) with bytes swapped = 0x4100, plus LE BOM */
		cpUcs2 u[] = { 0xFFFE, 0x4100 };
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 2, NULL, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 1);
		ck_assert_int_eq(b->data[0], 'A');
		bdestroy(b);
	}

	/* Big-endian BOM (0xFEFF) is consumed and removed from output */
	{
		cpUcs2 u[] = { 0xFEFF, 0x0041 };
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 2, NULL, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 1);
		ck_assert_int_eq(b->data[0], 'A');
		bdestroy(b);
	}

	/* Invalid low surrogate alone with valid errCh → substituted */
	{
		cpUcs2 u[] = { 0xDC00 };
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 1, NULL, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 1);
		ck_assert_int_eq(b->data[0], '?');
		bdestroy(b);
	}

	/* Invalid low surrogate then ASCII with valid errCh */
	{
		cpUcs2 u[] = { 0xDC00, 0x0041 };
		unsigned char expected[] = { '?', 'A' };
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 2, NULL, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 2);
		ret = memcmp(b->data, expected, 2);
		ck_assert_int_eq(ret, 0);
		bdestroy(b);
	}

	/* Invalid surrogate with invalid errCh → BSTR_ERR and rollback */
	{
		cpUcs2 u[] = { 0xDC00 };
		b = bfromcstr("pre");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 1, NULL, 0xD800); /* invalid errCh */
		ck_assert_int_eq(ret, BSTR_ERR);
		ck_assert_int_eq(b->slen, 3); /* unchanged */
		ck_assert_int_eq(b->data[0], 'p');
		ck_assert_int_eq(b->data[1], 'r');
		ck_assert_int_eq(b->data[2], 'e');
		bdestroy(b);
	}

	/* bom out-parameter gets set when BOM appears in stream */
	{
		cpUcs2 in_bom = 0;
		cpUcs2 u[] = { 0xFEFF, 0x0041 };
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 2, &in_bom, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(in_bom, 0xFEFF);
		ck_assert_int_eq(b->slen, 1);
		ck_assert_int_eq(b->data[0], 'A');
		bdestroy(b);
	}

	/* Pre-seeded bom controls endianness even without BOM in input */
	{
		cpUcs2 in_bom = 0xFFFE;
		cpUcs2 u[] = { 0x4100 }; /* bytes for 0x0041 in opposite endian */
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 1, &in_bom, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(in_bom, 0xFFFE); /* preserved */
		ck_assert_int_eq(b->slen, 1);
		ck_assert_int_eq(b->data[0], 'A');
		bdestroy(b);
	}

	/* Larger than TEMP_UCS4_BUFFER_SIZE exercises internal flush path */
	{
		cpUcs2 u[80];
		for (int j = 0; j < 80; j++) {
			u[j] = (cpUcs2)('A' + (j % 26));
		}
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 80, NULL, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 80);
		for (int j = 0; j < 80; j++) {
			ck_assert_int_eq(b->data[j], 'A' + (j % 26));
		}
		bdestroy(b);
	}
}
END_TEST

/* -----------------------------------------------------------------------
 * core_013: regression guard
 *
 * Guard against regressions for:
 *   high surrogate followed by non-low surrogate.
 *
 * Expected behavior is:
 *   first code unit substituted with errCh, second processed normally.
 * ----------------------------------------------------------------------- */
START_TEST(core_013)
{
	bstring b;
	int ret;

	{
		cpUcs2 u[] = { 0xD83D, 0x0041 };
		unsigned char expected[] = { '?', 'A' };
		b = bfromcstr("");
		ck_assert(b != NULL);
		ret = buAppendBlkUTF16(b, u, 2, NULL, '?');
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b->slen, 2);
		ret = memcmp(b->data, expected, 2);
		ck_assert_int_eq(ret, 0);
		bdestroy(b);
	}
}
END_TEST

/* -----------------------------------------------------------------------
 * core_013: utf8ScanBackwardsForCodePoint — invalid continuation bytes
 *
 * Each case starts at a lead byte but includes one or more non-continuation
 * trailing bytes. Scanner must reject these and return an error.
 * ----------------------------------------------------------------------- */
START_TEST(core_014)
{
	cpUcs4 out = 0;
	int ret;

	/* Invalid 2-byte sequence: second byte must be 10xxxxxx */
	{
		unsigned char data[] = { 0xC2, 0x41 };
		ret = utf8ScanBackwardsForCodePoint(data, 2, 0, &out);
		ck_assert(ret < 0);
	}

	/* Invalid 3-byte sequence: middle byte must be 10xxxxxx */
	{
		unsigned char data[] = { 0xE2, 0x28, 0xAC };
		ret = utf8ScanBackwardsForCodePoint(data, 3, 0, &out);
		ck_assert(ret < 0);
	}

	/* Invalid 4-byte sequence: third byte must be 10xxxxxx */
	{
		unsigned char data[] = { 0xF0, 0x9F, 0x41, 0x80 };
		ret = utf8ScanBackwardsForCodePoint(data, 4, 0, &out);
		ck_assert(ret < 0);
	}
}
END_TEST

int
main(void)
{
	/* Build test suite */
	Suite *suite = suite_create("bstr-utf8");
	/* Core tests */
	TCase *core = tcase_create("Core");
	tcase_add_test(core, core_000);
	tcase_add_test(core, core_001);
	tcase_add_test(core, core_002);
	tcase_add_test(core, core_003);
	tcase_add_test(core, core_004);
	tcase_add_test(core, core_005);
	tcase_add_test(core, core_006);
	tcase_add_test(core, core_007);
	tcase_add_test(core, core_008);
	tcase_add_test(core, core_009);
	tcase_add_test(core, core_010);
	tcase_add_test(core, core_011);
	tcase_add_test(core, core_012);
	tcase_add_test(core, core_013);
	tcase_add_test(core, core_014);
	suite_add_tcase(suite, core);
	/* Run tests */
	SRunner *runner = srunner_create(suite);
	srunner_run_all(runner, CK_ENV);
	int number_failed = srunner_ntests_failed(runner);
	srunner_free(runner);
	return (0 == number_failed) ? EXIT_SUCCESS : EXIT_FAILURE;
}
