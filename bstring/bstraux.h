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

/**
 * \brief C example that implements trivial additional functions
 *
 * This file is not a necessary part of the core bstring library itself, but
 * is just an auxilliary module which includes miscellaneous or trivial
 * functions.
 */

#ifndef BSTRAUX_H
#define BSTRAUX_H

#include <time.h>
#include "bstrlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Safety mechanisms */
#define bstrDeclare(b)               bstring (b) = NULL;
#define bstrFree(b)                  {if ((b) != NULL && (b)->slen >= 0 && (b)->mlen >= (b)->slen) { bdestroy (b); (b) = NULL; }}

/* Backward compatibilty with previous versions of Bstrlib */
#define bAssign(a,b)                 ((bassign)((a), (b)))
#define bSubs(b,pos,len,a,c)         ((breplace)((b),(pos),(len),(a),(unsigned char)(c)))
#define bStrchr(b,c)                 ((bstrchr)((b), (c)))
#define bStrchrFast(b,c)             ((bstrchr)((b), (c)))
#define bCatCstr(b,s)                ((bcatcstr)((b), (s)))
#define bCatBlk(b,s,len)             ((bcatblk)((b),(s),(len)))
#define bCatStatic(b,s)              bCatBlk ((b), ("" s ""), sizeof (s) - 1)
#define bTrunc(b,n)                  ((btrunc)((b), (n)))
#define bReplaceAll(b,find,repl,pos) ((bfindreplace)((b),(find),(repl),(pos)))
#define bUppercase(b)                ((btoupper)(b))
#define bLowercase(b)                ((btolower)(b))
#define bCaselessCmp(a,b)            ((bstricmp)((a), (b)))
#define bCaselessNCmp(a,b,n)         ((bstrnicmp)((a), (b), (n)))
#define bBase64Decode(b)             (bBase64DecodeEx ((b), NULL))
#define bUuDecode(b)                 (bUuDecodeEx ((b), NULL))

/* Unusual functions */
BSTR_PUBLIC struct bStream *
bsFromBstr(const bstring b);

BSTR_PUBLIC bstring
bTail(bstring b, int n);

BSTR_PUBLIC bstring
bHead(bstring b, int n);

BSTR_PUBLIC int
bSetCstrChar(bstring a, int pos, char c);

BSTR_PUBLIC int
bSetChar(bstring b, int pos, char c);

BSTR_PUBLIC int
bFill(bstring a, char c, int len);

BSTR_PUBLIC int
bReplicate(bstring b, int n);

BSTR_PUBLIC int
bReverse(bstring b);

BSTR_PUBLIC int
bInsertChrs(bstring b, int pos, int len, unsigned char c, unsigned char fill);

BSTR_PUBLIC bstring
bStrfTime(const char * fmt, const struct tm * timeptr);

#define bAscTime(t) (bStrfTime ("%c\n", (t)))

#define bCTime(t)   ((t) ? bAscTime (localtime (t)) : NULL)

/* Spacing formatting */
BSTR_PUBLIC int
bJustifyLeft(bstring b, int space);

BSTR_PUBLIC int
bJustifyRight(bstring b, int width, int space);

BSTR_PUBLIC int
bJustifyMargin(bstring b, int width, int space);

BSTR_PUBLIC int
bJustifyCenter(bstring b, int width, int space);

/* Esoteric standards specific functions */
BSTR_PUBLIC char *
bStr2NetStr(const bstring b);

BSTR_PUBLIC bstring
bNetStr2Bstr(const char * buf);

BSTR_PUBLIC bstring
bBase64Encode(const bstring b);

BSTR_PUBLIC bstring
bBase64DecodeEx(const bstring b, int * boolTruncError);

BSTR_PUBLIC struct bStream *
bsUuDecode(struct bStream * sInp, int * badlines);

BSTR_PUBLIC bstring
bUuDecodeEx(const bstring src, int * badlines);

BSTR_PUBLIC bstring
bUuEncode(const bstring src);

BSTR_PUBLIC bstring
bYEncode(const bstring src);

BSTR_PUBLIC bstring
bYDecode(const bstring src);

/* Writable stream */
typedef int
(* bNwrite)(const void * buf, size_t elsize, size_t nelem, void * parm);

BSTR_PUBLIC struct bwriteStream *
bwsOpen(bNwrite writeFn, void * parm);

BSTR_PUBLIC int
bwsWriteBstr(struct bwriteStream * stream, const bstring b);

BSTR_PUBLIC int
bwsWriteBlk(struct bwriteStream * stream, void * blk, int len);

BSTR_PUBLIC int
bwsWriteFlush(struct bwriteStream * stream);

BSTR_PUBLIC int
bwsIsEOF(const struct bwriteStream * stream);

BSTR_PUBLIC int
bwsBuffLength(struct bwriteStream * stream, int sz);

BSTR_PUBLIC void *
bwsClose(struct bwriteStream * stream);

/* Security functions */
#define bSecureDestroy(b) \
do { \
	if ((b) && (b)->mlen > 0 && (b)->data) { \
	    (void)memset((b)->data, 0, (size_t)(b)->mlen); \
	    (void)bdestroy((b)); \
	} \
} while (0)

#define bSecureWriteProtect(t) \
do { \
	if ((t).mlen >= 0) { \
	    if ((t).mlen > (t).slen)) { \
	        (void)memset((t).data + (t).slen, 0, (size_t)(t).mlen - (t).slen); \
	    } \
	    (t).mlen = -1; \
	} \
} while (0)

BSTR_PUBLIC bstring
bSecureInput(int maxlen, int termchar, bNgetc vgetchar, void * vgcCtx);

#ifdef __cplusplus
}
#endif

#endif /* BSTRAUX_H */
