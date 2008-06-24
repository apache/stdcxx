/***************************************************************************
 *
 * punct.cpp
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
 * Copyright 2001-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC
#include <rw/_defs.h>

#include <locale.h>   // for lconv, localeconv(), setlocale()
#include <stdio.h>    // for sprintf()
#include <stdlib.h>   // for mbstowcs()
#include <string.h>   // for memcpy()

#include <ios>        // for ios_base, needed by <rw/_punct.h>
#include <memory>     // for operator new() and delete()

#include "access.h"

#include <loc/_localedef.h>
#include <loc/_moneypunct.h>
#include <loc/_numpunct.h>

#include <rw/_error.h>

#include "setlocale.h"
#include "punct.h"


#if defined (_RWSTD_MSVC) && defined (_WIN64)
  // shut up MSVC/Win64 complaints about possible loss of data
#  pragma warning (disable: 4244)
#endif   // MSVC/Win64


_RWSTD_NAMESPACE (__rw) {


static inline void
__rw_copy (void *dst, const void *src, size_t n)
{
    memcpy (dst, src, n);
}

static inline void
__rw_widen (void *dst, const void *src, size_t n)
{
    mbstowcs (_RWSTD_STATIC_CAST (wchar_t*, dst),
              _RWSTD_STATIC_CAST (const char*, src), n);
}


static const void*
__rw_get_numpunct (const __rw_facet *pfacet, int flags)
{
    const int   member = flags & ~(__rw_mon | __rw_wide | __rw_intl);
    const bool  winx   = !!(flags & __rw_wide);   // wide or narrow index
    const void *pdata  = pfacet->_C_data ();      // mapped locale database

    if (pdata) {

        const __rw_punct_t* const pun =
            _RWSTD_STATIC_CAST (const __rw_punct_t*, pdata);

        const __rw_num_t *num;
    
        switch (member) {

        case __rw_dp: return pun->decimal_point (winx);
        case __rw_ts: return pun->thousands_sep (winx);
        case __rw_gr: return pun->grouping ();

        case __rw_tn:
            num = _RWSTD_STATIC_CAST (const __rw_num_t*, pun->get_ext ());
            _RWSTD_ASSERT (num);
            return num->truename (winx);

        case __rw_fn:
            num = _RWSTD_STATIC_CAST (const __rw_num_t*, pun->get_ext ());
            _RWSTD_ASSERT (num);
            return num->falsename (winx);

        default: _RWSTD_ASSERT (!"bad discriminant");
        }

        return 0;
    }
    else if (0 == __rw_access::_C_get_name (*pfacet)) {   // "C" locale

        static const void* const cdata[] = {
            ".", L".", ",", L",", "", L"", "true", L"true", "false", L"false"
        };

        switch (member) {

        case __rw_dp: return cdata [0 + winx];   // 22.2.3.1.2, p1
        case __rw_ts: return cdata [2 + winx];   // 22.2.3.1.2, p2
        case __rw_gr: return cdata [4 + winx];   // 22.2.3.1.2, p3
        case __rw_tn: return cdata [6 + winx];   // 22.2.3.1.2, p4
        case __rw_fn: return cdata [8 + winx];

        default: _RWSTD_ASSERT (!"bad discriminant");
        }

        return 0;
    }

    const char* const locname = __rw_access::_C_get_name (*pfacet) ?
        __rw_access::_C_get_name (*pfacet) : "C";
    
    // set all categories -- need LC_NUMERIC and LC_CTYPE
    const __rw_setlocale clocale (locname, _RWSTD_LC_ALL);

    if (pfacet->_C_data ()) {
        // check to see if another thread may have set _C_data()
        // while we were waiting for the lock in __rw_setlocale
        // ctor above and, if so, call self recursively on the
        // already initialized `impdata'
        return __rw_get_numpunct (pfacet, flags);
    }

    // get the lconv data
    const lconv* const pconv = localeconv ();
    if (!pconv)
        __rw_throw (_RWSTD_ERROR_LOCALE_BAD_NAME, locname);

    // be prepared to handle (non-conforming) null pointers
    // (e.g., AIX -- see bug #558)
    const char* const grp = pconv->grouping ? pconv->grouping : "";
    const char* const dp  = pconv->decimal_point ? pconv->decimal_point : "";
    const char* const ts  = pconv->thousands_sep ? pconv->thousands_sep : "";

    // the size of the narrow grouping string in bytes
    const size_t grsz = strlen (grp) + 1;

    // compute the sizes of all members in bytes
    size_t dpsz = strlen (dp) + 1;
    size_t tssz = strlen (ts) + 1;
    size_t tnsz = sizeof "true";
    size_t fnsz = sizeof "false";

    if (winx) {
        // adjust for wide characters
        dpsz *= sizeof (wchar_t);
        tssz *= sizeof (wchar_t);
        tnsz *= sizeof (wchar_t);
        fnsz *= sizeof (wchar_t);
    }

    // compute the actual size of the variable size structure
    const size_t size =
          sizeof (__rw_punct_t)
        + sizeof (__rw_num_t)
        + sizeof (size_t)   // maximum padding for alignment
        + grsz + dpsz + tssz + tnsz + fnsz;

    // allocate variable size structure(s)
    __rw_punct_t* const pun = 
        _RWSTD_STATIC_CAST (__rw_punct_t*, ::operator new (size));

    // compute offsets into the varaiable size structure
    pun->decimal_point_off [winx] = 0;
    pun->thousands_sep_off [winx] = pun->decimal_point_off [winx] + int (dpsz);

    // point widen to a wrapper for mbstowcs() for wchar_t or memcpy otherwise
    void (*widen)(void*, const void*, size_t) =
        winx ? __rw_widen : __rw_copy;

    // copy strings from lconv data members to the varaiable size structure
    char* s = _RWSTD_REINTERPRET_CAST (char*, pun + 1);

    widen (s + pun->decimal_point_off [winx], dp, dpsz);
    widen (s + pun->thousands_sep_off [winx], ts, tssz);

    // set the offset of the extended punctuation data
    pun->punct_ext_off = pun->thousands_sep_off [winx] + int (tssz);

    // adjust alignment
    const size_t misalign = pun->punct_ext_off % sizeof (size_t);
    if (misalign)
        pun->punct_ext_off += int (sizeof (size_t) - misalign);

    s = _RWSTD_REINTERPRET_CAST (char*, pun + 1) + pun->punct_ext_off;

    __rw_num_t* const num = _RWSTD_REINTERPRET_CAST (__rw_num_t*, s);

    s += sizeof *num;

    // invalidate members (offsets) by setting them to 0xff
    memset (num, ~0, sizeof *num);

    num->truename_off  [winx] = 0;
    num->falsename_off [winx] = num->truename_off [winx] + int (tnsz);

    widen (s + num->truename_off  [winx], "true", tnsz);
    widen (s + num->falsename_off [winx], "false", fnsz);

    // place narrow grouping string last to avoid alignment issues
    pun->grouping_off =
        _RWSTD_STATIC_CAST (_RWSTD_UINT32_T,
        pun->punct_ext_off + sizeof *num + num->falsename_off [winx] + fnsz);

    memcpy (s + num->falsename_off [winx] + fnsz, grp, grsz);

    // set `impdata' and `impsize' (facet base dtor will delete)
    __rw_access::_C_get_impdata (*_RWSTD_CONST_CAST (__rw_facet*, pfacet)) =
        pun;

    __rw_access::_C_get_impsize (*_RWSTD_CONST_CAST (__rw_facet*, pfacet)) =
        (size_t)(-1);

    // call self recursively on already initialized `impdata'
    return __rw_get_numpunct (pfacet, flags);
}

/***************************************************************************/


