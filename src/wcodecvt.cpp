/***************************************************************************
 *
 * wcodecvt.cpp - definition of codecvt<wchar_t, char, mbstate_t> members
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

#include <rw/_defs.h>


// working around a Compaq C++ bug (see PR #26778)
#if __DECCXX_VER >= 60300000 && __DECCXX_VER < 60400000

#  include <stdarg.h>
_USING (std::va_list);

// override autoconfigured macro whose value is incorrect
// if <unistd.h> is #included before <iconv.h>
#  include <unistd.h>
#  define _RWSTD_NO_ICONV_CONST_CHAR

#endif   // Compaq C++ 6.3

#include <wchar.h>
#include <limits.h>
#include <locale.h>
#include <stdlib.h>   // for MB_CUR_MAX, mblen(), mbtowc()
#include <string.h>   // for memcmp()
#include <errno.h>

#if    defined (__SUNPRO_CC) && __SUNPRO_CC <= 0x540 \
    && (defined (__SunOS_5_8) || defined (__SunOS_5_9))
   // working around SunOS/SunPro header dependencies (see PR #26255)
#  undef _TIME_T
#endif   // SunPro <= 5.4 && SunOS 5.{8,9}

#include <loc/_codecvt.h>
#include <loc/_locale.h>
#include <loc/_localedef.h>

#include <rw/_traits.h>   // for _RWSTD_WCSLEN()

#include "iso2022.h"
#include "setlocale.h"
#include "use_facet.h"


// declare mbrlen() if it's not declared in the system headers
// but is known to be defined in the libc binary
#if defined (_RWSTD_NO_MBRLEN) && !defined (_RWSTD_NO_MBRLEN_IN_LIBC)

#  undef _RWSTD_NO_MBRLEN

extern "C" size_t
mbrlen (const char*, size_t, _RWSTD_MBSTATE_T*) _LIBC_THROWS();

#endif   // _RWSTD_NO_MBRLEN && !_RWSTD_NO_MBRLEN_IN_LIBC


// declare mblen() if it's not declared in the system headers
// but is known to be defined in the libc binary
#if defined (_RWSTD_NO_MBLEN) && !defined (_RWSTD_NO_MBLEN_IN_LIBC)

#  undef _RWSTD_NO_MBLEN

extern "C" int
mblen (const char*, size_t) _LIBC_THROWS();

#endif   // _RWSTD_NO_MBLEN && !_RWSTD_NO_MBLEN_IN_LIBC


// declare mbtowc() if it's not declared in the system headers
// but is known to be defined in the libc binary
#if defined (_RWSTD_NO_MBTOWC) && !defined (_RWSTD_NO_MBTOWC_IN_LIBC)

#  undef _RWSTD_NO_MBTOWC

extern "C" int
mbtowc (wchar_t*, const char*, size_t) _LIBC_THROWS();

#endif   // _RWSTD_NO_MBTOWC && !_RWSTD_NO_MBTOWC_IN_LIBC


// declare wcsrtombs() if it's not declared in the system headers
// but is known to be defined in the libc binary
#if defined (_RWSTD_NO_WCSRTOMBS) && !defined (_RWSTD_NO_WCSRTOMBS_IN_LIBC)

#  undef _RWSTD_NO_WCSRTOMBS

extern "C" size_t
wcsrtombs (char*, const wchar_t**, size_t, _RWSTD_MBSTATE_T*) _LIBC_THROWS();

#endif   // _RWSTD_NO_WCSRTOMBS && !_RWSTD_NO_WCSRTOMBS_IN_LIBC


// declare wcrtomb() if it's not declared in the system headers
// but is known to be defined in the libc binary
#if defined (_RWSTD_NO_WCRTOMB) && !defined (_RWSTD_NO_WCRTOMB_IN_LIBC)

#  undef _RWSTD_NO_WCRTOMB

extern "C" size_t
wcrtomb (char*, wchar_t, _RWSTD_MBSTATE_T*) _LIBC_THROWS();

#endif   // _RWSTD_NO_WCRTOMB && !_RWSTD_NO_WCRTOMB_IN_LIBC


// declare wctomb() if it's not declared in the system headers
// but is known to be defined in the libc binary
#if defined (_RWSTD_NO_WCTOMB) && !defined (_RWSTD_NO_WCTOMB_IN_LIBC)

#  undef _RWSTD_NO_WCTOMB

extern "C" {

_RWSTD_DLLIMPORT int
wctomb (char*, wchar_t) _LIBC_THROWS();

}   // extern "C"

#endif   // _RWSTD_NO_WCTOMB && !_RWSTD_NO_WCTOMB_IN_LIBC


// utf8 encoding maximum size
#undef  _UTF8_MB_CUR_MAX
#define _UTF8_MB_CUR_MAX   6


enum {
    // mask to retrieve ISO-2022 encoding type
    __rw_2022_mask   = 0x000f,

    // UCS-4 or UCS-2 (wchar_t width) encoding type
    __rw_ucs_be      = 0x000010,   // big-endian word and byte order
    __rw_ucs_le      = 0x000040,   // little-endian word and byte order
    __rw_ucs         = 0x0000f0,   // platform-endianness

    // UCS-4 encoding type
    __rw_ucs4_be     = 0x000100,   // big-endian word and byte order
    __rw_ucs4_be_le  = 0x000200,   // big-endian word, little-endian byte order
    __rw_ucs4_le     = 0x000400,   // little-endian word and byte order
    __rw_ucs4_le_be  = 0x000800,   // little-endian word, big-endian byte order
    __rw_ucs4        = 0x000f00,   // platform-endianness

    // UCS-2 encoding type
    __rw_ucs2_be     = 0x001000,   // big-endian word and byte order
    __rw_ucs2_le     = 0x004000,   // little-endian word and byte order
    __rw_ucs2        = 0x00f000,   // platform-endianness

    // UTF encoding type
    __rw_utf8        = 0x010000,   // UTF-8
    __rw_utf16_be    = 0x020000,   // UTF-16, big-endian byte order
    __rw_utf16_le    = 0x040000,   // UTF-16, little-endian byte order
    __rw_utf16       = 0x060000,   // UTF-16, platform endianness

    __rw_strict      = 0x100000,   // do strict UCS validation

    __rw_use_libc    = 0x200000,   // always use libc locale

    // mask to retrieve UCS encoding type
    __rw_ucs_mask    =   __rw_ucs_be | __rw_ucs_le | __rw_ucs
                       | __rw_ucs4_be | __rw_ucs4_le
                       | __rw_ucs4_be_le | __rw_ucs4_le_be | __rw_ucs4
                       | __rw_ucs2_be | __rw_ucs2_le | __rw_ucs2,

    // mask to retrieve UTF encoding type
    __rw_utf_mask    =   __rw_utf8 | __rw_utf16_be | __rw_utf16_le
};

#define ISO2022_TYPE(x)   ((x) & __rw_2022_mask)

// helper macros to retrieve the type of an UCS and UTF encoding
#define UCS_TYPE(x)       ((x) & __rw_ucs_mask)
#define UTF_TYPE(x)       ((x) & __rw_utf_mask)

#define IS_UCS(x)         (UCS_TYPE (x) == __rw_ucs)
#define IS_UCS_BE(x)      (UCS_TYPE (x) == __rw_ucs_be)
#define IS_UCS_LE(x)      (UCS_TYPE (x) == __rw_ucs_le)
#define IS_UCS_BE_LE(x)   (UCS_TYPE (x) == __rw_ucs_be_le)
#define IS_UCS_LE_BE(x)   (UCS_TYPE (x) == __rw_ucs_le_be)

#define IS_UTF8(x)        (UCS_TYPE (x) == __rw_utf8)
#define IS_UTF16(x)       (UCS_TYPE (x) == __rw_utf16)
#define IS_UTF16_BE(x)    (UCS_TYPE (x) == __rw_utf16_be)
#define IS_UTF16_LE(x)    (UCS_TYPE (x) == __rw_utf16_le)

// evaluates to 1 if the facet should use the libc locale, 0 otherwise
#define USE_LIBC(impl, x) \
    ((!(impl) || ((x) & __rw_use_libc)) && !(UCS_TYPE (x) || UTF_TYPE (x)))


// _RWSTD_MBSTATE_T macro might expand to char* (on AIX)
typedef _RWSTD_MBSTATE_T StateT;


_RWSTD_NAMESPACE (__rw) {


static const struct {
    const char *mod;
    int         flags;
} __rw_ucsmods[] = {
    { "UCS",         __rw_ucs },
    { "UCS-4",       __rw_ucs4 },
    { "UCS-2",       __rw_ucs2 },
    { "UCS-4-BE",    __rw_ucs4_be },
    { "UCS-4-LE",    __rw_ucs4_le },
    { "UCS-2-BE",    __rw_ucs2_be },
    { "UCS-2-LE",    __rw_ucs2_le },
    { "UCS-4-BE-LE", __rw_ucs4_be_le },
    { "UCS-4-LE-BE", __rw_ucs4_le_be },
    { "UCS-BE",      __rw_ucs_be },
    { "UCS-LE",      __rw_ucs_le }
};

static const size_t
__rw_n_ucsmods = sizeof __rw_ucsmods / sizeof *__rw_ucsmods;


static inline int
__rw_mbsinit (const StateT *psrc)
{
#ifndef _RWSTD_NO_MBSINIT

    return mbsinit (psrc);

#else   // if defined (_RWSTD_NO_MBSINIT)

    // commented out to work around an HP aCC 1.21 bug
    /* static */ const StateT state = StateT ();
    return !psrc || 0 == memcmp (psrc, &state, sizeof state);

