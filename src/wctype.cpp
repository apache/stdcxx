/***************************************************************************
 *
 * wctype.cpp - source for the C++ Standard Library ctype<wchar_t> facets
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
 * Copyright 2001-2005 Rogue Wave Software.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <rw/_defs.h>

#ifndef _RWSTD_NO_WCHAR_T

#include <ctype.h>
#include <limits.h>
#include <locale.h>

#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>   // for wctob()
#endif   // _RWSTD_NO_WCHAR_H

#include <stdlib.h>   // for wctomb()
#include <string.h>   // for memset()
#include <wctype.h>   // for iswspace(), ...

#include <loc/_ctype.h>
#include <loc/_locale.h>
#include <loc/_localedef.h>

#include "setlocale.h"
#include "use_facet.h"

#ifdef _RWSTD_NO_EQUAL_CTYPE_MASK
#  include "once.h"   // for __rw_once()
#endif   // _RWSTD_NO_EQUAL_CTYPE_MASK

// utf8 encoding maximum size
#undef  _UTF8_MB_CUR_MAX
#define _UTF8_MB_CUR_MAX   6


_RWSTD_NAMESPACE (__rw) {

typedef _STD::ctype_base::mask MaskT;

extern const MaskT         __rw_classic_tab [];
extern const unsigned char __rw_upper_tab [];
extern const unsigned char __rw_lower_tab [];


static const wchar_t*
__rw_get_mask (__rw_ctype_t    *impl,
               const wchar_t   *beg,
               const wchar_t   *end,
               MaskT            mask,
               MaskT           *vec,
               bool             scan_is,
               bool             use_libstd,
               const char      *locname)
{
    _RWSTD_ASSERT (!end || beg <= end);

    if (use_libstd && impl != 0) {

        for (; beg != end; ++beg) {

            // using binary search look up the character and its mask
            _RWSTD_SIZE_T low  = 0;
            _RWSTD_SIZE_T high = (_RWSTD_SIZE_T)impl->wmask_s;

            if (vec)
                *vec = MaskT ();

            while (low <= high) {

                const _RWSTD_SIZE_T cur = (low + high) / 2;
                const __rw_mask_elm next_elm = impl->wmask_tab (cur);

                if (next_elm.ch == *beg) {

                    // character found
                    // if vec is non-0, fill it with the character's mask
                    // otherwise, if scanning for a match and the char's
                    // mask matches, or if scanning for a mismatch and
                    // the char's mask does not match, return the pointer
                    // to the current character; otherwise continue
                    if (vec)
                        *vec = MaskT (next_elm.mask & mask);
                    else if (    scan_is && next_elm.mask & mask
                             || !scan_is && !(next_elm.mask & mask))
                        return beg;

                    break;
                }

                // convert both sides to a sufficiently large type
                // with the same signedness
                if (  _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, next_elm.ch)
                    < _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, *beg))
                    low = cur + 1;
                else
                    high = cur - 1;
            }

            if (vec)
                ++vec;
        }

        return beg;
    }

    const __rw_setlocale clocale (locname, LC_CTYPE);

    for (; beg != end; ++beg) {

        int m = 0;

        // does the value of *beg fit into the range of an unsigned char?
        const bool fits_char =
            _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, *beg) <= _RWSTD_UCHAR_MAX;

        // `fits_char' is not used if all wide character
        // classification functions use below are present
        _RWSTD_UNUSED (fits_char);

        if (mask & __rw_space) {
#ifndef _RWSTD_NO_ISWSPACE
            if ((::iswspace)(*beg))
#else
            if (fits_char && (::isspace)(*beg))
#endif   // _RWSTD_NO_ISWSPACE
                m |= __rw_space;
        }

        if (mask & __rw_print) {
#ifndef _RWSTD_NO_ISWPRINT
            if ((::iswprint)(*beg))
#else
            if (fits_char && (::isprint)(*beg))
#endif   // _RWSTD_NO_ISWPRINT
                m |= __rw_print;
        }

        if (mask & __rw_cntrl) {
#ifndef _RWSTD_NO_ISWCNTRL
            if ((::iswcntrl)(*beg))
#else
            if (fits_char && (::iscntrl)(*beg))
#endif   // _RWSTD_NO_ISWCNTRL
                m |= __rw_cntrl;
        }

        if (mask & __rw_upper) {
#ifndef _RWSTD_NO_ISWUPPER
            if ((::iswupper)(*beg))
#else
            if (fits_char && (::isupper)(*beg))
#endif   // _RWSTD_NO_ISWUPPER
                m |= __rw_upper;
        }

        if (mask & __rw_lower) {
#ifndef _RWSTD_NO_ISWLOWER
            if ((::iswlower)(*beg))
#else
            if (fits_char && (::islower)(*beg))
#endif   // _RWSTD_NO_ISWLOWER
                m |= __rw_lower;
        }

        if (mask & __rw_alpha) {
#ifndef _RWSTD_NO_ISWALPHA
            if ((::iswalpha)(*beg))
#else
            if (fits_char && (::isalpha)(*beg))
#endif   // _RWSTD_NO_ISWALPHA
                m |= __rw_alpha;
        }

        if (mask & __rw_digit) {
#ifndef _RWSTD_NO_ISWDIGIT
            if ((::iswdigit)(*beg))
#else
            if (fits_char && (::isdigit)(*beg))
#endif
                m |= __rw_digit;
        }

        if (mask & __rw_punct) {
#ifndef _RWSTD_NO_ISWPUNCT
            if ((::iswpunct)(*beg))
#else
            if (fits_char && (::ispunct)(*beg))
#endif   // _RWSTD_NO_ISWPUNCT
                m |= __rw_punct;
        }

        if (mask & __rw_xdigit) {
#ifndef _RWSTD_NO_ISWXDIGIT
            if ((::iswxdigit)(*beg))
#else
            if (fits_char && (::isxdigit) (*beg))
#endif   // _RWSTD_NO_ISWXDIGIT
                m |= __rw_xdigit;
        }

        if (mask & __rw_graph) {
#ifndef _RWSTD_NO_ISWGRAPH
            if ((::iswgraph)(*beg))
#else
            if (fits_char && (::isgraph)(*beg))
#endif   // _RWSTD_NO_ISWGRAPH
                m |= __rw_graph;
        }

        if (vec)
            *vec++ = MaskT (m);
        else if (scan_is && m & mask || !scan_is && !(m & mask))
            break;
    }

    return beg;
}


static _STD::ctype_byname<wchar_t>::char_type
__rw_toupper (__rw_ctype_t* impl,
              wchar_t c, bool use_libstd,
              const char* locname)
{
    if (use_libstd && impl != 0) {

        _RWSTD_SIZE_T low  = 0;
        _RWSTD_SIZE_T high = (_RWSTD_SIZE_T)impl->wtoupper_s();
        _RWSTD_SIZE_T cur;

        __rw_upper_elm next_elm;

        while (low <= high) {
            cur = (low + high) / 2;
            next_elm = impl->wtoupper_tab(cur);
            if (next_elm.lower == c) {
                c = next_elm.upper;
                break;
            }

            // convert both sides to a sufficiently large type
            // with the same signedness
            if (  _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, next_elm.lower)
                < _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, c))
                low = cur + 1;
            else
                high = cur - 1;
        }
    }
    else {

#ifndef _RWSTD_NO_TOWUPPER

        const __rw_setlocale clocale (locname, LC_CTYPE);

        c = (::towupper)(c);

#else   // if defined (_RWSTD_NO_TOWUPPER)

        if (_RWSTD_STATIC_CAST (_RWSTD_SIZE_T, c) <= _RWSTD_UCHAR_MAX) {

            const __rw_setlocale clocale (locname, LC_CTYPE);

            c = (::toupper)(c);
        }

#endif   // _RWSTD_NO_TOWUPPER

    }

    return c;
}


static _STD::ctype_byname<wchar_t>::char_type
__rw_tolower (__rw_ctype_t* impl,
              wchar_t c, bool use_libstd,
              const char* locname)
{
    if (use_libstd && impl != 0) {

        _RWSTD_SIZE_T low  = 0;
        _RWSTD_SIZE_T high = (_RWSTD_SIZE_T)impl->wtolower_s ();
        _RWSTD_SIZE_T cur;

        __rw_lower_elm next_elm;

        while (low <= high) {
            cur = (low + high) / 2;
            next_elm = impl->wtolower_tab(cur);
            if (next_elm.upper == c) {
                c = next_elm.lower;
                break;
            }

            // convert both sides to a sufficiently large type
            // with the same signedness
            if (  _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, next_elm.upper)
                < _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, c))
                low = cur + 1;
            else
                high = cur - 1;
        }
    }
    else {

#ifndef _RWSTD_NO_TOWLOWER

        const __rw_setlocale clocale (locname, LC_CTYPE);

        c = (::towlower)(c);

#else   // if defined (_RWSTD_NO_TOWLOWER)

        if (_RWSTD_STATIC_CAST (_RWSTD_SIZE_T, c) <= _RWSTD_UCHAR_MAX) {

            const __rw_setlocale clocale (locname, LC_CTYPE);

            c = (::tolower)(c);
        }

#endif   // _RWSTD_NO_TOWLOWER

    }

    return c;
}


#if 0   // FIXME: implement analogously to the above

static void
__rw_narrow (const __rw_codecvt_t *cvt, bool use_libstd,
             const wchar_t *lo, const wchar_t *hi, char dfault, char *dst)
{
}


static void
__rw_widen (const __rw_codecvt_t *cvt, bool use_libstd,
            const char *lo, const char *hi, wchar_t *dst)
{
}

#endif   // 0/1

#ifdef _RWSTD_NO_EQUAL_CTYPE_MASK

// table of wide character classes
static _STD::ctype_base::mask
__rw_classic_wide_tab [_STD::ctype<char>::table_size];

// init-once flag for the classic wide tab
static __rw_once_t
__rw_classic_wide_tab_once_init = _RWSTD_ONCE_INIT;

extern "C" {

// one-time initializer for the classic wide_tab
static void
__rw_init_classic_wide_tab ()
{
#  ifdef _RWSTDDEBUG

    static int init;

    // paranoid check: verify that one-time initialization works
    _RWSTD_ASSERT (0 == init);

    ++init;

#  endif   // _RWSTDDEBUG

    // init the classic wide tab
    wchar_t wc_array [_STD::ctype<char>::table_size];

    for (wchar_t wc = 0; wc < _STD::ctype<char>::table_size; ++wc)
        wc_array [wc] = wc;

    __rw_get_mask (0, wc_array, wc_array + _STD::ctype<char>::table_size,
                   __rw_all, __rw_classic_wide_tab, false, false, "C");
}

}   // extern "C"

#endif   // _RWSTD_NO_EQUAL_CTYPE_MASK

}   // namespace __rw


_RWSTD_NAMESPACE (std) {


_RW::__rw_facet_id ctype<wchar_t>::id;


ctype<wchar_t>::ctype (_RWSTD_SIZE_T ref)
    : _RW::__rw_facet (ref)
{
#ifndef _RWSTD_NO_EQUAL_CTYPE_MASK
    _C_mask_tab  = _RW::__rw_classic_tab;
#else
    // initialize classic wide tab exactly once
    _RW::__rw_once (&_RW::__rw_classic_wide_tab_once_init,
                    _RW::__rw_init_classic_wide_tab);

    _C_mask_tab  = _RW::__rw_classic_wide_tab;
#endif
    _C_upper_tab = _RWSTD_CONST_CAST (_UChar*, _RW::__rw_upper_tab);
    _C_lower_tab = _RWSTD_CONST_CAST (_UChar*, _RW::__rw_lower_tab);
    _C_delete_it = false;

    memset (_C_narrow_tab, 0, sizeof _C_narrow_tab);
    memset (_C_wide_tab, 0, sizeof _C_wide_tab);
}


ctype<wchar_t>::~ctype ()
{
    if (_C_delete_it) {
        delete[] _RWSTD_CONST_CAST (mask*, _C_mask_tab);

        if (_C_upper_tab != _RW::__rw_upper_tab) {
            // lower_tab is allocated in the same block of memory
            // as upper_tab and must not be deleted
            delete[] _C_upper_tab;
        }
    }
}


bool ctype<wchar_t>::
do_is (mask m, char_type c) const
{
    return    _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, c) <= _C_tab_size
           && (_C_mask_tab [_UChar (c)] & m);
}


const ctype<wchar_t>::char_type*
ctype<wchar_t>::
do_is (const char_type *lo, const char_type *hi, mask *mvec) const
{
    _RWSTD_ASSERT (lo <= hi);

    for ( ; lo != hi; ++lo, ++mvec) {
        *mvec = _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, *lo) < _C_tab_size ?
            _C_mask_tab [_UChar (*lo)] : mask (0);
    }

    return hi;

}


ctype<wchar_t>::char_type
ctype<wchar_t>::
do_widen (char c) const
{
    // explicitly specifying template argument list to work around
    // HP aCC 3 and 5 bug (STDCXX-445)
    return _RWSTD_CONST_CAST (ctype<wchar_t>*, this)->
        _C_wide_tab [_UChar (c)] = char_type (_UChar (c));
}


const char*
ctype<wchar_t>::
do_widen (const char *lo, const char *hi, char_type *dest) const
{
    _RWSTD_ASSERT (lo <= hi);
    while (lo < hi) {
        // call widen instead of do_widen to take advantage of the
        // table optimization
        *dest++ = widen (*lo++);
    }
    return hi;
}


ctype<wchar_t>::char_type
ctype<wchar_t>::
do_toupper (char_type c) const
{
    return _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, c) < _C_tab_size ?
        _C_upper_tab [_UChar (c)] : c;
}


ctype<wchar_t>::char_type
ctype<wchar_t>::
do_tolower (char_type c) const
{
    return _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, c) < _C_tab_size ?
        _C_lower_tab [_UChar (c)] : c;
}


const ctype<wchar_t>::char_type*
ctype<wchar_t>::
do_toupper (char_type *lo, const char_type *hi) const
{
    _RWSTD_ASSERT (lo <= hi);

    for ( ; lo < hi; ++lo)
        *lo = _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, *lo) < _C_tab_size ?
            _C_upper_tab [_UChar (*lo)] : 0;

    return lo;
}


const ctype<wchar_t>::char_type*
ctype<wchar_t>::
do_tolower (char_type *lo, const char_type *hi) const
{
    _RWSTD_ASSERT (lo <= hi);

    for ( ; lo < hi; ++lo)
        *lo = _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, *lo) < _C_tab_size ?
            _C_lower_tab [_UChar (*lo)] : 0;

    return lo;
}


const ctype<wchar_t>::char_type*
ctype<wchar_t>::
do_scan_is (mask m, const char_type *lo, const char_type *hi) const
{
    _RWSTD_ASSERT (lo <= hi);

    for ( ; lo != hi; ++lo) {
        if (   _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, *lo) <= _C_tab_size
            && (_C_mask_tab [_UChar (*lo)] & m))
            break;
    }

    return lo;
}


const ctype<wchar_t>::char_type*
ctype<wchar_t>::
do_scan_not (mask m, const char_type *lo, const char_type *hi) const
{
    _RWSTD_ASSERT (lo <= hi);
    for ( ; lo != hi; ++lo) {
        if (   _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, *lo) >= _C_tab_size
            || !(_C_mask_tab [_UChar (*lo)] & m))
            break;
    }

    return lo;
}


char
ctype<wchar_t>::
do_narrow (char_type c, char dfault) const
{
    return _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, c) <= _RWSTD_SCHAR_MAX ?
        _RWSTD_STATIC_CAST (char, c) : dfault;
}


const ctype<wchar_t>::char_type*
ctype<wchar_t>::
do_narrow (const char_type *lo, const char_type *hi,
           char dfault, char *dest) const
{
    _RWSTD_ASSERT (lo <= hi);
    while (lo != hi) {
        *dest++ = do_narrow (*lo++, dfault);
    }
    return hi;
}


ctype_byname<wchar_t>::
ctype_byname (const char *name, _RWSTD_SIZE_T refs)
    : ctype<wchar_t> (refs), _C_cvtimpl (0), _C_cvtsize (0)
{
    this->_C_set_name (name, _C_namebuf, sizeof _C_namebuf);

    if ('C' == name [0] && '\0' == name [1])
        return;

    // delayed initialization not used for efficiency
    _RW::__rw_ctype_t* impl =
        _RWSTD_CONST_CAST (_RW::__rw_ctype_t*,
            _RWSTD_STATIC_CAST (const _RW::__rw_ctype_t*,
                _RW::__rw_get_facet_data (LC_CTYPE, _C_impsize, _C_name)));

    _C_impdata = impl;

    // Memory map the codecvt database and store it for future use
    if (impl)
        _C_cvtimpl = _RW::__rw_get_facet_data (LC_CTYPE, _C_cvtsize, 0, 
                                               impl->codeset_name ());

    if (impl == 0 || (this->_C_opts & this->_C_use_libc)) {

        // set locale first to avoid unnecessary allocation
        // if `name' doesn't refer to a valid locale (and
        // __rw_setlocale throws an exception)
        const _RW::__rw_setlocale clocale (name, LC_CTYPE);

        _C_delete_it = true;

        // casts prevent bogus gcc 2.95.2 warning:
        //     size in array new must have integral type
        _C_mask_tab = new mask [(_RWSTD_SIZE_T)_C_tab_size];

        _TRY {
            // avoid doing one extra allocation by allocating
            // both the upper and lower tables in the same space
            _C_upper_tab = new _UChar [(_RWSTD_SIZE_T)_C_tab_size * 2];
            _C_lower_tab = _C_upper_tab + _C_tab_size;
        }
        _CATCH (...) {
            delete[] _RWSTD_CONST_CAST (mask*, _C_mask_tab);
            _RETHROW;
        }

        for (char_type i = 0; i <= char_type (_RWSTD_UCHAR_MAX); i++) {

            int m = 0;

#ifndef _RWSTD_NO_ISWSPACE
            if ((::iswspace)(i))
#else
            if ((::isspace)(i))
#endif   // _RWSTD_NO_ISWSPACE
                m |= _RW::__rw_space;

#ifndef _RWSTD_NO_ISWPRINT
            if ((::iswprint)(i))
#else
            if ((::isprint)(i))
#endif   // _RWSTD_NO_ISWPRINT
                m |= _RW::__rw_print;

#ifndef _RWSTD_NO_ISWCNTRL
            if ((::iswcntrl)(i))
#else
            if ((::iscntrl)(i))
#endif   // _RWSTD_NO_ISWCNTRL
                m |= _RW::__rw_cntrl;

#ifndef _RWSTD_NO_ISWUPPER
            if ((::iswupper)(i))
#else
            if ((::isupper)(i))
#endif   // _RWSTD_NO_ISWUPPER
                m |= _RW::__rw_upper;

#ifndef _RWSTD_NO_ISWLOWER
            if ((::iswlower)(i))
#else
            if ((::islower)(i))
#endif   // _RWSTD_NO_ISWLOWER
                m |= _RW::__rw_lower;

#ifndef _RWSTD_NO_ISWALPHA
            if ((::iswalpha)(i))
#else
            if ((::isalpha)(i))
#endif   // _RWSTD_NO_ISWALPHA
                m |= _RW::__rw_alpha;

#ifndef _RWSTD_NO_ISWDIGIT
            if ((::iswdigit)(i))
#else
            if ((::isdigit)(i))
#endif   // _RWSTD_NO_ISWDIGIT
                m |= _RW::__rw_digit;

#ifndef _RWSTD_NO_ISWPUNCT
            if ((::iswpunct)(i))
#else
            if ((::ispunct)(i))
#endif   // _RWSTD_NO_ISWPUNCT
                m |= _RW::__rw_punct;

#ifndef _RWSTD_NO_ISWXDIGIT
            if ((::iswxdigit)(i))
#else
            if ((::isxdigit)(i))
#endif   // _RWSTD_NO_ISWXDIGIT
                m |= _RW::__rw_xdigit;

#ifndef _RWSTD_NO_ISWGRAPH
            if ((::iswgraph)(i))
#else
            if ((::isgraph)(i))
#endif   // _RWSTD_NO_ISWGRAPH
                m |= _RW::__rw_graph;

            _RWSTD_CONST_CAST (mask*, _C_mask_tab) [i]  = mask (m);

            typedef _RWSTD_SIZE_T SizeT;

#ifndef _RWSTD_NO_TOWUPPER
            const SizeT upr = SizeT ((::towupper)(i));
#else   // if defined (_RWSTD_NO_TOWUPPER)
            const SizeT upr = SizeT ((::toupper)(i));
#endif   // _RWSTD_NO_TOWUPPER

#ifndef _RWSTD_NO_TOWLOWER
            const SizeT lwr = SizeT ((::towlower)(i));
#else   // if defined (_RWSTD_NO_TOWLOWER)
            const SizeT lwr = SizeT ((::tolower)(i));
#endif   // _RWSTD_NO_TOWLOWER

            // optimize (and avoid warnings) when wint_t is unsigned
            _C_upper_tab [i] = upr <= SizeT (_RWSTD_UCHAR_MAX) ?
                _UChar (upr) : _UChar (0);

            _C_lower_tab [i] = lwr <= SizeT (_RWSTD_UCHAR_MAX) ?
                _UChar (lwr) : _UChar (0);
        }
    }
    else {

        _C_delete_it = false;

        _C_mask_tab  = (mask*)impl->mask_tab;
        _C_upper_tab = impl->toupper_tab;
        _C_lower_tab = impl->tolower_tab;
    }
}


ctype_byname<wchar_t>::
~ctype_byname ()
{
    // Release the codecvt database at this level
    if (_C_cvtimpl != 0 && _C_cvtsize != 0)
        _RW::__rw_release_facet_data (_C_cvtimpl, _C_cvtsize);
}


bool
ctype_byname<wchar_t>::
do_is (mask m, wchar_t c) const
{
    _RW::__rw_ctype_t* impl =
        _RWSTD_CONST_CAST (_RW::__rw_ctype_t*,
                           _RWSTD_STATIC_CAST (const _RW::__rw_ctype_t*,
                                               _C_data ()));

    const bool libstd = 0 != (this->_C_opts & this->_C_use_libstd);

    mask res = mask ();

    _RW::__rw_get_mask (impl, &c, &c + 1, m, &res, false, libstd, _C_name);

    return res != 0;
}


const ctype_byname<wchar_t>::char_type*
ctype_byname<wchar_t>::
do_is (const char_type *lo, const char_type *hi, mask *mvec) const
{
    _RWSTD_ASSERT (lo <= hi);
    _RW::__rw_ctype_t* impl =
        _RWSTD_CONST_CAST (_RW::__rw_ctype_t*,
                           _RWSTD_STATIC_CAST (const _RW::__rw_ctype_t*,
                                               _C_data ()));

    const bool libstd = 0 != (this->_C_opts & this->_C_use_libstd);

    return _RW::__rw_get_mask (impl, lo, hi, _RW::__rw_all,
                               mvec, false, libstd, _C_name);
}


ctype_byname<wchar_t>::char_type
ctype_byname<wchar_t>::
do_toupper (char_type c) const
{
    _RW::__rw_ctype_t* impl =
        _RWSTD_CONST_CAST (_RW::__rw_ctype_t*,
                           _RWSTD_STATIC_CAST (const _RW::__rw_ctype_t*,
                                               _C_data ()));

    const bool use_libstd = 0 != (this->_C_opts & this->_C_use_libstd);

    return _RW::__rw_toupper (impl, c, use_libstd, _C_name);
}


ctype_byname<wchar_t>::char_type
ctype_byname<wchar_t>::
do_tolower (char_type c) const
{
    _RW::__rw_ctype_t* impl =
        _RWSTD_CONST_CAST (_RW::__rw_ctype_t*,
                           _RWSTD_STATIC_CAST (const _RW::__rw_ctype_t*,
                                               _C_data ()));

    const bool use_libstd = 0 != (this->_C_opts & this->_C_use_libstd);

    return _RW::__rw_tolower (impl, c, use_libstd, _C_name);
}


const ctype_byname<wchar_t>::char_type*
ctype_byname<wchar_t>::
do_toupper (char_type *lo, const char_type *hi) const
{
    _RWSTD_ASSERT (lo <= hi);
    _RW::__rw_ctype_t* impl =
        _RWSTD_CONST_CAST (_RW::__rw_ctype_t*,
                           _RWSTD_STATIC_CAST (const _RW::__rw_ctype_t*,
                                               _C_data ()));

    const bool use_libstd = 0 != (this->_C_opts & this->_C_use_libstd);

    // FIXME: implement efficiently (__rw_toupper() may call setlocale())
    for ( ; lo < hi; ++lo)
        *lo = _RW::__rw_toupper (impl, *lo, use_libstd, _C_name);
    return lo;
}


const ctype_byname<wchar_t>::char_type*
ctype_byname<wchar_t>::
do_tolower (char_type *lo, const char_type *hi) const
{
    _RWSTD_ASSERT (lo <= hi);
    _RW::__rw_ctype_t* impl =
        _RWSTD_CONST_CAST (_RW::__rw_ctype_t*,
                           _RWSTD_STATIC_CAST (const _RW::__rw_ctype_t*,
                                               _C_data ()));

    const bool use_libstd = 0 != (this->_C_opts & this->_C_use_libstd);

    // FIXME: implement efficiently (__rw_tolower() may call setlocale())
    for ( ; lo < hi; ++lo)
        *lo = _RW::__rw_tolower (impl, *lo, use_libstd, _C_name);
    return lo;
}


const ctype_byname<wchar_t>::char_type*
ctype_byname<wchar_t>::
do_scan_is (mask m, const char_type* lo, const char_type* hi) const
{
    _RWSTD_ASSERT (lo <= hi);

    _RW::__rw_ctype_t* impl =
        _RWSTD_CONST_CAST (_RW::__rw_ctype_t*,
                           _RWSTD_STATIC_CAST (const _RW::__rw_ctype_t*,
                                               _C_data ()));
    const bool libstd = 0 != (this->_C_opts & this->_C_use_libstd);

    return _RW::__rw_get_mask (impl, lo, hi, m, 0, true, libstd, _C_name);
}


const ctype_byname<wchar_t>::char_type*
ctype_byname<wchar_t>::
do_scan_not (mask m, const char_type *lo, const char_type *hi) const
{
    _RWSTD_ASSERT (lo <= hi);

    _RW::__rw_ctype_t* impl =
        _RWSTD_CONST_CAST (_RW::__rw_ctype_t*,
                           _RWSTD_STATIC_CAST (const _RW::__rw_ctype_t*,
                                               _C_data ()));
    const bool libstd = 0 != (this->_C_opts & this->_C_use_libstd);

    return _RW::__rw_get_mask (impl, lo, hi, m, 0, false, libstd, _C_name);
}


char
ctype_byname<wchar_t>::
do_narrow (char_type c, char dfault) const
{
#if _RWSTD_WCHAR_MIN < 0

    // wchar_t is a signed type
    if (c > L'\0' && c < SCHAR_MAX)
        return char (c);

#else   // if _RWSTD_WCHAR_MIN >= 0

    // wchar_t is an unsigned type
    if (c < SCHAR_MAX)
        return char (c);

#endif   // _RWSTD_WCHAR_MIN

    const _RW::__rw_codecvt_t* cvt = 
        _RWSTD_STATIC_CAST(const _RW::__rw_codecvt_t*, _C_cvtimpl);

    char ch = 0;
    if (0 == cvt || (this->_C_opts & this->_C_use_libc)) {

#ifndef _RWSTD_NO_WCTOB

        const _RW::__rw_setlocale clocale (_C_name, LC_CTYPE);

        const int tmp = wctob (c);

        ch = tmp < 0 ? dfault : char (tmp);

#elif !defined (_RWSTD_NO_WCTOMB)

        char tmp [_RWSTD_MB_LEN_MAX];
        ch = 1 == wctomb (tmp, c) ? *tmp : dfault;

#else   // if defined (_RWSTD_NO_WCTOMB)

        ch = dfault;

#endif   // _RWSTD_NO_WCTOB, _RWSTD_NO_WCTOMB

    }
    else {
        
#if _RWSTD_WCHAR_MIN < 0
        // For a signed wchar_t test if the character has a negative value
        if (c < 0)
            return dfault;
#endif // _RWSTD_WCHAR_MIN < 0

        // Look up narrow character value in database; be aware that there 
        // is no way to know what the internal representation is for this 
        // wide character; we have to assume that the internal representation 
        // was native and use wide-to-narrow tables for conversion
        char_type wc = 0;

        unsigned int* tbl  = _RWSTD_CONST_CAST(unsigned int*, 
                                               cvt->w_to_n_tab ());
        unsigned int* ptbl = tbl;

        char  tmp [_UTF8_MB_CUR_MAX + 1];
        char* ptmp = tmp;
        _RWSTD_SIZE_T utf8_sz = _RW::__rw_itoutf8 (c, tmp);

#if _RWSTD_WCHAR_MIN < 0
        // compute the invalid bit mask (the MSB set)
        const wchar_t imask = wchar_t (~(_RWSTD_WCHAR_MAX));
#else
        const wchar_t imask = wchar_t (~(_RWSTD_WCHAR_MAX >> 1));
#endif // _RWSTD_WCHAR_MIN < 0

        typedef unsigned char UChar;
        
        _RWSTD_SIZE_T i = 0;
        wc = ptbl [UChar (*ptmp)];
        while (wc & imask) {
            // check validity of the value
            if (wc == (imask | _RWSTD_WCHAR_MAX)) 
                return dfault;

            ptbl = tbl + 256 * (wc & (~imask));
            ptmp++;

            // check for invalid sequences
            if (++i >= utf8_sz)
                return dfault;

            // fetch next byte
            wc = ptbl [UChar (*ptmp)];
        }

        // the resulting ch value is an index to the storage space of
        // the narrow sequence; for our purposes here, that sequence has
        // to have length of one
        const char* impl_raw = 
            _RWSTD_REINTERPRET_CAST(const char*, cvt);
        _RWSTD_SIZE_T offset = wc + sizeof (_RW::__rw_codecvt_t);

        if (impl_raw [offset + 1])
            return dfault;
        else
            ch = impl_raw [offset];
    }

    return ch;
}


const ctype_byname<wchar_t>::char_type*
ctype_byname<wchar_t>::
do_narrow (const char_type *lo, const char_type *hi,
           char dfault, char *dest) const
{
    _RWSTD_ASSERT (lo <= hi);

    // FIXME: implement w/o relying on another virtual function
    while (lo != hi) {
        *dest++ = do_narrow (*lo, dfault);
        lo++;
    }
    return hi;
}


ctype_byname<wchar_t>::char_type
ctype_byname<wchar_t>::
do_widen (char c) const
{
    const unsigned char u_c = _UChar (c);

    if (u_c <= SCHAR_MAX)
        return char_type (u_c);

    // when using the C++ locale database files then we have to do
    // a table lookup to get the wide version of this character
    // otherwise we have to call setlocale() and btowc() to get the
    // corresponding wchar_t
    const _RW::__rw_codecvt_t* cvt = 
        _RWSTD_STATIC_CAST (const _RW::__rw_codecvt_t*, _C_cvtimpl);

    char_type ch;

    if (0 == cvt || (this->_C_opts & this->_C_use_libc)) {

#ifndef _RWSTD_NO_BTOWC

        const _RW::__rw_setlocale clocale (_C_name, LC_CTYPE);

        // prevent sign extension if `c' is negative
        ch = btowc (u_c);

#else   // if defined (_RWSTD_NO_BTOWC)

        ch = char_type (u_c);

#endif   // _RWSTD_NO_BTOWC

    }
    else {

#if _RWSTD_WCHAR_MIN < 0
        // compute the invalid bit mask (the MSB set)
        const char_type imask = char_type (~(_RWSTD_WCHAR_MAX));
#else
        const char_type imask = char_type (~(_RWSTD_WCHAR_MAX >> 1));
#endif   // _RWSTD_WCHAR_MIN < 0

        // Lookup the narrow character in the table; be aware that 
        // the result of the lookup might be the index for another
        // table (i.e., the narrow character may be the first byte
        // of a multibyte character) in which case return an error.
        ch = cvt->n_to_w_tab ()[u_c];
        if (ch & imask)
            return char_type (_RWSTD_WEOF);

        ch = cvt->get_internal_at_offset (ch);
    }

    return ch;
}


const char*
ctype_byname<wchar_t>::
do_widen (const char *lo, const char *hi, char_type *dest) const
{
    _RWSTD_ASSERT (lo <= hi);

    // FIXME: implement w/o relying on another virtual function
    while (lo < hi) {
        *dest++ = widen (*lo++);
    }
    
    return hi;
}


}   // namespace std


_RWSTD_DEFINE_FACET_FACTORY (static, ctype, <wchar_t>, wctype);
_RWSTD_SPECIALIZE_USE_FACET (wctype);

#endif   // _RWSTD_NO_WCHAR_T
