/***************************************************************************
 *
 * num_put.cpp
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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

#include <limits>     // for numeric_limits

#include <stdio.h>    // for snprintf()
#include <string.h>   // for memmove(), memset()

#include <loc/_num_put.h>

#include "fpclass.h"  // for __rw_isfinite(), ...
#include "strtol.h"   // for __rw_digit_map
#include "punct.h"    // for __rw_get_stdio_fmat


#ifndef _RWSTD_NO_SNPRINTF
   // cast away constness of the format argument to prevent errors
   // on platforms such as HP-UX 11.00 that incorrectly declare it
   // without the const qualifier
#  define _SNPRINTF(buf, size, fmt, arg)   \
     snprintf (buf, size, _RWSTD_CONST_CAST (char*, fmt), arg)
#else   // _RWSTD_NO_SNPRINTF
#  ifndef _RWSTD_NO_SNPRINTF_IN_LIBC

#    undef snprintf
#    define snprintf                         _RWSTD_LIBC_SYM (snprintf)
#    define _SNPRINTF(buf, size, fmt, arg)   snprintf (buf, size, fmt, arg)

extern "C" {

_RWSTD_DLLIMPORT int
snprintf (char*, _RWSTD_SIZE_T, const char*, ...) _LIBC_THROWS ();

}   // extern "C"

#  else   // if defined (_RWSTD_NO_SNPRINTF_IN_LIBC)
#    define _SNPRINTF(buf, ignore, fmt, arg) sprintf (buf, fmt, arg)
#  endif   // _RWSTD_NO_SNPRINTF_IN_LIBC
#endif   // _RWSTD_NO_SNPRINTF


_RWSTD_NAMESPACE (__rw) { 

static const char __rw_digits[] =
    "0123456789abcdefghijklmnopqrstuvwxyz"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";


static int
__rw_fmat_infinite (char *buf, size_t bufsize, double val, unsigned flags)
{
    _RWSTD_ASSERT (!__rw_isfinite (val));
    _RWSTD_ASSERT (5 <= bufsize);

    _RWSTD_UNUSED (bufsize); 

    char* end = buf;
    const bool cap = !!(flags & _RWSTD_IOS_UPPERCASE);

    if (__rw_isinf (val)) {
        if (__rw_signbit (val)) {
            *end++ = '-';
        }
        else if (flags & _RWSTD_IOS_SHOWPOS) {
            *end++ = '+';
        }

        const char str [] = "iInNfF";
        *end++ = str [cap + 0];
        *end++ = str [cap + 2];
        *end++ = str [cap + 4];
    }
    else {
        _RWSTD_ASSERT (__rw_isnan (val));

        if (__rw_signbit (val)) {
            *end++ = '-';
        }
        else if (flags & _RWSTD_IOS_SHOWPOS) {
            *end++ = '+';
        }

        const char str [] = "nNaAqQsS";

        if (__rw_isqnan (val))
            *end++ = str [cap + 4];
        else if (__rw_issnan (val))
            *end++ = str [cap + 6];

        *end++ = str [cap + 0];
        *end++ = str [cap + 2];
        *end++ = str [cap + 0];
    }

    return int (end - buf);
}


#ifdef _RWSTD_LONG_LONG


typedef _RWSTD_LONG_LONG          _LLong;
typedef unsigned _RWSTD_LONG_LONG _ULLong;


static _RWSTD_SIZE_T
__rw_btoa (char *buf, _ULLong i, unsigned base)
{
    _RWSTD_ASSERT (base && base <= 36);

    const _RWSTD_SIZE_T dig = _STD::numeric_limits<_ULLong>::digits + 0U;

    char*             end   = buf + dig;
    const char* const begin = end;

    _ULLong tmp;
    do {
        tmp = i / base;

        // PA RISC has no integer divide, optimize the modulo operator
        // this is also faster than computing the modulo on i86, and
        // faster than calling ldiv()
        *--end = __rw_digits [i - tmp * base];
    } while ((i = tmp));

    const _RWSTD_SIZE_T len = begin - end;

   memmove (buf, end, len);

    return len;
}


static _RWSTD_SIZE_T
__rw_dtoa (char *buf, _ULLong i, unsigned flags)
{
    // get the maximum number of decimal digits for an unsigned long
    // with the largest magnitude, plus 1 for the (optional) sign
    const _RWSTD_SIZE_T dig10 = _STD::numeric_limits<_ULLong>::digits10 + 2U;

    char*             end   = buf + dig10;
    const char* const begin = end;

    _ULLong tmp;
    do {
        tmp = i / 10;

        // PA RISC has no integer divide, optimize the modulo operator
        // this is also faster than computing the modulo on i86, and
        // faster than calling ldiv()
        *--end = __rw_digits [i - tmp * 10];
    } while ((i = tmp));


    if (flags & _RWSTD_IOS_SHOWPOS)
        *--end = '+';

    const _RWSTD_SIZE_T len = begin - end;

   memmove (buf, end, len);

    return len;
}


static inline _RWSTD_SIZE_T
__rw_dtoa (char *buf, _LLong i, unsigned flags)
{
    if (i < 0) {
        flags  &= ~_RWSTD_IOS_SHOWPOS;
        *buf++  = '-';
        return 1 + __rw_dtoa (buf, _RWSTD_STATIC_CAST (_ULLong, -i), flags);
    }
        
    return __rw_dtoa (buf, _RWSTD_STATIC_CAST (_ULLong, i), flags);
}


static _RWSTD_SIZE_T
__rw_itoa (char *buf, _ULLong i, unsigned flags)
{
    const char* const pdigs = flags & _RWSTD_IOS_UPPERCASE ?
        __rw_digits + 36 : __rw_digits;

    const _LLong base = (flags >> _RWSTD_IOS_BASEOFF) - _LLong (1);

    int bits;

    char *end = buf;

    switch (base) {

    case  1: bits = 1; break;
    case  3: bits = 2; break;
    case  7:
        bits = 3;
        if (i && flags & _RWSTD_IOS_SHOWBASE)
            *end++ = '0';
        break;

    case 9:
        return __rw_dtoa (end, i, flags);

    case 15:
        bits = 4;
        if (i && flags & _RWSTD_IOS_SHOWBASE) {
            *end++ = '0';
            *end++ = pdigs [33];   // 'X' or 'x'
        }
        break;

    case 31: bits = 5; break;

    default:
        _RWSTD_ASSERT (base >= 0 && base <= _RWSTD_UINT_MAX);

        return __rw_btoa (buf, i, unsigned (base));
    }

    // maximum number of base-digits
    const int digits =
        (_STD::numeric_limits<_ULLong>::digits + bits - 1) / bits;

    int j;

    // skip leading zeros
    if (i) {
        for (j = digits - 1; j && !(i & (base << j * bits)); --j);
    }
    else
        j = 0;

    // format the remaining digits
    do {
        const int dig = int ((i & (base << j * bits)) >> j * bits);

        _RWSTD_ASSERT (dig >= 0 && dig < 36);

        *end++ = pdigs [dig];
    } while (j--);

    return end - buf;
}


static inline _RWSTD_SIZE_T
__rw_itoa (char *buf, _LLong i, unsigned flags)
{
    if (10 == flags >> _RWSTD_IOS_BASEOFF)
        return __rw_dtoa (buf, i, flags);

    return __rw_itoa (buf, _RWSTD_STATIC_CAST (_ULLong, i), flags);
}


#endif   // _RWSTD_LONG_LONG


static _RWSTD_SIZE_T
__rw_btoa (char *buf, unsigned long i, unsigned base)
{
    _RWSTD_ASSERT (base && base <= 36);

    const _RWSTD_SIZE_T dig = _STD::numeric_limits<unsigned long>::digits + 0U;

    char*             end   = buf + dig;
    const char* const begin = end;

    unsigned long tmp;
    do {
        tmp = i / base;

        // PA RISC has no integer divide, optimize the modulo operator
        // this is also faster than computing the modulo on i86, and
        // faster than calling ldiv()
        *--end = __rw_digits [i - tmp * base];
    } while ((i = tmp));

    const _RWSTD_SIZE_T len = begin - end;

   memmove (buf, end, len);

    return len;
}


static  inline _RWSTD_SIZE_T
__rw_dtoa (char *buf, unsigned long i, unsigned flags)
{
    // get the maximum number of decimal digits for an unsigned long
    // with the largest magnitude, plus 1 for the (optional) sign
    const _RWSTD_SIZE_T dig10 =
        _STD::numeric_limits<unsigned long>::digits10 + 2U;

    char*             end   = buf + dig10;
    const char* const begin = end;

    unsigned long tmp;
    do {
        tmp = i / 10;

        // PA RISC has no integer divide, optimize the modulo operator
        // this is also faster than computing the modulo on i86, and
        // faster than calling ldiv()
        *--end = __rw_digits [i - tmp * 10];
    } while ((i = tmp));


    // prepend a sign if necessary
    if (flags & _RWSTD_IOS_SHOWPOS)
        *--end = '+';

    // move the contents of the buffer to the beginning
    const _RWSTD_SIZE_T len = begin - end;

   memmove (buf, end, len);

    return len;
}


static inline _RWSTD_SIZE_T
__rw_dtoa (char *buf, long i, unsigned flags)
{
    if (i < 0) {
        flags  &= ~_RWSTD_IOS_SHOWPOS;
        *buf++  = '-';
        return 1 + __rw_dtoa (buf, _RWSTD_STATIC_CAST (unsigned long, -i),
                              flags);
    }
        
    return __rw_dtoa (buf, _RWSTD_STATIC_CAST (unsigned long, i), flags);
}


// convert unsigned long to a roman number
static _RWSTD_SIZE_T
__rw_utor (char *buf, unsigned long i, unsigned flags)
{
    //                          01234560123456
    static const char lit [] = "ivxlcdmIVXLCDM";

    if (0 == i || i > 4999)
        return __rw_dtoa (buf, i, flags);

    const char* const pdigs = flags & _RWSTD_IOS_UPPERCASE ? lit + 7 : lit;

    const char *begin = buf;

    for (; i >= 1000; i -= 1000)
        *buf++ = pdigs [6];   // 'M'

    for (unsigned long j = 0, ord = 100; j != 6; j += 2, ord /= 10) {

        unsigned long fact;

        if (i >= (fact = 9 * ord)) {
            *buf++ = pdigs [4 - j];   // {C,X,I}
            *buf++ = pdigs [6 - j];   // {M,C,X}
            i -= fact;
        }
        else if (i >= (fact = 5 * ord)) {
            *buf++ = pdigs [5 - j];   // {D,L,V}
            for (i -= fact; i >= ord; i -= ord)
                *buf++ = pdigs [4 - j];   // {C,X,I}
        }
        else if (i >= (fact = 4 * ord)) {
            *buf++ = pdigs [4 - j];   // {C,X,I}
            *buf++ = pdigs [5 - j];   // {D,L,V}
            i -= fact;
        }
        else {
            for (; i >= ord; i -= ord)
                *buf++ = pdigs [4 - j];   // {C,X,I}
        }
    }

    return buf - begin;
}


static _RWSTD_SIZE_T
__rw_itoa (char *buf, unsigned long i, unsigned flags)
{
    const char* const pdigs = flags & _RWSTD_IOS_UPPERCASE ?
        __rw_digits + 36 : __rw_digits;

    const unsigned basemask = (flags >> _RWSTD_IOS_BASEOFF) - 1;

    int bits;

    char *end = buf;

    switch (basemask) {
    case unsigned (-1): case 9:
        return __rw_dtoa (end, i, flags);

    case 0:
        return __rw_utor (end, i, flags);

    case  1: bits = 1; break;
    case  3: bits = 2; break;
    case  7:
        bits = 3;
        if (i && flags & _RWSTD_IOS_SHOWBASE)
            *end++ = '0';
        break;

    case 15:
        bits = 4;
        if (i && flags & _RWSTD_IOS_SHOWBASE) {
            *end++ = '0';
            *end++ = pdigs [33];   // 'X' or 'x'
        }
        break;

    case 31:
        bits = 5; break;

    default:
        return __rw_btoa (buf, i, basemask + 1);
    }

    int j;

    // skip leading zeros
    if (i) {
        // maximum number of digits in the given base (basemask + 1)
        const int digits =
            (_STD::numeric_limits<unsigned long>::digits + bits - 1) / bits;

        for (j = digits - 1; !(i & (long (basemask) << j * bits)); --j);
    }
    else
        j = 0;

    do {
        const unsigned dig = unsigned ((i >> (j * bits)) & basemask);

        _RWSTD_ASSERT (dig <= basemask);

        *end++ = pdigs [dig];
    } while (j--);

    return end - buf;
}


static inline _RWSTD_SIZE_T
__rw_itoa (char *buf, long i, unsigned flags)
{
    if (10 == flags >> _RWSTD_IOS_BASEOFF)
        return __rw_dtoa (buf, i, flags);

    return __rw_itoa (buf, _RWSTD_STATIC_CAST (unsigned long, i), flags);
}


_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_put_groups (char **pbuf, _RWSTD_SIZE_T len, _RWSTD_SIZE_T bufsize,
                 const char *end, const char *grouping)
{
    _RWSTD_ASSERT (0 != *pbuf);
    _RWSTD_ASSERT (0 != grouping);

    _RWSTD_UNUSED (bufsize);

    if (end) {
        // for floats, find the decimal point or one of { 'e', 'E' }
        end = strchr (*pbuf, '.');
        if (!end) {
            // try the comma in case sprintf() was used after
            // a call setlocale() that changed '.' to ','
            end = strchr (*pbuf, ',');
            if (!end)
                end = strpbrk (*pbuf, "Ee");
        }
    }

    if (!end) {
        // no decimal point for all other types
        end = *pbuf + len;
    }

    typedef unsigned char UChar;

    char *pd = *pbuf;   // pointer to the integral part (past any prefix)

    if (43 /* '-' or '+' */ == __rw_digit_map [UChar (*pd)])
        ++pd;
    else if ('0' == *pd) {
        ++pd;

        if (33 /* 'X' or 'x' */ == __rw_digit_map [UChar (*pd)])
            ++pd;
    }

    _RWSTD_SIZE_T ngrps  = 0;   // number of complete groups
    _RWSTD_SIZE_T grplen = 0;   // length of complete groups

    // compute the amount of buffer space needed for group separators
    const char *pg = grouping;
    for ( ; ; ++ngrps) {

        UChar grp = UChar (*pg);

        if (0 == grp) {
            // '\0' causes the last grouping to be repeated
            grp = UChar (pg [-1]);
        }
        else if (_RWSTD_CHAR_MAX == grp) {
            // _RWSTD_CHAR_MAX terminates the grouping algorithm
            break;
        }
        else
            ++pg;

        if (grplen + grp >= _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, end - pd))
            break;

        grplen += grp;
    }

    pg   = grouping;   // reset to point to the first grouping
    end += ngrps;      // adjust the new position of the decimal point
    len += ngrps;      // adjust the length of buffer

    grplen = *pg++;    // length of the first grouping

    const bool no_groups = '\0' == **pbuf;

    // copy buffer onto itself starting with the rightmost digit,
    // inserting thousands_sep placeholders (';') according to
    // `grouping' in the integral portion of the number
    // using ';' rather than ',' to prevent potential collisions
    // with a comma inserted by sprintf() in some locales
    for (pd = *pbuf + (len - 1); pd != *pbuf && ngrps; --pd) {
        if (pd >= end) {
            // copy the fractional part (if any) of the number
            *pd = pd [-_RWSTD_STATIC_CAST (_RWSTD_PTRDIFF_T, ngrps)];
        }
        else {
            if ((_RWSTD_SIZE_T)(end - pd) == grplen + 1) {
                // insert throusands separator and compute
                // the offset of the next separator
                *pd = ';';
                --ngrps;

                if (_RWSTD_CHAR_MAX == *pg)
                    break;

                // '\0' forces the repetition of the last grouping
                grplen += (*pg ? *pg++ : pg [-1]) + 1;
            }
            else {
                // copy the integral part
                *pd = pd [-_RWSTD_STATIC_CAST (_RWSTD_PTRDIFF_T, ngrps)];
            }
        }
    }

    if (no_groups) {

        pd = *pbuf;

        // replace each group including the thousands_sep placeholder
        // in the buffer with its size
        for (char *ps = *pbuf, *last = *pbuf; ; ++pd) {

            if (pd == *pbuf + len) {
                // append the size of the last group, set `len', and break
                *ps++ = char (pd - last);
                len   = ps - *pbuf;
                break;
            }

            if (';' == *pd) {
                // replace group with it size
                *ps++ = char (pd - last);
                last  = pd + 1;
            }
        }
    }

    return len;
}