#endif   // _RWSTD_NO_MBSINIT

}


#undef min
#define min(a, b)   ((a) < (b) ? (a) : (b))
#undef max
#define max(a,b)    ((a) > (b)) ? (a) : (b)


#ifndef _RWSTD_NO_WCHAR_T

// behaves just like mbrlen(), except that if the character pointed to
// by `str' is the NUL character and `emax' is non-zero, the function
// returns 1
static inline size_t
__rw_libc_mbrlen (_RWSTD_MBSTATE_T &state,
                  const char       *str,
                  size_t            emax)
{
    _RWSTD_ASSERT (0 != str);

    if ('\0' == *str)
        return 0 < emax;

#ifndef _RWSTD_NO_MBRLEN

    // `emax' is the maximum number of elements of type char in `str'
    return mbrlen (str, emax, &state);

#elif !defined (_RWSTD_NO_MBLEN)

    _RWSTD_UNUSED (state);

    return mblen (str, emax);

#else   // if defined (_RWSTD_NO_MBLEN)

    _RWSTD_UNUSED (state);

    // this is bogus but it's the best we can do given the absence
    // of libc support for this functionality (more likely than not,
    // this code will never end up getting executed because we'll
    // never get this far)
    const char *next = str;

    for (; *next && emax; ++next, --emax);
        
    return next - str;

#endif   // _RWSTD_NO_MBRLEN

}


// does a simple transliteration of the UTF-8 encoded character string
static unsigned int
__rw_xlit (const _RW::__rw_codecvt_t* impl,
           const char *utf8s, size_t sz)
{
    const unsigned int* const ptbls = impl->get_xliteration_tab ();

    unsigned int index = 0;

    for (const unsigned int* ptbl = ptbls; sz; --sz, ++utf8s) {

        typedef unsigned char UChar;

        index = ptbl [UChar (*utf8s)];

        if (_RWSTD_UINT_MAX == index) 
            // transliteration not provided for this character
            return 0;

        if (index & 0x80000000) {
            ptbl = ptbls + 256 * index;
        }
        else {
            // utf-8 string representation should end here
            _RWSTD_ASSERT (0 == sz);
        }
    }

    return index & 0x80000000 ? 0 : index;
}


static _STD::codecvt_base::result
__rw_libc_do_in (_RWSTD_MBSTATE_T &state,
                 const char       *from, 
                 const char       *from_end,
                 const char*      &from_next,
                 wchar_t          *to, 
                 wchar_t          *to_limit,
                 wchar_t*         &to_next)
{
    _RWSTD_ASSERT (from <= from_end);
    _RWSTD_ASSERT (to <= to_limit);

    _STD::codecvt_base::result res = _STD::codecvt_base::ok;

    // compute the length of the source sequence in bytes and
    // the size of the destination buffer in wide characters
    size_t src_len  = from_end - from;
    size_t dst_size = to_limit - to;

    // set the initial values to the source and destination pointers
    const char* psrc = from;
    wchar_t*    pdst = to;

    while (dst_size && src_len) {

        // the number of bytes that form the next multibyte character
        size_t nbytes;

#ifndef _RWSTD_NO_MBRTOWC
        nbytes = mbrtowc (pdst, psrc, src_len, &state);
#elif !defined (_RWSTD_NO_MBTOWC)
        nbytes = mbtowc (pdst, psrc, src_len);
#else
        nbytes = _RWSTD_SIZE_MAX;
#endif

        // -1 indicates an invalid sequence (i.e., error)
        if (nbytes == size_t (-1)) {
        res = _STD::codecvt_base::error;
            break;
        }
 
        // -2 indicates an ambiguous but valid subsequence
        // (i.e., ok)
        if (nbytes == size_t (-2))
            break;

        // 0 indicates the NUL character (skip over it)
        if (nbytes == 0) 
            ++nbytes;

        // > 0 indicates the number of bytes in the successfully
        // converted multibyte character
        psrc    += nbytes;
        src_len -= nbytes;
        ++pdst;
        --dst_size;
    }

    // adjust "next" pointers
    from_next = psrc;
    to_next   = pdst;

    // if the conversion has exhausted all space in the destination
    // range AND there are more COMPLETE characters in the source
    // range then we have a "partial" conversion
    if (res == _STD::codecvt_base::ok && src_len && !dst_size) {
        _RWSTD_MBSTATE_T tmp_state = state;
        size_t tmp = __rw_libc_mbrlen (tmp_state, psrc, src_len);
        if (tmp < size_t (-2))
            res = _STD::codecvt_base::partial;
    }

    return res;
}


