/***************************************************************************
 *
 * collate.cpp - specializations of collate facet
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 2001-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#if    3 == __GNUG__ && 3 > __GNUC_MINOR__ \
    || 3 == __GNUC_MINOR__ && 1 >__GNUC_PATCHLEVEL__

  // working around a gcc bug (PR #29570)
#  include <rw/_config.h>
#  ifndef _RWSTD_NO_EXTERN_TEMPLATE
#    define _RWSTD_NO_EXTERN_TEMPLATE
#  endif
#endif   // gcc >= 3.0 && gcc < 3.3.1

#include <rw/_defs.h>

#include <limits>     // for numeric_limits

#include <limits.h>
#include <stdlib.h>   // for wcstombs()
#include <string.h>   // for memchr(), memcpy()

#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>    // for wcscoll(), wcsxfrm(), wmemcmp()
#endif   // _RWSTD_NO_WCHAR_H

#include <loc/_collate.h>
#include <loc/_locale.h>
#include <loc/_localedef.h>

#include "locale_body.h"
#include "podarray.h"      // for __rw_pod_array
#include "setlocale.h"     // for __rw_setlocale

// define _RWSTD_MB_MAX to the greater of MB_LEN_MAX and 8
// where 8 the maximum length necessary to encode a Unicode
// character in UTF-8
#if _RWSTD_MB_LEN_MAX < 8
#  define _RWSTD_MB_MAX   8
#else
#  define _RWSTD_MB_MAX   _RWSTD_MB_LEN_MAX
#endif   // _RWSTD_MB_LEN_MAX


#if defined (_RWSTD_NO_WCSCOLL) && !defined (_RWSTD_NO_WCSCOLL_IN_LIBC)

extern "C" {

// declare if not declared in the system header(s)
int wcscoll (const wchar_t*, const wchar_t*) _LIBC_THROWS ();

#  undef _RWSTD_NO_WCSCOLL

}   // extern "C"

#endif   // _RWSTD_NO_WCSCOLL && !_RWSTD_NO_WCSCOLL_IN_LIBC


#ifdef _RWSTD_NO_WCSXFRM
#  ifndef _RWSTD_NO_WCSXFRM_IN_LIBC

extern "C" {

// declare if not declared in the system header(s)
_RWSTD_SIZE_T wcsxfrm (wchar_t*, const wchar_t*, _RWSTD_SIZE_T) _LIBC_THROWS ();

#    define _RWSTD_WCSXFRM   wcsxfrm
#    undef _RWSTD_NO_WCSXFRM

}   // extern "C"

#  else
#    define _RWSTD_WCSXFRM   _RW::__rw_wcsxfrm
#  endif   // _RWSTD_NO_WCSXFRM_IN_LIBC
#else
#    define _RWSTD_WCSXFRM   wcsxfrm
#endif   // _RWSTD_NO_WCSXFRM


#if defined (_RWSTD_NO_WCSTOMBS) && !defined (_RWSTD_NO_WCSTOMBS_IN_LIBC)

extern "C" {

// declare if not declared in the system header(s)
_RWSTD_DLLIMPORT _RWSTD_SIZE_T
wcstombs (char*, const wchar_t*, _RWSTD_SIZE_T) _LIBC_THROWS ();

#  undef _RWSTD_NO_WCSTOMBS

}   // extern "C"

#endif   // _RWSTD_NO_WCSTOMBS && !_RWSTD_NO_WCSTOMBS_IN_LIBC


// for convenience
typedef unsigned char UChar;


_RWSTD_NAMESPACE (__rw) {
 
#undef min
#undef max


// computes LC_XXX category from a numeric facet id, returns the
// LC_XXX category for standard facets, LC_ALL for all others
int __rw_get_cat (int);


#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_INTERNAL void 
__rw_append_weight (const _RW::__rw_collate_t *impl,
                    const unsigned            *weights,
                    _STD::wstring             &out)
{
    // append the weight(s) to the out string
    for (int i = 0; i < impl->longest_weight; ++i, ++weights) {

        const unsigned int wt = *weights;

        // if the weight is not an IGNORE weight
        // then add it to the out string
        if (wt && wt != _RWSTD_UINT_MAX) {
            out += wchar_t (wt);
        }
      
    }
}

#endif   // _RWSTD_NO_WCHAR_T


_RWSTD_INTERNAL void 
__rw_append_weight (const _RW::__rw_collate_t *impl, 
                    const unsigned int        *weights, 
                    _STD::string              &out)
{
    // append the weight(s) to the out string
    for (int i = 0; i < impl->longest_weight; ++i, ++weights) {

        unsigned wt = *weights;

        // if the weight is not an IGNORE weight
        // then add it to the out string

        if (wt && wt != _RWSTD_UINT_MAX) {

            while (_RWSTD_CHAR_MAX < wt) {
                out += char (_RWSTD_CHAR_MAX);
                wt  -= _RWSTD_CHAR_MAX;
            }

            // the weight will fit a signed char
            // so add it to the out str
            out += char (wt);
        }
    }
}


template <class _STR_T>
static void
__rw_process_offsets (_RW::__rw_collate_t  *impl,
                      const unsigned      **start,
                      const unsigned      **end,
                      _STR_T               &out)
{
    
    // process the offset list for each pass.

    for (unsigned int pass = 0; pass < impl->num_weights; pass++) {
        // first get the order for the start element in this pass
        unsigned int shift_value = (impl->num_weights - (pass + 1)) * 2;

        const unsigned int** cur_start = start;
        const unsigned int** cur_end = start;

        if (*cur_start == *end)
            return;

        do {
            // calculate the order from the bitmask order in front of each
            // character's weight information.  Each character may have
            // different orderings although most likely there will be groups
            // of characters with the same ordering.
            unsigned int cur_ord = (**cur_end >> shift_value) & 0x00000003;
            while (cur_end != end && ((**cur_end >> shift_value) 
                                      & 0x00000003) == cur_ord) {
               cur_end++;
            }

            const unsigned int** cur;
            switch (cur_ord) {
            case 0:
                // forward weight type
                for (cur = cur_start; cur < cur_end; cur++) {
                    const unsigned int* weightp = *cur;
                    weightp = weightp + 1 + pass * impl->longest_weight;

                    // qualify the call to make sure the function
                    // is found during lookup when declared static
                    _RW::__rw_append_weight (impl, weightp, out);
                }
                break;

            case 1:
                // backward weight type
                for (cur = cur_end - 1; cur >= cur_start; cur --) {
                    const unsigned int* weightp = *cur;
                    weightp = weightp + 1 + pass * impl->longest_weight;

                    // qualify the call to make sure the function
                    // is found during lookup when declared static
                    _RW::__rw_append_weight (impl, weightp, out);
                }
                break;

            case 2:
                // forward,position weight type.
                // The string with a non-IGNOREd element after the 
                // fewest IGNORED elements should collate first
                for (cur = cur_start; cur < cur_end; cur++) {
                    const unsigned int* weightp = *cur;
                    weightp = weightp + 1 + pass * impl->longest_weight;
                    // output the CHAR_MAX value for every IGNORE
                    // weight that we see.  The first string that has a
                    // non-ignore value will collate first

                    if (*weightp == 0) {
                        typedef typename _STR_T::value_type CharT;

                        const CharT ign =
                            _STD::numeric_limits<CharT>::max ();

                        out += ign;
                    }
                    else {
                        // qualify the call to make sure the function
                        // is found during lookup when declared static
                        _RW::__rw_append_weight (impl, weightp, out);
                    }
                }
                break;

            case 3:
                // backward,position weight type
                for (cur = cur_end - 1; cur >= cur_start; cur --) {
                    const unsigned int* weightp = *cur;
                    weightp = weightp + 1 + pass * impl->longest_weight;
                    // non-ignore value will collate first
                    if (*weightp == 0) {
                        typedef typename _STR_T::value_type CharT;

                        const CharT ign =
                            _STD::numeric_limits<CharT>::max ();

                        out += ign;
                    }
                    else
                        // qualify the call to make sure the function
                        // is found during lookup when declared static
                        _RW::__rw_append_weight (impl, weightp, out);
                }
                break;
            }

            // append a 1 to designate the end of the pass
            typedef typename _STR_T::value_type CharT;
            out += CharT (1);

            cur_start = cur_end;
        } while (cur_end != end);
    }
}

#ifndef _RWSTD_NO_WCHAR_T

static int
__rw_get_w_ce_offset (const __rw_collate_t *impl,
                      const wchar_t** cur_char,
                      const wchar_t* end)
{
    // obtain the offset of a wide collating element.  If no collating
    // elements can be made with the current position of the string 
    // then return -1

    if (impl->largest_ce > 1) {

        unsigned int cur_tab = 0;

        char utf8_enc [_RWSTD_MB_MAX];

        const unsigned int* tab;
        while (*cur_char < end) {

            // convert the next wchar_t character to a utf8 encoded character
            const _RWSTD_SIZE_T nbytes =
                _RW::__rw_itoutf8 (**cur_char, utf8_enc);

            for (_RWSTD_SIZE_T i = 0; i < nbytes; i++) {

                const unsigned c1 = impl->get_first_char_in_w_ce_tab (cur_tab);
                const unsigned c2 = impl->get_last_char_in_w_ce_tab (cur_tab);

                if (UChar (utf8_enc [i]) < c1 || UChar (utf8_enc [i]) > c2)
                    return -1;
        
                tab = impl->get_w_ce_tab (cur_tab);
            
                const unsigned int next_off = tab [UChar (utf8_enc [i]) - c1];
                if (next_off == _RWSTD_UINT_MAX) {
                    return -1;
                }

                if (next_off & 0x80000000) {
                    cur_tab = next_off &~ 0x80000000;
                    continue;
                }

                return next_off;
            }
            (*cur_char)++;
        }
    }
    return -1;
}


static int
__rw_get_wchar_offset (const __rw_collate_t *impl,
                       const wchar_t** cur_char,
                       const wchar_t* end)
{
    // get the offset of the weight information for the current wide character

    unsigned cur_tab = 0;
    char utf8_enc [_RWSTD_MB_MAX];

    while (*cur_char < end) {
        // convert the next wchar_t character to a utf8 encoded character
        const _RWSTD_SIZE_T nbytes =
            _RW::__rw_itoutf8 (**cur_char, utf8_enc);

        for (_RWSTD_SIZE_T i = 0; i < nbytes; i++) {
            const unsigned c1 = impl->get_first_char_in_w_tab (cur_tab);
            if (UChar (utf8_enc [i]) < c1)
                return -1;
        
            const unsigned* const tab = impl->get_w_tab (cur_tab);
            
            const unsigned int next_off = tab [UChar (utf8_enc [i]) - c1];
            if (next_off == _RWSTD_UINT_MAX) {
                return -1;
            }

            if (next_off & 0x80000000) {
                cur_tab = next_off &~ 0x80000000;
                continue;
            }

            return next_off;
        }
        (*cur_char)++;
    }
    return -1;
}

#endif   // _RWSTD_NO_WCHAR_T


static int
__rw_get_n_ce_offset (const __rw_collate_t *impl,
                      const char **cur_char, 
                      const char  *end)
{
    if (impl->largest_ce > 1) {
        unsigned int cur_tab = 0;
        const unsigned int* tab;

        while (*cur_char < end ) {
            const unsigned c1 = impl->get_first_char_in_n_ce_tab (cur_tab);
            const unsigned c2 = impl->get_last_char_in_n_ce_tab (cur_tab);
            if (UChar (**cur_char) < c1 || UChar (**cur_char) > c2)
                return -1;

            tab = impl->get_n_ce_tab (cur_tab);

            unsigned int next_off = tab [UChar (**cur_char) - c1];
            if (next_off == _RWSTD_UINT_MAX) {
                return -1;
            }

            if (next_off & 0x80000000) {
                (*cur_char)++;
                cur_tab = next_off &~ 0x80000000;
            }
            else {
                return next_off;
            }
        }
    }
    return -1;
}


static int
__rw_get_char_offset (const __rw_collate_t *impl,
                      const char** cur_char, 
                      const char* end)
{
    unsigned int cur_tab = 0;

    while (*cur_char < end) {
        unsigned c1 = impl->get_first_char_in_n_tab (cur_tab);
        if (UChar (**cur_char) < c1)
            return -1;

        const unsigned int* tab = impl->get_n_tab (cur_tab);
        c1 = impl->get_first_char_in_n_tab (cur_tab);

        unsigned int next_off = tab [UChar (**cur_char) - c1];
        if (next_off == _RWSTD_UINT_MAX) {
            return -1;
        }

        if (next_off & 0x80000000) {
            (*cur_char)++;
            cur_tab = next_off &~ 0x80000000;
        }
        else {
            return next_off;
        }
    }

    return -1;
}


// returns true if the character sequence starting at `from'
// does not form a valid complete multibyte character
static inline bool
__rw_is_invalid (const unsigned int* first_tab, const char *from)
{
    return 0 != (__rw_mbtowco (first_tab, from, 0) & 0x80000000);
}


// same as strxfrm() except that it takes the number of characters
// in an array that may contain embedded NULs; these are inserted
// into the transformed string
static _STD::string
__rw_strnxfrm (const char *src, _RWSTD_SIZE_T nchars)
{
    _STD::string res;

    char buf [256];
    char *pbuf = buf;

    _RWSTD_SIZE_T bufsize = sizeof buf;
    char *psrc = buf;

    while (nchars) {

        // using a C-style cast instead of static_cast to avoid
        // a gcc 2.95.2 bug causing an error on some platforms:
        //   static_cast from `void *' to `const char *'
        const char* const last = (const char*)memchr (src, '\0', nchars);

        if (0 == last) {

            // no NUL found in the initial portion of the source string
            // that fits into the local temporary buffer; copy as many
            // characters as fit into the buffer

            if (bufsize <= nchars) {
                if (pbuf != buf)
                    delete[] pbuf;
                pbuf = new char [nchars + 1];
            }

            psrc = pbuf;
            memcpy (psrc, src, nchars);

            // append a terminating NUL and decrement the number
            // of characters that remain to be processed
            psrc [nchars] = '\0';
            src          += nchars;
            nchars        = 0;
        }
        else {

            // terminating NUL found in the source buffer
            nchars -= (last - src) + 1;
            psrc    = _RWSTD_CONST_CAST (char*, src);
            src    += (last - src) + 1;
        }

        // provide a destination buffer to strxfrm() in case
        // it's buggy (such as MSVC's) and tries to write to
        // the buffer even if it's 0
        char just_in_case_buf [8];
        const _RWSTD_SIZE_T dst_size = strxfrm (just_in_case_buf, psrc, 0);

        // check for strxfrm() errors
        if (0 == (dst_size << 1))
            return _STD::string ();

        _RWSTD_SIZE_T res_size = res.size ();

        _TRY {
            // resize the result string to fit itself plus the result
            // of the transformation including the terminatin NUL
            // appended by strxfrm()
            res.resize (res_size + dst_size + 1);
        }
        _CATCH (...) {
            if (pbuf != buf)
                delete[] pbuf;
            _RETHROW;
        }

        // transfor the source string up to the terminating NUL
        _RWSTD_SIZE_T xfrm_size =
            strxfrm (&res [0] + res_size, psrc, dst_size + 1);

#if defined _MSC_VER && _MSC_VER < 1400
        // compute the correct value that should have been returned from
        // strxfrm() after the transformation has completed (MSVC strxfrm()
        // returns a bogus result; see PR #29935)
        xfrm_size = strlen (&res [0] + res_size);
#endif   // MSVC < 8.0

        // increment the size of the result string by the number
        // of transformed characters excluding the terminating NUL
        // if strxfrm() transforms the empty string into the empty
        // string, keep the terminating NUL, otherwise drop it
        res_size += xfrm_size + (last && !*psrc && !xfrm_size);

        _TRY {
            res.resize (res_size);
        }
        _CATCH (...) {
            if (pbuf != buf)
                delete[] pbuf;
            _RETHROW;
        }
    }

    if (pbuf != buf)
        delete[] pbuf;

    return res;
}


#ifndef _RWSTD_NO_WCHAR_T

#  ifdef _RWSTD_NO_WCSXFRM

// implements wcsxfrm() using wcstombs() and strxfrm() on platforms
// such as some versions of BSD where the function isn't defined in
// the C Standard Library
static _RWSTD_SIZE_T
__rw_wcsxfrm (wchar_t *dst, const wchar_t *src, _RWSTD_SIZE_T dstsize)
{
    // src must be non-null
    _RWSTD_ASSERT (0 != src);

    // dst is permitted to be null only when (dstsize == 0)
    _RWSTD_ASSERT (0 == dstsize || dst);

#    ifndef _RWSTD_NO_WCSTOMBS

    // convert wide string to a multibyte string before tranforming it
    // using strxfrm() and widening the result into the destination buffer

    const _RWSTD_SIZE_T srclen = _RWSTD_WCSLEN (src);

    // compute the size of the temporary nearrow buffer where to narrow
    // the source wide string to
    const _RWSTD_SIZE_T needbytes =
        (dstsize ? dstsize : srclen) * MB_LEN_MAX;

    char narrow_buf [256];
    char* const nbuf =
        sizeof narrow_buf < needbytes ? new char [needbytes + 1] : narrow_buf;

    _RWSTD_SIZE_T result;

    const _RWSTD_SIZE_T nmbchars = wcstombs (nbuf, src, needbytes);

    if (_RWSTD_SIZE_MAX == nmbchars)
        result = _RWSTD_SIZE_MAX;
    else {
        // allocate a small buffer 8 times the size of the multibyte
        // buffer (where 8 is a guess at the maximum number of bytes
        // needed to transform the longest multibyte character)
        char xfrm_buf [sizeof narrow_buf * 8];
        const _RWSTD_SIZE_T xbufsize = sizeof xfrm_buf;
        const _RWSTD_SIZE_T xbufneed = needbytes * 8;

        // allocate a larger buffer if the small statically buffer
        // isn't big enough
        char* const xbuf =
            xbufsize < xbufneed ? new char [xbufneed + 1] : xfrm_buf;

        // transform the multibyte character string into the narrow
        // buffer, storing the returned value
        result = strxfrm (xbuf, nbuf, xbufneed);

        if (_RWSTD_SIZE_MAX != result && dstsize) {
            // widen the bytes (not characters) of the transformed string
            // if the transformation was successful and the size of the
            // destination buffer is non-zero

            if (result < dstsize)
                dstsize = result;

            for (_RWSTD_SIZE_T i = 0; i != dstsize; ++i)
                dst [i] = wchar_t (UChar (xbuf [i]));
        }

        // free the transformation buffer if dynamically allocated
        if (xbuf != xfrm_buf)
            delete[] xbuf;
    }

    // free the multibyte buffer if dynamically allocated
    if (nbuf != narrow_buf)
        delete[] nbuf;

    return result;

#    else   // if defined (_RWSTD_NO_WCSTOMBS)

    _RWSTD_UNUSED (dst);
    _RWSTD_UNUSED (src);
    _RWSTD_UNUSED (dstsize);

    // fail when there is no way to convert a wchar_t array
    // to a multibyte string
    return _RWSTD_SIZE_MAX;

#    endif   // _RWSTD_NO_WCSTOMBS

}

#  endif   // _RWSTD_NO_WCSXFRM


// same as wcsxfrm() except that it takes the number of characters
// in an array that may contain embedded NULs; these are inserted
// into the transformed string
static _STD::wstring
__rw_wcsnxfrm (const wchar_t *src, _RWSTD_SIZE_T nchars)
{
    _STD::wstring res;

    wchar_t buf [256];
    wchar_t *pbuf = buf;

    _RWSTD_SIZE_T bufsize = sizeof buf / sizeof *buf;
    wchar_t *psrc = buf;

    while (nchars) {

        typedef _STD::char_traits<wchar_t> Traits;

        const wchar_t* const last = Traits::find (src, nchars, L'\0');

        if (0 == last) {

            // no NUL found in the initial portion of the source string
            // that fits into the local temporary buffer; copy as many
            // characters as fit into the buffer

            if (bufsize <= nchars) {
                if (pbuf != buf)
                    delete[] pbuf;
                pbuf = new wchar_t [nchars + 1];
            }

            psrc = pbuf;
            memcpy (psrc, src, nchars * sizeof *psrc);

            // append a terminating NUL and decrement the number
            // of characters that remain to be processed
            psrc [nchars] = 0;
            src          += nchars;
            nchars        = 0;
        }
        else {

            // terminating NUL found in the source buffer
            nchars -= (last - src) + 1;
            psrc    = _RWSTD_CONST_CAST (wchar_t*, src);
            src    += (last - src) + 1;
        }

        // provide a destination buffer to strxfrm() in case
        // it's buggy (such as MSVC's) and tries to write to
        // the buffer even if it's 0
        wchar_t just_in_case_buf [8];

        const _RWSTD_SIZE_T dst_size =
            _RWSTD_WCSXFRM (just_in_case_buf, psrc, 0);

        // check for wcsxfrm() errors
        if (_RWSTD_SIZE_MAX == dst_size)
            return _STD::wstring ();

        _RWSTD_SIZE_T res_size = res.size ();

        _TRY {
            // resize the result string to fit itself plus the result
            // of the transformation including the terminatin NUL
            // appended by strxfrm()
            res.resize (res_size + dst_size + 1);
        }
        _CATCH (...) {
            if (pbuf != buf)
                delete[] pbuf;
            _RETHROW;
        }

        // transfor the source string up to the terminating NUL
        _RWSTD_SIZE_T xfrm_size =
            _RWSTD_WCSXFRM (&res [0] + res_size, psrc, dst_size + 1);

#  if defined _MSC_VER && _MSC_VER < 1400
        // compute the correct value that should have been returned from
        // strxfrm() after the transformation has completed (MSVC strxfrm()
        // returns a bogus result; see PR #29935)
        xfrm_size = Traits::length (&res [0] + res_size);
#  endif   // MSVC < 8.0

        // increment the size of the result string by the number
        // of transformed characters excluding the terminating NUL
        // if strxfrm() transforms the empty string into the empty
        // string, keep the terminating NUL, otherwise drop it
        res_size += xfrm_size + (last && !*psrc && !xfrm_size);

        _TRY {
            res.resize (res_size);
        }
        _CATCH (...) {
            if (pbuf != buf)
                delete[] pbuf;
            _RETHROW;
        }
    }

    if (pbuf != buf)
        delete[] pbuf;

    return res;
}

#endif   // _RWSTD_NO_WCHAR_T


template <class _CharT>
long __rw_hash (const _CharT *lo, const _CharT *hi) _THROWS (())
{
    // Peter Weinberger's generic hashing algorithm, adapted by Andrew
    // Binstock from a version by Allen Holub (see Andrew Binstock,
    // "Hashing Revisited", Dr. Dobb's Journal, April 1996)

    const int  long_bits     = _RWSTD_CHAR_BIT * int (sizeof (long));
    const int  one_eighth    = long_bits / 8;
    const int  three_fourths = long_bits * 3 / 4;
    // subexpression parenthesized to prevent MSVC 13.00 warning C4554
    const long hi_bits       = ~0L << (long_bits - one_eighth);

    long res = 0;

    for ( ; lo != hi; ++lo) {
        typedef _STD::char_traits<_CharT> _Traits;

        res = (res << one_eighth) + _Traits::to_int_type (*lo);

        const long tmp = res & hi_bits;
        if (tmp)
            res = (res ^ (tmp >> three_fourths)) & ~hi_bits;
    }

    return res;
}

#ifndef _RWSTD_NO_INSTANTIATE
#  ifndef _RWSTD_NO_EXPLICIT_FUNC_INSTANTIATION

// explicitly instantiate for compilers with automatic template
// instantiation that don't emit symbols for implicitly instantiated
// templates even of they are completely defined in .cpp files
// (e.g., Compaq C++)

template long __rw_hash (const char*, const char*) _THROWS(());

#    ifndef _RWSTD_NO_WCHAR_T

template long __rw_hash (const wchar_t*, const wchar_t*) _THROWS(());

#    endif   // _RWSTD_NO_WCHAR_T

#  endif   // _RWSTD_NO_EXPLICIT_FUNC_INSTANTIATION
#endif   // _RWSTD_NO_INSTANTIATE


}    // namespace __rw


_RWSTD_NAMESPACE (std) { 


_RW::__rw_facet_id collate<char>::id;


// outlined to avoid generating a vtable in each translation unit
// that uses the class
/* virtual */ collate<char>::
~collate () /* nothrow */
{
    // no-op
}