static inline void
__rw_fix_flt (char *&end, _RWSTD_SIZE_T &len,
              unsigned flags, _RWSTD_STREAMSIZE prec)
{
#ifdef _WIN32

    char* beg = end - len;

    // workaround for the STDCXX-2 issue
    if (   _RWSTD_IOS_FIXED != (flags & _RWSTD_IOS_FLOATFIELD)
        && _RWSTD_IOS_SCIENTIFIC != (flags & _RWSTD_IOS_FLOATFIELD)
        && 2 < len
        && '0' == beg [0]
        && ('.' == beg [1] || ',' == beg [1])) {

        // for the 0.0 value the MSVC libc inserts redundant '0' character
        // when %g format is used in sprintf()
        const char* ptr;
        for (ptr = beg + 2; ptr != end && '0' == *ptr; ++ptr) ;

        if (ptr == end) {
            const _RWSTD_SIZE_T exp_len =
                0 > prec ? 7 : (1 < prec ? prec + 1 : 2);
            if (exp_len < len) {
                end = beg + exp_len;
                len = exp_len;
            }
        }
    }

    if (len > 5) {
        // make Win32 output conform to C99 printf() requirements
        // on the exponent: The exponent always contains at least
        // two digits, and only as many more digits as necessary
        // to represent the exponent.
        if (('e' == end [-5] || 'E' == end [-5]) && '0' == end [-3]) {
            end [-3] = end [-2];
            end [-2] = end [-1];
            --len;
            --end;
        }
        else if ('#' == end [-4]) {
            // may be #INF or #IND
            const bool cap = !!(flags & _RWSTD_IOS_UPPERCASE);

            if ('F' == end [-1]) {
                // assuming #INF
                // normalize the format of infinity to conform to C99

                const char str[] = "iInNfF";

                end [-6] = str [cap + 0];
                end [-5] = str [cap + 2];
                end [-4] = str [cap + 4];
            }
            else {
                // assuming #IND
                // normalize the format of NaN to conform to C99

                const char str[] = "nNaA";

                end [-6] = str [cap + 0];
                end [-5] = str [cap + 2];
                end [-4] = str [cap + 0];
            }

            end -= 3;
            len -= 3;
        }
        else if ('#' == end [-5]) {
            // normalize the format of NaN to conform to C99

            const char str[] = "nNaA";

            const bool cap = !!(flags & _RWSTD_IOS_UPPERCASE);

            end [-7] = str [cap + 0];
            end [-6] = str [cap + 2];
            end [-5] = str [cap + 0];
            end -= 4;
            len -= 4;
        }
    }

#else

    _RWSTD_UNUSED (prec);

    // normalize the format of infinity and NaN to one of
    // { INF, inf, NAN, nan, NANQ, nanq, NANS, nans }
    // for portability
    char* beg = end - len;

    typedef unsigned char UChar;

    // is there a sign?
    bool sgn = false;

    if (43 /* '+' or '-' */ == __rw_digit_map [UChar (*beg)]) {
        sgn = true;
        ++beg;
    }

    const char* pstr;

    switch (__rw_digit_map [UChar (*beg)]) {

    case 18 /* 'I' or 'i' */ :
        pstr = "iInNfF\0\0";
        len  = 3;
        break;

    case 23 /* 'N' or 'n' */ : {

        // distinguish between quiet and signaling NaN

        // FIXME: do this portably, regardless of printf output
        // (some libc, such as Solaris, output the same string
        // for both quiet and signaling NaN; others, such as
        // AIX, output NaNQ and NaNS, respectively)

        const UChar last = __rw_digit_map [UChar (*(end - 1))];
        if (26 /* 'Q' or 'q' */ == last) {
            pstr = "nNaAnNqQ";
            len  = 4;
        }
        else if (28 /* 'S' or 's' */ == last) {
            pstr = "nNaAnNsS";
            len  = 4;
        }
        else {
            pstr = "nNaAnN\0\0";
            len  = 3;
        }
        break;
    }

    default:
        return;
    }

    const bool cap = !!(flags & _RWSTD_IOS_UPPERCASE);

    beg [0] = pstr [cap + 0];
    beg [1] = pstr [cap + 2];
    beg [2] = pstr [cap + 4];
    beg [3] = pstr [cap + 6];

    end = beg + 3 + ('\0' != beg [3]);

    // increase the length by one for the sign
    if (sgn)
        ++len;

#endif   // _WIN32

}