static _STD::codecvt_base::result
__rw_libc_do_out (_RWSTD_MBSTATE_T &state,
                  const wchar_t    *from, 
                  const wchar_t    *from_end,
                  const wchar_t*   &from_next,
                  char             *to, 
                  char             *to_limit,
                  char*            &to_next)
{
    _RWSTD_ASSERT (from <= from_end);
    _RWSTD_ASSERT (to <= to_limit);

    // verify that both ranges are valid
    _RWSTD_ASSERT (from && from_end || !from && !from_end);
    _RWSTD_ASSERT (to && to_limit || !to && !to_limit);

    // set the (possibly uninitialized) next pointers
    // to point to the beginning of each sequence
    from_next = from;
    to_next   = to;

    // save the value of MB_CUR_MAX and avoid repeatedly using
    // the macro for efficiency (it may expand to a function call)
    const size_t mb_cur_max = size_t (MB_CUR_MAX);

    // the result of conversion
    _STD::codecvt_base::result res = _STD::codecvt_base::ok;

    // the size of the available space in the destination range
    size_t dst_free = size_t (to_limit - to_next);

    // small temporary buffer used when the space in the destination
    // buffer is less than MB_CUR_MAX
    char buff [_RWSTD_MB_LEN_MAX];

    // convert the source sequence one character at a time
    for ( ; from_next < from_end; ++from_next) {

        // since there is no guarantee that the converted internal
        // character would fit in the remaining space in the
        // destination buffer, use the small local buffer if
        // the remaining space is less that the longest external
        // multibyte character (i.e., MB_CUR_MAX)
        char* const tmpdst = dst_free < mb_cur_max ? buff : to_next;

        // save the state in case it's changed but the converted
        // character doesn't fit in the destination buffer
        const _RWSTD_MBSTATE_T save_state = state;

        // the number of bytes in the resulting multibyte character
        // sequence, not including the terminating NUL
        size_t dst_len = 0;

#ifndef _RWSTD_NO_WCRTOMB

        // convert the next source character (note that it would be
        // unsafe to use wcsrtombs() since the source sequence may
        // not be NUL terminated)
        dst_len = wcrtomb (tmpdst, *from_next, &state);

#elif !defined (_RWSTD_NO_WCTOMB)

        _RWSTD_UNUSED (state);
        dst_len = wctomb (tmpdst, *from_next);

#else   // error

        _RWSTD_UNUSED (state);
        dst_len = _RWSTD_SIZE_MAX;

#endif   // _RWSTD_NO_WCRTOMB, _RWSTD_NO_WCTOMB

        // -1 is returned as an indication of an illegal sequence
        if (_RWSTD_SIZE_MAX == dst_len) {
            res = _STD::codecvt_base::error;
            break;
        }
 
        // if the multibyte sequence is the NUL character
        // adjust the result by one (i.e., the length of
        // the multibyte NUL character)
        if (0 == dst_len)
            ++dst_len;

        // if the remaining space in the destination sequence
        // is less than MB_CUR_MAX, check to see if the multibyte
        // character will fit in there
        if (dst_free < mb_cur_max) {
            if (dst_free < dst_len) {
                // the source character converted to a multibyte
                // character whose length in bytes is greater than
                // the available space in the destination sequence

                // restore the state to the value prior to the last
                // conversion and return partial
                state = save_state;
                res   = _STD::codecvt_base::partial;
                break;
            } 

            // the destination sequence has sufficient space
            // for the multibyte character
            memcpy (to_next, buff, dst_len);
        }

        // advance the destination next pointer one past the end
        // of the multibyte character and decrement the size of
        // of the available space in the destination sequence
        to_next  += dst_len;
        dst_free -= dst_len;
    }

    return res;
}


static _STD::codecvt_base::result
__rw_libc_do_unshift (_RWSTD_MBSTATE_T& state, char*& to_next, char* to_limit)
{
    // save current state
    _RWSTD_MBSTATE_T tmp_state = state;

    // use libc locale to obtain the shift sequence
    char tmp [_RWSTD_MB_LEN_MAX];

    size_t ret;

#ifndef _RWSTD_NO_WCRTOMB
    ret = wcrtomb (tmp, wchar_t (0), &state);
#elif !defined (_RWSTD_NO_WCTOMB)
    ret = wctomb (tmp, wchar_t (0));
#else
    ret = _RWSTD_SIZE_MAX;
#endif

    if (_RWSTD_SIZE_MAX == ret)
        return  _STD::codecvt_base::error;

    if (ret > size_t (to_limit - to_next)) {
        // restore the state and return partial
        state = tmp_state;
        return _STD::codecvt_base::partial;
    }

    // copy the shift sequence
    memcpy (to_next, tmp, ret);
    to_next += ret;
    return _STD::codecvt_base::ok;
}


// checks a UTF-8 sequence representing a single character
// for validity by performing a number of computationally
// relatively expensive tests; used only in strict mode
static bool
__rw_utf8validate (const char* from, size_t nbytes)
{
    _RWSTD_ASSERT (0 != from);
    _RWSTD_ASSERT (1 < nbytes && 7 > nbytes);

    const unsigned char* const byte =
        _RWSTD_REINTERPRET_CAST (const unsigned char*, from);

    // check for overlong sequences with the bit pattern shown below

    // 2 bytes: 1100 000x (10xx xxxx)
    if (2 == nbytes && (byte [0] & 0xfeU) == 0xfeU)
        return false;

    // 3 bytes: 1110 0000 100x xxxx (10xx xxxx)
    if (3 == nbytes) {

        // first detect and reject all overlong sequences
        if (0xe0U == byte [0] && (byte [1] & 0xe0U) == 0x80U)
            return false;

        // detect and reject UTF-16 surrogate pairs, i.e., UCS characters
        // in the range [U+D800, U+DFFF] (i.e., inclusive of both ends),
        // i.e., "\xed\xa0\x80" through "\xed\xbf\xbf", as well as U+FFFE
        // and U+FFFF, i.e., "\xef\xbf\xbe" and "\xef\xbf\xbf"
        if (0xedU == byte [0]) {
            if (byte [1] >= 0xa0)
                return false;
        }
        else if (   0xefU == byte [0] && 0xbfU == byte [1]
                 && 0xbeU <= byte [2] && 0xbfU >= byte [2])
            return false;
    }

    // 4 bytes: 1111 0000 1000 xxxx (10xx xxxx 10xx xxxx)
    if (4 == nbytes && 0xf0U == byte [0] && (byte [1] & 0xf0U) == 0x80U)
        return false;

    // 5 bytes: 11111 000 1000 0xxx (10xxx xxx 10xx xxxx 10xx xxxx)
    if (5 == nbytes && 0xf8U == byte [0] && (byte [1] & 0xf8U) == 0x80U)
        return false;

    // 6 bytes: 1111 1100 1000 00xx (10xx xxxx 10xx xxxx 10xxx xxx 10xxx xxx)
    if (6 == nbytes && 0xfcU == byte [0] && (byte [1] & 0xfcU) == 0x80U)
        return false;

    for (size_t i = 1; i < nbytes; ++i) {
        if ((byte [i] & ~0x3fU) != 0x80U)
            return false;   // invalid byte
    }

    return true;
}