static const void*
__rw_get_moneypunct (const __rw_facet *pfacet, int flags)
{
    const int   member = flags & ~(__rw_mon | __rw_wide | __rw_intl);
    const bool  winx   = !!(flags & __rw_wide);   // wide or narrow index
    const bool  intl   = !!(flags & __rw_intl);   // international or domestic
    const void *pdata  = pfacet->_C_data ();      // mapped locale database

    if (pdata) {

        const __rw_punct_t* const pun =
            _RWSTD_STATIC_CAST (const __rw_punct_t*, pdata);

        const __rw_mon_t *mon;
    
        switch (member) {
        case __rw_dp: return pun->decimal_point (winx);
        case __rw_ts: return pun->thousands_sep (winx);
        case __rw_gr: return pun->grouping ();

        }

        // get extended data
        mon = _RWSTD_STATIC_CAST (const __rw_mon_t*, pun->get_ext ());
        _RWSTD_ASSERT (mon);

        typedef unsigned char UChar;

        switch (member) {

        case __rw_cs: return mon->curr_symbol (intl, winx);
        case __rw_ps: return mon->positive_sign (winx);
        case __rw_ns: return mon->negative_sign (winx);

        case __rw_pf: return mon->pos_format [intl];
        case __rw_nf: return mon->neg_format [intl];

        case __rw_fd:
            return _RWSTD_REINTERPRET_CAST (void*, (size_t)
                                            UChar (mon->frac_digits [intl]));

        default: _RWSTD_ASSERT (!"bad discriminant");
        }

        return 0;
    }
    else if (0 == __rw_access::_C_get_name (*pfacet)) {   // "C" locale

        static const void* const cdata[] = {
            ".", L".", ",", L",", "", L"", "", L"", "", L"",
            "", L"", "", L"", "\2\3\0\4", "\2\3\0\4", 
        };

        switch (member) {

        case __rw_dp: return cdata [0 + winx];
        case __rw_ts: return cdata [2 + winx];
        case __rw_gr: return cdata [4 + winx];
        case __rw_cs: return cdata [6 + winx + 2 * intl];
        case __rw_ps: return cdata [10 + winx];
        case __rw_ns: return cdata [12 + winx];
        case __rw_pf: return cdata [14];
        case __rw_nf: return cdata [15];

        // the number of fractional digits in the "C" locale is not specified
        case __rw_fd: return 0;   // no fractional digits

        default: _RWSTD_ASSERT (!"bad discriminant");
        }

        return 0;
    }

    const char* const locname = __rw_access::_C_get_name (*pfacet) ?
        __rw_access::_C_get_name (*pfacet) : "C";
    
    // set all categories -- need LC_NUMERIC and LC_CTYPE
    const __rw_setlocale clocale (locname, _RWSTD_LC_ALL);

    if (pfacet->_C_data ()) {
        // check to see if another thread may have set _C_data()
        // while we were waiting for the lock in __rw_setlocale
        // ctor above and, if so, call self recursively on the
        // already initialized `impdata'
        return __rw_get_moneypunct (pfacet, flags);
    }

    // get the lconv data
    const lconv* const pconv = localeconv ();
    if (!pconv)
        __rw_throw (_RWSTD_ERROR_LOCALE_BAD_NAME, locname);

    _RWSTD_ASSERT (pconv->mon_grouping);
    _RWSTD_ASSERT (pconv->mon_decimal_point);
    _RWSTD_ASSERT (pconv->mon_thousands_sep);

    // the size of the narrow grouping string in bytes
    const size_t grsz = strlen (pconv->mon_grouping) + 1;

    // compute the sizes of all members in bytes
    size_t dpsz = strlen (pconv->mon_decimal_point) + 1;
    size_t tssz = strlen (pconv->mon_thousands_sep) + 1;
    size_t pssz = strlen (pconv->positive_sign) + 1;
    size_t nssz = strlen (pconv->negative_sign) + 1;

    const char* const cs = intl ? pconv->int_curr_symbol
                                : pconv->currency_symbol;

    size_t cssz = strlen (cs) + 1;

    if (winx) {
        // adjust for wide characters
        dpsz *= sizeof (wchar_t);
        tssz *= sizeof (wchar_t);
        pssz *= sizeof (wchar_t);
        nssz *= sizeof (wchar_t);
        cssz *= sizeof (wchar_t);
    }

    // compute the actual size of the variable size structure
    const size_t size =
          sizeof (__rw_punct_t)
        + sizeof (__rw_mon_t)
        + sizeof (size_t)   // maximum padding for alignment
        + grsz + dpsz + tssz + pssz + nssz + cssz;

    // allocate variable size structure(s)
    __rw_punct_t* const pun = 
        _RWSTD_STATIC_CAST (__rw_punct_t*, ::operator new (size));

    // compute offsets into the varaiable size structure
    pun->decimal_point_off [winx] = 0;
    pun->thousands_sep_off [winx] = pun->decimal_point_off [winx] + int (dpsz);

    // point widen to a wrapper for mbstowcs() for wchar_t or memcpy otherwise
    void (*widen)(void*, const void*, size_t) =
        winx ? __rw_widen : __rw_copy;

    // copy strings from lconv data members to the varaiable size structure
    char* s = _RWSTD_REINTERPRET_CAST (char*, pun + 1);

    widen (s + pun->decimal_point_off [winx], pconv->mon_decimal_point, dpsz);
    widen (s + pun->thousands_sep_off [winx], pconv->mon_thousands_sep, tssz);

    // set the offset of the extended punctuation data
    pun->punct_ext_off = pun->thousands_sep_off [winx] + int (tssz);

    // adjust alignment
    const size_t misalign = pun->punct_ext_off % sizeof (size_t);
    if (misalign)
        pun->punct_ext_off += int (sizeof (size_t) - misalign);

    s = _RWSTD_REINTERPRET_CAST (char*, pun + 1) + pun->punct_ext_off;

    __rw_mon_t* const mon = _RWSTD_REINTERPRET_CAST (__rw_mon_t*, s);

    s += sizeof *mon;

    // invalidate members (offsets) by setting them to 0xff
    memset (mon, ~0, sizeof *mon);

    mon->positive_sign_off [winx] = 0;
    mon->negative_sign_off [winx] = mon->positive_sign_off [winx] + int (pssz);

    widen (s + mon->positive_sign_off [winx], pconv->positive_sign, pssz);
    widen (s + mon->negative_sign_off [winx], pconv->negative_sign, nssz);

    mon->curr_symbol_off [intl][winx] =
        mon->negative_sign_off [winx] + int (nssz);

    widen (s + mon->curr_symbol_off [intl][winx], cs, cssz);

    pun->grouping_off =
        _RWSTD_STATIC_CAST (_RWSTD_UINT32_T,
          pun->punct_ext_off + sizeof *mon
        + mon->curr_symbol_off [intl][winx] + cssz);

    // place narrow grouping string last to avoid alignment issues
    memcpy (s + mon->curr_symbol_off [intl][winx] + cssz,
            pconv->mon_grouping, grsz);

    mon->frac_digits [0]    = pconv->frac_digits;
    mon->frac_digits [1]    = pconv->int_frac_digits;

    // initialize domestic C90 formats
    mon->p_cs_precedes [0]  = pconv->p_cs_precedes;
    mon->n_cs_precedes [0]  = pconv->n_cs_precedes;
    mon->p_sep_by_space [0] = pconv->p_sep_by_space;
    mon->n_sep_by_space [0] = pconv->n_sep_by_space;
    mon->p_sign_posn [0]    = pconv->p_sign_posn;
    mon->n_sign_posn [0]    = pconv->n_sign_posn;

#ifndef _RWSTD_NO_LCONV_INT_FMAT

    // copy international formats from C99 members
    mon->p_cs_precedes [1]  = pconv->int_p_cs_precedes;
    mon->n_cs_precedes [1]  = pconv->int_n_cs_precedes;
    mon->p_sep_by_space [1] = pconv->int_p_sep_by_space;
    mon->n_sep_by_space [1] = pconv->int_n_sep_by_space;
    mon->p_sign_posn [1]    = pconv->int_p_sign_posn;
    mon->n_sign_posn [1]    = pconv->int_n_sign_posn;

#else   // if defined (_RWSTD_NO_LCONV_INT_FMAT)

    // copy international formats from local members
    mon->p_cs_precedes [1]  = mon->p_cs_precedes [0];
    mon->n_cs_precedes [1]  = mon->n_cs_precedes [0];
    mon->p_sep_by_space [1] = mon->p_sep_by_space [0];
    mon->n_sep_by_space [1] = mon->n_sep_by_space [0];
    mon->p_sign_posn [1]    = mon->p_sign_posn [0];
    mon->n_sign_posn [1]    = mon->n_sign_posn [0];

#endif   // _RWSTD_NO_LCONV_INT_FMAT

    enum {
        // for syntactic convenience
        none   = _STD::money_base::none,
        space  = _STD::money_base::space,
        symbol = _STD::money_base::symbol,
        sign   = _STD::money_base::sign,
        value  = _STD::money_base::value
    };

    static const _STD::money_base::pattern pat[] = {

        // cs_precedes [0..1]:
        //
        // An integer set to 1 if the currency_symbol precedes the value
        // for a monetary value, and set to 0 if the symbol succeeds
        // the value.

        // sep_by_space [0..2]:
        //
        // 0  No space separates the currency_symbol from the value for
        //    a monetary value.
        // 1  If the currency symbol and sign string are adjacent, a space
        //    separates them from the value; otherwise, a space separates
        //    the currency symbol from the value.
        // 2  If the currency symbol and sign string are adjacent, a space
        //    separates them; otherwise, a space separates the sign string
        //    from the value.

        // sign_posn [0..4]:
        //
        // An integer set to a value indicating the positioning of the
        // positive_sign for a monetary value. The following integer
        // values shall be recognized:
        //
        // 0  Parentheses enclose the value and the currency_symbol.
        // 1  The sign string precedes the value and the currency_symbol.
        // 2  The sign string succeeds the value and the currency_symbol.
        // 3  The sign string immediately precedes the currency_symbol.
        // 4  The sign string immediately succeeds the currency_symbol.

        // +-------- cs_precedes
        // |+----- sep_by_space
        // ||+-- sign_posn
        // |||
        // VVV  ....        -     1      $       .           // pattern
        /* 000: -1$. */ { { sign, value, symbol, none } },   // "\3\4\2\0"
        /* 001: -1$. */ { { sign, value, symbol, none } },   // "\3\4\2\0"
        /* 002: 1$-. */ { { value, symbol, sign, none } },   // "\4\2\3\0"
        /* 003: 1-$. */ { { value, sign, symbol, none } },   // "\4\3\2\0"
        /* 004: 1$-. */ { { value, symbol, sign, none } },   // "\4\2\3\0"

        /* 010: -1 $ */ { { sign, value, space, symbol } },  // "\3\4\1\2"
        /* 011: -1 $ */ { { sign, value, space, symbol } },  // "\3\4\1\2"
        /* 012: 1 $- */ { { value, space, symbol, sign } },  // "\4\1\2\3"
        /* 013: 1 -$ */ { { value, space, sign, symbol } },  // "\4\3\3\2"
        /* 014: 1 $- */ { { value, space, symbol, sign } },  // "\4\1\2\3"

        /* 020: - 1$ */ { { sign, space, value, symbol } },  // "\3\1\4\2"
        /* 021: - 1$ */ { { sign, space, value, symbol } },  // "\3\1\4\2"
        /* 022: 1$ - */ { { value, symbol, space, sign } },  // "\4\2\1\3"
        /* 023: 1- $ */ { { value, sign, space, symbol } },  // "\4\3\1\2"
        /* 024: 1$ - */ { { value, symbol, space, sign } },  // "\4\2\1\3"

        /* 100: -$1. */ { { sign, symbol, value, none } },   // "\3\2\4\0"
        /* 101: -$1. */ { { sign, symbol, value, none } },   // "\3\2\4\0"
        /* 102: $1-. */ { { symbol, value, sign, none } },   // "\2\4\3\0"
        /* 103: -$1. */ { { sign, symbol, value, none } },   // "\3\2\4\0"
        /* 104: $-1. */ { { symbol, sign, value, none } },   // "\2\3\4\0"

        /* 110: -$ 1 */ { { sign, symbol, space, value } },  // "\3\2\1\4"
        /* 111: -$ 1 */ { { sign, symbol, space, value } },  // "\3\2\1\4"
        /* 112: $ 1- */ { { symbol, space, value, sign } },  // "\2\1\4\3"
        /* 113: -$ 1 */ { { sign, symbol, space, value } },  // "\3\2\1\4"
        /* 114: $- 1 */ { { symbol, sign, space, value } },  // "\2\3\1\4"

        /* 120: - $1 */ { { sign, space, symbol, value } },  // "\3\1\2\4"
        /* 121: - $1 */ { { sign, space, symbol, value } },  // "\3\1\2\4"
        /* 122: $1 - */ { { symbol, value, space, sign } },  // "\2\4\1\3"
        /* 123: - $1 */ { { sign, space, symbol, value } },  // "\3\1\2\4"
        /* 124: $ -1 */ { { symbol, space, sign, value } }   // "\2\1\3\4"
    };

    size_t inx =   size_t (mon->p_cs_precedes [intl]) * (3U * 5U)
                 + size_t (mon->p_sep_by_space [intl]) * 5U
                 + size_t (mon->p_sign_posn [intl]);

    if (inx < sizeof pat / sizeof *pat)
        memcpy (mon->pos_format [intl], pat + inx, sizeof *pat);
    else
        memset (mon->pos_format [intl], none, sizeof *pat);

    inx =   size_t (mon->n_cs_precedes [intl]) * (3U * 5U)
          + size_t (mon->n_sep_by_space [intl]) * 5U
          + size_t (mon->n_sign_posn [intl]);

    if (inx < sizeof pat / sizeof *pat)
        memcpy (mon->neg_format [intl], pat + inx, sizeof *pat);
    else
        memset (mon->neg_format [intl], none, sizeof *pat);

    // set `impdata' and `impsize' (facet base dtor will delete)
    __rw_access::_C_get_impdata (*_RWSTD_CONST_CAST (__rw_facet*, pfacet)) =
        pun;

    __rw_access::_C_get_impsize(*_RWSTD_CONST_CAST (__rw_facet*, pfacet)) =
        (size_t)(-1);

    // call self recursively on already initialized `impdata'
    return __rw_get_moneypunct (pfacet, flags);
}