int collate<char>::
do_compare (const char_type *__lo1, const char_type *__hi1,
            const char_type *__lo2, const char_type *__hi2) const
{
    _RWSTD_ASSERT (__lo1 <= __hi1 && __lo2 <= __hi2);

    const _RWSTD_SIZE_T __len1 = __hi1 - __lo1;
    const _RWSTD_SIZE_T __len2 = __hi2 - __lo2;

    const int cmp = memcmp (__lo1, __lo2, __len1 < __len2 ? __len1 : __len2);

    if (cmp)
        return cmp < 0 ? -1 : 1;

    return __len1 < __len2 ? -1 : __len2 < __len1 ? +1 : 0;
}


collate<char>::string_type collate<char>::
do_transform (const char_type *__lo, const char_type *__hi) const
{
    _RWSTD_ASSERT (0 != __lo);
    _RWSTD_ASSERT (__lo <= __hi);

    return string_type (__lo, size_t (__hi - __lo));
}


long collate<char>::
do_hash (const char_type *__lo, const char_type *__hi) const
{
    // hash the result of do_transform, so that keys that transform equally
    // will hash equally, as per 22.2.4.1.2, p3

    const string_type __str = do_transform (__lo, __hi);

    __lo = __str.data ();
    __hi = __lo + __str.length ();

    const long res = _RW::__rw_hash (__lo, __hi);

    return res;
}


