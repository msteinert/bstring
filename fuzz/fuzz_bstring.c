/* Copyright (C) 2026 Daniel Markstedt <daniel@mindani.net>
 * libFuzzer fuzz target for the Better String Library
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
 * Exercises the full public API of bstrlib and bstraux using raw bytes
 * supplied by the fuzzer engine.
 *
 * Input layout
 * ────────────
 *   data[0]      fill character   (gap-fill for insert / replace)
 *   data[1]      split character  (separator for bsplit)
 *   data[2]      position hint    (used modulo string length)
 *   data[3]      length hint      (used modulo a safe upper bound)
 *   data[4..mid] content of b0
 *   data[mid..]  content of b1   (also used as find / replace target)
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bstraux.h"
#include "bstrlib.h"

#ifdef BSTRING_ENABLE_UTF8
#include "buniutil.h"
#include "utf8util.h"

/* Maximum UCS-2 output words allocated on the stack for buGetBlkUTF16. */
#define MAX_UCS2_OUT 256

static const cpUcs4 errch_table[] = {
    0xFFFD,        /* replacement character */
    '?',           /* ASCII fallback        */
    0,             /* NUL                   */
    ~(cpUcs4)0,   /* abort sentinel        */
};
#define ERRCH_TABLE_SZ ((int)(sizeof errch_table / sizeof errch_table[0]))
#endif /* BSTRING_ENABLE_UTF8 */