static _STD::codecvt_base::result
__rw_libstd_do_in (const char                *from_end,
                   const char               *&from_next,
                   wchar_t                   *to_limit,
                   wchar_t                  *&to_next,
                   int                        flags,
                   const _RW::__rw_codecvt_t *impl)
{
    // use UCS as the internal encoding if `impl' is 0
    // (i.e., if the codecvt database is not available)
    if (!impl && !UCS_TYPE (flags))
        flags |= __rw_ucs;

    const bool use_ucs    = 0 != UCS_TYPE (flags);
    const bool strict_utf = 0 != (flags & __rw_strict);

    _STD::codecvt_base::result res;
    
    for (const unsigned* const tab = impl ? impl->n_to_w_tab () : 0; ; ) {

        if (from_next == from_end) {
            res = _STD::codecvt_base::ok;
            break;
        }

        if (to_next == to_limit) {
            res = _STD::codecvt_base::partial;
            break;
        }

        const char* from = from_next;

        wchar_t wchar;

        if (impl) {

            // compute the offset of the multibyte character into
            // one of the tables (UCS or wchar_t) where the value
            // of the internal character corresponding to the
            // multibyte character is stored
            const unsigned off = __rw_mbtowco (tab, from, from_end);

            if (_RWSTD_UINT_MAX == off) {
                // the source sequence forms neither a valid multibyte
                // character, nor is it an initial subsequence of one
                res = _STD::codecvt_base::error;
                break;
            }

            if (from == from_next) {
                // the source sequence forms an incomplete initial
                // subsequence of a valid multibyte character
                res = _STD::codecvt_base::partial;
                break;
            }

            wchar = use_ucs ? impl->get_ucs4_at_offset (off)
                            : impl->get_internal_at_offset (off);
        }
        else {
            // use an algorithmic transformation from UTF-8 to UCS-4
            _RWSTD_INT32_T wi;

            const char* tmp = __rw_utf8toucs4 (&wi, from, from_end);

            if (strict_utf && tmp) {

                // perform additional expensive UTF-8 validation
                const size_t utf_len = tmp - from;

                if (utf_len > 1 && !__rw_utf8validate (from, utf_len))
                    tmp = 0;
            }

            from = tmp;

            if (!from) {
                // the source sequence forms neither a valid UTF-8
                // character, nor is it an initial subsequence of one
                res = _STD::codecvt_base::error;
                break;
            }

            if (from_next == from) {
                // the source sequence forms an incomplete initial
                // subsequence of a valid UTF-8 character
                res = _STD::codecvt_base::partial;
                break;
            }

            wchar = wchar_t (wi);
        }

        from_next  = from;
        *to_next++ = wchar;
    }

    return res;
}


static _STD::codecvt_base::result
__rw_libstd_do_out (const wchar_t             *from,
                    const wchar_t             *from_end,
                    const wchar_t            *&from_next,
                    char                      *to,
                    char                      *to_limit,
                    char                     *&to_next,
                    int                        flags,
                    const _RW::__rw_codecvt_t *impl)
{
    // final result of the transformation
    _STD::codecvt_base::result res = _STD::codecvt_base::ok;

    const bool use_ucs = IS_UCS (flags);

    // utf8 temporary buffer
    char utfbuf [_UTF8_MB_CUR_MAX + 1];

    const unsigned int* const tbl =
        impl ? use_ucs ? impl->utf8_to_ext_tab () : impl->w_to_n_tab () : 0;

    for (from_next = from, to_next = to; from_next != from_end; ++from_next) {

        if (to_next == to_limit) {
            res = _STD::codecvt_base::partial;
            break;
        }

        if (flags & __rw_strict) {

            // in strict mode check wide character for validity
            // (i.e., diagnose surrogate pairs as illegal)

            typedef _RWSTD_UWCHAR_INT_T WIntT;

            // convert wchar_t to the unsigned form of its underlying
            // integer type that's safe in arithmetic comaprisons and
            // doesn't generate signed/unsigned compiler warnings
            const WIntT wi = _RWSTD_STATIC_CAST (WIntT, *from_next);

            if (   WIntT (0xd800U) <= wi && wi <= WIntT (0xdfffU)
                || WIntT (0xfffeU) <= wi && wi <= WIntT (0xffffU)) {
                res = _STD::codecvt_base::error;
                break;
            }
        }

        // compute the number of bytes available in the destination sequence
        const size_t bytes_avail = to_limit - to_next;

        size_t utf8_len;

        if (impl) {

            // encode the wide character value in UTF-8 as if it was UCS
            utf8_len = __rw_itoutf8 (*from_next, utfbuf);

            // convert the UTF-8 encoded wchar_t value into
            // into the external representation
            const char* utf = utfbuf;
            unsigned    off = __rw_mbtowco (tbl, utf, utf + utf8_len);

            // FIXME: block below has been disabled but is being compiled
            // to avoid syntax regressions; it might produce "unreachable
            // code" warnings with some compilers
            if (0 /* disbled */ && _RWSTD_UINT_MAX == off) {
                // try transliteration
                off = __rw_xlit (impl, utfbuf, utf8_len);
                if (0 == off) {
                    res = _STD::codecvt_base::error;
                    break;
                }
            }

            if (_RWSTD_UINT_MAX == off) {
                // the sequence does not form a valid character
                res = _STD::codecvt_base::error;
                break;
            }

            if (utf == utfbuf) {
                // the next multibyte character position was incomplete
                res = _STD::codecvt_base::ok;
                break;
            }

            // get the UTF-8 sequence corresponding to the wide character
            utf = _RWSTD_REINTERPRET_CAST (const char*, impl + 1) + off;

            // check that there's enough space in the destination sequence
            utf8_len = *utf ? strlen (utf) : 1;
            if (bytes_avail < utf8_len) {
                res = _STD::codecvt_base::partial;
                break;
            }

            // copy the UTF-8 sequence into the destination sequence
            memcpy (to_next, utf, utf8_len);
        }
        else  {

            // codeset is UTF-8
            // external encoding is UTF-8, so no lookup is necessary

            // encode the wide char in the destination buffer if it fits

            if (bytes_avail < _UTF8_MB_CUR_MAX) {
                utf8_len = __rw_itoutf8 (*from_next, utfbuf);

                if (bytes_avail < utf8_len) {
                    res = _STD::codecvt_base::partial;
                    break;
                }

                // move from temporary buffer to destination buffer
                memcpy (to_next, utfbuf, utf8_len);
            }
            else
                utf8_len = __rw_itoutf8 (*from_next, to_next);
        }

        to_next += utf8_len;
    }

    return res;
}