// outlined to avoid generating a vtable in each translation unit
// that uses the class
/* virtual */ collate_byname<char>::
~collate_byname () /* nothrow */
{
    // no-op
}


int collate_byname<char>::
do_compare (const char* low1, const char* high1,
            const char* low2, const char* high2) const
{
    const string_type s1 = do_transform (low1, high1);
    const string_type s2 = do_transform (low2, high2);

    // FIXME: optimize: doing a full transformation of the two
    // strings is not necessary, it might be quicker to only do
    // a partial transformation
    const int cmp = s1.compare (s2);

    // normalize
    return cmp < 0 ? -1 : cmp ? 1 : 0;
}


collate_byname<char>::string_type
collate_byname<char>::
do_transform (const char* low, const char* high) const
{
    _RWSTD_ASSERT (low <= high);

    const int ccvt_cat = _RW::__rw_get_cat ((_C_wcodecvt_byname + 1) / 2);

    _RW::__rw_collate_t* const impl = 
        _RWSTD_CONST_CAST (_RW::__rw_collate_t*,
            _RWSTD_STATIC_CAST (const _RW::__rw_collate_t*, _C_data ()));

    if (!impl || (this->_C_opts & this->_C_use_libc)) {

        // set the global libc locale in a thread-safe way
        const _RW::__rw_setlocale clocale (_C_name, _RWSTD_LC_COLLATE);

        return _RW::__rw_strnxfrm (low, high - low);
    }

    _RWSTD_ASSERT (0 != impl);

    // the maximum size that we could need to hold all the weight offsets
    // is high - low
    _RW::__rw_pod_array<const unsigned int*, 1024> indexes;

    // first go through the string getting a weight offset for
    // each character, in the process check for collating elements.
    const char* tmp_lo = low;

    for (; tmp_lo < high; tmp_lo++) {
        const char* tmp_lo2 = tmp_lo;
        int ret = _RW::__rw_get_n_ce_offset (impl, &tmp_lo2, high);

        if (ret == -1) {
            tmp_lo2 = tmp_lo;
            ret = _RW::__rw_get_char_offset (impl, &tmp_lo2, high);
            if (ret == -1) {
                // The character is not explicitely defined, but it
                // may have been defined implicitly by UNDEFINED.
                // because we cannot tell the difference between 
                // an UNDEFINED character and an invalid character
                // in the collate database we must use the codecvt 
                // database to discover this information
                if (impl->undefined_optimization) {

                    _RWSTD_SIZE_T size;

                    const _RW::__rw_codecvt_t *cvt = 
                        _RWSTD_STATIC_CAST (const _RW::__rw_codecvt_t*, 
                                            _RW::__rw_get_facet_data (
                                                ccvt_cat, size, _C_name, 
                                                impl->codeset_name ()));

                    if (_RW::__rw_is_invalid (cvt->n_to_w_tab(), tmp_lo2))
                        return 0;

                    const unsigned int *pwt =
                        impl->get_weight (impl->undefined_weight_idx);
                    indexes.append (&pwt, 1);

                    tmp_lo = tmp_lo2;
                }
            }
            else {
                const unsigned int *pwt = impl->get_weight (ret);
                indexes.append (&pwt, 1);
                tmp_lo = tmp_lo2;
            }
        }
        else {
            // we found a collating element
            const unsigned int *pwt = impl->get_weight (ret);
            indexes.append (&pwt, 1);
            tmp_lo = tmp_lo2;
        }
    }
    // now process the weights
    string_type out;
    _RW::__rw_process_offsets (impl, indexes.data (),
                               indexes.data () + indexes.size (), out);

    return out;
}


