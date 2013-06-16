/* Copyright 2002-2010 Paul Hsieh
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
 * This file is the C unit test for the bstraux module of Bstrlib.
 */

#include "bstraux.h"
#include "bstrlib.h"
#include <check.h>
#include <stdio.h>
#include <stdlib.h>

static int
tWrite(const void *buf, size_t elsize, size_t nelem, void *parm)
{
	bstring b = (bstring) parm;
	size_t i;
	if (NULL == b || NULL == buf || 0 == elsize || 0 == nelem) {
		return -__LINE__;
	}
	for (i = 0; i < nelem; ++i) {
		if (0 > bcatblk(b, buf, elsize)) {
			break;
		}
		buf = (const void *)(elsize + (const char *)buf);
	}
	return (int)i;
}

START_TEST(core_000)
{
	int ret = 0;
	bstring s, t;
	struct bwriteStream *ws;
	s = bfromcstr("");
	ck_assert(s != NULL);
	ws = bwsOpen((bNwrite)tWrite, s);
	ck_assert(ws != NULL);
	(void)bwsBuffLength(ws, 8);
	ret = bwsBuffLength(ws, 0);
	ck_assert_int_eq(ret, 8);
	ret = bwsWriteBlk(ws, bsStaticBlkParms("Hello "));
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseqcstr(s, "");
	ck_assert_int_eq(ret, 1);
	ret = bwsWriteBlk(ws, bsStaticBlkParms("World\n"));
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseqcstr(s, "Hello Wo");
	ck_assert_int_eq(ret, 1);
	t = bwsClose(ws);
	ck_assert(t == s);
	ret = biseqcstr(s, "Hello World\n");
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(s);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

START_TEST(core_001)
{
	struct tagbstring t = bsStatic("Hello world");
	bstring b, c, d;
	int ret = 0;
	b = bTail(&t, 5);
	ck_assert(b != NULL);
	c = bHead(&t, 5);
	ck_assert(c != NULL);
	ret = biseqcstr(b, "world");
	ck_assert_int_eq(ret, 1);
	ret = biseqcstr(c, "Hello");
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	b = bTail(&t, 0);
	ck_assert(b != NULL);
	c = bHead(&t, 0);
	ck_assert(c != NULL);
	ret = biseqcstr(b, "");
	ck_assert_int_eq(ret, 1);
	ret = biseqcstr(c, "");
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	d = bstrcpy(&t);
	ck_assert(d != NULL);
	b = bTail(d, 5);
	ck_assert(b != NULL);
	c = bHead(d, 5);
	ck_assert(c != NULL);
	ret = biseqcstr(b, "world");
	ck_assert_int_eq(ret, 1);
	ret = biseqcstr(c, "Hello");
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(d);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

START_TEST(core_002)
{
	struct tagbstring t = bsStatic("Hello world");
	int ret = 0;
	bstring b;
	ret = bSetChar(&t, 4, ',');
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bSetChar(b = bstrcpy(&t), 4, ',');
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "Hell, world");
	ck_assert_int_eq(ret, 1);
	ret = bSetChar(b, -1, 'x');
	ck_assert_int_eq(ret, BSTR_ERR);
	b->slen = 2;
	ret = bSetChar(b, 1, 'i');
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "Hi");
	ck_assert_int_eq(ret, 1);
	ret = bSetChar(b, 2, 's');
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "His");
	ck_assert_int_eq(ret, 1);
	ret = bSetChar(b, 1, '\0');
	ck_assert_int_eq(ret, 0);
	ret = blength(b);
	ck_assert_int_eq(ret, 3);
	ret = bchare(b, 0, '?');
	ck_assert_int_eq(ret, 'H');
	ret = bchare(b, 1, '?');
	ck_assert_int_eq(ret, '\0');
	ret = bchare(b, 2, '?');
	ck_assert_int_eq(ret, 's');
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = 0;
	ret = bSetCstrChar(&t, 4, ',');
	ck_assert_int_eq(ret, BSTR_ERR);
	b = bstrcpy(&t);
	ck_assert(b != NULL);
	ret = bSetCstrChar(b, 4, ',');
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "Hell, world");
	ck_assert_int_eq(ret, 1);
	ret = bSetCstrChar(b, -1, 'x');
	ck_assert_int_eq(ret, BSTR_ERR);
	b->slen = 2;
	ret = bSetCstrChar(b, 1, 'i');
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "Hi");
	ck_assert_int_eq(ret, 1);
	ret = bSetCstrChar(b, 2, 's');
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "His");
	ck_assert_int_eq(ret, 1);
	ret = bSetCstrChar(b, 1, '\0');
	ck_assert_int_eq(ret, 0);
	ret = blength(b);
	ck_assert_int_eq(ret, 1);
	ret = bchare(b, 0, '?');
	ck_assert_int_eq(ret, 'H');
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

