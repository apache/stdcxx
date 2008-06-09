/***************************************************************************
 *
 * num_get.cpp
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

#include "setlocale.h"
#include "strtol.h"
#include "punct.h"      // for __rw_get_stdio_fmat

#include <ios>          // for ios_base, needed by <rw/_punct.h>
#include <loc/_num_get.h>

#include <errno.h>      // for ERANGE, errno
#include <float.h>      // for {DBL,FLT,LDBL}_{MIN,MAX}
#include <stdio.h>      // for printf()
#include <stdlib.h>     // for strtod()
#include <string.h>     // for memcpy()

#include <rw/_defs.h>     


// in case EINVAL is not #defined in errno.h
#ifdef EINVAL
#  define _RWSTD_EINVAL EINVAL
#else   // if !defined (EINVAL)
// actual value of EINVAL on both Linux and SunOS is 22
#  define _RWSTD_EINVAL 22
#endif   // EINVAL



#if defined (_RWSTD_NO_STRTOF) && !defined (_RWSTD_NO_STRTOF_IN_LIBC)

#  undef _RWSTD_NO_STRTOF

extern "C" float strtof (const char*, char**);

#endif   // NO_STRTOF && !NO_STRTOF_IN_LIBC

#if defined (_RWSTD_NO_STRTOLD) && !defined (_RWSTD_NO_STRTOLD_IN_LIBC)

#  undef _RWSTD_NO_STRTOLD

extern "C" long double strtold (const char*, char**);

#endif   // NO_STRTOLD && !NO_STRTOLD_IN_LIBC


#ifdef _RWSTD_MSVC
    // shut up useless MSVC warning: unary minus operator
    // applied to unsigned type, result still unsigned
#  pragma warning (disable: 4146)
#endif   // _RWSTD_MSVC


_RWSTD_NAMESPACE (__rw) { 


typedef unsigned char UChar;


#ifndef _RWSTD_NO_OBJECT_MANGLING

extern "C" {

#else   // if defined (_RWSTD_NO_OBJECT_MANGLING)

extern "C++" {

#endif   // _RWSTD_NO_OBJECT_MANGLING


extern const float       __rw_flt_infinity;
extern const double      __rw_dbl_infinity;

#ifndef _RWSTD_NO_LONG_DOUBLE

extern const long double __rw_ldbl_infinity;

#endif   // _RWSTD_NO_LONG_DOUBLE

}   // extern "C"/"C++"


// verifies that the `grps' array of size `ngrps' representing the
// lengths of individual groups of digits separated by thousands_sep
// in the representation of a number corresponds to the `grping' array
// of `ngrpings' elements
// returns a non-negative number of success, negative number whose
// absolute value indicates the bad group on failure

// Examples of valid groups:
//
//      sequence        grps  grping
//
//           123        "\3"  "\3\2"   3 <= 3,
//         1,234      "\1\3"  "\3\2"   1 <= 2 && 3 == 3
//      1,23,456    "\1\2\3"  "\3\2"   1 <= 2 && 2 == 2 && 3 == 3
//     12,34,567    "\2\2\3"  "\3\2"   2 <= 2 && 2 == 2 && 3 == 3
//   1,34,56,789  "\1\2\2\3"  "\3\2"   1 <= 2 && 2 == 2 && 2 == 2 && 3 == 3

// Examples of invalid groups:
//
//      sequence        grps  grping
//
//           123        "\3"  "\1\2"   !(3 <= 1)
//        1,2,34    "\1\1\2"  "\1\3"   1 <= 1 && 1 == 1 && !(2 == 3)
//      1,23,456    "\1\2\3"  "\1\2"   1 <= 1 && 2 == 2 && !(3 == 2)

_RWSTD_EXPORT int
__rw_check_grouping (const char *grps,   _RWSTD_SIZE_T ngrps,
                     const char *grping, _RWSTD_SIZE_T ngrpings)
{
    if (!ngrps || !ngrpings)
        return 0;

    _RWSTD_ASSERT (0 != grps);
    _RWSTD_ASSERT (0 != grping);

    // current grouping number
    _RWSTD_SIZE_T grpn = ngrpings;

    // find the last non-zero grouping if it exists (see below)
    while (grpn-- != ngrpings && !grping [grpn]);

    // the index of the last meaningful grouping (e.g., `grphi' of
    // "\3\0\0\0" is 2 since the redundant NULs can be safely ignored)
    const _RWSTD_SIZE_T grphi = (_RWSTD_SIZE_T)(-1) == grpn ? 0 : grpn;

    grpn = 0;

    typedef const UChar* PCUChar;

    // iterate over elements of `groups' starting with the least significant
    // one (at the greatest index), comparing each against the relevant
    // element of `groupings' (at the lowest index)
    for (PCUChar g = _RWSTD_REINTERPRET_CAST (PCUChar, grps + ngrps - 1),
                 e = _RWSTD_REINTERPRET_CAST (PCUChar, grping); ; --g) {

        typedef const void* PVoid;

        if (PVoid (g) == PVoid (grps)) {
            // the most significant group may be shorter (but not empty)
            if (*g > *e || !*g) {
                // subtracting from 0 to avoid warnings about
                // applying the - operator to an unsigned type
                grpn = 0U - (grpn + 1U);
            }

            break;
        }

        if (*g != *e) {
            // all but the most significant group must be exactly
            // as long as the last expected group length
            grpn = 0U - (grpn + 1U);
            break;
        }

        if (grpn++ < grphi)
            ++e;   // `*e' is the expected length of the group
    }

    // negative return value indicates an error
    return int (grpn);
}


// array of 1-based indices for each 8-bit character into an internal
// table of values of each roman digit, i.e., I, V, X, L, C, D, and M
// elements with value greater than 7 do not correspond to a roman digit
_RWSTD_EXPORT extern const UChar
__rw_roman_inxs[] = {
    //        0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
    //       NUL SOH STX ETX EOT ENQ ACK BEL BS  TAB LF  VT  FF  CR  SO  SI
    /*  0 */  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    //       DLE DC1 DC2 DC3 DC4 NAK SYN ETB CAN EM  SUB ESC FS  GS  RS  US
    /*  1 */  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    //       SPC  !   "   #   $   %   '   '   (   )   *   +   ,   -   .   /
    /*  2 */  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    //        0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
    /*  3 */  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    //        @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
    /*  4 */  9,  9,  9,  5,  6,  9,  9,  9,  9,  1,  9,  9,  4,  7,  9,  9,
    //        P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
    /*  5 */  9,  9,  9,  9,  9,  9,  2,  9,  3,  9,  9,  9,  9,  9,  9,  9,
    //        `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
    /*  6 */  9,  9,  9,  5,  6,  9,  9,  9,  9,  1,  9,  9,  4,  7,  9,  9,
    //        p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~  DEL
    /*  7 */  9,  9,  9,  9,  9,  9,  2,  9,  3,  9,  9,  9,  9,  9,  9,  9,
    // extended ascii below
    /*  8 */  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    /*  9 */  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    /*  a */  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    /*  b */  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    /*  c */  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    /*  d */  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    /*  e */  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
    /*  f */  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9
};