#ifndef _RWSTD_NO_WCHAR_T

_RW::__rw_facet_id collate<wchar_t>::id;


// outlined to avoid generating a vtable in each translation unit
// that uses the class
/* virtual */ collate<wchar_t>::
~collate () /* nothrow */
{
    // no-op
}


int collate<wchar_t>::
do_compare (const char_type *__lo1, const char_type *__hi1,
            const char_type *__lo2, const char_type *__hi2) const
{
    _RWSTD_ASSERT (__lo1 <= __hi1 && __lo2 <= __hi2);

    const _RWSTD_SIZE_T __len1 = __hi1 - __lo1;
    const _RWSTD_SIZE_T __len2 = __hi2 - __lo2;

#ifndef _RWSTD_NO_WMEMCMP

    const int cmp = wmemcmp (__lo1, __lo2, __len1 < __len2 ? __len1 : __len2);

    if (cmp)
        return cmp < 0 ? -1 : 1;

#else   // if defined (_RWSTD_NO_WMEMCMP)

    for (_RWSTD_SIZE_T __len = __len1 < __len2 ? __len1 : __len2;
         __len--; ++__lo1, ++__lo2) {

        typedef string_type::traits_type _Traits;
        typedef _Traits::int_type        _Int;

        // avoid arithmetic on unknown char types
        const _Int __i1 = _Traits::to_int_type (*__lo1);
        const _Int __i2 = _Traits::to_int_type (*__lo2);
        
        // use int_type to prevent signed versus unsigned char comparison
        if (!_Traits::eq_int_type (__i1, __i2)) 
            return __i1 < __i2 ? -1 : 1;
    }

#endif   // _RWSTD_NO_WMEMCMP

    return __len1 < __len2 ? -1 : __len2 < __len1 ? +1 : 0;
}