START_TEST(core_003)
{
	struct tagbstring t = bsStatic("Hello world");
	bstring b;
	int ret = 0;
	ret = bFill(&t, 'x', 7);
	ck_assert_int_eq(ret, BSTR_ERR);
	b = bstrcpy(&t);
	ck_assert(b != NULL);
	ret = bFill(b, 'x', 7);
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "xxxxxxx");
	ck_assert_int_eq(ret, 1);
	ret = bFill(b, 'x', -1);
	ck_assert(ret < 0);
	ret = bFill(b, 'x', 0);
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "");
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

START_TEST(core_004)
{
	struct tagbstring t = bsStatic("foo");
	int ret = 0;
	bstring b;
	ret = bReplicate(&t, 4);
	ck_assert_int_eq(ret, BSTR_ERR);
	b = bstrcpy(&t);
	ck_assert(b != NULL);
	ret = bReplicate(b, -1);
	ck_assert_int_eq(ret, BSTR_ERR);
	ret = bReplicate(b, 4);
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "foofoofoofoo");
	ck_assert_int_eq(ret, 1);
	ret = bReplicate(b, 0);
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "");
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

START_TEST(core_005)
{
	struct tagbstring t = bsStatic("Hello world");
	int ret = 0;
	bstring b;
	ret = bReverse(&t);
	ck_assert(ret < 0);
	b = bstrcpy(&t);
	ck_assert(b != NULL);
	ret = bReverse(b);
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "dlrow olleH");
	ck_assert_int_eq(ret, 1);
	b->slen = 0;
	ret = bReverse(b);
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "");
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

START_TEST(core_006)
{
	struct tagbstring t = bsStatic("Hello world");
	int ret = 0;
	bstring b;
	ret = bInsertChrs(&t, 6, 4, 'x', '?');
	ck_assert(ret < 0);
	b = bstrcpy(&t);
	ck_assert(b != NULL);
	ret = bInsertChrs(b, 6, 4, 'x', '?');
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "Hello xxxxworld");
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

