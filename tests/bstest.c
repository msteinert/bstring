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
 * This file is the C unit test for Bstrlib.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bstraux.h"
#include "bstrlib.h"
#include <check.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void
test0_0(const char *s, const char *res)
{
	int ret = 0;
	bstring b0 = bfromcstr(s);
	if (s) {
		if (b0 == NULL || res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = strlen(res);
		ck_assert_int_eq(b0->slen, ret);
		ret = memcmp(res, b0->data, b0->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b0->data[b0->slen], '\0');
		ret = bdestroy(b0);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ck_assert(res == NULL);
	}
}

static void
test0_1(const char *s, int len, const char *res)
{
	int ret = 0;
	bstring b0 = bfromcstralloc(len, s);
	if (s) {
		if (b0 == NULL || res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = strlen(res);
		ck_assert_int_eq(b0->slen, ret);
		ret = memcmp(res, b0->data, b0->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b0->data[b0->slen], '\0');
		ck_assert(b0->mlen >= len);
		ret = bdestroy(b0);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ck_assert(res == NULL);
	}
}

#define EMPTY_STRING ""

#define SHORT_STRING "bogus"

#define EIGHT_CHAR_STRING "Waterloo"

#define LONG_STRING  \
	"This is a bogus but reasonably long string.  " \
	"Just long enough to cause some mallocing."

START_TEST(core_000)
{
	/* tests with NULL */
	test0_0(NULL, NULL);
	/* normal operation tests */
	test0_0(EMPTY_STRING, EMPTY_STRING);
	test0_0(SHORT_STRING, SHORT_STRING);
	test0_0(LONG_STRING, LONG_STRING);
	/* tests with NULL */
	test0_1(NULL,  0, NULL);
	test0_1(NULL, 30, NULL);
	/* normal operation tests */
	test0_1(EMPTY_STRING, 0, EMPTY_STRING);
	test0_1(EMPTY_STRING, 30, EMPTY_STRING);
	test0_1(SHORT_STRING, 0, SHORT_STRING);
	test0_1(SHORT_STRING, 30, SHORT_STRING);
	test0_1(LONG_STRING, 0, LONG_STRING);
	test0_1(LONG_STRING, 30, LONG_STRING);
}
END_TEST

static void
test1_0(const void *blk, int len, const char *res)
{
	int ret = 0;
	bstring b0 = blk2bstr(blk, len);
	if (res && b0) {
		ck_assert_int_eq(b0->slen, len);
		ret = memcmp(res, b0->data, len);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b0->data[b0->slen], '\0');
		ret = bdestroy(b0);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ck_assert(b0 == NULL);
	}
}

START_TEST(core_001)
{
	/* tests with NULL */
	test1_0(NULL, 10, NULL);
	test1_0(NULL, 0, NULL);
	test1_0(NULL, -1, NULL);
	/* normal operation tests */
	test1_0(SHORT_STRING, sizeof(SHORT_STRING) - 1, SHORT_STRING);
	test1_0(LONG_STRING, sizeof(LONG_STRING) - 1, LONG_STRING);
	test1_0(LONG_STRING, 5, "This ");
	test1_0(LONG_STRING, 0, "");
	test1_0(LONG_STRING, -1, NULL);
}
END_TEST

static void
test2_0(const bstring b, char z, const unsigned char *res)
{
	int ret = 0;
	char *s = bstr2cstr(b, z);
	if (res && s) {
		ret = strlen(s);
		ck_assert_int_eq(b->slen, ret);
		ret = memcmp(res, b->data, b->slen);
		ck_assert_int_eq(ret, 0);
	} else {
		ck_assert(s == NULL);
	}
	free(s);
}

/* test input used below */
struct tagbstring emptyBstring = bsStatic(EMPTY_STRING);

struct tagbstring shortBstring = bsStatic(SHORT_STRING);

struct tagbstring longBstring  = bsStatic(LONG_STRING);

struct tagbstring badBstring1 = {
	8,
	4,
	(unsigned char *)NULL
};

struct tagbstring badBstring2 = {
	2,
	-5,
	(unsigned char *)SHORT_STRING
};

struct tagbstring badBstring3 = {
	2,
	5,
	(unsigned char *)SHORT_STRING
};

struct tagbstring xxxxxBstring = bsStatic("xxxxx");

START_TEST(core_002)
{
	/* tests with NULL */
	test2_0(NULL, '?', NULL);
	/* normal operation tests */
	test2_0(&emptyBstring, '?', emptyBstring.data);
	test2_0(&shortBstring, '?', shortBstring.data);
	test2_0(&longBstring, '?', longBstring.data);
	test2_0(&badBstring1, '?', NULL);
	test2_0(&badBstring2, '?', NULL);
}
END_TEST

static void
test3_0(const bstring b)
{
	int ret = 0;
	bstring b0 = bstrcpy(b);
	if (!b || !b->data || b->slen < 0) {
		ck_assert(b0 == NULL);
	} else if (b0 == NULL) {
		ck_abort();
	} else {
		ck_assert_int_eq(b0->slen, b->slen);
		ret = memcmp(b->data, b0->data, b->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b0->data[b0->slen], '\0');
		ret = bdestroy(b0);
		ck_assert_int_eq(ret, BSTR_OK);
	}
}

START_TEST(core_003)
{
	/* tests with NULL to make sure that there is NULL propogation */
	test3_0(NULL);
	test3_0(&badBstring1);
	test3_0(&badBstring2);
	/* normal operation tests */
	test3_0(&emptyBstring);
	test3_0(&shortBstring);
	test3_0(&longBstring);
}
END_TEST

static void
test4_0(const bstring b, int left, int len, const char *res)
{
	int ret = 0;
	bstring b0 = bmidstr(b, left, len);
	if (b0 == NULL) {
		ck_assert(!b || !b->data || b->slen < 0 || len < 0);
	} else {
		if (b == NULL || res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		if (len >= 0) {
			ck_assert(b0->slen <= len);
		}
		ret = strlen(res);
		ck_assert_int_eq(b0->slen, ret);
		ret = memcmp(res, b0->data, b0->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b0->data[b0->slen], '\0');
		ret = bdestroy(b0);
		ck_assert_int_eq(ret, BSTR_OK);
	}
}

START_TEST(core_004)
{
	/* tests with NULL to make sure that there is NULL propogation */
	test4_0(NULL, 0, 0, NULL);
	test4_0(NULL, 0, 2, NULL);
	test4_0(NULL, 0, -2, NULL);
	test4_0(NULL, -5, 2, NULL);
	test4_0(NULL, -5, -2, NULL);
	test4_0(&badBstring1, 1, 3, NULL);
	test4_0(&badBstring2, 1, 3, NULL);

	/* normal operation tests on all sorts of subranges */
	test4_0(&emptyBstring, 0, 0, "");
	test4_0(&emptyBstring, 0, -1, "");
	test4_0(&emptyBstring, 1, 3, "");
	test4_0(&shortBstring, 0, 0, "");
	test4_0(&shortBstring, 0, -1, "");
	test4_0(&shortBstring, 1, 3, "ogu");
	test4_0(&shortBstring, -1, 3, "bo");
	test4_0(&shortBstring, -1, 9, "bogus");
	test4_0(&shortBstring, 3, -1, "");
	test4_0(&shortBstring, 9, 3, "");
}
END_TEST

static void
test5_0(bstring b0, const bstring b1, const char *res)
{
	bstring b2;
	int ret = 0;
	if (b0 && b0->data && b0->slen >= 0 &&
	    b1 && b1->data && b1->slen >= 0) {
		b2 = bstrcpy(b0);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = bconcat(b2, b1);
		ck_assert_int_ne(ret, 0);
		ret = biseq(b0, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = bconcat(b2, b1);
		ck_assert_int_eq(b2->slen, b0->slen + b1->slen);
		if (res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = bconcat(b0, b1);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

static void
test5_1(void)
{
	bstring b, c;
	struct tagbstring t;
	int i, ret;
	for (i = 0; i < longBstring.slen; i++) {
		b = bstrcpy(&longBstring);
		if (b == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		c = bstrcpy(&longBstring);
		if (c == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bmid2tbstr(t, b, i, longBstring.slen);
		ret = bconcat(c, &t);
		ck_assert_int_eq(ret, 0);
		ret = bconcat(b, &t);
		ck_assert_int_eq(ret, 0);
		ret = biseq(b, c);
		ck_assert_int_eq(ret, 1);
		ret = bdestroy(b);
		ck_assert_int_eq(ret, BSTR_OK);
		ret = bdestroy(c);
		ck_assert_int_eq(ret, BSTR_OK);
	}
	b = bfromcstr("abcd");
	if (b == NULL) {
		ck_abort();
		return; /* Just a safeguard */
	}
	c = bfromcstr("abcd");
	if (c == NULL) {
		ck_abort();
		return; /* Just a safeguard */
	}
	for (i = 0; i < 100; i++) {
		bmid2tbstr(t, b, 0, 3);
		ret = bcatcstr(c, "abc");
		ck_assert_int_eq(ret, 0);
		ret = bconcat(b, &t);
		ck_assert_int_eq(ret, 0);
		ret = biseq(b, c);
		ck_assert_int_eq(ret, 1);
	}
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
}

START_TEST(core_005)
{
	/* tests with NULL */
	test5_0(NULL, NULL, NULL);
	test5_0(NULL, &emptyBstring, NULL);
	test5_0(&emptyBstring, NULL, "");
	test5_0(&emptyBstring, &badBstring1, NULL);
	test5_0(&emptyBstring, &badBstring2, NULL);
	test5_0(&badBstring1, &emptyBstring, NULL);
	test5_0(&badBstring2, &emptyBstring, NULL);
	/* normal operation tests on all sorts of subranges */
	test5_0(&emptyBstring, &emptyBstring, "");
	test5_0(&emptyBstring, &shortBstring, "bogus");
	test5_0(&shortBstring, &emptyBstring, "bogus");
	test5_0(&shortBstring, &shortBstring, "bogusbogus");
	test5_1();
}
END_TEST

static void
test6_0(bstring b, char c, const char *res)
{
	bstring b0;
	int ret = 0;
	if (b && b->data && b->slen >= 0) {
		b0 = bstrcpy(b);
		if (b0 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b0);
		ret = bconchar(b0, c);
		ck_assert_int_ne(ret, 0);
		ret = biseq(b0, b);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b0);
		ret = bconchar(b0, c);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b0->slen, b->slen + 1);
		if (res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = strlen(res);
		ck_assert_int_eq(b0->slen, ret);
		ret = memcmp(b0->data, res, b0->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b0->data[b0->slen], '\0');
		ret = bdestroy(b0);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = bconchar(b, c);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

START_TEST(core_006)
{
	/* tests with NULL */
	test6_0(NULL, 'x', NULL);
	test6_0(&badBstring1, 'x', NULL);
	test6_0(&badBstring2, 'x', NULL);
	/* normal operation tests on all sorts of subranges */
	test6_0(&emptyBstring, 'x', "x");
	test6_0(&shortBstring, 'x', "bogusx");
}
END_TEST

static void
test7x8_0(int (*fnptr)(const bstring, const bstring),
	  const bstring b0, const bstring b1, int res)
{
	int ret = 0;
	ret = fnptr(b0, b1);
	ck_assert_int_eq(ret, res);
}

static void
test7x8(int (* fnptr)(const bstring, const bstring),
	int retFail, int retLT, int retGT, int retEQ)
{
	/* tests with NULL */
	test7x8_0(fnptr, NULL, NULL, retFail);
	test7x8_0(fnptr, &emptyBstring, NULL, retFail);
	test7x8_0(fnptr, NULL, &emptyBstring, retFail);
	test7x8_0(fnptr, &shortBstring, NULL, retFail);
	test7x8_0(fnptr, NULL, &shortBstring, retFail);
	test7x8_0(fnptr, &badBstring1, &badBstring1, retFail);
	test7x8_0(fnptr, &badBstring2, &badBstring2, retFail);
	test7x8_0(fnptr, &shortBstring, &badBstring2, retFail);
	test7x8_0(fnptr, &badBstring2, &shortBstring, retFail);
	/* normal operation tests on all sorts of subranges */
	test7x8_0(fnptr, &emptyBstring, &emptyBstring, retEQ);
	test7x8_0(fnptr, &shortBstring, &emptyBstring, retGT);
	test7x8_0(fnptr, &emptyBstring, &shortBstring, retLT);
	test7x8_0(fnptr, &shortBstring, &shortBstring, retEQ);
	bstring b = bstrcpy(&shortBstring);
	if (b == NULL) {
		ck_abort();
	} else {
		b->data[1]++;
		test7x8_0(fnptr, b, &shortBstring, retGT);
		int ret = bdestroy(b);
		ck_assert_int_eq(ret, BSTR_OK);
		if (fnptr == biseq) {
			test7x8_0(fnptr, &shortBstring, &longBstring, retGT);
			test7x8_0(fnptr, &longBstring, &shortBstring, retLT);
		} else {
			test7x8_0(fnptr, &shortBstring, &longBstring, 'b' - 'T');
			test7x8_0(fnptr, &longBstring, &shortBstring, 'T' - 'b');
		}
	}
}

START_TEST(core_007)
{
	test7x8(biseq, -1, 0, 0, 1);
}
END_TEST

START_TEST(core_008)
{
	test7x8(bstrcmp, SHRT_MIN, -1, 1, 0);
}
END_TEST

static void
test9_0(const bstring b0, const bstring b1, int n, int res)
{
	int ret = 0;
	ret = bstrncmp(b0, b1, n);
	ck_assert_int_eq(ret, res);
}

START_TEST(core_009)
{
	/* tests with NULL */
	test9_0(NULL, NULL, 0, SHRT_MIN);
	test9_0(NULL, NULL, -1, SHRT_MIN);
	test9_0(NULL, NULL, 1, SHRT_MIN);
	test9_0(&emptyBstring, NULL, 0, SHRT_MIN);
	test9_0(NULL, &emptyBstring, 0, SHRT_MIN);
	test9_0(&emptyBstring, NULL, 1, SHRT_MIN);
	test9_0(NULL, &emptyBstring, 1, SHRT_MIN);
	test9_0(&badBstring1, &badBstring1, 1, SHRT_MIN);
	test9_0(&badBstring2, &badBstring2, 1, SHRT_MIN);
	test9_0(&emptyBstring, &badBstring1, 1, SHRT_MIN);
	test9_0(&emptyBstring, &badBstring2, 1, SHRT_MIN);
	test9_0(&badBstring1, &emptyBstring, 1, SHRT_MIN);
	test9_0(&badBstring2, &emptyBstring, 1, SHRT_MIN);
	/* normal operation tests on all sorts of subranges */
	test9_0(&emptyBstring, &emptyBstring, -1, 0);
	test9_0(&emptyBstring, &emptyBstring, 0, 0);
	test9_0(&emptyBstring, &emptyBstring, 1, 0);
	test9_0(&shortBstring, &shortBstring, -1, 0);
	test9_0(&shortBstring, &shortBstring, 0, 0);
	test9_0(&shortBstring, &shortBstring, 1, 0);
	test9_0(&shortBstring, &shortBstring, 9, 0);
}
END_TEST

static void
test10_0(bstring b, int res, int nochange)
{
	struct tagbstring sb = bsStatic ("<NULL>");
	int x, ret = 0;
	if (b) {
		sb = *b;
	}
	ret = bdestroy(b);
	if (b) {
		if (ret >= 0) {
			/* If the bdestroy was successful we have to assume
			 * the contents were "changed"
			 */
			x = 1;
		} else {
			x = memcmp (&sb, b, sizeof sb);
		}
	} else {
		x = !nochange;
	}
	ck_assert_int_eq(ret, res);
	ck_assert_int_ne(!nochange, !x);
}

START_TEST(core_010)
{
	bstring c = bstrcpy(&shortBstring);
	if (c == NULL) {
		ck_abort();
		return; /* Just a safeguard */
	}
	bstring b = bstrcpy(&emptyBstring);
	if (b == NULL) {
		ck_abort();
		return; /* Just a safeguard */
	}
	/* tests with NULL */
	test10_0(NULL, BSTR_ERR, 1);
	/* protected, constant and regular instantiations on empty or not */
	bwriteprotect(*b);
	bwriteprotect(*c);
	test10_0(b, BSTR_ERR, 1);
	test10_0(c, BSTR_ERR, 1);
	bwriteallow(*b);
	bwriteallow(*c);
	test10_0(b, BSTR_OK, 0);
	test10_0(c, BSTR_OK, 0);
	test10_0(&emptyBstring, BSTR_ERR, 1);
	bwriteallow(emptyBstring);
	test10_0(&emptyBstring, BSTR_ERR, 1);
	test10_0(&shortBstring, BSTR_ERR, 1);
	bwriteallow (emptyBstring);
	test10_0(&shortBstring, BSTR_ERR, 1);
	test10_0(&badBstring1, BSTR_ERR, 1);
	test10_0(&badBstring2, BSTR_ERR, 1);
}
END_TEST

static void
test11_0(bstring s1, int pos, const bstring s2, int res)
{
	int ret = binstr(s1, pos, s2);
	ck_assert_int_eq(ret, res);
}

static void
test11_1(bstring s1, int pos, const bstring s2, int res)
{
	int ret = binstrcaseless(s1, pos, s2);
	ck_assert_int_eq(ret, res);
}

START_TEST(core_011)
{
	int ret;
	bstring b, c;
	test11_0(NULL, 0, NULL, BSTR_ERR);
	test11_0(&emptyBstring, 0, NULL, BSTR_ERR);
	test11_0(NULL, 0, &emptyBstring, BSTR_ERR);
	test11_0(&emptyBstring, 0, &badBstring1, BSTR_ERR);
	test11_0(&emptyBstring, 0, &badBstring2, BSTR_ERR);
	test11_0(&badBstring1, 0, &emptyBstring, BSTR_ERR);
	test11_0(&badBstring2, 0, &emptyBstring, BSTR_ERR);
	test11_0(&badBstring1, 0, &badBstring2, BSTR_ERR);
	test11_0(&badBstring2, 0, &badBstring1, BSTR_ERR);
	test11_0(&emptyBstring, 0, &emptyBstring, 0);
	test11_0(&emptyBstring, 1, &emptyBstring, BSTR_ERR);
	test11_0(&shortBstring, 1, &shortBstring, BSTR_ERR);
	test11_0(&shortBstring, 5, &emptyBstring, 5);
	test11_0(&shortBstring, -1, &shortBstring, BSTR_ERR);
	test11_0(&shortBstring, 0, &shortBstring, 0);
	test11_0(&shortBstring, 0, b = bstrcpy(&shortBstring), 0);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_0(&shortBstring, 0, b = bfromcstr("BOGUS"), BSTR_ERR);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_0(&longBstring, 0, &shortBstring, 10);
	test11_0(&longBstring, 20, &shortBstring, BSTR_ERR);
	test11_0(c = bfromcstr("sssssssssap"), 0, b = bfromcstr("sap"), 8);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_0(c = bfromcstr("sssssssssap"), 3, b = bfromcstr("sap"), 8);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_0(c = bfromcstr("ssssssssssap"), 3, b = bfromcstr("sap"), 9);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_0(c = bfromcstr("sssssssssap"), 0, b = bfromcstr("s"), 0);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_0(c = bfromcstr("sssssssssap"), 3, b = bfromcstr("s"), 3);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_0(c = bfromcstr("sssssssssap"), 0, b = bfromcstr("a"), 9);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_0(c = bfromcstr("sssssssssap"), 5, b = bfromcstr("a"), 9);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_0(c = bfromcstr("sasasasasap"), 0, b = bfromcstr("sap"), 8);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_0(c = bfromcstr("ssasasasasap"), 0, b = bfromcstr("sap"), 9);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_1(NULL, 0, NULL, BSTR_ERR);
	test11_1(&emptyBstring, 0, NULL, BSTR_ERR);
	test11_1(NULL, 0, &emptyBstring, BSTR_ERR);
	test11_1(&emptyBstring, 0, &badBstring1, BSTR_ERR);
	test11_1(&emptyBstring, 0, &badBstring2, BSTR_ERR);
	test11_1(&badBstring1, 0, &emptyBstring, BSTR_ERR);
	test11_1(&badBstring2, 0, &emptyBstring, BSTR_ERR);
	test11_1(&badBstring1, 0, &badBstring2, BSTR_ERR);
	test11_1(&badBstring2, 0, &badBstring1, BSTR_ERR);
	test11_1(&emptyBstring, 0, &emptyBstring, 0);
	test11_1(&emptyBstring, 1, &emptyBstring, BSTR_ERR);
	test11_1(&shortBstring, 1, &shortBstring, BSTR_ERR);
	test11_1(&shortBstring, 5, &emptyBstring, 5);
	test11_1(&shortBstring, -1, &shortBstring, BSTR_ERR);
	test11_1(&shortBstring, 0, &shortBstring, 0);
	test11_1(&shortBstring, 0, b = bstrcpy(&shortBstring), 0);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_1(&shortBstring, 0, b = bfromcstr("BOGUS"), 0);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test11_1(&longBstring, 0, &shortBstring, 10);
	test11_1(&longBstring, 20, &shortBstring, BSTR_ERR);
}
END_TEST

static void
test12_0(bstring s1, int pos, const bstring s2, int res)
{
	int ret = binstrr(s1, pos, s2);
	ck_assert_int_eq(ret, res);
}

static void
test12_1(bstring s1, int pos, const bstring s2, int res)
{
	int ret = binstrrcaseless (s1, pos, s2);
	ck_assert_int_eq(ret, res);
}

START_TEST(core_012)
{
	bstring b;
	int ret = 0;
	test12_0(NULL, 0, NULL, BSTR_ERR);
	test12_0(&emptyBstring, 0, NULL, BSTR_ERR);
	test12_0(NULL, 0, &emptyBstring, BSTR_ERR);
	test12_0(&emptyBstring, 0, &badBstring1, BSTR_ERR);
	test12_0(&emptyBstring, 0, &badBstring2, BSTR_ERR);
	test12_0(&badBstring1, 0, &emptyBstring, BSTR_ERR);
	test12_0(&badBstring2, 0, &emptyBstring, BSTR_ERR);
	test12_0(&badBstring1, 0, &badBstring2, BSTR_ERR);
	test12_0(&badBstring2, 0, &badBstring1, BSTR_ERR);
	test12_0(&emptyBstring, 0, &emptyBstring, 0);
	test12_0(&emptyBstring, 1, &emptyBstring, BSTR_ERR);
	test12_0(&shortBstring, 1, &shortBstring, 0);
	test12_0(&shortBstring, 5, &emptyBstring, 5);
	test12_0(&shortBstring, -1, &shortBstring, BSTR_ERR);
	test12_0(&shortBstring, 0, &shortBstring, 0);
	test12_0(&shortBstring, 0, b = bstrcpy(&shortBstring), 0);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test12_0(&shortBstring, 0, b = bfromcstr("BOGUS"), BSTR_ERR);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test12_0(&longBstring, 0, &shortBstring, BSTR_ERR);
	test12_0(&longBstring, 20, &shortBstring, 10);
	test12_1(NULL, 0, NULL, BSTR_ERR);
	test12_1(&emptyBstring, 0, NULL, BSTR_ERR);
	test12_1(NULL, 0, &emptyBstring, BSTR_ERR);
	test12_1(&emptyBstring, 0, &badBstring1, BSTR_ERR);
	test12_1(&emptyBstring, 0, &badBstring2, BSTR_ERR);
	test12_1(&badBstring1, 0, &emptyBstring, BSTR_ERR);
	test12_1(&badBstring2, 0, &emptyBstring, BSTR_ERR);
	test12_1(&badBstring1, 0, &badBstring2, BSTR_ERR);
	test12_1(&badBstring2, 0, &badBstring1, BSTR_ERR);
	test12_1(&emptyBstring, 0, &emptyBstring, 0);
	test12_1(&emptyBstring, 1, &emptyBstring, BSTR_ERR);
	test12_1(&shortBstring, 1, &shortBstring, 0);
	test12_1(&shortBstring, 5, &emptyBstring, 5);
	test12_1(&shortBstring, -1, &shortBstring, BSTR_ERR);
	test12_1(&shortBstring, 0, &shortBstring, 0);
	test12_1(&shortBstring, 0, b = bstrcpy(&shortBstring), 0);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test12_1(&shortBstring, 0, b = bfromcstr("BOGUS"), 0);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test12_1(&longBstring, 0, &shortBstring, BSTR_ERR);
	test12_1(&longBstring, 20, &shortBstring, 10);
}
END_TEST

static void
test13_0(bstring s1, int pos, const bstring s2, int res)
{
	int ret = binchr(s1, pos, s2);
	ck_assert_int_eq(ret, res);
}

START_TEST(core_013)
{
	bstring b;
	int ret = 0;
	struct tagbstring multipleOs = bsStatic("ooooo");
	test13_0(NULL, 0, NULL, BSTR_ERR);
	test13_0(&emptyBstring, 0, NULL, BSTR_ERR);
	test13_0(NULL, 0, &emptyBstring, BSTR_ERR);
	test13_0(&emptyBstring, 0, &badBstring1, BSTR_ERR);
	test13_0(&emptyBstring, 0, &badBstring2, BSTR_ERR);
	test13_0(&badBstring1, 0, &emptyBstring, BSTR_ERR);
	test13_0(&badBstring2, 0, &emptyBstring, BSTR_ERR);
	test13_0(&badBstring2, 0, &badBstring1, BSTR_ERR);
	test13_0(&badBstring1, 0, &badBstring2, BSTR_ERR);
	test13_0(&emptyBstring, 0, &emptyBstring, BSTR_ERR);
	test13_0(&shortBstring, 0, &emptyBstring, BSTR_ERR);
	test13_0(&shortBstring,  0, &shortBstring, 0);
	test13_0(&shortBstring,  0, &multipleOs, 1);
	test13_0(&shortBstring, 0, b = bstrcpy(&shortBstring), 0);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test13_0(&shortBstring, -1, &shortBstring, BSTR_ERR);
	test13_0(&shortBstring, 10, &shortBstring, BSTR_ERR);
	test13_0(&shortBstring, 1, &shortBstring, 1);
	test13_0(&emptyBstring, 0, &shortBstring, BSTR_ERR);
	test13_0(&xxxxxBstring, 0, &shortBstring, BSTR_ERR);
	test13_0(&longBstring, 0, &shortBstring, 3);
	test13_0(&longBstring, 10, &shortBstring, 10);
}
END_TEST

static void
test14_0(bstring s1, int pos, const bstring s2, int res)
{
	int ret = binchrr(s1, pos, s2);
	ck_assert_int_eq(ret, res);
}

START_TEST(core_014)
{
	bstring b;
	int ret = 0;
	test14_0(NULL, 0, NULL, BSTR_ERR);
	test14_0(&emptyBstring, 0, NULL, BSTR_ERR);
	test14_0(NULL, 0, &emptyBstring, BSTR_ERR);
	test14_0(&emptyBstring, 0, &emptyBstring, BSTR_ERR);
	test14_0(&shortBstring, 0, &emptyBstring, BSTR_ERR);
	test14_0(&emptyBstring, 0, &badBstring1, BSTR_ERR);
	test14_0(&emptyBstring, 0, &badBstring2, BSTR_ERR);
	test14_0(&badBstring1, 0, &emptyBstring, BSTR_ERR);
	test14_0(&badBstring2, 0, &emptyBstring, BSTR_ERR);
	test14_0(&badBstring2, 0, &badBstring1, BSTR_ERR);
	test14_0(&badBstring1, 0, &badBstring2, BSTR_ERR);
	test14_0(&shortBstring,  0, &shortBstring, 0);
	test14_0(&shortBstring, 0, b = bstrcpy(&shortBstring), 0);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test14_0(&shortBstring, -1, &shortBstring, BSTR_ERR);
	test14_0(&shortBstring, 5, &shortBstring, 4);
	test14_0(&shortBstring, 4, &shortBstring, 4);
	test14_0(&shortBstring, 1, &shortBstring, 1);
	test14_0(&emptyBstring, 0, &shortBstring, BSTR_ERR);
	test14_0(&xxxxxBstring, 4, &shortBstring, BSTR_ERR);
	test14_0(&longBstring, 0, &shortBstring, BSTR_ERR);
	test14_0(&longBstring, 10, &shortBstring, 10);
}
END_TEST

static void
test15_0(bstring b0, int pos, const bstring b1, unsigned char fill, char * res)
{
	bstring b2;
	int ret = 0;
	if (b0 && b0->data && b0->slen >= 0 &&
	    b1 && b1->data && b1->slen >= 0) {
		b2 = bstrcpy(b0);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = bsetstr(b2, pos, b1, fill);
		ck_assert_int_ne(ret, 0);
		ret = biseq(b0, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = bsetstr(b2, pos, b1, fill);
		if (b1) {
			if (pos < 0) {
				ck_assert_int_eq(b2->slen, b0->slen);
			} else {
				if (b2->slen != b0->slen + b1->slen) {
					ck_assert_int_eq(b2->slen,
							 pos + b1->slen);
				}
			}
		}
		if (res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ck_assert(!((ret == 0) != (pos >= 0)));
		ret = strlen(res);
		ck_assert(b2->slen >= ret);
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = bsetstr(b0, pos, b1, fill);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

START_TEST(core_015)
{
	/* tests with NULL */
	test15_0(NULL, 0, NULL, (unsigned char) '?', NULL);
	test15_0(NULL, 0, &emptyBstring, (unsigned char) '?', NULL);
	test15_0(&badBstring1, 0, NULL, (unsigned char) '?', NULL);
	test15_0(&badBstring1, 0, &badBstring1, (unsigned char) '?', NULL);
	test15_0(&emptyBstring, 0, &badBstring1, (unsigned char) '?', NULL);
	test15_0(&badBstring1, 0, &emptyBstring, (unsigned char) '?', NULL);
	test15_0(&badBstring2, 0, NULL, (unsigned char) '?', NULL);
	test15_0(&badBstring2, 0, &badBstring2, (unsigned char) '?', NULL);
	test15_0(&emptyBstring, 0, &badBstring2, (unsigned char) '?', NULL);
	test15_0(&badBstring2, 0, &emptyBstring, (unsigned char) '?', NULL);
	/* normal operation tests */
	test15_0(&emptyBstring, 0, &emptyBstring, (unsigned char) '?', "");
	test15_0(&emptyBstring, 5, &emptyBstring, (unsigned char) '?', "?????");
	test15_0(&emptyBstring, 5, &shortBstring, (unsigned char) '?', "?????bogus");
	test15_0(&shortBstring, 0, &emptyBstring, (unsigned char) '?', "bogus");
	test15_0(&emptyBstring, 0, &shortBstring, (unsigned char) '?', "bogus");
	test15_0(&shortBstring, 0, &shortBstring, (unsigned char) '?', "bogus");
	test15_0(&shortBstring, -1, &shortBstring, (unsigned char) '?', "bogus");
	test15_0(&shortBstring, 2, &shortBstring, (unsigned char) '?', "bobogus");
	test15_0(&shortBstring, 6, &shortBstring, (unsigned char) '?', "bogus?bogus");
	test15_0(&shortBstring, 6, NULL, (unsigned char) '?', "bogus?");
}
END_TEST

static void
test16_0(bstring b0, int pos, const bstring b1, unsigned char fill, char *res)
{
	bstring b2;
	int ret = 0;
	if (b0 && b0->data && b0->slen >= 0 &&
	    b1 && b1->data && b1->slen >= 0) {
		b2 = bstrcpy(b0);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = binsert(b2, pos, b1, fill);
		ck_assert_int_ne(ret, 0);
		ret = biseq(b0, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = binsert(b2, pos, b1, fill);
		if (b1) {
			ck_assert(!((pos >= 0) && (b2->slen != b0->slen + b1->slen) && (b2->slen != pos + b1->slen)));
			ck_assert(!((pos < 0) && (b2->slen != b0->slen)));
			ck_assert(!((ret == 0) != (pos >= 0 && pos <= b2->slen)));
			ck_assert(!((ret == 0) != (pos >= 0 && pos <= b2->slen)));
		}
		if (res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = strlen(res);
		ck_assert_int_eq(b2->slen, ret);
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = binsert (b0, pos, b1, fill);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

START_TEST(core_016)
{
	/* tests with NULL */
	test16_0(NULL, 0, NULL, (unsigned char) '?', NULL);
	test16_0(NULL, 0, &emptyBstring, (unsigned char) '?', NULL);
	test16_0(&badBstring1, 0, NULL, (unsigned char) '?', NULL);
	test16_0(&badBstring1, 0, &badBstring1, (unsigned char) '?', NULL);
	test16_0(&emptyBstring, 0, &badBstring1, (unsigned char) '?', NULL);
	test16_0(&badBstring1, 0, &emptyBstring, (unsigned char) '?', NULL);
	test16_0(&badBstring2, 0, NULL, (unsigned char) '?', NULL);
	test16_0(&badBstring2, 0, &badBstring2, (unsigned char) '?', NULL);
	test16_0(&emptyBstring, 0, &badBstring2, (unsigned char) '?', NULL);
	test16_0(&badBstring2, 0, &emptyBstring, (unsigned char) '?', NULL);
	/* normal operation tests */
	test16_0(&emptyBstring,  0, &emptyBstring, (unsigned char) '?', "");
	test16_0(&emptyBstring,  5, &emptyBstring, (unsigned char) '?', "?????");
	test16_0(&emptyBstring,  5, &shortBstring, (unsigned char) '?', "?????bogus");
	test16_0(&shortBstring,  0, &emptyBstring, (unsigned char) '?', "bogus");
	test16_0(&emptyBstring,  0, &shortBstring, (unsigned char) '?', "bogus");
	test16_0(&shortBstring,  0, &shortBstring, (unsigned char) '?', "bogusbogus");
	test16_0(&shortBstring, -1, &shortBstring, (unsigned char) '?', "bogus");
	test16_0(&shortBstring,  2, &shortBstring, (unsigned char) '?', "bobogusgus");
	test16_0(&shortBstring,  6, &shortBstring, (unsigned char) '?', "bogus?bogus");
	test16_0(&shortBstring,  6, NULL, (unsigned char) '?', "bogus");
}
END_TEST

static void
test17_0(bstring s1, int pos, int len, char * res)
{
	bstring b2;
	int ret = 0;
	if (s1 && s1->data && s1->slen >= 0) {
		b2 = bstrcpy(s1);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = bdelete(b2, pos, len);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = biseq(s1, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = bdelete(b2, pos, len);
		ck_assert(!((len >= 0) != (ret == 0)));
		ck_assert(!((b2->slen > s1->slen) || (b2->slen < pos && s1->slen >= pos)));
		if (res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = strlen(res);
		ck_assert_int_eq(b2->slen, ret);
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = bdelete (s1, pos, len);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

START_TEST(core_017)
{
	/* tests with NULL */
	test17_0(NULL, 0, 0, NULL);
	test17_0(&badBstring1, 0, 0, NULL);
	test17_0(&badBstring2, 0, 0, NULL);
	/* normal operation tests */
	test17_0(&emptyBstring, 0, 0, "");
	test17_0(&shortBstring, 1, 3, "bs");
	test17_0(&shortBstring, -1, 3, "gus");
	test17_0(&shortBstring, 1, -3, "bogus");
	test17_0(&shortBstring, 3, 9, "bog");
	test17_0(&shortBstring, 3, 1, "bogs");
	test17_0(&longBstring, 4, 300, "This");
}
END_TEST

static void
test18_0(bstring b, int len, int res, int mlen)
{
	int ret = 0;
	int ol = 0;
	if (b) {
		ol = b->mlen;
	}
	ret = balloc(b, len);
	ck_assert(!((b && b->data && b->slen >=0 && ol > b->mlen)));
	ck_assert_int_eq(ret, res);
	ck_assert(!((b && (mlen > b->mlen || b->mlen == 0))));
}

static void
test18_1(bstring b, int len, int res, int mlen)
{
	int ret = 0;
	ret = ballocmin(b, len);
	if (b && b->data) {
		ck_assert_int_eq(b->mlen, mlen);
	}
	ck_assert_int_eq(ret, res);
}

START_TEST(core_018)
{
	int ret = 0;
	bstring b = bfromcstr("test");
	ck_assert(b != NULL);
	/* tests with NULL */
	test18_0(NULL, 2, BSTR_ERR, 0);
	test18_0(&badBstring1, 2, BSTR_ERR, 0);
	test18_0(&badBstring2, 2, BSTR_ERR, 0);
	/* normal operation tests */
	test18_0(b, 2, 0, b->mlen);
	test18_0(b, -1, BSTR_ERR, b->mlen);
	test18_0(b, 9, 0, 9);
	test18_0(b, 2, 0, 9);
	bwriteprotect(*b);
	test18_0(b, 4, BSTR_ERR, b->mlen);
	bwriteallow(*b);
	test18_0(b, 2, 0, b->mlen);
	test18_0(&emptyBstring, 9, BSTR_ERR, emptyBstring.mlen);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	b = bfromcstr ("test");
	/* tests with NULL */
	test18_1(NULL, 2, BSTR_ERR, 0);
	test18_1(&badBstring1, 2, BSTR_ERR, 0);
	test18_1(&badBstring2, 2, BSTR_ERR, 2);
	/* normal operation tests */
	test18_1(b, 2, 0, b->slen + 1);
	test18_1(b, -1, BSTR_ERR, b->mlen);
	test18_1(b, 9, 0, 9);
	test18_1(b, 2, 0, b->slen + 1);
	test18_1(b, 9, 0, 9);
	bwriteprotect (*b);
	test18_1(b, 4, BSTR_ERR, -1);
	bwriteallow (*b);
	test18_1(b, 2, 0, b->slen + 1);
	test18_1(&emptyBstring, 9, BSTR_ERR, emptyBstring.mlen);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

static void
test19_0(bstring b, int len, const char *res, int erv)
{
	int ret = 0;
	bstring b1 = NULL;
	if (b && b->data && b->slen >= 0) {
		b1 = bstrcpy(b);
		if (b1 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b1);
		ret = bpattern(b1, len);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = biseq(b1, b);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b1);
		ret = bpattern(b1, len);
		ck_assert_int_eq(ret, erv);
		if (res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = strlen(res);
		ck_assert_int_eq(b1->slen, ret);
		ret = memcmp(b1->data, res, b1->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b1->data[b1->slen], '\0');
		ret = bdestroy(b1);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = bpattern(b, len);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

START_TEST(core_019)
{
	/* tests with NULL */
	test19_0(NULL, 0, NULL, BSTR_ERR);
	test19_0(NULL, 5, NULL, BSTR_ERR);
	test19_0(NULL, -5, NULL, BSTR_ERR);
	test19_0(&badBstring1, 5, NULL, BSTR_ERR);
	test19_0(&badBstring2, 5, NULL, BSTR_ERR);
	/* normal operation tests */
	test19_0(&emptyBstring, 0, "", BSTR_ERR);
	test19_0(&emptyBstring, 10, "", BSTR_ERR);
	test19_0(&emptyBstring, -1, "", BSTR_ERR);
	test19_0(&shortBstring, 0, "", 0);
	test19_0(&shortBstring, 12, "bogusbogusbo", 0);
	test19_0(&shortBstring, -1, "bogus", BSTR_ERR);
}
END_TEST

START_TEST(core_020)
{
	int ret = 0;
	bstring b, c;
	/* tests with NULL */
	b = bformat(NULL, 1, 2);
	ck_assert(b == NULL);
	/* normal operation tests */
	b = bformat("%d %s", 1, "xy");
	ck_assert(b != NULL);
	ret = biseq(c = bfromcstr("1 xy"), b);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	b = bformat("%d %s(%s)", 6, c->data, shortBstring.data);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseq(c = bfromcstr("6 1 xy(bogus)"), b);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(c);
	ret = bdestroy(b);
	b = bformat("%s%s%s%s%s%s%s%s",longBstring.data, longBstring.data,
		    longBstring.data, longBstring.data, longBstring.data,
		    longBstring.data, longBstring.data, longBstring.data);
	c = bstrcpy(&longBstring);
	bconcat(c, c);
	bconcat(c, c);
	bconcat(c, c);
	ret = biseq(c, b);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(c);
	ret = bdestroy(b);
	b = bfromcstr("");
	/* tests with NULL */
	ret = bformata(b, NULL, 1, 2);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bformata(&badBstring1, "%d %d", 1, 2);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bformata(b, "%d %d", 1, 2);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseq(c = bfromcstr("1 2"), b);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bformata(b = bfromcstr("x"), "%s%s%s%s%s%s%s%s",
	               longBstring.data, longBstring.data,
	               longBstring.data, longBstring.data,
	               longBstring.data, longBstring.data,
	               longBstring.data, longBstring.data);
	ck_assert_int_eq(ret, BSTR_OK);
	c = bstrcpy(&longBstring);
	bconcat(c, c);
	bconcat(c, c);
	bconcat(c, c);
	binsertch(c, 0, 1, 'x');
	ret = biseq(c, b);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	b = bfromcstr("Initial");
	/* tests with NULL */
	ret = bassignformat(b, NULL, 1, 2);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bassignformat(&badBstring1, "%d %d", 1, 2);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bassignformat(b, "%d %d", 1, 2);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseq(c = bfromcstr("1 2"), b);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bassignformat(b = bfromcstr("x"), "%s%s%s%s%s%s%s%s",
			    longBstring.data, longBstring.data,
			    longBstring.data, longBstring.data,
			    longBstring.data, longBstring.data,
			    longBstring.data, longBstring.data);
	ck_assert_int_eq(ret, BSTR_OK);
	c = bstrcpy(&longBstring);
	ck_assert(c != NULL);
	bconcat(c, c);
	bconcat(c, c);
	bconcat(c, c);
	ret = biseq(c, b);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

static void
test21_0(bstring b, char sc, int ns)
{
	struct bstrList * l;
	int ret = 0;
	if (b && b->data && b->slen >= 0) {
		bstring c;
		struct tagbstring t;
		blk2tbstr(t, &sc, 1);
		l = bsplit (b, sc);
		if (l == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ck_assert_int_eq(ns, l->qty);
		c = bjoin (l, &t);
		ret = biseq(c, b);
		ck_assert_int_eq(ret, 1);
		ret = bdestroy(c);
		ck_assert_int_eq(ret, BSTR_OK);
		ret = bstrListDestroy(l);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		l = bsplit(b, sc);
		ck_assert(l == NULL);
	}
}

static void
test21_1(bstring b, const bstring sc, int ns)
{
	struct bstrList * l;
	int ret = 0;
	if (b && b->data && b->slen >= 0) {
		bstring c;
		l = bsplitstr (b, sc);
		if (l == NULL) {
			ck_abort();
		} else {
			ck_assert_int_eq(ns, l->qty);
			c = bjoin(l, sc);
			ck_assert(c != NULL);
			ret = biseq (c, b);
			ck_assert_int_eq(ret, 1);
			ret = bdestroy(c);
			ck_assert_int_eq(ret, BSTR_OK);
			ret = bstrListDestroy(l);
			ck_assert_int_eq(ret, BSTR_OK);
		}
	} else {
		l = bsplitstr(b, sc);
		ck_assert(l == NULL);
	}
}

START_TEST(core_021)
{
	struct tagbstring is = bsStatic ("is");
	struct tagbstring ng = bsStatic ("ng");
	struct tagbstring delim = bsStatic("aa");
	struct tagbstring beginWithDelim = bsStatic("aaabcdaa1");
	struct tagbstring endWithDelim = bsStatic("1aaabcdaa");
	struct tagbstring conseqDelim = bsStatic("1aaaa1");
	struct tagbstring oneCharLeft = bsStatic("aaaaaaa");
	struct tagbstring allDelim = bsStatic("aaaaaa");
	int ret = 0;
	/* tests with NULL */
	test21_0(NULL, (char) '?', 0);
	test21_0(&badBstring1, (char) '?', 0);
	test21_0(&badBstring2, (char) '?', 0);
	/* normal operation tests */
	test21_0(&emptyBstring, (char) '?', 1);
	test21_0(&shortBstring, (char) 'o', 2);
	test21_0(&shortBstring, (char) 's', 2);
	test21_0(&shortBstring, (char) 'b', 2);
	test21_0(&longBstring, (char) 'o', 9);
	test21_1(NULL, NULL, 0);
	test21_1(&badBstring1, &emptyBstring, 0);
	test21_1(&badBstring2, &emptyBstring, 0);
	/* normal operation tests */
	test21_1(&shortBstring, &emptyBstring, 5);
	test21_1(&longBstring, &is, 3);
	test21_1(&longBstring, &ng, 5);
	/* corner cases */
	test21_1(&emptyBstring, &delim, 1);
	test21_1(&delim, &delim, 2);
	test21_1(&beginWithDelim, &delim, 3);
	test21_1(&endWithDelim, &delim, 3);
	test21_1(&conseqDelim, &delim, 3);
	test21_1(&oneCharLeft, &delim, 4);
	test21_1(&allDelim, &delim, 4);
	struct bstrList * l;
	unsigned char c;
	struct tagbstring t;
	bstring b;
	bstring list[3] = { &emptyBstring, &shortBstring, &longBstring };
	int i;
	blk2tbstr (t, &c, 1);
	for (i = 0; i < 3; i++) {
		c = '\0';
		while (1) {
			l = bsplit(list[i], c);
			ck_assert(l != NULL);
			b = bjoin(l, &t);
			ck_assert(b != NULL);
			ret = biseq(b, list[i]);
			ck_assert_int_eq(ret, 1);
			ret = bdestroy(b);
			ck_assert_int_eq(ret, BSTR_OK);
			ret = bstrListDestroy(l);
			ck_assert_int_eq(ret, BSTR_OK);
			l = bsplitstr(list[i], &t);
			ck_assert(l != NULL);
			b = bjoin(l, &t);
			ck_assert(b != NULL);
			ret = biseq(b, list[i]);
			ck_assert_int_eq(ret, 1);
			ret = bdestroy(b);
			ck_assert_int_eq(ret, BSTR_OK);
			ret = bstrListDestroy(l);
			ck_assert_int_eq(ret, BSTR_OK);
			if (UCHAR_MAX == c) {
				break;
			}
			c++;
		}
	}
}
END_TEST

static void
test22_0(const bstring b, const bstring sep, int ns, ...)
{
	va_list arglist;
	struct bstrList * l;
	int ret = 0;
	if (b && b->data && b->slen >= 0 &&
	    sep && sep->data && sep->slen >= 0) {
		l = bsplits(b, sep);
		if (l) {
			int i;
			va_start(arglist, ns);
			for (i = 0; i < l->qty; i++) {
				char *res;
				res = va_arg(arglist, char *);
				ck_assert(res != NULL);
				ret = strlen(res);
				ck_assert(ret <= l->entry[i]->slen);
				ret = memcmp(l->entry[i]->data, res,
					     l->entry[i]->slen);
				ck_assert_int_eq(ret, 0);
				ck_assert_int_eq(
					l->entry[i]->data[l->entry[i]->slen],
					'\0');
			}
			va_end(arglist);
			ck_assert_int_eq(ns, l->qty);
			ret = bstrListDestroy(l);
			ck_assert_int_eq(ret, BSTR_OK);
		} else {
			ck_assert_int_eq(ns, 0);
		}
	} else {
		l = bsplits(b, sep);
		ck_assert(l == NULL);
	}
}

START_TEST(core_022)
{
	int ret = 0;
	struct tagbstring o = bsStatic("o");
	struct tagbstring s = bsStatic("s");
	struct tagbstring b = bsStatic("b");
	struct tagbstring bs = bsStatic("bs");
	struct tagbstring uo = bsStatic("uo");
	/* tests with NULL */
	test22_0(NULL, &o, 0);
	test22_0(&o, NULL, 0);
	/* normal operation tests */
	test22_0(&emptyBstring, &o, 1, "");
	test22_0(&emptyBstring, &uo, 1, "");
	test22_0(&shortBstring, &emptyBstring, 1, "bogus");
	test22_0(&shortBstring, &o, 2, "b", "gus");
	test22_0(&shortBstring, &s, 2, "bogu", "");
	test22_0(&shortBstring, &b, 2, "" , "ogus");
	test22_0(&shortBstring, &bs, 3, "" , "ogu", "");
	test22_0(&longBstring, &o, 9, "This is a b", "gus but reas",
		 "nably l", "ng string.  Just l", "ng en", "ugh t",
		 " cause s", "me mall", "cing.");
	test22_0(&shortBstring, &uo, 3, "b", "g", "s");
	struct bstrList *l;
	unsigned char c;
	struct tagbstring t;
	bstring bb;
	bstring list[3] = { &emptyBstring, &shortBstring, &longBstring };
	int i;
	blk2tbstr(t, &c, 1);
	for (i = 0; i < 3; i++) {
		c = '\0';
		while (1) {
			bb = bjoin(l = bsplits(list[i], &t), &t);
			ret = biseq(bb, list[i]);
			ck_assert_int_eq(ret, 1);
			ret = bdestroy(bb);
			ck_assert_int_eq(ret, BSTR_OK);
			bstrListDestroy(l);
			if (UCHAR_MAX == c) {
				break;
			}
			c++;
		}
	}
}
END_TEST

struct sbstr {
	int ofs;
	bstring b;
};

static size_t
test23_aux_read(void *buff, size_t elsize, size_t nelem, void *parm)
{
	struct sbstr *sb = (struct sbstr *)parm;
	int els, len;
	if (!parm || elsize == 0 || nelem == 0) {
		return 0;
	}
	len = (int)(nelem * elsize);
	if (len <= 0) {
		return 0;
	}
	if (len + sb->ofs > sb->b->slen) {
		len = sb->b->slen - sb->ofs;
	}
	els = (int)(len / elsize);
	len = (int)(els * elsize);
	if (len > 0) {
		memcpy(buff, sb->b->data + sb->ofs, len);
		sb->ofs += len;
	}
	return els;
}

static int
test23_aux_open(struct sbstr * sb, bstring b)
{
	if (!sb || !b || !b->data) {
		return -__LINE__;
	}
	sb->ofs = 0;
	sb->b = b;
	return 0;
}

static int
test23_aux_splitcb(void * parm, BSTR_UNUSED int ofs, const bstring entry)
{
	bstring b = (bstring)parm;
	if (b->slen > 0) {
		bconchar(b, '|');
	}
	bconcat(b, entry);
	return 0;
}

struct tagBss {
	int first;
	unsigned char sc;
	bstring b;
};

static int
test23_aux_splitcbx(void * parm, BSTR_UNUSED int ofs, const bstring entry)
{
	struct tagBss *p = (struct tagBss *)parm;
	if (!p->first) {
		bconchar(p->b, (char) p->sc);
	} else {
		p->first = 0;
	}
	bconcat(p->b, entry);
	return 0;
}

START_TEST(core_023)
{
	struct tagbstring space = bsStatic(" ");
	struct sbstr sb;
	struct bStream *bs;
	bstring b;
	int l, ret = 0;
	test23_aux_open(&sb, &longBstring);
	bs = bsopen((bNread)NULL, &sb);
	ck_assert(bs == NULL);
	bs = bsopen((bNread)test23_aux_read, &sb);
	ck_assert(bs != NULL);
	ret = bseof(bs);
	ck_assert_int_eq(ret, 0);
	ret = bsbufflength(NULL, -1);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bsbufflength(NULL, 1);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bsbufflength(bs, -1);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bsbufflength(bs, 1);
	ck_assert_int_ne(ret, BSTR_ERR);
	ret = bspeek(NULL, bs);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bsreadln(NULL, bs,(char) '?');
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bsreadln(&emptyBstring, bs,(char) '?');
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bspeek(&emptyBstring, bs);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bspeek(b = bfromcstr(""), bs);
	ck_assert_int_ne(ret, BSTR_ERR);
	ret = bsreadln(b, NULL,(char) '?');
	ck_assert_int_eq(ret, BSTR_ERR);
	b->slen = 0;
	ret = bsreadln(b, bs,(char) '?');
	ck_assert_int_ne(ret, BSTR_ERR);
	ret = bseof(bs);
	ck_assert_int_eq(ret, 1);
	ret = biseq(b, &longBstring);
	ck_assert_int_eq(ret, 1);
	ret = bsunread(bs, b);
	ck_assert_int_ne(ret, BSTR_ERR);
	ret = bseof(bs);
	ck_assert_int_eq(ret, 0);
	b->slen = 0;
	ret = bspeek(b, bs);
	ck_assert_int_ne(ret, BSTR_ERR);
	ret = biseq(b, &longBstring);
	ck_assert_int_eq(ret, 1);
	b->slen = 0;
	ret = bsreadln(b, bs,(char) '?');
	ck_assert_int_ne(ret, BSTR_ERR);
	ret = bseof(bs);
	ck_assert_int_eq(ret, 1);
	ret = biseq(b, &longBstring);
	ck_assert_int_eq(ret, 1);
	bs = bsclose(bs);
	ck_assert(bs != NULL);
	sb.ofs = 0;
	bs = bsopen((bNread)test23_aux_read, &sb);
	ck_assert(bs != NULL);
	b->slen = 0;
	ret = bsreadln(b, bs,(char) '.');
	ck_assert_int_ne(ret, BSTR_ERR);
	l = b->slen;
	ret = bstrncmp(b, &longBstring, l);
	ck_assert_int_eq(ret, 0);
	ck_assert_int_eq(longBstring.data[l-1], '.');
	ret = bsunread(bs, b);
	ck_assert_int_ne(ret, BSTR_ERR);
	b->slen = 0;
	ret = bspeek(b, bs);
	ck_assert_int_ne(ret, BSTR_ERR);
	ret = biseq(b, &longBstring);
	ck_assert_int_eq(ret, 1);
	b->slen = 0;
	ret = bsreadln(b, bs, '.');
	ck_assert_int_ne(ret, BSTR_ERR);
	ck_assert_int_eq(b->slen, l);
	ret = bstrncmp(b, &longBstring, l);
	ck_assert_int_eq(ret, 0);
	ck_assert_int_eq(longBstring.data[l-1], '.');
	bs = bsclose(bs);
	ck_assert(bs != NULL);
	test23_aux_open(&sb, &longBstring);
	bs = bsopen((bNread) test23_aux_read, &sb);
	ck_assert(bs != NULL);
	ret = bseof(bs);
	ck_assert_int_eq(ret, 0);
	b->slen = 0;
	l = bssplitscb(bs, &space, test23_aux_splitcb, b);
	ret = bseof(bs);
	ck_assert_int_eq(ret, 1);
	bs = bsclose(bs);
	ck_assert(bs != NULL);
	for (l = 1; l < 4; l++) {
		char *str;
		for (str = (char *)longBstring.data; *str; str++) {
			test23_aux_open(&sb, &longBstring);
			bs = bsopen((bNread)test23_aux_read, &sb);
			ck_assert(bs != NULL);
			ret = bseof(bs);
			ck_assert_int_eq(ret, 0);
			ret = bsbufflength(bs, l);
			ck_assert(0 <= ret);
			b->slen = 0;
			while (0 == bsreadlna(b, bs, *str))
				;
			ret = biseq(b, &longBstring);
			ck_assert_int_eq(ret, 1);
			ret = bseof(bs);
			ck_assert_int_eq(ret, 1);
			bs = bsclose(bs);
			ck_assert(bs != NULL);
		}
	}
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	unsigned char c;
	struct tagbstring t;
	bstring list[3] = { &emptyBstring, &shortBstring, &longBstring };
	int i;
	blk2tbstr(t, &c, 1);
	for (i = 0; i < 3; i++) {
		c = '\0';
		while (1) {
			struct tagBss bss;
			bss.sc = c;
			bss.b = bfromcstr("");
			ck_assert(bss.b != NULL);
			bss.first = 1;
			test23_aux_open(&sb, list[i]);
			bs = bsopen((bNread)test23_aux_read, &sb);
			ck_assert(bs != NULL);
			bssplitscb(bs, &t, test23_aux_splitcbx, &bss);
			bs = bsclose(bs);
			ck_assert(bs != NULL);
			ret = biseq(bss.b, list[i]);
			ck_assert_int_eq(ret, 1);
			ret = bdestroy(bss.b);
			ck_assert_int_eq(ret, BSTR_OK);
			if (UCHAR_MAX == c) {
				break;
			}
			c++;
		}
		while (1) {
			struct tagBss bss;
			bss.sc = c;
			bss.b = bfromcstr("");
			ck_assert(bss.b != NULL);
			bss.first = 1;
			test23_aux_open(&sb, list[i]);
			bs = bsopen((bNread) test23_aux_read, &sb);
			ck_assert(bs != NULL);
			bssplitstrcb(bs, &t, test23_aux_splitcbx, &bss);
			bs = bsclose(bs);
			ck_assert(bs != NULL);
			ret = biseq(bss.b, list[i]);
			ck_assert_int_eq(ret, 1);
			ret = bdestroy(bss.b);
			ck_assert_int_eq(ret, BSTR_OK);
			if (UCHAR_MAX == c) {
				break;
			}
			c++;
		}
	}
}
END_TEST

static void
test24_0(bstring s1, int pos, const bstring s2, int res)
{
	int ret = bninchr(s1, pos, s2);
	ck_assert_int_eq(ret, res);
}

START_TEST(core_024)
{
	bstring b;
	int ret = 0;
	test24_0(NULL, 0, NULL, BSTR_ERR);
	test24_0(&emptyBstring, 0, NULL, BSTR_ERR);
	test24_0(NULL, 0, &emptyBstring, BSTR_ERR);
	test24_0(&shortBstring, 3, &badBstring1, BSTR_ERR);
	test24_0(&badBstring1, 3, &shortBstring, BSTR_ERR);
	test24_0(&emptyBstring, 0, &emptyBstring, BSTR_ERR);
	test24_0(&shortBstring, 0, &emptyBstring, BSTR_ERR);
	test24_0(&shortBstring, 0, &shortBstring, BSTR_ERR);
	test24_0(&shortBstring, 1, &shortBstring, BSTR_ERR);
	test24_0(&longBstring, 3, &shortBstring, 4);
	b = bstrcpy(&shortBstring);
	ck_assert(b != NULL);
	test24_0(&longBstring, 3, b, 4);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test24_0(&longBstring, -1, &shortBstring, BSTR_ERR);
	test24_0(&longBstring, 1000, &shortBstring, BSTR_ERR);
	test24_0(&xxxxxBstring, 0, &shortBstring, 0);
	test24_0(&xxxxxBstring, 1, &shortBstring, 1);
	test24_0(&emptyBstring, 0, &shortBstring, BSTR_ERR);
	test24_0(&longBstring, 0, &shortBstring, 0);
	test24_0(&longBstring, 10, &shortBstring, 15);
}
END_TEST

static void
test25_0(bstring s1, int pos, const bstring s2, int res)
{
	int ret = bninchrr(s1, pos, s2);
	ck_assert_int_eq(ret, res);
}

START_TEST(core_025)
{
	bstring b;
	int ret = 0;
	test25_0(NULL, 0, NULL, BSTR_ERR);
	test25_0(&emptyBstring, 0, NULL, BSTR_ERR);
	test25_0(NULL, 0, &emptyBstring, BSTR_ERR);
	test25_0(&emptyBstring, 0, &emptyBstring, BSTR_ERR);
	test25_0(&shortBstring, 0, &emptyBstring, BSTR_ERR);
	test25_0(&shortBstring, 0, &badBstring1, BSTR_ERR);
	test25_0(&badBstring1, 0, &shortBstring, BSTR_ERR);
	test25_0(&shortBstring,  0, &shortBstring, BSTR_ERR);
	test25_0(&shortBstring,  4, &shortBstring, BSTR_ERR);
	test25_0(&longBstring, 10, &shortBstring, 9);
	b = bstrcpy(&shortBstring);
	ck_assert(b != NULL);
	test25_0(&longBstring, 10, b, 9);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	test25_0(&xxxxxBstring, 4, &shortBstring, 4);
	test25_0(&emptyBstring, 0, &shortBstring, BSTR_ERR);
}
END_TEST

static void
test26_0(bstring b0, int pos, int len, const bstring b1,
	 unsigned char fill, char *res)
{
	bstring b2;
	int ret = 0;
	if (b0 && b0->data && b0->slen >= 0 &&
	    b1 && b1->data && b1->slen >= 0) {
		b2 = bstrcpy(b0);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = breplace(b2, pos, len, b1, fill);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = biseq(b0, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = breplace(b2, pos, len, b1, fill);
		if (b1) {
			ck_assert(!(((ret == 0) !=
				     (pos >= 0 && pos <= b2->slen))));
		}
		if (res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = strlen(res);
		ck_assert(b2->slen >= ret);
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = breplace (b0, pos, len, b1, fill);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

START_TEST(core_026)
{
	/* tests with NULL */
	test26_0(NULL, 0, 0, NULL, '?', NULL);
	test26_0(NULL, 0, 0, &emptyBstring, '?', NULL);
	test26_0(&badBstring1, 1, 3, &shortBstring, '?', NULL);
	test26_0(&shortBstring, 1, 3,  &badBstring1, '?', NULL);
	/* normal operation tests */
	test26_0(&emptyBstring, 0, 0, &emptyBstring, '?', "");
	test26_0(&emptyBstring, 5, 0, &emptyBstring, '?', "?????");
	test26_0(&emptyBstring, 5, 0, &shortBstring, '?', "?????bogus");
	test26_0(&shortBstring, 0, 0, &emptyBstring, '?', "bogus");
	test26_0(&emptyBstring, 0, 0, &shortBstring, '?', "bogus");
	test26_0(&shortBstring, 0, 0, &shortBstring, '?', "bogusbogus");
	test26_0(&shortBstring, 1, 3, &shortBstring, '?', "bboguss");
	test26_0(&shortBstring, 3, 8, &shortBstring, '?', "bogbogus");
	test26_0(&shortBstring, -1, 0, &shortBstring, '?', "bogus");
	test26_0(&shortBstring, 2, 0, &shortBstring, '?', "bobogusgus");
	test26_0(&shortBstring, 6, 0, &shortBstring, '?', "bogus?bogus");
	test26_0(&shortBstring, 6, 0, NULL, '?', "bogus");
}
END_TEST

static void
test27_0(bstring b0, const bstring b1, const char *res)
{
	bstring b2;
	int ret = 0;
	if (b0 && b0->data && b0->slen >= 0 &&
	    b1 && b1->data && b1->slen >= 0) {
		b2 = bstrcpy(b0);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = bassign(b2, b1);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = biseq(b0, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = bassign(b2, b1);
		if (b1) {
			ck_assert_int_eq(b2->slen, b1->slen);
		}
		ck_assert(!(((0 != ret) && (b1 != NULL)) ||
			    ((0 == ret) && (b1 == NULL))));
		if (res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = strlen(res);
		ck_assert_int_eq(b2->slen, ret);
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = bassign(b0, b1);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

START_TEST(core_027)
{
	/* tests with NULL */
	test27_0(NULL, NULL, NULL);
	test27_0(NULL, &emptyBstring, NULL);
	test27_0(&emptyBstring, NULL, "");
	test27_0(&badBstring1, &emptyBstring, NULL);
	test27_0(&badBstring2, &emptyBstring, NULL);
	test27_0(&emptyBstring, &badBstring1, NULL);
	test27_0(&emptyBstring, &badBstring2, NULL);
	/* normal operation tests on all sorts of subranges */
	test27_0(&emptyBstring, &emptyBstring, "");
	test27_0(&emptyBstring, &shortBstring, "bogus");
	test27_0(&shortBstring, &emptyBstring, "");
	test27_0(&shortBstring, &shortBstring, "bogus");
}
END_TEST

static void
test28_0(bstring s1, int c, int res)
{
	int ret = bstrchr (s1, c);
	ck_assert_int_eq(ret, res);
}

static void
test28_1(bstring s1, int c, int res)
{
	int ret = bstrrchr (s1, c);
	ck_assert_int_eq(ret, res);
}

static void
test28_2(bstring s1, int c, int pos, int res)
{
	int ret = bstrchrp (s1, c, pos);
	ck_assert_int_eq(ret, res);
}

static void
test28_3(bstring s1, int c, int pos, int res)
{
	int ret = bstrrchrp (s1, c, pos);
	ck_assert_int_eq(ret, res);
}

START_TEST(core_028)
{
	test28_0(NULL, 0, BSTR_ERR);
	test28_0(&badBstring1, 'b', BSTR_ERR);
	test28_0(&badBstring2, 's', BSTR_ERR);
	test28_0(&emptyBstring, 0, BSTR_ERR);
	test28_0(&shortBstring, 0, BSTR_ERR);
	test28_0(&shortBstring, 'b', 0);
	test28_0(&shortBstring, 's', 4);
	test28_0(&shortBstring, 'q', BSTR_ERR);
	test28_0(&xxxxxBstring, 0, BSTR_ERR);
	test28_0(&xxxxxBstring, 'b', BSTR_ERR);
	test28_0(&longBstring, 'i', 2);
	test28_1(NULL, 0, BSTR_ERR);
	test28_1(&badBstring1, 'b', BSTR_ERR);
	test28_1(&badBstring2, 's', BSTR_ERR);
	test28_1(&emptyBstring, 0, BSTR_ERR);
	test28_1(&shortBstring, 0, BSTR_ERR);
	test28_1(&shortBstring, 'b', 0);
	test28_1(&shortBstring, 's', 4);
	test28_1(&shortBstring, 'q', BSTR_ERR);
	test28_1(&xxxxxBstring, 0, BSTR_ERR);
	test28_1(&xxxxxBstring, 'b', BSTR_ERR);
	test28_1(&longBstring, 'i', 82);
	test28_2(NULL, 0, 0, BSTR_ERR);
	test28_2(&badBstring1, 'b', 0, BSTR_ERR);
	test28_2(&badBstring2, 's', 0, BSTR_ERR);
	test28_2(&shortBstring, 'b', -1, BSTR_ERR);
	test28_2(&shortBstring, 'b', shortBstring.slen, BSTR_ERR);
	test28_2(&emptyBstring, 0, 0, BSTR_ERR);
	test28_2(&shortBstring, 0, 0, BSTR_ERR);
	test28_2(&shortBstring, 'b', 0, 0);
	test28_2(&shortBstring, 'b', 1, BSTR_ERR);
	test28_2(&shortBstring, 's', 0, 4);
	test28_2(&shortBstring, 'q', 0, BSTR_ERR);
	test28_3(NULL, 0, 0, BSTR_ERR);
	test28_3(&badBstring1, 'b', 0, BSTR_ERR);
	test28_3(&badBstring2, 's', 0, BSTR_ERR);
	test28_3(&shortBstring, 'b', -1, BSTR_ERR);
	test28_3(&shortBstring, 'b', shortBstring.slen, BSTR_ERR);
	test28_3(&emptyBstring, 0, 0, BSTR_ERR);
	test28_3(&shortBstring, 0, 0, BSTR_ERR);
	test28_3(&shortBstring, 'b', 0, 0);
	test28_3(&shortBstring, 'b', shortBstring.slen - 1, 0);
	test28_3(&shortBstring, 's', shortBstring.slen - 1, 4);
	test28_3(&shortBstring, 's', 0, BSTR_ERR);
}
END_TEST

static void
test29_0(bstring b0, char *s, const char *res)
{
	bstring b2;
	int ret = 0;
	if (b0 && b0->data && b0->slen >= 0) {
		b2 = bstrcpy(b0);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = bcatcstr(b2, s);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = biseq(b0, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = bcatcstr(b2, s);
		if (s) {
			ck_assert_int_eq(b2->slen, b0->slen + (int)strlen(s));
		}
		ck_assert(!(((0 != ret) && (s != NULL)) ||
			   ((0 == ret) && (s == NULL))));
		ret = strlen(res);
		ck_assert_int_eq(b2->slen, ret);
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = bcatcstr (b0, s);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

START_TEST(core_029)
{
	/* tests with NULL */
	test29_0(NULL, NULL, NULL);
	test29_0(NULL, "", NULL);
	test29_0(&emptyBstring, NULL, "");
	test29_0(&badBstring1, "bogus", NULL);
	test29_0(&badBstring2, "bogus", NULL);
	/* normal operation tests on all sorts of subranges */
	test29_0(&emptyBstring, "", "");
	test29_0(&emptyBstring, "bogus", "bogus");
	test29_0(&shortBstring, "", "bogus");
	test29_0(&shortBstring, "bogus", "bogusbogus");
}
END_TEST

static void
test30_0(bstring b0, const unsigned char *s, int len, const char *res)
{
	bstring b2;
	int ret = 0;
	if (b0 && b0->data && b0->slen >= 0) {
		b2 = bstrcpy(b0);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = bcatblk(b2, s, len);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = biseq(b0, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = bcatblk(b2, s, len);
		if (s) {
			if (len >= 0) {
				ck_assert_int_eq(b2->slen, b0->slen + len);
			} else {
				ck_assert_int_eq(b2->slen, b0->slen);
			}
		}
		ck_assert(!(((0 != ret) && (s && len >= 0)) ||
			    ((0 == ret) && (s == NULL || len < 0))));
		if (res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = strlen(res);
		ck_assert_int_eq(ret, b2->slen);
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = bcatblk (b0, s, len);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

START_TEST(core_030)
{
	/* tests with NULL */
	test30_0(NULL, NULL, 0, NULL);
	test30_0(NULL, (unsigned char *) "", 0, NULL);
	test30_0(&emptyBstring, NULL, 0, "");
	test30_0(&emptyBstring, NULL, -1, "");
	test30_0(&badBstring1, NULL, 0, NULL);
	test30_0(&badBstring2, NULL, 0, NULL);
	/* normal operation tests on all sorts of subranges */
	test30_0(&emptyBstring, (unsigned char *) "", -1, "");
	test30_0(&emptyBstring, (unsigned char *) "", 0, "");
	test30_0(&emptyBstring, (unsigned char *) "bogus", 5, "bogus");
	test30_0(&shortBstring, (unsigned char *) "", 0, "bogus");
	test30_0(&shortBstring, (unsigned char *) "bogus", 5, "bogusbogus");
	test30_0(&shortBstring, (unsigned char *) "bogus", -1, "bogus");
}
END_TEST

static void
test31_0(bstring b0, const bstring find, const bstring replace,
	 int pos, char *res)
{
	bstring b2;
	int ret = 0;
	if (b0 && b0->data && b0->slen >= 0 &&
	    find && find->data && find->slen >= 0 &&
	    replace && replace->data && replace->slen >= 0) {
		b2 = bstrcpy(b0);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = bfindreplace(b2, find, replace, pos);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = biseq (b0, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = bfindreplace(b2, find, replace, pos);
		ck_assert_int_eq(ret, BSTR_OK);
		ret = strlen(res);
		ck_assert(b2->slen >= ret);
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = bfindreplace (b0, find, replace, pos);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

static void
test31_1(bstring b0, const bstring find, const bstring replace,
	 int pos, char *res)
{
	bstring b2;
	int ret = 0;
	if (b0 && b0->data && b0->slen >= 0 &&
	    find && find->data && find->slen >= 0 &&
	    replace && replace->data && replace->slen >= 0) {
		b2 = bstrcpy(b0);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = bfindreplacecaseless(b2, find, replace, pos);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = biseq(b0, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = bfindreplacecaseless(b2, find, replace, pos);
		ck_assert_int_eq(ret, BSTR_OK);
		if (res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = strlen(res);
		ck_assert(b2->slen >= ret);
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = bfindreplacecaseless (b0, find, replace, pos);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

#define LOTS_OF_S \
	"sssssssssssssssssssssssssssssssss" \
	"sssssssssssssssssssssssssssssssss"

START_TEST(core_031)
{
	struct tagbstring t0 = bsStatic("funny");
	struct tagbstring t1 = bsStatic("weird");
	struct tagbstring t2 = bsStatic("s");
	struct tagbstring t3 = bsStatic("long");
	struct tagbstring t4 = bsStatic("big");
	struct tagbstring t5 = bsStatic("ss");
	struct tagbstring t6 = bsStatic("sstsst");
	struct tagbstring t7 = bsStatic("xx" LOTS_OF_S "xx");
	struct tagbstring t8 = bsStatic("S");
	struct tagbstring t9 = bsStatic("LONG");
	/* tests with NULL */
	test31_0(NULL, NULL, NULL, 0, NULL);
	test31_0(&shortBstring, NULL, &t1, 0, (char *)shortBstring.data);
	test31_0(&shortBstring, &t2, NULL, 0, (char *)shortBstring.data);
	test31_0(&badBstring1, &t2, &t1, 0, NULL);
	test31_0(&badBstring2, &t2, &t1, 0, NULL);
	/* normal operation tests */
	test31_0(&longBstring, &shortBstring, &t0, 0,
		 "This is a funny but reasonably long string.  "
		 "Just long enough to cause some mallocing.");
	test31_0(&longBstring, &t2, &t1, 0,
		 "Thiweird iweird a boguweird but reaweirdonably "
		 "long weirdtring.  Juweirdt long enough to cauweirde "
		 "weirdome mallocing.");
	test31_0(&shortBstring, &t2, &t1, 0, "boguweird");
	test31_0(&shortBstring, &t8, &t1, 0, "bogus");
	test31_0(&longBstring, &t2, &t1, 27,
		 "This is a bogus but reasonably long weirdtring.  "
		 "Juweirdt long enough to cauweirde weirdome mallocing.");
	test31_0(&longBstring, &t3, &t4, 0,
		 "This is a bogus but reasonably big string.  "
		 "Just big enough to cause some mallocing.");
	test31_0(&longBstring, &t9, &t4, 0,
		 "This is a bogus but reasonably long string.  "
		 "Just long enough to cause some mallocing.");
	test31_0(&t6, &t2, &t5, 0, "sssstsssst");
	test31_0(&t7, &t2, &t5, 0, "xx" LOTS_OF_S LOTS_OF_S "xx");
	/* tests with NULL */
	test31_1(NULL, NULL, NULL, 0, NULL);
	test31_1(&shortBstring, NULL, &t1, 0, (char *)shortBstring.data);
	test31_1(&shortBstring, &t2, NULL, 0, (char *)shortBstring.data);
	test31_1(&badBstring1, &t2, &t1, 0, NULL);
	test31_1(&badBstring2, &t2, &t1, 0, NULL);
	/* normal operation tests */
	test31_1(&longBstring, &shortBstring, &t0, 0,
		 "This is a funny but reasonably long string.  "
		 "Just long enough to cause some mallocing.");
	test31_1(&longBstring, &t2, &t1, 0,
		 "Thiweird iweird a boguweird but reaweirdonably "
		 "long weirdtring.  Juweirdt long enough to cauweirde "
		 "weirdome mallocing.");
	test31_1(&shortBstring, &t2, &t1, 0, "boguweird");
	test31_1(&shortBstring, &t8, &t1, 0, "boguweird");
	test31_1(&longBstring, &t2, &t1, 27,
		 "This is a bogus but reasonably long weirdtring.  "
		 "Juweirdt long enough to cauweirde weirdome mallocing.");
	test31_1(&longBstring, &t3, &t4, 0,
		 "This is a bogus but reasonably big string.  "
		 "Just big enough to cause some mallocing.");
	test31_1(&longBstring, &t9, &t4, 0,
		 "This is a bogus but reasonably big string.  "
		 "Just big enough to cause some mallocing.");
	test31_1(&t6, &t2, &t5, 0, "sssstsssst");
	test31_1(&t6, &t8, &t5, 0, "sssstsssst");
	test31_1(&t7, &t2, &t5, 0, "xx" LOTS_OF_S LOTS_OF_S "xx");
}
END_TEST

static void
test32_0(const bstring b, const char *s, int res)
{
	int ret = biseqcstr (b, s);
	ck_assert_int_eq(ret, res);
}

static void
test32_1(const bstring b, const char *s, int res)
{
	int ret = biseqcstrcaseless (b, s);
	ck_assert_int_eq(ret, res);
}

START_TEST(core_032)
{
	int ret;
	bstring b;
	/* tests with NULL */
	test32_0(NULL, NULL, BSTR_ERR);
	test32_0(&emptyBstring, NULL, BSTR_ERR);
	test32_0(NULL, "", BSTR_ERR);
	test32_0(&badBstring1, "", BSTR_ERR);
	test32_0(&badBstring2, "bogus", BSTR_ERR);
	/* normal operation tests on all sorts of subranges */
	test32_0(&emptyBstring, "", 1);
	test32_0(&shortBstring, "bogus", 1);
	test32_0(&emptyBstring, "bogus", 0);
	test32_0(&shortBstring, "", 0);
	b = bstrcpy(&shortBstring);
	ck_assert(b != NULL);
	b->data[1]++;
	test32_0(b, (char *)shortBstring.data, 0);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	/* tests with NULL */
	test32_1(NULL, NULL, BSTR_ERR);
	test32_1(&emptyBstring, NULL, BSTR_ERR);
	test32_1(NULL, "", BSTR_ERR);
	test32_1(&badBstring1, "", BSTR_ERR);
	test32_1(&badBstring2, "bogus", BSTR_ERR);
	/* normal operation tests on all sorts of subranges */
	test32_1(&emptyBstring, "", 1);
	test32_1(&shortBstring, "bogus", 1);
	test32_1(&shortBstring, "BOGUS", 1);
	test32_1(&emptyBstring, "bogus", 0);
	test32_1(&shortBstring, "", 0);
	b = bstrcpy(&shortBstring);
	ck_assert(b != NULL);
	b->data[1]++;
	test32_1(b, (char *)shortBstring.data, 0);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

static void
test33_0(bstring b0, const char *res)
{
	bstring b2;
	int ret = 0;
	if (b0 && b0->data && b0->slen >= 0) {
		b2 = bstrcpy(b0);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = btoupper(b2);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = biseq(b0, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = btoupper(b2);
		ck_assert_int_eq(ret, BSTR_OK);
		ck_assert_int_eq(b2->slen, b0->slen);
		ret = strlen(res);
		ck_assert_int_eq(b2->slen, ret);
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = btoupper(b0);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

START_TEST(core_033)
{
	/* tests with NULL */
	test33_0(NULL, NULL);
	test33_0(&badBstring1, NULL);
	test33_0(&badBstring2, NULL);
	/* normal operation tests on all sorts of subranges */
	test33_0(&emptyBstring, "");
	test33_0(&shortBstring, "BOGUS");
	test33_0(&longBstring,
		 "THIS IS A BOGUS BUT REASONABLY LONG STRING.  "
		 "JUST LONG ENOUGH TO CAUSE SOME MALLOCING.");

}
END_TEST

static void
test34_0(bstring b0, const char *res)
{
	bstring b2;
	int ret = 0;
	if (b0 != NULL && b0->data != NULL && b0->slen >= 0) {
		b2 = bstrcpy(b0);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = btolower(b2);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = biseq(b0, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = btolower(b2);
		ck_assert_int_eq(b2->slen, b0->slen);
		ck_assert_int_eq(ret, BSTR_OK);
		if (res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = strlen(res);
		ck_assert_int_eq(b2->slen, ret);
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		bdestroy(b2);
	} else {
		ret = btolower(b0);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

START_TEST(core_034)
{
	/* tests with NULL */
	test34_0(NULL, NULL);
	test34_0(&badBstring1, NULL);
	test34_0(&badBstring2, NULL);
	/* normal operation tests on all sorts of subranges */
	test34_0(&emptyBstring, "");
	test34_0(&shortBstring, "bogus");
	test34_0(&longBstring,
		 "this is a bogus but reasonably long string.  "
		 "just long enough to cause some mallocing.");

}
END_TEST

static void
test35_0(const bstring b0, const bstring b1, int res)
{
	int ret = bstricmp (b0, b1);
	ck_assert_int_eq(ret, res);
}

START_TEST(core_035)
{
	struct tagbstring t0 = bsStatic("bOgUs");
	struct tagbstring t1 = bsStatic("bOgUR");
	struct tagbstring t2 = bsStatic("bOgUt");
	/* tests with NULL */
	test35_0(NULL, NULL, SHRT_MIN);
	test35_0(&emptyBstring, NULL, SHRT_MIN);
	test35_0(NULL, &emptyBstring, SHRT_MIN);
	test35_0(&emptyBstring, &badBstring1, SHRT_MIN);
	test35_0(&badBstring1, &emptyBstring, SHRT_MIN);
	test35_0(&shortBstring, &badBstring2, SHRT_MIN);
	test35_0(&badBstring2, &shortBstring, SHRT_MIN);
	/* normal operation tests on all sorts of subranges */
	test35_0(&emptyBstring, &emptyBstring, 0);
	test35_0(&shortBstring, &t0, 0);
	test35_0(&shortBstring, &t1,
		 tolower(shortBstring.data[4]) - tolower(t1.data[4]));
	test35_0(&shortBstring, &t2,
		 tolower(shortBstring.data[4]) - tolower(t2.data[4]));
	t0.slen++;
	test35_0(&shortBstring, &t0, -(UCHAR_MAX+1));
	test35_0(&t0, &shortBstring, (UCHAR_MAX+1));
}
END_TEST

static void
test36_0(const bstring b0, const bstring b1, int n, int res)
{
	int ret = bstrnicmp(b0, b1, n);
	ck_assert_int_eq(ret, res);
}

START_TEST(core_036)
{
	struct tagbstring t0 = bsStatic("bOgUs");
	struct tagbstring t1 = bsStatic("bOgUR");
	struct tagbstring t2 = bsStatic("bOgUt");
	/* tests with NULL */
	test36_0(NULL, NULL, 0, SHRT_MIN);
	test36_0(&emptyBstring, NULL, 0, SHRT_MIN);
	test36_0(NULL, &emptyBstring, 0, SHRT_MIN);
	test36_0(&emptyBstring, &badBstring1, 0, SHRT_MIN);
	test36_0(&badBstring1, &emptyBstring, 0, SHRT_MIN);
	test36_0(&shortBstring, &badBstring2, 5, SHRT_MIN);
	test36_0(&badBstring2, &shortBstring, 5, SHRT_MIN);
	/* normal operation tests on all sorts of subranges */
	test36_0(&emptyBstring, &emptyBstring, 0, 0);
	test36_0(&shortBstring, &t0, 0, 0);
	test36_0(&shortBstring, &t0, 5, 0);
	test36_0(&shortBstring, &t0, 4, 0);
	test36_0(&shortBstring, &t0, 6, 0);
	test36_0(&shortBstring, &t1, 5, shortBstring.data[4] - t1.data[4]);
	test36_0(&shortBstring, &t1, 4, 0);
	test36_0(&shortBstring, &t1, 6, shortBstring.data[4] - t1.data[4]);
	test36_0(&shortBstring, &t2, 5, shortBstring.data[4] - t2.data[4]);
	test36_0(&shortBstring, &t2, 4, 0);
	test36_0(&shortBstring, &t2, 6, shortBstring.data[4] - t2.data[4]);
	t0.slen++;
	test36_0(&shortBstring, &t0, 5, 0);
	test36_0(&shortBstring, &t0, 6, -(UCHAR_MAX+1));
	test36_0(&t0, &shortBstring, 6, (UCHAR_MAX+1));
}
END_TEST

static void
test37_0(const bstring b0, const bstring b1, int res)
{
	int ret = biseqcaseless (b0, b1);
	ck_assert_int_eq(ret, res);
}

START_TEST(core_037)
{
	struct tagbstring t0 = bsStatic("bOgUs");
	struct tagbstring t1 = bsStatic("bOgUR");
	struct tagbstring t2 = bsStatic("bOgUt");
	/* tests with NULL */
	test37_0(NULL, NULL, BSTR_ERR);
	test37_0(&emptyBstring, NULL, BSTR_ERR);
	test37_0(NULL, &emptyBstring, BSTR_ERR);
	test37_0(&emptyBstring, &badBstring1, BSTR_ERR);
	test37_0(&badBstring1, &emptyBstring, BSTR_ERR);
	test37_0(&shortBstring, &badBstring2, BSTR_ERR);
	test37_0(&badBstring2, &shortBstring, BSTR_ERR);
	/* normal operation tests on all sorts of subranges */
	test37_0(&emptyBstring, &emptyBstring, 1);
	test37_0(&shortBstring, &t0, 1);
	test37_0(&shortBstring, &t1, 0);
	test37_0(&shortBstring, &t2, 0);
}
END_TEST

struct emuFile {
	int ofs;
	bstring contents;
};

static int
test38_aux_bngetc(struct emuFile * f)
{
	int v = EOF;
	if (f) {
		v = bchare(f->contents, f->ofs, EOF);
		if (EOF != v) {
			f->ofs++;
		}
	}
	return v;
}

static size_t
test38_aux_bnread (void *buff, size_t elsize, size_t nelem, struct emuFile *f)
{
	char * b = (char *) buff;
	int v;
	size_t i, j, c = 0;
	if (!f || !b) {
		return c;
	}
	for (i = 0; i < nelem; i++) {
		for (j = 0; j < elsize; j++) {
			v = test38_aux_bngetc(f);
			if (EOF == v) {
				*b = '\0';
				return c;
			} else {
				*b = v;
				b++;
				c++;
			}
		}
	}
	return c;
}

static int
test38_aux_bnopen(struct emuFile *f, bstring b) {
	if (!f || !b) {
		return -__LINE__;
	}
	f->ofs = 0;
	f->contents = b;
	return 0;
}

START_TEST(core_038)
{
	struct emuFile f;
	bstring b0, b1, b2, b3;
	int ret = test38_aux_bnopen(&f, &shortBstring);
	ck_assert_int_eq(ret, 0);
	/* Creation/reads */
#if defined(HAVE_BGETS)
	b0 = bgetstream((bNgetc)test38_aux_bngetc, &f, 'b');
#else
	b0 = bgets((bNgetc)test38_aux_bngetc, &f, 'b');
#endif
	ck_assert(b0 != NULL);
	b1 = bread((bNread)test38_aux_bnread, &f);
	ck_assert(b1 != NULL);
#if defined(HAVE_BGETS)
	b2 = bgetstream((bNgetc)test38_aux_bngetc, &f, '\0');
#else
	b2 = bgets((bNgetc)test38_aux_bngetc, &f, '\0');
#endif
	ck_assert(b2 == NULL);
	b3 = bread((bNread)test38_aux_bnread, &f);
	ck_assert(b3 != NULL);
	ret = biseqcstr(b0, "b");
	ck_assert_int_eq(ret, 1);
	ret = biseqcstr(b1, "ogus");
	ck_assert_int_eq(ret, 1);
	ret = biseqcstr(b3, "");
	ck_assert_int_eq(ret, 1);
	/* Bogus accumulations */
	f.ofs = 0;
	ret = bgetsa(NULL, (bNgetc)test38_aux_bngetc, &f, 'o');
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = breada(NULL, (bNread)test38_aux_bnread, &f);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bgetsa(&shortBstring, (bNgetc)test38_aux_bngetc, &f, 'o');
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = breada(&shortBstring, (bNread)test38_aux_bnread, &f);
	ck_assert_int_eq(ret, BSTR_ERR);
	/* Normal accumulations */
	ret = bgetsa(b0, (bNgetc) test38_aux_bngetc, &f, 'o');
	ck_assert_int_eq(ret, BSTR_OK);
	ret = breada(b1, (bNread) test38_aux_bnread, &f);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseqcstr(b0, "bbo");
	ck_assert_int_eq(ret, 1);
	ret = biseqcstr(b1, "ogusgus");
	ck_assert_int_eq(ret, 1);
	/* Attempt to append past end should do nothing */
	ret = bgetsa(b0, (bNgetc)test38_aux_bngetc, &f, 'o');
	ck_assert_int_eq(ret, BSTR_OK);
	ret = breada(b1, (bNread)test38_aux_bnread, &f);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseqcstr(b0, "bbo");
	ck_assert_int_eq(ret, 1);
	ret = biseqcstr(b1, "ogusgus");
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b0);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b1);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(b2);
	ck_assert_int_eq(ret, BSTR_ERR);
	bdestroy(b3);
	ck_assert_int_eq(ret, BSTR_ERR);
}
END_TEST

static void
test39_0(const bstring b, const bstring lt, const bstring rt, const bstring t)
{
	bstring r;
	int ret = 0;
	ret = bltrimws(NULL);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = brtrimws(NULL);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = btrimws(NULL);
	ck_assert_int_eq(ret, BSTR_ERR);
	r = bstrcpy(b);
	if (r == NULL) {
		ck_abort();
		return; /* Just a safeguard */
	}
	bwriteprotect(*r);
	ret = bltrimws(r);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = brtrimws(r);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = btrimws(r);
	ck_assert_int_eq(ret, BSTR_ERR);
	bwriteallow(*r);
	ret = bltrimws(r);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseq(r, lt);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(r);
	ck_assert_int_eq(ret, BSTR_OK);
	r = bstrcpy(b);
	ck_assert(r != NULL);
	ret = brtrimws(r);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseq(r, rt);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(r);
	ck_assert_int_eq(ret, BSTR_OK);
	r = bstrcpy(b);
	ck_assert(r != NULL);
	ret = btrimws(r);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseq(r, t);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(r);
	ck_assert_int_eq(ret, BSTR_OK);
}

START_TEST(core_039)
{
	struct tagbstring t0 = bsStatic("   bogus string   ");
	struct tagbstring t1 = bsStatic("bogus string   ");
	struct tagbstring t2 = bsStatic("   bogus string");
	struct tagbstring t3 = bsStatic("bogus string");
	struct tagbstring t4 = bsStatic("     ");
	struct tagbstring t5 = bsStatic("");
	test39_0(&t0, &t1, &t2, &t3);
	test39_0(&t1, &t1, &t3, &t3);
	test39_0(&t2, &t3, &t2, &t3);
	test39_0(&t3, &t3, &t3, &t3);
	test39_0(&t4, &t5, &t5, &t5);
	test39_0(&t5, &t5, &t5, &t5);
}
END_TEST

static void
test40_0(bstring b0, const bstring b1, int left, int len, const char *res)
{
	bstring b2;
	int ret = 0;
	if (b0 != NULL && b0->data != NULL && b0->slen >= 0 &&
	    b1 != NULL && b1->data != NULL && b1->slen >= 0) {
		b2 = bstrcpy(b0);
		if (b2 == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		bwriteprotect(*b2);
		ret = bassignmidstr(b2, b1, left, len);
		ck_assert_int_ne(ret, 0);
		ret = biseq(b0, b2);
		ck_assert_int_eq(ret, 1);
		bwriteallow(*b2);
		ret = bassignmidstr(b2, b1, left, len);
		if (b1) {
			ck_assert(!((b2->slen > len) | (b2->slen < 0)));
		}
		ck_assert(!(((0 != ret) && (b1 != NULL)) ||
			    ((0 == ret) && (b1 == NULL))));
		if (res == NULL) {
			ck_abort();
			return; /* Just a safeguard */
		}
		ret = strlen(res);
		ck_assert_int_eq(b2->slen, ret);
		ret = memcmp(b2->data, res, b2->slen);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b2->data[b2->slen], '\0');
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ret = bassignmidstr(b0, b1, left, len);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
}

START_TEST(core_040)
{
	/* tests with NULL */
	test40_0(NULL, NULL, 0, 1, NULL);
	test40_0(NULL, &emptyBstring, 0, 1, NULL);
	test40_0(&emptyBstring, NULL, 0, 1, "");
	test40_0(&badBstring1, &emptyBstring, 0, 1, NULL);
	test40_0(&badBstring2, &emptyBstring, 0, 1, NULL);
	test40_0(&emptyBstring, &badBstring1, 0, 1, NULL);
	test40_0(&emptyBstring, &badBstring2, 0, 1, NULL);
	/* normal operation tests on all sorts of subranges */
	test40_0(&emptyBstring, &emptyBstring, 0, 1, "");
	test40_0(&emptyBstring, &shortBstring, 1, 3, "ogu");
	test40_0(&shortBstring, &emptyBstring, 0, 1, "");
	test40_0(&shortBstring, &shortBstring, 1, 3, "ogu");
	test40_0(&shortBstring, &shortBstring, -1, 4, "bog");
	test40_0(&shortBstring, &shortBstring, 1, 9, "ogus");
	test40_0(&shortBstring, &shortBstring, 9, 1, "");
}
END_TEST

static void
test41_0(bstring b1, int left, int len)
{
	struct tagbstring t;
	bstring b2, b3;
	int ret = 0;
	if (b1 && b1->data && b1->slen >= 0) {
		b2 = bfromcstr("");
		ck_assert(b2 != NULL);
		bassignmidstr(b2, b1, left, len);
		bmid2tbstr(t, b1, left, len);
		b3 = bstrcpy(&t);
		ck_assert(b3 != NULL);
		ret = biseq(&t, b2);
		ck_assert_int_eq(ret, 1);
		ret = bdestroy(b2);
		ck_assert_int_eq(ret, BSTR_OK);
		ret = bdestroy(b3);
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		bmid2tbstr(t, b1, left, len);
		b3 = bstrcpy(&t);
		ck_assert(b3 != NULL);
		ck_assert_int_eq(t.slen, 0);
		ret = bdestroy(b3);
		ck_assert_int_eq(ret, BSTR_OK);
	}
}

START_TEST(core_041)
{
	/* tests with NULL */
	test41_0(NULL, 0, 1);
	test41_0(&emptyBstring, 0, 1);
	test41_0(NULL, 0, 1);
	test41_0(&emptyBstring, 0, 1);
	test41_0(&emptyBstring, 0, 1);
	test41_0(&badBstring1, 0, 1);
	test41_0(&badBstring2, 0, 1);
	/* normal operation tests on all sorts of subranges */
	test41_0(&emptyBstring, 0, 1);
	test41_0(&shortBstring, 1, 3);
	test41_0(&emptyBstring, 0, 1);
	test41_0(&shortBstring, 1, 3);
	test41_0(&shortBstring, -1, 4);
	test41_0(&shortBstring, 1, 9);
	test41_0(&shortBstring, 9, 1);
}
END_TEST

static void
test42_0(const bstring bi, int len, const char *res)
{
	bstring b;
	int ret = 0;
	ret = btrunc(b = bstrcpy (bi), len);
	if (len >= 0) {
		ck_assert_int_eq(ret, BSTR_OK);
	} else {
		ck_assert_int_eq(ret, BSTR_ERR);
	}
	if (res) {
		ret = biseqcstr(b, res);
		ck_assert_int_eq(ret, 1);
	}
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}

START_TEST(core_042)
{
	int ret = 0;
	/* tests with NULL */
	ret = btrunc(NULL, 2);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = btrunc (NULL, 0);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = btrunc (NULL, -1);
	ck_assert_int_eq(ret, BSTR_ERR);
	/* write protected */
	ret = btrunc (&shortBstring,  2);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = btrunc (&shortBstring,  0);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = btrunc (&shortBstring, -1);
	ck_assert_int_eq(ret, BSTR_ERR);
	test42_0(&emptyBstring, 10, "");
	test42_0(&emptyBstring, 0, "");
	test42_0(&emptyBstring, -1, NULL);
	test42_0(&shortBstring, 10, "bogus");
	test42_0(&shortBstring, 3, "bog");
	test42_0(&shortBstring, 0, "");
	test42_0(&shortBstring, -1, NULL);
}
END_TEST

START_TEST(core_043)
{
	static struct tagbstring ts0 = bsStatic("");
	static struct tagbstring ts1 = bsStatic("    ");
	static struct tagbstring ts2 = bsStatic(" abc");
	static struct tagbstring ts3 = bsStatic("abc ");
	static struct tagbstring ts4 = bsStatic(" abc ");
	static struct tagbstring ts5 = bsStatic("abc");
	bstring tstrs[6] = { &ts0, &ts1, &ts2, &ts3, &ts4, &ts5 };
	int ret = 0;
	for (int i = 0; i < 6; i++) {
		struct tagbstring t;
		bstring b;
		btfromblkltrimws(t, tstrs[i]->data, tstrs[i]->slen);
		bltrimws(b = bstrcpy (tstrs[i]));
		ret = biseq(b, &t);
		ck_assert_int_eq(ret, 1);
		ret = bdestroy(b);
		ck_assert_int_eq(ret, BSTR_OK);
		btfromblkrtrimws(t, tstrs[i]->data, tstrs[i]->slen);
		brtrimws(b = bstrcpy(tstrs[i]));
		ret = biseq(b, &t);
		ck_assert_int_eq(ret, 1);
		ret = bdestroy(b);
		ck_assert_int_eq(ret, BSTR_OK);
		btfromblktrimws(t, tstrs[i]->data, tstrs[i]->slen);
		btrimws(b = bstrcpy (tstrs[i]));
		ret = biseq(b, &t);
		ck_assert_int_eq(ret, 1);
		ret = bdestroy(b);
		ck_assert_int_eq(ret, BSTR_OK);
	}
}
END_TEST

static void
test44_0(const char *str)
{
	int ret = 0;
	bstring b = bfromcstr("");
	if (b == NULL) {
		ck_abort();
		return; /* Just a safeguard */
	}
	if (NULL == str) {
		ret = bassigncstr(NULL, "test");
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = bassigncstr(b, NULL);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = bassigncstr(&shortBstring, NULL);
		ck_assert_int_eq(ret, BSTR_ERR);
	} else {
		ret = bassigncstr(NULL, str);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = bassigncstr(b, str);
		ck_assert_int_eq(ret, BSTR_OK);
		ret = strcmp(bdatae(b, ""), str);
		ck_assert_int_eq(ret, 0);
		ret = strlen(str);
		ck_assert_int_eq(b->slen, ret);
		ret = bassigncstr(b, "xxxxx");
		ck_assert_int_eq(ret, BSTR_OK);
		bwriteprotect(*b);
		ret = bassigncstr (b, str);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = strcmp(bdatae(b, ""), "xxxxx");
		ck_assert_int_eq(ret, 0);
		ret = strlen("xxxxx");
		ck_assert_int_eq(b->slen, ret);
		bwriteallow(*b);
		ret = bassigncstr(&shortBstring, str);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = bassigncstr(&shortBstring, str);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}

START_TEST(core_044)
{
	/* tests with NULL */
	test44_0(NULL);
	test44_0(EMPTY_STRING);
	test44_0(SHORT_STRING);
	test44_0(LONG_STRING);
}
END_TEST

static void
test45_0(const char *str)
{
	int ret = 0, len;
	bstring b = bfromcstr("");
	if (b == NULL) {
		ck_abort();
		return; /* Just a safeguard */
	}
	if (!str) {
		ret = bassignblk(NULL, "test", 4);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = bassignblk(b, NULL, 1);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = bassignblk(&shortBstring, NULL, 1);
		ck_assert_int_eq(ret, BSTR_ERR);
	} else {
		len = strlen(str);
		ret = bassignblk(NULL, str, len);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = bassignblk(b, str, len);
		ck_assert_int_eq(ret, BSTR_OK);
		ret = strcmp(bdatae(b, ""), str);
		ck_assert_int_eq(ret, 0);
		ck_assert_int_eq(b->slen, len);
		ret = bassigncstr(b, "xxxxx");
		ck_assert_int_eq(ret, BSTR_OK);
		bwriteprotect(*b);
		ret = bassignblk(b, str, len);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = strcmp(bdatae(b, ""), "xxxxx");
		ck_assert_int_eq(ret, 0);
		ret = strlen("xxxxx");
		ck_assert_int_eq(b->slen, ret);
		bwriteallow(*b);
		ret = bassignblk(&shortBstring, str, len);
		ck_assert_int_eq(ret, BSTR_ERR);
		ret = bassignblk (&shortBstring, str, len);
		ck_assert_int_eq(ret, BSTR_ERR);
	}
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}

START_TEST(core_045)
{
	/* tests with NULL */
	test45_0(NULL);
	test45_0(EMPTY_STRING);
	test45_0(SHORT_STRING);
	test45_0(LONG_STRING);
}
END_TEST

static void
test46_0(const bstring r, bstring b, int count, const char *fmt, ...)
{
	int ret;
	va_list arglist;
	va_start(arglist, fmt);
	ret = bvcformata(b, count, fmt, arglist);
	va_end(arglist);
	if (ret < 0) {
		ck_assert(r == NULL);
	} else {
		ret = biseq(r, b);
		ck_assert_int_eq(ret, 1);
	}
}

static void
test46_1(bstring b, const char * fmt, const bstring r, ...)
{
	int ret;
 	bvformata(ret, b, fmt, r);
	if (ret < 0) {
		ck_assert(r == NULL);
	} else {
		ret = biseq(r, b);
		ck_assert_int_eq(ret, 1);
	}
}

START_TEST(core_046)
{
	bstring b, b2;
	int ret = 0;
	test46_0(NULL, NULL, 8, "[%d]", 15);
	test46_0(NULL, &shortBstring, 8, "[%d]", 15);
	test46_0(NULL, &badBstring1, 8, "[%d]", 15);
	test46_0(NULL, &badBstring2, 8, "[%d]", 15);
	test46_0(NULL, &badBstring3, 8, "[%d]", 15);
	b = bfromcstr("");
	ck_assert(b != NULL);
	test46_0(&shortBstring, b, shortBstring.slen, "%s",
		 (char *)shortBstring.data);
	b->slen = 0;
	test46_0(&shortBstring, b, shortBstring.slen + 1, "%s",
		 (char *)shortBstring.data);
	b->slen = 0;
	test46_0(NULL, b, shortBstring.slen-1, "%s",
		 (char *)shortBstring.data);
	test46_1(NULL, "[%d]", NULL, 15);
	test46_1(&shortBstring, "[%d]", NULL, 15);
	test46_1(&badBstring1, "[%d]", NULL, 15);
	test46_1(&badBstring2, "[%d]", NULL, 15);
	test46_1(&badBstring3, "[%d]", NULL, 15);
	b->slen = 0;
	test46_1(b, "%s", &shortBstring, (char *)shortBstring.data);
	b->slen = 0;
	test46_1(b, "%s", &longBstring, (char *)longBstring.data);
    b->slen = 0;
	b2 = bfromcstr(EIGHT_CHAR_STRING);
	bconcat(b2, b2);
	bconcat(b2, b2);
	bconcat(b2, b2);
	test46_1(b, "%s%s%s%s%s%s%s%s", b2,
	         EIGHT_CHAR_STRING, EIGHT_CHAR_STRING, EIGHT_CHAR_STRING, EIGHT_CHAR_STRING,
	         EIGHT_CHAR_STRING, EIGHT_CHAR_STRING, EIGHT_CHAR_STRING, EIGHT_CHAR_STRING);
	bdestroy(b2);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

int
main(void)
{
	/* Build test suite */
	Suite *suite = suite_create("bstr-core");
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
	tcase_add_test(core, core_015);
	tcase_add_test(core, core_016);
	tcase_add_test(core, core_017);
	tcase_add_test(core, core_018);
	tcase_add_test(core, core_019);
	tcase_add_test(core, core_020);
	tcase_add_test(core, core_021);
	tcase_add_test(core, core_022);
	tcase_add_test(core, core_023);
	tcase_add_test(core, core_024);
	tcase_add_test(core, core_025);
	tcase_add_test(core, core_026);
	tcase_add_test(core, core_027);
	tcase_add_test(core, core_028);
	tcase_add_test(core, core_029);
	tcase_add_test(core, core_030);
	tcase_add_test(core, core_031);
	tcase_add_test(core, core_032);
	tcase_add_test(core, core_033);
	tcase_add_test(core, core_034);
	tcase_add_test(core, core_035);
	tcase_add_test(core, core_036);
	tcase_add_test(core, core_037);
	tcase_add_test(core, core_038);
	tcase_add_test(core, core_039);
	tcase_add_test(core, core_040);
	tcase_add_test(core, core_041);
	tcase_add_test(core, core_042);
	tcase_add_test(core, core_043);
	tcase_add_test(core, core_044);
	tcase_add_test(core, core_045);
	tcase_add_test(core, core_046);
	suite_add_tcase(suite, core);
	/* Run tests */
	SRunner *runner = srunner_create(suite);
	srunner_run_all(runner, CK_ENV);
	int number_failed = srunner_ntests_failed(runner);
	srunner_free(runner);
	return (0 == number_failed) ? EXIT_SUCCESS : EXIT_FAILURE;
}