// implements do_length() on top of libc mbrlen()
static size_t 
__rw_libc_do_length (_RWSTD_MBSTATE_T &state,
                     const char       *from,
                     const char       *from_end,
                     size_t            imax)
{
    const char* const from_begin = from;

    for ( ; imax && from < from_end; --imax) {

        // compute the maximum length (in bytes) of the multibyte
        // character sequence starting at `from'
        size_t nbytes = from_end - from;
        if (_RWSTD_MB_LEN_MAX < nbytes)
            nbytes = _RWSTD_MB_LEN_MAX;

        // get the number of bytes used to encode the next multibyte
        // character (including NUL), or -1 if the sequence at `from'
        // does not form a valid multibyte character (and -2 if it's
        // an incomplete subsequence of a valid multibyte character)
        nbytes = __rw_libc_mbrlen (state, from, nbytes);

        // stop when an invalid or incomplete character is encountered
        if (nbytes >= size_t (-2))
            break;

        from += nbytes;
    }

    // return the number of bytes (extern_type characters) in the sequence
    return from - from_begin;
}


// implements do_length() for UTF-8@UCS
static size_t
__rw_utf8_do_length (const char    *from,
                     const char    *from_end,
                     size_t         imax,
                     int            flags)
{
    _RWSTD_ASSERT (from <= from_end);

    const bool strict_utf = 0 != (flags & __rw_strict);

    const char *from_next = from;

    // count successive utf8 characters - this is a UTF-8 encoding
    for ( ; imax && from_next < from_end; --imax) {

        // use an algorithmic transformation from UTF-8 to UCS-4
        _RWSTD_INT32_T wi;

        const char* const next = __rw_utf8toucs4 (&wi, from_next, from_end);

        // stop if the character is either invalid or incomplete
        if (!next || next == from_next)
            break;

        _RWSTD_ASSERT (from_next <= next);
        _RWSTD_ASSERT (next <= from_end);

        // perform additional expensive UTF-8 validation in strict mode
        const size_t utf_len = next - from_next;

        if (   strict_utf
            && utf_len > 1 && !__rw_utf8validate (from_next, utf_len))
            break;

        from_next = next;
    }

    // return the number of extern characters
    return from_next - from;
}


// 22.2.1.5.2  [lib.locale.codecvt.virtuals]
// including the resolution of lwg issue 305
//
// -9-  Preconditions: (from<=from_end) well-defined and true; state
//      initialized, if at the beginning of a sequence, or else equal
//      to the result of converting the preceding characters in the
//      sequence.
//
// -9a- Effects: The effect on the state argument is "as if" it called
//      do_in(state, from, from_end, from, to, to+max, to) for to pointing
//      to a buffer of at least max elements.
//
// -10- Returns: (from_next-from) where from_next is the largest value
//      in the range [from,from_end] such that the sequence of values
//      in the range [from,from_next) represents max or fewer valid
//      complete characters of type internT. The instantiation
//      codecvt<char, char, mbstate_t> returns the lesser of max
//      and (from_end-from).

// Note that the function returns the number of externT characters
// (i.e., those of type char for the required instantiations).

static size_t
__rw_libstd_do_length (const char*                from,
                       const char*                from_end, 
                       size_t                     imax,
                       int                        flags,
                       const _RW::__rw_codecvt_t* impl)  
{
    // use UCS as the internal encoding if `impl' is 0
    // (i.e., if the codecvt database is not available)
    if (!impl && !UCS_TYPE (flags))
        flags |= __rw_ucs;

    const bool use_ucs = 0 != UCS_TYPE (flags);

    if (impl) {

        const char* from_next = from;

        const unsigned char* const limit =
            _RWSTD_REINTERPRET_CAST (const unsigned char*, from_end);

        const unsigned int* const tbls =
            use_ucs ? impl->utf8_to_ext_tab () : impl->n_to_w_tab ();

        _RWSTD_ASSERT (tbls);

        // `imax' is the maximum number of intern_type characters
        for ( ; imax && from_next < from_end; --imax) {

            const unsigned char* next =
                _RWSTD_REINTERPRET_CAST (const unsigned char*, from_next);

            for (const unsigned* tbl = tbls; tbl [*next] & 0x80000000; ) {

                // check if the extern character is valid
                if (_RWSTD_UINT_MAX == tbl [*next]) 
                    return from_next - from;
                
                // the value here is the table index
                const unsigned index = tbl [*next] & 0x7fffffff;
                tbl = tbls + 256 * index;

                ++next;

                if (next == limit) 
                    return from_next - from;
            }

            from_next = _RWSTD_REINTERPRET_CAST (const char*, next + 1);
        }

        // return the number of extern characters
        return from_next - from;
    }

    return __rw_utf8_do_length (from, from_end, imax, flags);
}

}  //  namespace __rw


