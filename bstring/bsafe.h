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
 * \brief C runtime stubs to abort usage of unsafe C functions
 *
 * This is an optional module that can be used to help enforce a safety
 * standard based on pervasive usage of bstrlib.  This file is not necessarily
 * portable, however, it has been tested to work correctly with Intel's C/C++
 * compiler, WATCOM C/C++ v11.x and Microsoft Visual C++.
 */

#ifndef BSTRLIB_BSAFE_H
#define BSTRLIB_BSAFE_H

#if __GNUC__ >= 4
#define BSAFE_PUBLIC \
	__attribute__ ((visibility ("default")))
#define BSAFE_PRIVATE \
	__attribute__ ((visibility ("hidden")))
#else
#define BSAFE_PUBLIC
#define BSAFE_PRIVATE
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined (__GNUC__) && (!defined(_MSC_VER) || (_MSC_VER <= 1310))
/* This is caught in the linker, so its not necessary for gcc. */
char *
gets(char * buf);
#endif

BSAFE_PUBLIC char *
strcpy(char *dst, const char *src);

BSAFE_PUBLIC char *
strcat(char *dst, const char *src);

BSAFE_PUBLIC char *
strncpy(char *dst, const char *src, size_t n);

BSAFE_PUBLIC char *
strncat(char *dst, const char *src, size_t n);

BSAFE_PUBLIC char *
strtok(char *s1, const char *s2);

BSAFE_PUBLIC char *
strdup(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* BSTRLIB_BSAFE_H */