static const int __rw_roman_digits[] = {
    //  1  2   3   4    5    6     7   8   9
    -1, 1, 5, 10, 50, 100, 500, 1000, -1, -1
};


// parse a string ot roman digits, produce a non-zero numeric value
// on success, 0 otherwise, set `end' to point one past the last
// successfully interpreted character
static unsigned long __rw_get_roman (const char *str)
{
    _RWSTD_ASSERT (str && *str);

#define ROMAN_DIGIT(c) __rw_roman_digits [__rw_roman_inxs [UChar (c)]]

    unsigned long val = 0;

    for (; str [1]; ++str) {
        const int digit_1 = ROMAN_DIGIT (str [0]);
        const int digit_2 = ROMAN_DIGIT (str [1]);

        if (digit_1 < digit_2)
            val -= digit_1;
        else
            val += digit_1;
    }

    val += ROMAN_DIGIT (*str);

    return val;
}


// validates a floating point value and errno value
template <class FloatT>
inline FloatT
__rw_validate (FloatT val, FloatT fmin, FloatT finf, char sign, int &err)
{
    if (ERANGE == err) {

        if (val < -(fmin)) {
            // negative overflow: store -infinity and set failbit
            val = -finf;
            err = _RWSTD_IOS_FAILBIT;
        }
        else if (val > (fmin)) {
            // positive overflow: store +infinity and set failbit
            val = finf;
            err = _RWSTD_IOS_FAILBIT;
        }
        else {   // val >= fmin && val <= max
            // benign underflow: store +/-fmin, clear failbit
            val = '-' == sign ? -(fmin) : (fmin);
            err = 0;
        }
    }
    else
        err = 0;

    return val;
}