collate<wchar_t>::string_type collate<wchar_t>::
do_transform (const char_type *__lo, const char_type *__hi) const
{
    _RWSTD_ASSERT (0 != __lo);
    _RWSTD_ASSERT (__lo <= __hi);

    return string_type (__lo, size_t (__hi - __lo));
}


long collate<wchar_t>::
do_hash (const char_type *__lo, const char_type *__hi) const
{
    // hash the result of do_transform, so that keys that transform equally
    // will hash equally, as per 22.2.4.1.2, p3

    const string_type __str = do_transform (__lo, __hi);

    __lo = __str.data ();
    __hi = __lo + __str.length ();

    const long res = _RW::__rw_hash (__lo, __hi);

    return res;
}


// outlined to avoid generating a vtable in each translation unit
// that uses the class
/* virtual */ collate_byname<wchar_t>::
~collate_byname () /* nothrow */
{
    // no-op
}


int collate_byname<wchar_t>::
do_compare (const wchar_t* low1, const wchar_t* high1,
            const wchar_t* low2, const wchar_t* high2) const
{
    if (this->_C_opts & this->_C_use_libstd) {
        const string_type s1 = do_transform (low1, high1);
        const string_type s2 = do_transform (low2, high2);

        // FIXME: optimize
        return s1.compare (s2);
    }

#ifndef _RWSTD_NO_WCSCOLL

    // use the system C library to compare the strings

    _RW::__rw_setlocale clocale (this->_C_name, _RWSTD_LC_COLLATE);
    
    const _RWSTD_SIZE_T len1 = high1 - low1;
    const _RWSTD_SIZE_T len2 = high2 - low2;
    const _RWSTD_SIZE_T len  = len1 + len2;

    // small local buffer
    wchar_t local_buffer [256];
    const _RWSTD_SIZE_T bufsize = sizeof local_buffer / sizeof *local_buffer;

    // allocate only if local buffer is too small
    wchar_t* const wbuf =
        len + 2 >= bufsize ? new wchar_t [len + 2] : local_buffer;

    // copy and null-terminate first sequence
    char_traits<wchar_t>::copy (wbuf, low1, len1);
    wbuf [len1] = '\0';

    // append and null-terminate first sequence
    char_traits<wchar_t>::copy (wbuf + len1 + 1, low2, len2);
    wbuf [len1 + 1 + len2] = '\0';

    // compare sequences using wcscoll()
    const int result = wcscoll (wbuf, wbuf + len1 + 1);

    // deallocate only if allocated
    if (wbuf != local_buffer)
        delete[] wbuf;

    return result ? result > 0 ? 1 : -1 : 0;

#else   // if defined (_RWSTD_NO_WCSCOLL)

    // transform strings first and compare the transformed results
    const string_type s1 = do_transform (low1, high1);
    const string_type s2 = do_transform (low2, high2);

    return s1.compare (s2);

#endif   // _RWSTD_NO_WCSCOLL

}