START_TEST(core_007)
{
	struct tagbstring t = bsStatic("  i am  ");
	int ret = 0;
	bstring b;
	ret = bJustifyLeft(&t, ' ');
	ck_assert(ret < 0);
	ret = bJustifyRight(&t, 8, ' ');
	ck_assert(ret < 0);
	ret = bJustifyMargin(&t, 8, ' ');
	ck_assert(ret < 0);
	ret = bJustifyCenter(&t, 8, ' ');
	ck_assert(ret < 0);
	b = bstrcpy(&t);
	ck_assert(b != NULL);
	ret = bJustifyLeft(b, ' ');
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseqcstr(b, "i am");
	ck_assert_int_eq(ret, 1);
	ret = bJustifyRight(b, 8, ' ');
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseqcstr(b, "    i am");
	ck_assert_int_eq(ret, 1);
	ret = bJustifyMargin(b, 8, ' ');
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseqcstr(b, "i     am");
	ck_assert_int_eq(ret, 1);
	ret = bJustifyCenter(b, 8, ' ');
	ck_assert_int_eq(ret, BSTR_OK);
	ret = biseqcstr(b, "  i am");
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

START_TEST(core_008)
{
	struct tagbstring t = bsStatic("Hello world");
	int ret = 0;
	bstring b;
	char *c;
	c = bStr2NetStr(&t);
	ck_assert(c != NULL);
	ret = strcmp(c, "11:Hello world,");
	ck_assert_int_eq(ret, 0);
	b = bNetStr2Bstr(c);
	ck_assert(b != NULL);
	ret = biseq(b, &t);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bcstrfree(c);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

START_TEST(core_009)
{
	struct tagbstring t = bsStatic("Hello world");
	int err, ret = 0;
	bstring b, c;
	b = bBase64Encode(&t);
	ck_assert(b != NULL);
	ret += 0 >= biseqcstr(b, "SGVsbG8gd29ybGQ=");
	c = bBase64DecodeEx(b, &err);
	ck_assert(b != NULL);
	ck_assert_int_eq(err, 0);
	ret += 0 >= biseq(c, &t);
	ck_assert_int_eq(ret, 0);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

START_TEST(core_010)
{
	struct tagbstring t = bsStatic("Hello world");
	int err, ret = 0;
	bstring b, c;
	b = bUuEncode(&t);
	ck_assert(b != NULL);
	ret = biseqcstr(b, "+2&5L;&\\@=V]R;&0`\r\n");
	ck_assert_int_eq(ret, 1);
	c = bUuDecodeEx(b, &err);
	ck_assert(c != NULL);
	ck_assert_int_eq(err, 0);
	ret = biseq(c, &t);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

START_TEST(core_011)
{
	struct tagbstring t = bsStatic("Hello world");
	unsigned char Ytstr[] = {
		0x72, 0x8f, 0x96, 0x96, 0x99, 0x4a,
		0xa1, 0x99, 0x9c, 0x96, 0x8e
	};
	bstring b, c;
	int ret = 0;
	b = bYEncode(&t);
	ck_assert(b != NULL);
	ck_assert_int_eq(ret, 0);
	ret = bisstemeqblk(b, Ytstr, 11);
	ck_assert_int_eq(ret, 1);
	c = bYDecode(b);
	ck_assert(c != NULL);
	ret = biseq(c, &t);
	ck_assert_int_eq(ret, 1);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
	ret = bdestroy(c);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

START_TEST(core_012)
{
	struct tagbstring t = bsStatic("Hello world");
	struct bStream * s;
	int ret = 0;
	bstring b, c;
	s = bsFromBstr(&t);
	ck_assert(s != NULL);
	b = bfromcstr("");
	ck_assert(b != NULL);
	ret = bsread(b, s, 6);
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "Hello ");
	ck_assert_int_eq(ret, 1);
	if (b) {
		b->slen = 0;
	}
	ret = bsread(b, s, 6);
	ck_assert_int_eq(ret, 0);
	ret = biseqcstr(b, "world");
	ck_assert_int_eq(ret, 1);
	c = bsclose(s);
	ck_assert(c == NULL);
	ret = bdestroy(b);
	ck_assert_int_eq(ret, BSTR_OK);
}
END_TEST

struct vfgetc {
	int ofs;
	bstring base;
};

static int
core13_fgetc(void *ctx)
{
	struct vfgetc * vctx = (struct vfgetc *) ctx;
	int c;
	if (NULL == vctx || NULL == vctx->base) {
		return EOF;
	}
	if (vctx->ofs >= blength (vctx->base)) {
		return EOF;
	}
	c = bchare(vctx->base, vctx->ofs, EOF);
	vctx->ofs++;
	return c;
}

START_TEST(core_013)
{
	struct tagbstring t0 = bsStatic("Random String");
	struct vfgetc vctx;
	bstring b;
	int ret = 0;
	int i;
	for (i = 0; i < 1000; i++) {
		vctx.ofs = 0;
		vctx.base = &t0;
		b = bSecureInput(INT_MAX, '\n', (bNgetc)core13_fgetc, &vctx);
		ret = biseq(b, &t0);
		ck_assert_int_eq(ret, 1);
		bSecureDestroy(b);
	}
}
END_TEST

int
main(void)
{
	/* Build test suite */
	Suite *suite = suite_create("bstr-aux");
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
	suite_add_tcase(suite, core);
	/* Run tests */
	SRunner *runner = srunner_create(suite);
	srunner_run_all(runner, CK_ENV);
	int number_failed = srunner_ntests_failed(runner);
	srunner_free(runner);
	return (0 == number_failed) ? EXIT_SUCCESS : EXIT_FAILURE;
}