int
LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Need at least four control bytes to do anything interesting. */
    if (size < 4) {
        return 0;
    }

    unsigned char fill     = data[0];
    unsigned char split_ch = data[1];
    int           pos_hint = (int)data[2];
    int           len_hint = (int)data[3];

    const uint8_t *payload = data + 4;
    size_t         psize   = size - 4;
    size_t         mid     = psize / 2;

    /* Primary bstrings built directly from the fuzzer payload. */
    bstring b0 = blk2bstr(payload,       (int)mid);
    bstring b1 = blk2bstr(payload + mid, (int)(psize - mid));

    if (!b0 || !b1) {
        bdestroy(b0);
        bdestroy(b1);
        return 0;
    }

    int slen0 = b0->slen;
    int slen1 = b1->slen;

    /* Clamp positional parameters to valid ranges. */
    int safe_pos = slen0 > 0 ? pos_hint % (slen0 + 1) : 0;
    int safe_len = (len_hint % 64) + 1; /* 1..64 */

    /* ── Search and comparison (read-only) ─────────────────────────────── */

    (void)binstr(b0, safe_pos, b1);
    (void)binstrr(b0, slen0 > 0 ? slen0 - 1 : 0, b1);
    (void)binstrcaseless(b0, safe_pos, b1);
    (void)binstrrcaseless(b0, slen0 > 0 ? slen0 - 1 : 0, b1);
    (void)bstrchrp(b0, split_ch, safe_pos);
    (void)bstrrchrp(b0, split_ch, slen0 > 0 ? slen0 - 1 : 0);
    (void)binchr(b0, 0, b1);
    (void)binchrr(b0, slen0 > 0 ? slen0 - 1 : 0, b1);
    (void)bninchr(b0, 0, b1);
    (void)bninchrr(b0, slen0 > 0 ? slen0 - 1 : 0, b1);
    (void)bstrcmp(b0, b1);
    (void)bstricmp(b0, b1);
    (void)bstrncmp(b0, b1, safe_len);
    (void)bstrnicmp(b0, b1, safe_len);
    (void)biseq(b0, b1);
    (void)biseqcaseless(b0, b1);
    (void)biseqblk(b0, payload, (int)mid);
    (void)bisstemeqblk(b0, payload, (int)mid);
    (void)bisstemeqcaselessblk(b0, payload, (int)mid);

    /* ── Substring extraction ───────────────────────────────────────────── */

    bstring sub = bmidstr(b0, safe_pos, safe_len);
    bdestroy(sub);

    bstring head = bHead(b0, safe_len);
    bdestroy(head);

    bstring tail = bTail(b0, safe_len);
    bdestroy(tail);

    /* ── NUL-terminated C-string round-trip ─────────────────────────────── */

    char *cstr0 = bstr2cstr(b0, '?');

    /* ── Encoding / decoding ────────────────────────────────────────────── */

    bstring enc_sgml = bstrcpy(b0);
    if (enc_sgml) {
        (void)bSGMLEncode(enc_sgml);
        bdestroy(enc_sgml);
    }

    bstring enc_b64 = bBase64Encode(b0);
    bdestroy(enc_b64);

    int b64_err = 0;
    bstring dec_b64 = bBase64DecodeEx(b0, &b64_err);
    bdestroy(dec_b64);

    bstring enc_uu = bUuEncode(b0);
    bdestroy(enc_uu);

    int uu_bad = 0;
    bstring dec_uu = bUuDecodeEx(b0, &uu_bad);
    bdestroy(dec_uu);

    bstring enc_yenc = bYEncode(b0);
    bdestroy(enc_yenc);

    bstring dec_yenc = bYDecode(b0);
    bdestroy(dec_yenc);

    /* ── Netstring parser (fuzzes parser with raw bytes as input) ───────── */

    if (psize > 0) {
        char *raw_cstr = malloc(psize + 1);
        if (raw_cstr) {
            memcpy(raw_cstr, payload, psize);
            raw_cstr[psize] = '\0';
            bstring fromnet = bNetStr2Bstr(raw_cstr);
            bdestroy(fromnet);
            free(raw_cstr);
        }
    }

    char *netstr = bStr2NetStr(b0);
    if (netstr) {
        bstring roundtrip = bNetStr2Bstr(netstr);
        bdestroy(roundtrip);
        bcstrfree(netstr);
    }

    /* ── Split / join ───────────────────────────────────────────────────── */

    struct bstrList *lst0 = bsplit(b0, split_ch);
    if (lst0) {
        bstring joined = bjoin(lst0, b1);
        bdestroy(joined);
        bstrListDestroy(lst0);
    }

    struct bstrList *lst1 = bsplits(b0, b1);
    if (lst1) {
        bstrListDestroy(lst1);
    }

    /* bsplitstr requires a non-empty separator. */
    if (slen1 > 0) {
        struct bstrList *lst2 = bsplitstr(b0, b1);
        if (lst2) {
            bstring joined2 = bjoin(lst2, b1);
            bdestroy(joined2);
            bstrListDestroy(lst2);
        }
    }

    /* ── In-place mutation (each operates on a fresh copy of b0) ─────────── */

    bstring work;

    work = bstrcpy(b0);
    if (work) {
        (void)bconcat(work, b1);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)bcatblk(work, payload + mid, (int)(psize - mid));
        bdestroy(work);
    }

    if (cstr0) {
        work = bstrcpy(b0);
        if (work) {
            (void)bcatcstr(work, cstr0);
            bdestroy(work);
        }

        work = bstrcpy(b1);
        if (work) {
            (void)bassigncstr(work, cstr0);
            bdestroy(work);
        }
    }

    work = bstrcpy(b0);
    if (work) {
        (void)bassignblk(work, payload + mid, (int)(psize - mid));
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)binsert(work, safe_pos, b1, fill);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)binsertblk(work, safe_pos, payload, (int)mid, fill);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)binsertch(work, safe_pos, safe_len, fill);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)bdelete(work, safe_pos, safe_len);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)bsetstr(work, safe_pos, b1, fill);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)breplace(work, safe_pos, safe_len, b1, fill);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)btrunc(work, safe_len);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)btoupper(work);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)btolower(work);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)bltrimws(work);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)brtrimws(work);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)btrimws(work);
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)bReverse(work);
        bdestroy(work);
    }

    /* bpattern errors on empty strings; guard to avoid false crash reports. */
    if (slen0 > 0) {
        work = bstrcpy(b0);
        if (work) {
            (void)bpattern(work, safe_len + slen0);
            bdestroy(work);
        }
    }

    /* Cap repetitions to prevent OOM in CI. */
    work = bstrcpy(b0);
    if (work) {
        (void)bReplicate(work, (len_hint % 8) + 1);
        bdestroy(work);
    }

    /* bfindreplace: find operand must be non-empty. */
    if (slen1 > 0) {
        work = bstrcpy(b0);
        if (work) {
            (void)bfindreplace(work, b1, b0, 0);
            bdestroy(work);
        }

        work = bstrcpy(b0);
        if (work) {
            (void)bfindreplacecaseless(work, b1, b0, 0);
            bdestroy(work);
        }
    }

    /* ── Justification (bstraux) ────────────────────────────────────────── */

    int width = safe_len + slen0;

    work = bstrcpy(b0);
    if (work) {
        (void)bJustifyLeft(work, ' ');
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)bJustifyRight(work, width, ' ');
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)bJustifyCenter(work, width, ' ');
        bdestroy(work);
    }

    work = bstrcpy(b0);
    if (work) {
        (void)bJustifyMargin(work, width, ' ');
        bdestroy(work);
    }

    bcstrfree(cstr0);
    bdestroy(b0);
    bdestroy(b1);