_RWSTD_EXPORT int
__rw_get_num (void *pval, const char *buf, int type, int flags,
              const char *groups, _RWSTD_SIZE_T ngroups,
              const char *grouping, _RWSTD_SIZE_T ngroupings)
{
    // 22.2.2.1.2, p3: Stage 1
    int err = 0;   // errno value, ERANGE, or 0

    _RWSTD_ASSERT (0 != buf);
    _RWSTD_ASSERT ('+' == *buf || '-' == *buf);

    if (!(type & __rw_facet::_C_floating)) {

        //////////////////////////////////////////////////////////////
        // integral parsing (including void*)

        int base = unsigned (flags) >> _RWSTD_IOS_BASEOFF;

        if (!base)
            base = 10;

        union {
#ifdef _RWSTD_LONG_LONG

            _RWSTD_LONG_LONG           ll;
            unsigned _RWSTD_LONG_LONG ull;

#endif   // _RWSTD_LONG_LONG

            long           l;
            unsigned long ul;
        } val;

        if (1 == base) {
            // treat numeric (i.e., non-Roman) input in base-1 as decimal
            const UChar digit = __rw_digit_map [UChar (buf [1])];

            if (digit < 10) {
                val.ul = _RW::__rw_strtoul (buf, &err, 10);
            }
            else {
                _RWSTD_ASSERT ('+' == *buf);

                val.ul = __rw_get_roman (buf + 1);
            }

            val.l = _RWSTD_STATIC_CAST (long, val.ul);
        }
        else if (__rw_facet::_C_ullong == type) {

#ifdef _RWSTD_LONG_LONG

            val.ull = _RW::__rw_strtoull (buf, &err, base);

#else   // if !defined (_RWSTD_LONG_LONG)

            _RWSTD_ASSERT (!"logic error: unsigned long long not configured");

#endif   // _RWSTD_LONG_LONG
        }
        else if (__rw_facet::_C_llong == type) {

#ifdef _RWSTD_LONG_LONG

            val.ll = _RW::__rw_strtoll (buf, &err, base);

#else   // if !defined (_RWSTD_LONG_LONG)

            _RWSTD_ASSERT (!"logic error: long long not configured");

#endif   // _RWSTD_LONG_LONG
        }
        else if (type & __rw_facet::_C_signed || __rw_facet::_C_bool == type) {
            val.l = _RW::__rw_strtol (buf, &err, base);
        }
        else if (__rw_facet::_C_pvoid == type) {

#if defined (_RWSTD_LONG_LONG) && _RWSTD_PTR_SIZE > _RWSTD_LONG_SIZE
            // assume pointers fit into long long
            val.ull = _RW::__rw_strtoull (buf, &err, base);
#else
            val.ul = _RW::__rw_strtoul (buf, &err, base);
#endif
        }
        else {
            val.ul = _RW::__rw_strtoul (buf, &err, base);
        }

        if (_RWSTD_EINVAL == err)
            return _RWSTD_IOS_FAILBIT;

        _RWSTD_ASSERT (0 != pval);

        switch (type) {
        case __rw_facet::_C_bool:
            if (0 == val.l || 1 == val.l) {
                *_RWSTD_STATIC_CAST (bool*, pval) = 1 == val.l;
            }
            else
                err = ERANGE;

            break;

        case __rw_facet::_C_ushort:
            // detect and handle unsigned overflow
            if (val.ul > _RWSTD_USHRT_MAX) {
                val.ul = _RWSTD_USHRT_MAX;
                err    = ERANGE;
            }
            else {
                if ('-' == *buf)
                    val.ul = _RWSTD_USHRT_MAX - val.ul + 1;
            }

            *_RWSTD_STATIC_CAST (unsigned short*, pval) =
                _RWSTD_STATIC_CAST (unsigned short, val.ul);
            break;
                
        case __rw_facet::_C_short:
            // detect and handle overflow
            if (val.l < _RWSTD_SHRT_MIN) {
                val.l = long (_RWSTD_SHRT_MIN);
                err   = ERANGE;
            }
            else if (val.l > _RWSTD_SHRT_MAX) {
                val.l = long (_RWSTD_SHRT_MAX);
                err   = ERANGE;
            }

            *_RWSTD_STATIC_CAST (short*, pval) =
                _RWSTD_STATIC_CAST (short, val.l);
            break;

        case __rw_facet::_C_uint:

            // detect and handle unsigned overflow

#if _RWSTD_UINT_MAX < _RWSTD_ULONG_MAX

            if (val.ul > _RWSTD_UINT_MAX)

#else   // if _RWSTD_UINT_MAX >= _RWSTD_ULONG_MAX

            if (val.ul == _RWSTD_ULONG_MAX && ERANGE == err)

#endif   // _RWSTD_UINT_MAX < _RWSTD_ULONG_MAX

            {
                val.ul = _RWSTD_UINT_MAX;
            }
            else {
                if ('-' == *buf)
                    val.ul = _RWSTD_UINT_MAX - val.ul + 1;
            }

            *_RWSTD_STATIC_CAST (unsigned*, pval) = unsigned (val.ul);
            break;

        case __rw_facet::_C_int:

            // detect and handle overflow

#if _RWSTD_UINT_MAX < _RWSTD_ULONG_MAX

            if (val.l < _RWSTD_INT_MIN) {
                val.l = long (_RWSTD_INT_MIN);
                err   = ERANGE;
            }
            else if (val.l > _RWSTD_INT_MAX) {
                val.l = long (_RWSTD_INT_MAX);
                err   = ERANGE;
            }

#else   // if _RWSTD_UINT_MAX >= _RWSTD_ULONG_MAX

            if (ERANGE == err) {
                if (_RWSTD_LONG_MIN == val.l)
                    val.l = long (_RWSTD_INT_MIN);
                else if (_RWSTD_LONG_MAX == val.l)
                    val.l = long (_RWSTD_INT_MAX);
            }

#endif   // _RWSTD_UINT_MAX < _RWSTD_ULONG_MAX

            *_RWSTD_STATIC_CAST (int*, pval) = int (val.l);
            break;

        case __rw_facet::_C_ulong:
            *_RWSTD_STATIC_CAST (unsigned long*, pval) = val.ul;
            break;
            
        case __rw_facet::_C_long:
            *_RWSTD_STATIC_CAST (long*, pval) = val.l;
            break;

#ifdef _RWSTD_LONG_LONG

        case __rw_facet::_C_ullong:
            *_RWSTD_STATIC_CAST (unsigned _RWSTD_LONG_LONG*, pval) = val.ull;
            break;

        case __rw_facet::_C_llong:
            *_RWSTD_STATIC_CAST (_RWSTD_LONG_LONG*, pval) = val.ll;
            break;

#endif   // _RWSTD_LONG_LONG

        case __rw_facet::_C_pvoid:
            *_RWSTD_STATIC_CAST (void**, pval) =
#if defined (_RWSTD_LONG_LONG) && _RWSTD_PTR_SIZE > _RWSTD_LONG_SIZE
                _RWSTD_REINTERPRET_CAST (void*, val.ull);
#else
                _RWSTD_REINTERPRET_CAST (void*, val.ul);
#endif

            // disable grouping
            grouping = "";
            break;

        }

        if (err)
            err = _RWSTD_IOS_FAILBIT;
    }
    else {
        //////////////////////////////////////////////////////////////
        // floating point parsing

        // fail if the sequence is empty
        if (!*buf)
            return _RWSTD_IOS_FAILBIT;

        char *end;

        // save errno value, reset only if ERANGE to distinguish strtol()
        // failure from success if errno already set on function entry
        const int errno_save = errno;

        if (ERANGE == errno_save)
            errno = 0;

        _RWSTD_ASSERT (0 != pval);

        switch (type) {
        case __rw_facet::_C_float: {

#ifndef _RWSTD_NO_STRTOF

            // assumes `buf' is formatted for the current locale
            // specifically, affects the value of decimal_point
            float f = strtof (_RWSTD_CONST_CAST (char*, buf), &end);

            err = errno;

            _RWSTD_ASSERT (0 != end);

            if ('.' == *end || end == buf && '.' == buf [1]) {
                // on failure caused by an unrecognized decimal point
                // set teporarily the global locale to "C" and reparse
                __rw_setlocale loc ("C", _RWSTD_LC_NUMERIC);

                f   = strtof (_RWSTD_CONST_CAST (char*, buf), &end);
                err = errno;
            }

            // restore errno if it was reset above
            if (ERANGE == errno_save)
                errno = ERANGE;

            if (*end)
                return _RWSTD_IOS_FAILBIT;

            *_RWSTD_STATIC_CAST (float*, pval) =
                __rw_validate (f, float (_RWSTD_FLT_MIN),
                               __rw_flt_infinity, *buf, err);

            break;

#else   // if defined (_RWSTD_NO_STRTOF)

            // assumes `buf' is formatted for the current locale
            // specifically, affects the value of decimal_point
            double d = strtod (buf, &end);
            err = errno;

            // restore errno if it was reset above
            if (ERANGE == errno_save)
                errno = ERANGE;

            _RWSTD_ASSERT (0 != end);

            if ('.' == *end || end == buf && '.' == buf [1]) {
                // on failure caused by an unrecognized decimal point
                // set teporarily the global locale to "C" and reparse
                __rw_setlocale loc ("C", _RWSTD_LC_NUMERIC);

                d = strtod (buf, &end);
            }

            if (*end)
                return _RWSTD_IOS_FAILBIT;

            // handle (benign) overflow and underflow

            if (d < -double (_RWSTD_FLT_MAX)) {
                // negative overflow (magnitude is too large to represent)
                // store negative infinity and set failbit
                d   = -__rw_flt_infinity;
                err = _RWSTD_IOS_FAILBIT;
            }
            else if (d > double (_RWSTD_FLT_MAX)) {
                // positive overflow (magnitude is too large to represent)
                // store positive infinity and set failbit
                d   = __rw_flt_infinity;
                err = _RWSTD_IOS_FAILBIT;
            }
            else if (d > 0.0 && d < double (_RWSTD_FLT_MIN)) {
                // positive underflow (magnitude is too small to represent)
                // store FLT_MIN and clear failbit
                d   = _RWSTD_FLT_MIN;
                err = 0;
            }
            else if (d < 0.0 && d > -double (_RWSTD_FLT_MIN)) {
                // negative underflow (magnitude is too small to represent)
                // store -FLT_MIN and clear failbit
                d   = -_RWSTD_FLT_MIN;
                err = 0;
            }
            else
                err = 0;

            *_RWSTD_STATIC_CAST (float*, pval) =
                _RWSTD_STATIC_CAST (float, d);
            break;

#endif   // _RWSTD_NO_STRTOF

        }

#ifdef _RWSTD_NO_STRTOLD
#  if DBL_DIG == LDBL_DIG && DBL_MAX_EXP == LDBL_MAX_EXP

     // strtold() is not defined and the size and domain of long double
     // is the same as that of double; use strtod() instead of sscanf()
     // for better precision
#    define LDBL_SAME_AS_DBL

        case __rw_facet::_C_ldouble:

#  endif   // DBL_DIG == LDBL_DIG && DBL_MAX_EXP == LDBL_MAX_EXP
#endif   // _RWSTD_NO_STRTOLD

        case __rw_facet::_C_double: {
            // assumes `buf' is formatted for the current locale
            // specifically, affects the value of decimal_point
            double d = strtod (buf, &end);
            err = errno;

            _RWSTD_ASSERT (0 != end);

            if ('.' == *end || end == buf && '.' == buf [1]) {
                // on failure caused by an unrecognized decimal point
                // set teporarily the global locale to "C" and reparse
                __rw_setlocale loc ("C", _RWSTD_LC_NUMERIC);

                d   = strtod (buf, &end);
                err = errno;
            }

            // restore errno if it was reset above
            if (ERANGE == errno_save)
                errno = errno_save;

            if (*end)
                return _RWSTD_IOS_FAILBIT;

            *_RWSTD_STATIC_CAST (double*, pval) =
               __rw_validate (d, double (_RWSTD_DBL_MIN),
                              __rw_dbl_infinity, *buf, err);
            break;
        }


#ifndef LDBL_SAME_AS_DBL

        case __rw_facet::_C_ldouble: {

            typedef long double LDbl;

#  ifndef _RWSTD_NO_LONG_DOUBLE

            LDbl ld;

#    ifndef _RWSTD_NO_STRTOLD

            // assumes `buf' is formatted for the current locale
            // specifically, affects the value of decimal_point
#      if !defined (_RWSTD_OS_HP_UX) || !defined (_LONG_DOUBLE)

            // not HP-UX or HP-UX with gcc
            ld = strtold (buf, &end);

#      else   // UP-UX with _LONG_DOUBLE

            // HP-UX strtold() returns struct long_double
            // the macro _LONG_DOUBLE is #defined when the struct is defined
            // note that gcc's replacement <stdlib.h> may actually define the
            // function with the correct signature

            union {
                LDbl        ld;
                long_double words;
            } u;

            u.words = strtold (buf, &end);
            ld      = u.ld;

#        ifndef __ia64

            // inefficiently working around HP-UX/PA-RISC strtold("0")
            // bug #615: strtold("0", &end) fails to set the end pointer
            if (   end == buf
                && 0 == errno
                && (   '0' == *buf || '.' == *buf
                    || /* '+' or '-' */ 43 == __rw_digit_map [UChar (*buf)])) {

                char *tmpend;
                if (   0.0 == strtod (buf, &tmpend)
                    && '\0' == *tmpend && 0 == errno)
                    end = tmpend;
            }

#        endif   // __ia64
#      endif   // HP-UX, _LONG_DOUBLE

            err = errno;

            _RWSTD_ASSERT (0 != end);

            if ('.' == *end || end == buf && '.' == buf [1]) {
                // on failure caused by an unrecognized decimal point
                // set teporarily the global locale to "C" and reparse
                __rw_setlocale loc ("C", _RWSTD_LC_NUMERIC);

#      if !defined (_RWSTD_OS_HP_UX) || !defined (_LONG_DOUBLE)
                ld = strtold (buf, &end);
#      else   // HP-UX with _LONG_DOUBLE #defined
                u.words = strtold (buf, &end);
                ld      = u.ld;
#      endif   // HP-UX, _LONG_DOUBLE

                err = errno;
            }

            // restore errno if it was reset above
            if (ERANGE == errno_save)
                errno = errno_save;

            if (*end)
                return _RWSTD_IOS_FAILBIT;

#    else   // if defined (!_RWSTD_NO_STRTOLD)

            flags &=
            ~(_RWSTD_IOS_SHOWBASE | _RWSTD_IOS_SHOWPOS | _RWSTD_IOS_SHOWPOINT);

            char fmatbuf [32];

            const char* const fmt =
                __rw_get_stdio_fmat (fmatbuf, type, flags, -1 /* ignore */);

            // assumes `buf' is formatted for the current locale
            // specifically, affects the value of decimal_point
            const int n = sscanf (buf, fmt, pval);

            // restore errno if it was reset above
            if (ERANGE == errno_save)
                errno = errno_save;

            if (1 != n)
                return _RWSTD_IOS_FAILBIT;

            ld = *_RWSTD_STATIC_CAST (LDbl*, pval);

#    endif   // _RWSTD_NO_STRTOLD

            *_RWSTD_STATIC_CAST (LDbl*, pval) =
                __rw_validate (ld, LDbl (_RWSTD_LDBL_MIN),
                               __rw_ldbl_infinity, *buf, err);

#  endif   // _RWSTD_LONG_DOUBLE

            break;
        }

#endif   //  LDBL_SAME_AS_DBL
        }
    }

    _RWSTD_ASSERT (grouping);

    // 22.2.2.1.2, p12: check digit grouping, if any
    // note that grouping is optional and parsing fails due to bad format
    // only if the positions thousands_sep's do not match those specified
    // by grouping; in such cases, the value is still stored
    if (   *grouping && ngroups > 1
        && 0 > __rw_check_grouping (groups, ngroups, grouping, ngroupings)
        || !*grouping && ngroups > 1)
        err |= _RWSTD_IOS_FAILBIT;

    return err;
}


}   // namespace __rw