collate_byname<wchar_t>::string_type 
collate_byname<wchar_t>::
do_transform (const wchar_t* low, const wchar_t* high) const
{
    const int ccvt_cat = _RW::__rw_get_cat ((_C_wcodecvt_byname + 1) / 2);

    _RW::__rw_collate_t *impl = 
        _RWSTD_CONST_CAST (_RW::__rw_collate_t*, _RWSTD_STATIC_CAST (
            const _RW::__rw_collate_t*, _C_data ()));

    if (!impl || (this->_C_opts & this->_C_use_libc)) {

        // set the global libc locale in a thread-safe way
        const _RW::__rw_setlocale clocale (_C_name, _RWSTD_LC_COLLATE);

        return _RW::__rw_wcsnxfrm (low, high - low);
    }
    else {
        _RWSTD_ASSERT (0 != impl);
        // the maximum size that we could need to hold all the weight offsets
        // is high - low
        _RW::__rw_pod_array<const unsigned int*, 1024> indexes;

        // first go through the string getting a weight offset for
        // each character, in the process check for collating elements.
        for (const wchar_t* tmp_lo =low; tmp_lo < high; tmp_lo++) {
            const wchar_t* tmp_lo2 = tmp_lo;
            int ret = _RW::__rw_get_w_ce_offset (impl, &tmp_lo2, high);
            if (ret == -1) {
                tmp_lo2 = tmp_lo;
                ret = _RW::__rw_get_wchar_offset (impl, &tmp_lo2, high);
                if (ret == -1) {
                    // The character is not explicitely defined, but it
                    // may have been defined implicitly by UNDEFINED.
                    // because we cannot tell the difference between 
                    // an UNDEFINED character and an invalid character
                    // in the collate database we must use the codecvt 
                    // database to discover this information 
                    if (impl->undefined_optimization) {
                        _RWSTD_SIZE_T size;
                        const _RW::__rw_codecvt_t *cvt = 
                            _RWSTD_STATIC_CAST (const _RW::__rw_codecvt_t*, 
                                                _RW::__rw_get_facet_data (
                                                    ccvt_cat, size, _C_name,
                                                    impl->codeset_name ()));

                        char tmp [_RWSTD_MB_MAX];

                        const _RWSTD_SIZE_T nbytes =
                            _RW::__rw_itoutf8 (*tmp_lo2, tmp);

                        tmp [nbytes] = '\0';

                        if (_RW::__rw_is_invalid (cvt->w_to_n_tab (), tmp))
                            return 0;

                        const unsigned int *pwt =
                            impl->get_weight (impl->undefined_weight_idx);
                        indexes.append (&pwt, 1);
                        tmp_lo = tmp_lo2;
                    }
                }
                else {
                    const unsigned int *pwt = impl->get_weight (ret);
                    indexes.append (&pwt, 1);
                    tmp_lo = tmp_lo2;
                }
            }
            else {
                // we found a collating element
                const unsigned int *pwt = impl->get_weight (ret);
                indexes.append (&pwt, 1);
                tmp_lo = tmp_lo2;
            }
        }
        // now process the weights
        string_type out;
        _RW::__rw_process_offsets (impl, indexes.data (),
                                   indexes.data () + indexes.size (), out);

        return out;
    }
}

#endif   // _RWSTD_NO_WCHAR_T



}   // namespace std
