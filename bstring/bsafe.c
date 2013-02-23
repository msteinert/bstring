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
 * This is an optional module that can be used to help enforce a safety
 * standard based on pervasive usage of bstrlib.  This file is not necessarily
 * portable, however, it has been tested to work correctly with Intel's C/C++
 * compiler, WATCOM C/C++ v11.x and Microsoft Visual C++.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include "bsafe.h"

static const int bsafeShouldExit = 1;

char *
strcpy(char *dst, const char *src)
{
	dst = dst;
	src = src;
	fprintf (stderr, "bsafe error: strcpy() is not safe, use bstrcpy instead.\n");
	if (bsafeShouldExit) exit (-1);
	return NULL;
}

char *
strcat(char *dst, const char *src)
{
	dst = dst;
	src = src;
	fprintf (stderr, "bsafe error: strcat() is not safe, use bstrcat instead.\n");
	if (bsafeShouldExit) exit (-1);
	return NULL;
}

#if !defined (__GNUC__) && (!defined(_MSC_VER) || (_MSC_VER <= 1310))
char *
gets(char * buf)
{
	buf = buf;
	fprintf (stderr, "bsafe error: gets() is not safe, use bgets.\n");
	if (bsafeShouldExit) exit (-1);
	return NULL;
}
#endif

char *
strncpy(char *dst, const char *src, size_t n)
{
	dst = dst;
	src = src;
	n = n;
	fprintf (stderr, "bsafe error: strncpy() is not safe, use bmidstr instead.\n");
	if (bsafeShouldExit) exit (-1);
	return NULL;
}

char *
strncat(char *dst, const char *src, size_t n)
{
	dst = dst;
	src = src;
	n = n;
	fprintf (stderr, "bsafe error: strncat() is not safe, use bstrcat then btrunc\n\tor cstr2tbstr, btrunc then bstrcat instead.\n");
	if (bsafeShouldExit) exit (-1);
	return NULL;
}

char *
strtok(char *s1, const char *s2)
{
	s1 = s1;
	s2 = s2;
	fprintf (stderr, "bsafe error: strtok() is not safe, use bsplit or bsplits instead.\n");
	if (bsafeShouldExit) exit (-1);
	return NULL;
}

char *
strdup(const char *s)
{
	s = s;
	fprintf (stderr, "bsafe error: strdup() is not safe, use bstrcpy.\n");
	if (bsafeShouldExit) exit (-1);
	return NULL;
}