#ifdef BSTRING_ENABLE_UTF8
    /*
     * Unicode fuzzing
     * ────────────────
     * Reuses the same payload bytes already parsed above.
     *   data[0]  → pos_hint   (same byte as fill)
     *   data[1]  → errch_sel  (same byte as split_ch)
     *   payload[0..mid)  → treated as raw UTF-8
     *   payload[mid..)   → reinterpreted as UCS-2 / UCS-4 word arrays
     */
    {
        unsigned char pos_hint  = data[0];
        unsigned char errch_sel = data[1];

        const uint8_t *utf8_data = payload;
        size_t         utf8_size = mid;
        const uint8_t *word_data = payload + mid;
        size_t         word_size = psize - mid;

        cpUcs4 errCh = errch_table[errch_sel % ERRCH_TABLE_SZ];

        /* ── utf8util: forward iterator (GetNext) ─────────────────────── */
        {
            struct utf8Iterator iter;
            utf8IteratorInit(&iter, (unsigned char *)(uintptr_t)utf8_data,
                             (int)utf8_size);
            while (!utf8IteratorNoMore(&iter)) {
                (void)utf8IteratorGetNextCodePoint(&iter, errCh);
            }
            utf8IteratorUninit(&iter);
        }

        /* ── utf8util: peek iterator (GetCurr without advancing) ──────── */
        {
            struct utf8Iterator iter;
            utf8IteratorInit(&iter, (unsigned char *)(uintptr_t)utf8_data,
                             (int)utf8_size);
            if (!utf8IteratorNoMore(&iter)) {
                (void)utf8IteratorGetCurrCodePoint(&iter, errCh);
            }
            utf8IteratorUninit(&iter);
        }

        /* ── utf8util: backward scanner ───────────────────────────────── */
        if (utf8_size > 0) {
            for (int i = 0; i < (int)utf8_size; i++) {
                cpUcs4 out;
                (void)utf8ScanBackwardsForCodePoint(utf8_data,
                                                    (int)utf8_size, i, &out);
            }
            int pos = (int)(pos_hint % utf8_size);
            (void)utf8ScanBackwardsForCodePoint(utf8_data,
                                                (int)utf8_size, pos, NULL);
        }

        /* ── buniutil: UTF-8 validation ───────────────────────────────── */
        {
            bstring bu = blk2bstr(utf8_data, (int)utf8_size);
            if (bu) {
                (void)buIsUTF8Content(bu);
                bdestroy(bu);
            }
        }

        /* ── buniutil: UTF-8 bstring → UTF-16 array ───────────────────── */
        {
            bstring bu = blk2bstr(utf8_data, (int)utf8_size);
            if (bu) {
                cpUcs2 out_buf[MAX_UCS2_OUT];
                (void)buGetBlkUTF16(out_buf, MAX_UCS2_OUT, errCh, bu, 0);
                if (utf8_size > 0) {
                    int pos = (int)(pos_hint % utf8_size);
                    (void)buGetBlkUTF16(out_buf, MAX_UCS2_OUT, errCh, bu, pos);
                }
                bdestroy(bu);
            }
        }

        /* ── buniutil: UCS-4 array → UTF-8 bstring ───────────────────── */
        if (word_size >= sizeof(cpUcs4)) {
            int n = (int)(word_size / sizeof(cpUcs4));
            cpUcs4 *ucs4_buf = malloc((size_t)n * sizeof(cpUcs4));
            if (ucs4_buf) {
                memcpy(ucs4_buf, word_data, (size_t)n * sizeof(cpUcs4));
                bstring out = bfromcstr("");
                if (out) {
                    (void)buAppendBlkUcs4(out, ucs4_buf, n, errCh);
                    bdestroy(out);
                }
                free(ucs4_buf);
            }
        }

        /* ── buniutil: UTF-16 array → UTF-8 bstring ──────────────────── */
        if (word_size >= sizeof(cpUcs2)) {
            int n = (int)(word_size / sizeof(cpUcs2));
            cpUcs2 *ucs2_buf = malloc((size_t)n * sizeof(cpUcs2));
            if (ucs2_buf) {
                memcpy(ucs2_buf, word_data, (size_t)n * sizeof(cpUcs2));

                bstring out;

                /* bom=NULL: no BOM tracking, native endianness assumed. */
                out = bfromcstr("");
                if (out) {
                    (void)buAppendBlkUTF16(out, ucs2_buf, n, NULL, errCh);
                    bdestroy(out);
                }

                /* *bom=0: let the function detect and consume a leading BOM. */
                out = bfromcstr("");
                if (out) {
                    cpUcs2 bom = 0;
                    (void)buAppendBlkUTF16(out, ucs2_buf, n, &bom, errCh);
                    bdestroy(out);
                }

                /* *bom=0xFEFF: caller pre-detected a little-endian BOM. */
                out = bfromcstr("");
                if (out) {
                    cpUcs2 bom = 0xFEFF;
                    (void)buAppendBlkUTF16(out, ucs2_buf, n, &bom, errCh);
                    bdestroy(out);
                }

                /* *bom=0xFFFE: caller pre-detected a big-endian BOM. */
                out = bfromcstr("");
                if (out) {
                    cpUcs2 bom = 0xFFFE;
                    (void)buAppendBlkUTF16(out, ucs2_buf, n, &bom, errCh);
                    bdestroy(out);
                }

                free(ucs2_buf);
            }
        }
    }
#endif /* BSTRING_ENABLE_UTF8 */

    return 0;
}