/***************************************************************************/


/* _RWSTD_SPECIALIZED_FUNCTION */ _RWSTD_EXPORT
const char*
__rw_get_punct(const __rw_facet *pfacet, int flags, char)
{
    typedef const char *CPChar;

    if (flags & __rw_mon)
        return CPChar (__rw_get_moneypunct (pfacet, flags & ~__rw_mon));

    return CPChar (__rw_get_numpunct (pfacet, flags));
}


#ifndef _RWSTD_NO_WCHAR_T

/* _RWSTD_SPECIALIZED_FUNCTION */ _RWSTD_EXPORT
const wchar_t*
__rw_get_punct(const __rw_facet *pfacet, int flags, wchar_t)
{
    flags |= __rw_wide;

    typedef const wchar_t *CPWChar;

    if (flags & __rw_mon)
        return CPWChar (__rw_get_moneypunct (pfacet, flags & ~__rw_mon));

    return CPWChar (__rw_get_numpunct (pfacet, flags));
}

#endif   // _RWSTD_NO_WCHAR_T


const char*
__rw_get_stdio_fmat (char buf [32], int type, unsigned fmtflags,
                     _RWSTD_STREAMSIZE prec)
{
    char *pbuf = buf;

    // format of the specifier:
    //     "%[+][#](.[0-9]*)(h|l|L|LL)(o|x|X|i|d|e|E|f|F|g|G|p)"

    *pbuf++ = '%';

    if (fmtflags & _RWSTD_IOS_SHOWPOS)
        *pbuf++ = '+';

    if (type & __rw_facet::_C_floating) {
        if (fmtflags & _RWSTD_IOS_SHOWPOINT)
            *pbuf++ = '#';

        // follows resolution of lwg issue 231
        if (0 <= prec) {

            // 7.19.6.1, p5 of C99 specifies that, when given using the
            // asterisk, negative precision is treated the same as if
            // it were omitted; treat negative precision the same here
            pbuf += sprintf (pbuf, ".%ld", long (prec));
        }
    }
    else if (fmtflags & _RWSTD_IOS_SHOWBASE)
        *pbuf++ = '#';

    enum {
        // for convenience
        Short   = __rw_facet::_C_ushort  & __rw_facet::_C_typemask,
        Long    = __rw_facet::_C_ulong   & __rw_facet::_C_typemask,
        LLong   = __rw_facet::_C_ullong  & __rw_facet::_C_typemask,
        Double  = __rw_facet::_C_double  & __rw_facet::_C_typemask,
        LDouble = __rw_facet::_C_ldouble & __rw_facet::_C_typemask
    };

    const int basetype = type & __rw_facet::_C_typemask;

    switch (basetype) {

    case Short: *pbuf++ = 'h'; break;
    case Long:
    case Double:
        *pbuf++ = 'l';
        break;

    case LDouble: *pbuf++ = 'L'; break;

    case LLong:

#ifdef _RWSTD_LLONG_PRINTF_PREFIX

        memcpy (pbuf, _RWSTD_LLONG_PRINTF_PREFIX,
                sizeof _RWSTD_LLONG_PRINTF_PREFIX - 1);
        pbuf += sizeof _RWSTD_LLONG_PRINTF_PREFIX - 1;

#else

        _RWSTD_ASSERT (!"long long printf prefix not configured");

#endif   // _RWSTD_LLONG_PRINTF_PREFIX

        break;
    }

    if (type & __rw_facet::_C_floating) {

        switch (fmtflags & _RWSTD_IOS_FLOATFIELD) {
        case _RWSTD_IOS_FIXED:
            *pbuf++ = 'f';
            break;

        case _RWSTD_IOS_SCIENTIFIC:
            *pbuf++ = fmtflags & _RWSTD_IOS_UPPERCASE ? 'E' : 'e';
            break;

        default:
            *pbuf++ = fmtflags & _RWSTD_IOS_UPPERCASE ? 'G' : 'g';
        }
    }
    else if (type == __rw_facet::_C_pvoid)
        *pbuf++ = 'p';
    else if (type == __rw_facet::_C_bool && fmtflags & _RWSTD_IOS_BOOLALPHA)
        --pbuf;   // empty format indicates bool
    else {
        switch (fmtflags & _RWSTD_IOS_BASEFIELD) {
        case _RWSTD_IOS_OCT: *pbuf++ = 'o'; break;
        case _RWSTD_IOS_DEC: *pbuf++ = 'd'; break;
        case _RWSTD_IOS_HEX:
            *pbuf++ = fmtflags & _RWSTD_IOS_UPPERCASE ? 'X' : 'x';
            break;
        default:
            *pbuf++ = 'i';
            
        }
    }

    *pbuf++ = '\0';

    _RWSTD_ASSERT (pbuf - buf < 32);

    return buf;
}


}   // namespace __rw