_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_put_num (char **pbuf, _RWSTD_SIZE_T bufsize,
              unsigned flags, int type, _RWSTD_STREAMSIZE prec,
              const void *pval, const char *grouping)
{
    char              fmtbuf [32];   // buffer for stdio format specifier
    const char       *fmt = 0;       // stdio format specifier
    char* const       buf = *pbuf;   // saved value of *pbuf
    char             *end = 0;       // decimal point
    _RWSTD_SIZE_T     len;           // length of formatted number
    _RWSTD_STREAMSIZE fpr;           // special case floating point precision

    typedef _RWSTD_SIZE_T SizeT;

    switch (type) {

    case __rw_facet::_C_bool:
        if (flags & _RWSTD_IOS_BOOLALPHA) {
            // no output
            len  = 0;
            *buf = '\0';
            break;
        }
        // else fall through...

    case __rw_facet::_C_short:
    case __rw_facet::_C_int:
    case __rw_facet::_C_long:
#if defined (__INTEL_COMPILER) && defined (_WIN64)
#  pragma warning (disable: 810)
#endif
        len = __rw_itoa (buf, _RWSTD_REINTERPRET_CAST (long, pval), flags);
#if defined (__INTEL_COMPILER) && defined (_WIN64)
#  pragma warning (default: 810)
#endif
        break;

    case __rw_facet::_C_ushort:
    case __rw_facet::_C_uint:
    case __rw_facet::_C_ulong:
        // unsigned output behaves analogously printf() with %u
        // note that the plus in "%+u" is meaningless (i.e., the
        // sign is only used in signed conversions; 7.19 6.1, p6
        // of C99: The result of a signed conversion always begins
        // with a plus or minus sign.)
#if defined (__INTEL_COMPILER) && defined (_WIN64)
#  pragma warning (disable: 810)
#endif
        len = __rw_itoa (buf, _RWSTD_REINTERPRET_CAST (unsigned long, pval),
                         flags & ~_RWSTD_IOS_SHOWPOS);
#if defined (__INTEL_COMPILER) && defined (_WIN64)
#  pragma warning (default: 810)
#endif
        break;

#ifdef _RWSTD_LONG_LONG

    case __rw_facet::_C_llong:
        len = __rw_itoa (buf, *(const _LLong*)pval, flags);
        break;

    case __rw_facet::_C_ullong:
        len = __rw_itoa (buf, *(const _ULLong*)pval,
                         flags & ~_RWSTD_IOS_SHOWPOS);
        break;

#endif   // _RWSTD_LONG_LONG

    case __rw_facet::_C_float | __rw_facet::_C_ptr: {
            const float fval = *(const float*)pval;
            if (!__rw_isfinite (fval)) {
                len = __rw_fmat_infinite (*pbuf, bufsize, fval, flags);
                end = *pbuf + len;
            }
            else {
                fpr = prec < 0 && flags & _RWSTD_IOS_FIXED ? 0 : prec;
                fmt = __rw_get_stdio_fmat (fmtbuf, type & ~__rw_facet::_C_ptr,
                                           flags, fpr);
                for (; ;) {
                    len = SizeT (_SNPRINTF (*pbuf, bufsize, fmt, fval));

                    if (len >= bufsize) {
                        if (*pbuf != buf)
                            delete[] *pbuf;

                        bufsize = len + 1 ? len + 1 : bufsize * 2;
                        *pbuf = new char [bufsize];
                    }
                    else {
                        _RWSTD_ASSERT (len > 0);

                        break;
                    }
                }

                end = *pbuf + len;

                // fix up output to conform to C99
                __rw_fix_flt (end, len, flags, fpr);
            }
        }
        break;

    case __rw_facet::_C_double | __rw_facet::_C_ptr: {
            const double dval = *(const double*)pval;
            if (!__rw_isfinite (dval)) {
                len = __rw_fmat_infinite (*pbuf, bufsize, dval, flags);
                end = *pbuf + len;
            }
            else {
                fpr = prec < 0 && flags & _RWSTD_IOS_FIXED ? 0 : prec;
                fmt = __rw_get_stdio_fmat (fmtbuf, type & ~__rw_facet::_C_ptr,
                                           flags, fpr);

                for ( ; ; ) {
                    len = SizeT (_SNPRINTF (*pbuf, bufsize, fmt, dval));

                    if (len >= bufsize) {
                        if (*pbuf != buf)
                            delete[] *pbuf;

                        bufsize = len + 1 ? len + 1 : bufsize * 2;
                        *pbuf = new char [bufsize];
                    }
                    else {
                        _RWSTD_ASSERT (len > 0);

                        break;
                    }
                }

                end = *pbuf + len;

                // fix up output to conform to C99
                __rw_fix_flt (end, len, flags, fpr);
            }
        }
        break;

#ifndef _RWSTD_NO_LONG_DOUBLE

    case __rw_facet::_C_ldouble | __rw_facet::_C_ptr: {
            const long double ldval = *(const long double*)pval;
            if (!__rw_isfinite (ldval)) {
                len = __rw_fmat_infinite (*pbuf, bufsize, ldval, flags);
                end = *pbuf + len;
            }
            else {
                fpr = prec < 0 && flags & _RWSTD_IOS_FIXED ? 0 : prec;
                fmt = __rw_get_stdio_fmat (fmtbuf, type & ~__rw_facet::_C_ptr,
                                           flags, fpr);

                for ( ; ; ) {
                    len = SizeT (_SNPRINTF (*pbuf, bufsize, fmt, ldval));
                    if (len >= bufsize) {
                        if (*pbuf != buf)
                            delete[] *pbuf;

                        bufsize = len + 1 ? len + 1 : bufsize * 2;
                        *pbuf = new char [bufsize];
                    }
                    else {
                        _RWSTD_ASSERT (len > 0);

                        break;
                    }
                }

                end = *pbuf + len;

                // fix up output to conform to C99
                __rw_fix_flt (end, len, flags, fpr);
            }
        }
        break;

#endif   // _RWSTD_NO_LONG_DOUBLE

    case __rw_facet::_C_pvoid:
        if (pval) {
            fmt = __rw_get_stdio_fmat (fmtbuf, type, flags, prec);
            len = SizeT (_SNPRINTF (buf, bufsize, fmt, pval));
        }
        else {
            // prevent GNU libc from formatting null pointers as "(nil)"
            *buf = '0';
            len  = 1;
        }

        // prevent grouping
        grouping = "";
        break;

    default:
        _RWSTD_ASSERT (!"logic error: bad type");
        len = 0;   // silence used before initialized warnings
    }

    if (prec < 0 && flags & _RWSTD_IOS_FIXED) {

        // extension: when ios::fixed is set, negative precision inserts
        // a decimal_point placeholder `prec' positions before the least
        // significant digit

        const bool sign = '-' == **pbuf || '+' == **pbuf;

        char* const beg = *pbuf + sign;

        char *src;   // first of the digits to move
        char *dst;   // where to move the first digit
        char *dp;    // where to insert the decimal point

        const _RWSTD_PTRDIFF_T ndigs = len - sign;

        _RWSTD_SIZE_T nfill;   // the number of 0's to insert
        _RWSTD_SIZE_T nmove;   // the number of digits to move

        if (-prec >= ndigs) {

            // insert 0's in addition to the decimal point
            // e.g., "1234" with prec == -6 yields "0.01234"

            nmove = ndigs;
            nfill = -prec - ndigs + 2;

            dst = beg + nfill;
            src = beg;
            dp  = beg + 1;

            // adjust the length of the block
            len += nfill;
        }
        else {

            // insert decimal point into the string of digits
            // e.g., "1234" with prec == -3 yields "1.234"

            nfill = 0;
            nmove = SizeT (-prec);

            src = beg + (ndigs + prec);
            dst = src + 1;
            dp  = src;

            ++len;   // grow the length
        }

        // move a block of digits to make room for 0's, if any,
        // and the decimal point placeholder
       memmove (dst, src, nmove);
       memset (beg, '0', nfill);

        *dp = '.';
        end = *pbuf + len;
    }

    // insert commas as thousands separators if grouping is required
    // integral and floating point types only (i.e., not for pointers)
    if (len > 1 && *grouping && _RWSTD_CHAR_MAX != *grouping)
        len = __rw_put_groups (pbuf, len, bufsize, end, grouping);

    // verify that the buffer hasn't overrun
    _RWSTD_ASSERT (len <= bufsize);

    return len;
}


}   // namespace __rw