_RWSTD_NAMESPACE (std) {
        

_RW::__rw_facet_id codecvt<wchar_t, char, _RWSTD_MBSTATE_T>::id;


/* explicit */ codecvt<wchar_t, char, _RWSTD_MBSTATE_T>::
codecvt (size_t __ref /* = 0 */)
    : _RW::__rw_facet (__ref)
{
    // no-op
}


// outlined to avoid generating a vtable in each translation unit
// that uses the class
/* virtual */ codecvt<wchar_t, char, _RWSTD_MBSTATE_T>::
~codecvt ()
{
    // no-op
}


/* virtual */ bool
codecvt<wchar_t, char, _RWSTD_MBSTATE_T>::
do_always_noconv () const _THROWS (())
{
    return false;   // conversion always necessary
}


/* virtual */ int
codecvt<wchar_t, char, _RWSTD_MBSTATE_T>::
do_encoding () const _THROWS (())
{
    // 22.2.1.5.2 p6    1(ext.) <=> 1(int.)
    return 1;   
}


/* virtual */ int
codecvt<wchar_t, char, _RWSTD_MBSTATE_T>::
do_max_length () const _THROWS (())
{
    return 1;
}


/* virtual */ codecvt_base::result
codecvt<wchar_t, char, _RWSTD_MBSTATE_T>::
do_out (state_type         &state,
        const intern_type  *from,
        const intern_type  *from_end,
        const intern_type *&from_next,
        extern_type        *to,
        extern_type        *to_end,
        extern_type       *&to_next) const
{
    // 22.2.1.5.2, p1 - preconditions
    _RWSTD_ASSERT (from <= from_end);
    _RWSTD_ASSERT (to   <= to_end);

    // verify that both ranges are valid
    _RWSTD_ASSERT (from && from_end || !from && !from_end);
    _RWSTD_ASSERT (to && to_end || !to && !to_end);

    // next pointers must always be set before returning, even on error
    from_next = from;
    to_next   = to;

#ifdef _RWSTDDEBUG

    // verify that the conversion state is valid
    const int mbstate_valid = _RW::__rw_mbsinit (&state);

    _RWSTD_ASSERT (0 != mbstate_valid);

#else   // if !defined (_RWSTDDEBUG)

    _RWSTD_UNUSED (state);

#endif   // _RWSTDDEBUG

    // copy internal sequence to external
    for (; from_next != from_end && to_next != to_end; ++from_next, ++to_next)
        // prevent conversion problems due to char signedness
        *to_next = _RWSTD_STATIC_CAST (unsigned char, *from_next);

    // Table 53, and lwg issue 382: do_out() returns partial
    // if not all source characters could be converted (e.g.,
    // because the destination range is full)
    return from_next == from_end ? ok : partial;
}


/* virtual */ codecvt_base::result
codecvt<wchar_t, char, _RWSTD_MBSTATE_T>::
do_in (state_type         &state,
       const extern_type  *from,
       const extern_type  *from_end,
       const extern_type *&from_next,
       intern_type        *to,
       intern_type        *to_end,
       intern_type       *&to_next) const
{
    // 22.2.1.5.2, p1 - preconditions
    _RWSTD_ASSERT (from <= from_end);
    _RWSTD_ASSERT (to   <= to_end);

    // verify that both ranges are valid
    _RWSTD_ASSERT (from && from_end || !from && !from_end);
    _RWSTD_ASSERT (to && to_end || !to && !to_end);

    // next pointers must always be set before returning, even on error
    from_next = from;
    to_next   = to;

    const int mbstate_valid = _RW::__rw_mbsinit (&state);

    _RWSTD_ASSERT (0 != mbstate_valid);
    if (!mbstate_valid)
        return error;

    // copy external sequence to internal
    for (; from_next != from_end && to_next != to_end;
         ++from_next, ++to_next) {
        // prevent conversion problems due to char signedness
        *to_next = _RWSTD_STATIC_CAST (unsigned char, *from_next);
    }

    return from_next == from_end ? ok : partial;
}


/* virtual */ codecvt_base::result
codecvt<wchar_t, char, _RWSTD_MBSTATE_T>::
do_unshift (state_type   &state,
            extern_type  *to,
            extern_type  *to_end,
            extern_type *&to_next) const
{
    // verify that the range is valid
    _RWSTD_ASSERT (to <= to_end);
    _RWSTD_ASSERT (to && to_end || !to && !to_end);

    _RWSTD_UNUSED (to_end);

    // next pointer must always be set before returning, even on error
    to_next = to;

    const int mbstate_valid = _RW::__rw_mbsinit (&state);

    _RWSTD_ASSERT (0 != mbstate_valid);
    if (!mbstate_valid)
        return error;

    return noconv;
}


/* virtual */ int
codecvt<wchar_t, char, _RWSTD_MBSTATE_T>::
do_length (state_type        &state,
           const extern_type *from,
           const extern_type *from_end,
           size_t             imax) const
{
    // 22.2.1.5.2, p9 - preconditions
    _RWSTD_ASSERT (from <= from_end);

    // verify that the range is valid
    _RWSTD_ASSERT (from && from_end || !from && !from_end);

    const int mbstate_valid = _RW::__rw_mbsinit (&state);

    _RWSTD_ASSERT (0 != mbstate_valid);
    if (!mbstate_valid)
        return 0;

    // 22.2.1.5.2, p10
    const size_t len = from_end - from;
    return int (len < imax ? len : imax);
}


// codecvt_byname <wchar,char> specialization
codecvt_byname<wchar_t, char, _RWSTD_MBSTATE_T>:: 
codecvt_byname (const char *name, size_t ref)
    : codecvt<wchar_t, char, _RWSTD_MBSTATE_T>(ref)
{
    _C_flags = _RW::__rw_encoding_from_name (name);

#if 0

    // FIXME: reliably detect whether the encoding is really stateless
    //        when `name' refers to a libc locale name

    if (_RW::stateless == (_C_flags & 0xf)) {
        const _RW::__rw_setlocale clocale (_C_name, LC_CTYPE);

        _C_flags = mbtowc (0, 0, 0) ? _RW::stateful : _RW::stateless;
    }

#endif   // 0/1

    char locale_name [256];

    // look for the `@' name modifier in the locale name
    const char* mod = strchr (name, '@');
    if (mod) {

        const char* const   mod_nam = mod + 1;
        const size_t        mod_len = strlen (mod_nam);

        // search for one of the known modifiers
        if (mod_len > 2 && !memcmp (mod_nam, "UCS", 3)) {

            int flags = 0;

            for (size_t i = 0; i != _RW::__rw_n_ucsmods; ++i) {
                if (!strcmp (_RW::__rw_ucsmods [i].mod, mod_nam)) {
                    flags = _RW::__rw_ucsmods [i].flags;
                    break;
                }
            }

            if (flags)
                _C_flags |= flags;
            else
                mod = 0;
        }
        else
            mod = 0;   // not a "@UCS-" modifier
    }

    if (mod) {
        _RWSTD_ASSERT (long (mod - name) < long (sizeof locale_name));

        memcpy (locale_name, name, mod - name);
        locale_name [mod - name] = '\0';
        name = locale_name;
    }

    // handle the special names: UTF-8, UTF-16, UTF-16-BE, UTF-16-LE,
    // denoting an external UTF encoding with strict validation rules
    // but slower processing, and their relaxed but faster equivalents,
    // utf-8, utf-16, utf-16-be, utf-16-le
    const size_t name_len = strlen (name);

    if (4 < name_len) {

        // check whether the UTF- prefix is in lowercase or capital letters
        const bool pfx_low = !memcmp (name, "utf-", 4);
        const bool pfx_cap = !pfx_low && !memcmp (name, "UTF-", 4);

        int flags = 0;

        if (pfx_low || pfx_cap) {

            if (5 == name_len && '8' == name [4])
                flags = __rw_utf8;
            else if (!strcmp (name + 4, "16"))
                flags = __rw_utf16;
            else if (pfx_low) {
                if (!strcmp (name + 4, "16-BE"))
                    flags = __rw_utf16_be;
                else if (!strcmp (name + 4, "16-LE"))
                    flags = __rw_utf16_le;
            }
            else if (pfx_cap) {
                if (!strcmp (name + 4, "16-BE"))
                    flags = __rw_utf16_be;
                else if (!strcmp (name + 4, "16-LE"))
                    flags = __rw_utf16_le;
            }
        }

        if (flags) {
            _C_flags |= flags;

            if (pfx_cap)
                _C_flags |= __rw_strict ;

            *locale_name  = '\0';
            name          = locale_name;
        }
    }

    if (   this->_C_opts & this->_C_use_libc
        && !UCS_TYPE (_C_flags) && !UTF_TYPE (_C_flags))
        _C_flags |= __rw_use_libc;

    this->_C_set_name (name, _C_namebuf, sizeof _C_namebuf);
}


// outlined to avoid generating a vtable in each translation unit
// that uses the class
/* virtual */ codecvt_byname<wchar_t, char, _RWSTD_MBSTATE_T>::~
codecvt_byname ()
{
    // no-op
}


/* virtual */ codecvt_base::result
codecvt_byname<wchar_t, char, _RWSTD_MBSTATE_T>::
do_in (state_type&         state,
       const extern_type*  from, 
       const extern_type*  from_end,
       const extern_type*& from_next,
       intern_type*        to, 
       intern_type*        to_limit,
       intern_type*&       to_next) const 
{
    // 22.2.1.5.2 p1
    _RWSTD_ASSERT (from <= from_end);
    _RWSTD_ASSERT (to <= to_limit);

    // verify that both ranges are valid
    _RWSTD_ASSERT (from && from_end || !from && !from_end);
    _RWSTD_ASSERT (to && to_limit || !to && !to_limit);

    // the standard does not specify the value of
    // *next pointers at entry;
    from_next = from;
    to_next = to;

    // do_in result
    result res = error;

    // test the type of the encoding that the facet is interpreting
    switch (ISO2022_TYPE (_C_flags)) {

    case _RW::stateless: {

        // obtain the mapping of the database file
        const _RW::__rw_codecvt_t* const impl =
            _RWSTD_STATIC_CAST (const _RW::__rw_codecvt_t*, this->_C_data ());
    
        if (USE_LIBC (impl, _C_flags)) {
            // use libc locale
            const _RW::__rw_setlocale clocale (_C_name, LC_CTYPE);

#ifndef _RWSTD_NO_MBTOWC

            // verify that either the encoding is stateful
            // or the state is in its initial shift state
            const bool mbstate_valid =
               mbtowc (0, 0, 0) || _RW::__rw_mbsinit (&state);

            _RWSTD_ASSERT (mbstate_valid);
            _RWSTD_UNUSED (mbstate_valid);

#endif   // _RWSTD_NO_MBTOWC

            res = _RW::__rw_libc_do_in (state, 
                                        from, from_end, from_next,
                                        to, to_limit, to_next);
        }
        else {

            // verify that the state is in its initial shift state
            const int mbstate_valid = _RW::__rw_mbsinit (&state);
            _RWSTD_ASSERT (mbstate_valid);

            _RWSTD_UNUSED (mbstate_valid);

            res = _RW::__rw_libstd_do_in (from_end, from_next,
                                          to_limit, to_next,
                                          _C_flags, impl);
        }
        break;
    }

    case _RW::iso2022_jp:
        res =  _RW::__rw_iso2022jp_do_in (state, from_next, from_end, 
                                          to_next, to_limit);
        break;

    case _RW::iso2022_jp2:
        res =  _RW::__rw_iso2022jp2_do_in (state, from_next, from_end, 
                                           to_next, to_limit);
        break;

    case _RW::iso2022_kr:
    case _RW::iso2022_cn:
    default:
        break;
    };

    return res;
}


/* virtual */ codecvt_base::result
codecvt_byname<wchar_t, char, _RWSTD_MBSTATE_T>::
do_out (state_type         &state, 
        const intern_type  *from, 
        const intern_type  *from_end, 
        const intern_type *&from_next,
        extern_type        *to, 
        extern_type        *to_limit, 
        extern_type       *&to_next) const
{
    // 22.2.1.5.2 p1
    _RWSTD_ASSERT (from <= from_end);
    _RWSTD_ASSERT (to <= to_limit);

    // verify that both ranges are valid
    _RWSTD_ASSERT (from && from_end || !from && !from_end);
    _RWSTD_ASSERT (to && to_limit || !to && !to_limit);

    // the standard does not specify the value of the next pointers on entry
    from_next = from;
    to_next   = to;

    // conversion result
    codecvt_base::result res = codecvt_base::error;

    // test the type of the encoding that the facet is interpreting
    switch (ISO2022_TYPE (_C_flags)) {

    case _RW::stateless: {

        // obtain the mapping of the database file
        const _RW::__rw_codecvt_t* impl = 
            _RWSTD_STATIC_CAST(const _RW::__rw_codecvt_t*, this->_C_data ());
    
        if (USE_LIBC (impl, _C_flags)) {
            // use libc locale
            const _RW::__rw_setlocale clocale (_C_name, LC_CTYPE);

#ifndef _RWSTD_NO_MBTOWC

            // verify that either the encoding is stateful
            // or the state is in its initial shift state
            const bool mbstate_valid =
               mbtowc (0, 0, 0) || _RW::__rw_mbsinit (&state);

            _RWSTD_ASSERT (mbstate_valid);
            _RWSTD_UNUSED (mbstate_valid);

#endif   // _RWSTD_NO_MBTOWC
            
            res = _RW::__rw_libc_do_out (state, from, from_end, from_next,
                                         to, to_limit, to_next);
        }
        else {
            // verify that the state is in its initial shift state
            const int mbstate_valid = _RW::__rw_mbsinit (&state);
            _RWSTD_ASSERT (mbstate_valid);

            _RWSTD_UNUSED (mbstate_valid);

            // use own implementation
            res = _RW::__rw_libstd_do_out (from, from_end, from_next,
                                           to, to_limit, to_next,
                                           _C_flags, impl);
        }
        break;
    }
    case _RW::iso2022_jp:
        res = _RW::__rw_iso2022jp_do_out (state, from_next, from_end, 
                                          to_next, to_limit);
        break;
    case _RW::iso2022_jp2:
        res = _RW::__rw_iso2022jp2_do_out (state, from_next, from_end, 
                                           to_next, to_limit);
        break;
    case _RW::iso2022_kr:
    case _RW::iso2022_cn:
    default:
        break;
    };

    return res;
}


/* virtual */ codecvt_base::result
codecvt_byname<wchar_t, char, _RWSTD_MBSTATE_T>::
do_unshift (state_type&   state, 
            extern_type*  to, 
            extern_type*  to_limit, 
            extern_type*& to_next) const
{
    // verify that the range is valid
    _RWSTD_ASSERT (to <= to_limit);
    _RWSTD_ASSERT (to && to_limit || !to && !to_limit);

    // the standard does not specify the value of
    // to_next pointer at entry;
    to_next = to;

    // do_unshift result
    codecvt_base::result res = codecvt_base::error;

    // test the type of the encoding that the facet is interpreting
    switch (ISO2022_TYPE (_C_flags)) {

    case _RW::stateless:

        if (USE_LIBC (_C_impdata, _C_flags)) {

            const _RW::__rw_setlocale clocale (_C_name, LC_CTYPE);
            
            if (mbtowc (0, 0, 0) == 0) {
                // verify that the state is in its initial shift state
                const int mbstate_valid = _RW::__rw_mbsinit (&state);
                _RWSTD_ASSERT (mbstate_valid);

                res = mbstate_valid ? noconv : error;
            }
            else
                res = _RW::__rw_libc_do_unshift (state, to_next, to_limit);
        }
        else {
            // use our own implementation

            // verify that the state is in its initial shift state
            const int mbstate_valid = _RW::__rw_mbsinit (&state);
            _RWSTD_ASSERT (mbstate_valid);

            res = mbstate_valid ? noconv : error;
        }
        break;

    case _RW::iso2022_jp:
        res = _RW::__rw_iso2022jp_do_unshift (state, to_next, to_limit);
        break;
    case _RW::iso2022_jp2:
        res = _RW::__rw_iso2022jp2_do_unshift (state, to_next, to_limit);
        break;
    case _RW::iso2022_kr:
    case _RW::iso2022_cn:
    default:
        break;
    };

    return res;
}


/* virtual */ int
codecvt_byname<wchar_t, char, _RWSTD_MBSTATE_T>::
do_length (state_type&        state, 
           const extern_type *from, 
           const extern_type *from_end, 
           size_t             cmax) const 
{
    // 22.2.1.5.2 p1
    _RWSTD_ASSERT (from <= from_end);

    // verify that the range is valid
    _RWSTD_ASSERT (from && from_end || !from && !from_end);

    size_t len = 0;

    // test the type of the encoding that the facet is interpreting
    switch (ISO2022_TYPE (_C_flags)) {

    case _RW::stateless: {

        // obtain the mapping of the database file
        const _RW::__rw_codecvt_t* impl = 
            _RWSTD_STATIC_CAST(const _RW::__rw_codecvt_t*, this->_C_data ());

        if (USE_LIBC (impl, _C_flags)) {
            // use libc locale
            const _RW::__rw_setlocale clocale (this->_C_name, LC_CTYPE);

#ifndef _RWSTD_NO_MBTOWC

            // verify that either the encoding is stateful
            // or the state is in its initial shift state
            const bool mbstate_valid =
                mbtowc (0, 0, 0) || _RW::__rw_mbsinit (&state);

            _RWSTD_ASSERT (mbstate_valid);
            _RWSTD_UNUSED (mbstate_valid);

#endif   // _RWSTD_NO_MBTOWC

            len = _RW::__rw_libc_do_length (state, from, from_end, cmax);
        }
        else {
            // verify that the state is in its initial shift state
            const int mbstate_valid = _RW::__rw_mbsinit (&state);
            _RWSTD_ASSERT (mbstate_valid);

            _RWSTD_UNUSED (mbstate_valid);

            len = _RW::__rw_libstd_do_length (from, from_end,
                                              cmax, _C_flags, impl);
        }
        break;
    }
    case _RW::iso2022_jp:
        len = _RW::__rw_iso2022jp_do_length (state, from, from_end, cmax);
        break;
    case _RW::iso2022_jp2:
        len = _RW::__rw_iso2022jp2_do_length (state, from, from_end, cmax);
        break;
    case _RW::iso2022_kr:
    case _RW::iso2022_cn:
    default:
        break;
    };

    return int (len);
}


/* virtual */ int
codecvt_byname<wchar_t, char, _RWSTD_MBSTATE_T>::
do_encoding () const _THROWS (())
{

    // -7- Returns: -1 if the encoding of the extern_type sequence is
    //     state-dependent; else the constant number of extern_type
    //     characters needed to produce an internal character;
    //     or 0 if this number is not a constant 227).
    // _____________
    // Footnote 227: If encoding() yields -1, then more than max_length()
    // extern_type elements may be consumed when producing a single internT
    // character, and additional extern_type elements may appear at the end
    // of a sequence after those that yield the final intern_type character.

    int enc = 0;   // variable-width stateless encoding

    // test the type of the encoding that the facet is interpreting
    switch (ISO2022_TYPE (_C_flags)) {

    case _RW::stateless: {
        // obtain the database file mapping
        const _RW::__rw_codecvt_t* impl = 
            _RWSTD_STATIC_CAST(const _RW::__rw_codecvt_t*, this->_C_data ());

        if (USE_LIBC (impl, _C_flags)) {
            // 7.20.7.2 of C99: if first arg is 0, mbtowc() returns non-zero 
            // or 0 whether or not the encoding is stateful or stateless
            const _RW::__rw_setlocale clocale (_C_name, LC_CTYPE);

            if (mbtowc (0, 0, 0)) {
                // in a stateful encoding, the number of extern_type
                // elements required to produce a single intern_type
                // character is indeterminate
                enc = -1;
            }
            else {
                // in a stateless encoding, the number of extern_type
                // elements required to produce a single intern_type
                // character is guaranteed to be 1 only when MB_CUR_MAX
                // is also 1, otherwise it's most likely to be variable
                // (e.g., EUC, UTF-8, etc.)
                if (1 == MB_CUR_MAX)
                    enc = 1;
            }
        }
        else {
            // use own implementation
            // 22.2.1.5.2 p6
            if (impl)
                enc = 1 < impl->mb_cur_max ? 0 : 1;
            else
                enc = 0;
        }

        break;
    }
    case _RW::iso2022_jp:
        enc = _RW::__rw_iso2022jp_do_encoding ();
        break;
    case _RW::iso2022_jp2:
        enc = _RW::__rw_iso2022jp2_do_encoding ();
        break;
    case _RW::iso2022_kr:
    case _RW::iso2022_cn:
    default:
        break;
    };

    return enc;
}


/* virtual */ int
codecvt_byname<wchar_t, char, _RWSTD_MBSTATE_T>::
do_max_length () const _THROWS (())
{
    // returns the max value do_length (s, from, from_end, 1) can return
    // for any valid range [from, from_end) - see LWG issue 74 (a DR)
    size_t max_len = 0;

    // test the type of the encoding that the facet is interpreting
    switch (ISO2022_TYPE (_C_flags)) {

    case _RW::stateless: {
        // obtain the database file mapping
        const _RW::__rw_codecvt_t* impl = 
            _RWSTD_STATIC_CAST(const _RW::__rw_codecvt_t*, this->_C_data ());
            
        if (USE_LIBC (impl, _C_flags)) {
            // use libc locale
            const _RW::__rw_setlocale clocale (_C_name, LC_CTYPE);

            max_len = size_t (MB_CUR_MAX);
        }
        else {
            // use own implementation
            if (IS_UTF8 (_C_flags))
                max_len = _UTF8_MB_CUR_MAX;
            else if (impl)
                max_len = size_t (impl->mb_cur_max);
        }
        break;
    }
    case _RW::iso2022_jp:
        max_len = _RW::__rw_iso2022jp_do_max_length ();
        break;
    case _RW::iso2022_jp2:
        max_len = _RW::__rw_iso2022jp2_do_max_length ();
        break;
    case _RW::iso2022_kr:
    case _RW::iso2022_cn:
    default:
        break;
    };

    return int (max_len);
}

#endif   // _RWSTD_NO_WCHAR_T

}   // namespace std


#ifndef _RWSTD_NO_WCHAR_T

#  define TARGS_W   <wchar_t, char, _RWSTD_MBSTATE_T>

_RWSTD_DEFINE_FACET_FACTORY (static, codecvt, TARGS_W, wcodecvt);
_RWSTD_SPECIALIZE_USE_FACET (wcodecvt);

#endif   // _RWSTD_NO_WCHAR_T
