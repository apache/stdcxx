/***************************************************************************
 *
 * strtol.cpp - definitions of __rw_strtol, __rw_strtoul, and other helpers
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
 * Copyright 1994-2006 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC
#include <rw/_defs.h>     

#include "strtol.h"
#include <errno.h>      // for ERANGE, errno


// in case EINVAL is not #defined in errno.h
#ifdef EINVAL
#  define _RWSTD_EINVAL EINVAL
#else   // if !defined (EINVAL)
// actual value of EINVAL on both Linux and SunOS is 22
#  define _RWSTD_EINVAL 22
#endif   // EINVAL


_RWSTD_NAMESPACE (__rw) {


typedef unsigned char UChar;


// array of values of each base 2 through 36 digits, i.e., 0-9, A-Z,
// and a-z; elements with value greater than 35 do not correspond to
// any valid digit
_RWSTD_EXPORT extern const UChar
__rw_digit_map[] = {

#if 'A' == 0x41

    // basic ASCII:
    //        0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
    //       NUL SOH STX ETX EOT ENQ ACK BEL BS  TAB LF  VT  FF  CR  SO  SI
    /*  0 */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    //       DLE DC1 DC2 DC3 DC4 NAK SYN ETB CAN EM  SUB ESC FS  GS  RS  US
    /*  1 */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    //       SPC  !   "   #   $   %   '   '   (   )   *   +   ,   -   .   /
    /*  2 */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 43, 99, 43, 99, 99,
    //        0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
    /*  3 */  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 99, 99, 99, 99, 99, 99,
    //        @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
    /*  4 */ 99, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    //        P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
    /*  5 */ 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 99, 99, 99, 99, 99,
    //        `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
    /*  6 */ 99, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    //        p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~  DEL
    /*  7 */ 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 99, 99, 99, 99, 99,

    // extended ASCII:
    //                       IND NEL SSA ESA HTS HTJ VTS PLD PLU RI SS2 SS3
    /*  8 */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    //       DCS PU1 PU2 STS CCH MW  SPA EPA             CSI ST  OSC PM  APC
    /*  9 */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    /*  a */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    /*  b */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    /*  c */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    /*  d */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    /*  e */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    /*  f */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99

#elif 'A' == 0xc1

    // EBCDIC:
    //        0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
    //       NUL SOH STX ETX PF  HT  LC  DEL         SMM VT  FF  CR  SO  SI
    /*  0 */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    //       DLE DC1 DC2 TM  RES NL  BS  IL  CAN EM  CC  CU1 IFS IGS IRS IUS
    /*  1 */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    //       DS  SOS FS      BYP LF  ETB ESC         SM  CU2     ENQ ACK BEL
    /*  2 */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    //               SYN     PN  RS  UC  EOT             CU3 DC4 NAK     SUB
    /*  3 */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    //       SP                                      ct.  .   <   (   +   |
    /*  4 */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 43, 99,
    //        &                                       !   $   *   )   ;   ~
    /*  5 */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    //        -   /                                       ,   %   _   >   ?
    /*  6 */ 43, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    //                                                :   #   @   '   =   "
    /*  7 */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    //            a   b   c   d   e   f   g   h   i
    /*  8 */ 99, 10, 11, 12, 13, 14, 15, 16, 17, 18, 99, 99, 99, 99, 99, 99,
    //            j   k   l   m   n   o   p   q   r
    /*  9 */ 99, 19, 20, 21, 22, 23, 24, 25, 26, 27, 99, 99, 99, 99, 99, 99,
    //                s   t   u   v   w   x   y   z
    /*  a */ 99, 99, 28, 29, 30, 31, 32, 33, 34, 35, 99, 99, 99, 99, 99, 99,
    //                                            `
    /*  b */ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    //            A   B   C   D   E   F   G   H   I
    /*  c */ 99, 10, 11, 12, 13, 14, 15, 16, 17, 18, 99, 99, 99, 99, 99, 99,
    //            J   K   L   M   N   O   P   Q   R
    /*  d */ 99, 19, 20, 21, 22, 23, 24, 25, 26, 27, 99, 99, 99, 99, 99, 99,
    //                S   T   U   V   W   X   Y   Z
    /*  e */ 99, 99, 28, 29, 30, 31, 32, 33, 34, 35, 99, 99, 99, 99, 99, 99,
    //        0   1   2   3   4   5   6   7   8   9
    /*  f */  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 99, 99, 99, 99, 99, 99

#else   // 'A' != 0x41 && 'A' != 0xc1
#  error unknown character set (neither ASCII nor EBCDIC)
#endif   // ASCII or EBCDIC

};


// the number of bits necessary to exactly represent
// a single digit in base 0 - 36 (base is index)
static const char
__rw_base_bits [] = {
    /*  0 - 15 */ 0, 0, 1, 0, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
    /* 16 - 31 */ 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 32 - 36 */ 0, 0, 0, 0, 0

    // Note: __rw_base_bits [32] is 0 above to eliminate overflow
    //       checking for strings of up to 8 hexadecimal digits
    //       in __rw_strtoul() below
};


#undef max
#undef min


// SHift Left and OR: helper macro used by __rw_strtol
// to multiply a number by a power of 2 (SHL) and add
// another number less than the power (OR)
//
// IMPORTANT: each argument must be evaluated exactly once
#undef SHLOR
#define SHLOR(x, dig)   (((x) << shift) | __rw_digit_map [UChar (dig)])

// helper macro to keep lines under 80 characters
#define SHLOR_4_DIGITS_BEGIN(res, nptr)              \
     if (*++nptr) {                                  \
         res = SHLOR (res, *nptr);                   \
         if (*++nptr) {                              \
             res = SHLOR (res, *nptr);               \
             if (*++nptr) {                          \
                 res = SHLOR (res, *nptr);           \
                 if (*++nptr) {                      \
                     res = SHLOR (res, *nptr)

#define SHLOR_4_DIGITS_END() } } } } (void)0


// MULtiply and ADD: helper macro used by __rw_strtol
//
// IMPORTANT: each argument must be evaluated exactly once
#undef MULADD
#define MULADD(x, dig)   (((x) * base) + __rw_digit_map [UChar (dig)])

// helper macro to keep lines under 80 characters
#define MULADD_4_DIGITS_BEGIN(res, nptr)              \
     if (nptr [1]) {                                  \
         res = MULADD (res, *++nptr);                 \
         if (nptr [1]) {                              \
             res = MULADD (res, *++nptr);             \
             if (nptr [1]) {                          \
                 res = MULADD (res, *++nptr);         \
                 if (nptr [1]) {                      \
                     res = MULADD (res, *++nptr)

#define MULADD_4_DIGITS_END() } } } } (void)0


unsigned long
__rw_strtoul (const char *nptr, int *errptr, int base) _THROWS (())
{
    _RWSTD_ASSERT (0 != nptr);
    _RWSTD_ASSERT (0 != errptr);
    _RWSTD_ASSERT ('+' == *nptr || '-' == *nptr);
    _RWSTD_ASSERT (1 < base && base < 37);

    const bool neg = '-' == *nptr++;

    if (!*nptr) {
        *errptr = _RWSTD_EINVAL;
        return 0;
    }

    const int shift = __rw_base_bits [base];

    unsigned long res = __rw_digit_map [UChar (*nptr)];

    _RWSTD_ASSERT (res < unsigned (base));

    if (shift) {

        // process subject sequence by shifting

        if (*++nptr) {

            // unroll loop w/o overflow checking for as many as
            // 8 (on ILP32) or 16 (on LP64) digits (max base is 16)

            // shift left and add second digit
            res = SHLOR (res, *nptr);

            if (*++nptr) {
                // third digit
                res = SHLOR (res, *nptr);

                if (*++nptr) {
                    // fourth digit
                    res = SHLOR (res, *nptr);

#if 2 < _RWSTD_LONG_SIZE

                    // parse up to 4 more digits w/o overflow checking
                    // (i.e., digits 5 through 8, inclusive)
                    SHLOR_4_DIGITS_BEGIN (res, nptr);

#  if 4 < _RWSTD_LONG_SIZE

                    //              .2....:....1....:....0
                    // ULONG_MAX >= 1777777777777777777777 (oct)
                    // ULONG_MAX >=       ffffffffffffffff (hex)

                    // parse up to 8 more digits w/o overflow checking
                    // (i.e., digits 9 through 16, inclusive)
                    SHLOR_4_DIGITS_BEGIN (res, nptr);
                    SHLOR_4_DIGITS_BEGIN (res, nptr);

#  endif   // 4 < _RWSTD_LONG_SIZE
#endif   // 2 < _RWSTD_LONG_SIZE

                    // parse any remaining digits with overflow checking
                    // (i.e., digits 9 on ILP32 or 17 on LP64 and beyond)
                    while (*++nptr) {
                        const unsigned digit =
                            __rw_digit_map [UChar (*nptr)];

                        _RWSTD_ASSERT (digit < unsigned (base));

                        // check for overflow
                        const unsigned long save = res;

                        res <<= shift;

                        if (res < save)
                            goto overflow;

                        // can't overflow
                        res += digit;
                    }

#if 2 < _RWSTD_LONG_SIZE
#  if 4 < _RWSTD_LONG_SIZE

                    // close brackets
                    SHLOR_4_DIGITS_END ();
                    SHLOR_4_DIGITS_END ();

#  endif   // 4 < _RWSTD_LONG_SIZE

                    // close brackets
                    SHLOR_4_DIGITS_END ();

#endif   // 2 < _RWSTD_LONG_SIZE

                }
            }
        }
    }
    else {
        // process subject sequence by multiplication

        // unroll loop w/o overflow checking
        if (*++nptr) {
            // multiply by base and add second digit
            res = MULADD (res, *nptr);

#if 2 < _RWSTD_LONG_SIZE

            //              ....:....0 (10 decimal digits)
            // ULONG_MAX >= 4294967295 (dec)
            // ULONG_MAX >=    1z141z3 (base-36)

            // digits 3 through 6, inclusive
            MULADD_4_DIGITS_BEGIN (res, nptr);

#  if 4 < _RWSTD_LONG_SIZE

            //              ....:....1....:....0 (20 decimal digits)
            // ULONG_MAX >= 18446744073709551615 (dec)
            // ULONG_MAX >=        5g24a25twkwff (base-36)

            // digits 7 through 10, inclusive
            MULADD_4_DIGITS_BEGIN (res, nptr);

            if (nptr [1]) {
                // digit 11
                res = MULADD (res, *++nptr);

                if (base < 12) {

                    // unroll w/o overflow checking for bases up to 11

                    // digits 12 - 19
                    MULADD_4_DIGITS_BEGIN (res, nptr);
                    MULADD_4_DIGITS_BEGIN (res, nptr);

                    // close brackets
                    MULADD_4_DIGITS_END ();
                    MULADD_4_DIGITS_END ();
                }
            }

            // close brackets
            MULADD_4_DIGITS_END ();

#  else   // if 4 >= _RWSTD_LONG_SIZE

            if (base < 12) {

                // unroll w/o overflow checking for bases up to 11

                if (nptr [1]) {
                    // seventh digit
                    res = MULADD (res, *++nptr);
                    if (nptr [1]) {
                        // eighth digit
                        res = MULADD (res, *++nptr);
                        if (nptr [1]) {
                            // ninth digit
                            res = MULADD (res, *++nptr);
                        }
                    }
                }
            }

#  endif   // 4 < _RWSTD_LONG_SIZE

            // close brackets
            MULADD_4_DIGITS_END ();

#endif   // 2 < _RWSTD_LONG_SIZE

            for (unsigned long maxres = _RWSTD_ULONG_MAX / base; *++nptr; ) {

                const unsigned digit = __rw_digit_map [UChar (*nptr)];

                _RWSTD_ASSERT (digit < unsigned (base));

                // check for overflow
                if (maxres < res)
                    goto overflow;

                res *= base;

                if (_RWSTD_ULONG_MAX - res < digit)
                    goto overflow;

                res += digit;
            }
        }
    }

    return neg ? 0UL - res : res;

overflow:

    // set errptr to ERANGE on overflow and return ULONG_MAX
    *errptr = ERANGE;
    return _RWSTD_ULONG_MAX;
}


long
__rw_strtol (const char *nptr, int *errptr, int base) _THROWS (())
{
    _RWSTD_ASSERT (0 != nptr);
    _RWSTD_ASSERT (0 != errptr);
    _RWSTD_ASSERT ('+' == *nptr || '-' == *nptr);

    const bool neg = '-' == *nptr++;

    if (!*nptr) {
        *errptr = _RWSTD_EINVAL;
        return 0;
    }

    const int shift = __rw_base_bits [base];

    unsigned long res = __rw_digit_map [UChar (*nptr)];

    _RWSTD_ASSERT (res < unsigned (base));

    if (shift) {

        // process subject sequence by shifting

        if (*++nptr) {

            // unroll loop w/o overflow checking for as many as
            // 8 (on ILP32) or 16 (on LP64) digits (max base is 16)

            // shift left and add second digit
            res = SHLOR (res, *nptr);

            if (*++nptr) {
                // third digit
                res = SHLOR (res, *nptr);

                if (*++nptr) {
                    // fourth digit
                    res = SHLOR (res, *nptr);

#if 2 < _RWSTD_LONG_SIZE

                    // parse up to 4 more digits w/o overflow checking
                    // (i.e., digits 5 through 8, inclusive)
                    SHLOR_4_DIGITS_BEGIN (res, nptr);

#  if 4 < _RWSTD_LONG_SIZE

                    //              .2....:....1....:....0
                    // ULONG_MAX >= 1777777777777777777777 (oct)
                    // ULONG_MAX >=       ffffffffffffffff (hex)

                    // parse up to 8 more digits w/o overflow checking
                    // (i.e., digits 9 through 16, inclusive)
                    SHLOR_4_DIGITS_BEGIN (res, nptr);
                    SHLOR_4_DIGITS_BEGIN (res, nptr);

#  endif   // 4 < _RWSTD_LONG_SIZE
#endif   // 2 < _RWSTD_LONG_SIZE

                    // parse any remaining digits with overflow checking
                    // (i.e., digits 9 on ILP32 or 17 on LP64 and beyond)
                    while (*++nptr) {
                        const unsigned digit =
                            __rw_digit_map [UChar (*nptr)];

                        _RWSTD_ASSERT (digit < unsigned (base));

                        // check for overflow
                        const unsigned long save = res;

                        res <<= shift;

                        if (res < save)
                            goto overflow;

                        // can't overflow
                        res += digit;
                    }

#if 2 < _RWSTD_LONG_SIZE
#  if 4 < _RWSTD_LONG_SIZE

                    // close brackets
                    SHLOR_4_DIGITS_END ();
                    SHLOR_4_DIGITS_END ();

#  endif   // 4 < _RWSTD_LONG_SIZE

                    // close brackets
                    SHLOR_4_DIGITS_END ();

#endif   // 2 < _RWSTD_LONG_SIZE

                }
            }
        }
    }
    else {
        // process subject sequence by multiplication

        // unroll loop w/o overflow checking
        if (*++nptr) {
            // multiply by base and add second digit
            res = MULADD (res, *nptr);

#if 2 < _RWSTD_LONG_SIZE

            //              ....:....0 (10 decimal digits)
            // ULONG_MAX >= 4294967295 (dec)
            // ULONG_MAX >=    1z141z3 (base-36)

            // digits 3 through 6, inclusive
            MULADD_4_DIGITS_BEGIN (res, nptr);

#  if 4 < _RWSTD_LONG_SIZE

            //              ....:....1....:....0 (20 decimal digits)
            // ULONG_MAX >= 18446744073709551615 (dec)
            // ULONG_MAX >=        5g24a25twkwff (base-36)

            // digits 7 through 10, inclusive
            MULADD_4_DIGITS_BEGIN (res, nptr);

            if (nptr [1]) {
                // digit 11
                res = MULADD (res, *++nptr);

                if (base < 12) {

                    // unroll w/o overflow checking for bases up to 11

                    // digits 12 - 19
                    MULADD_4_DIGITS_BEGIN (res, nptr);
                    MULADD_4_DIGITS_BEGIN (res, nptr);

                    // close brackets
                    MULADD_4_DIGITS_END ();
                    MULADD_4_DIGITS_END ();
                }
            }

            // close brackets
            MULADD_4_DIGITS_END ();

#  else   // if 4 >= _RWSTD_LONG_SIZE

            if (base < 12) {

                // unroll w/o overflow checking for bases up to 11

                if (nptr [1]) {
                    // seventh digit
                    res = MULADD (res, *++nptr);
                    if (nptr [1]) {
                        // eighth digit
                        res = MULADD (res, *++nptr);
                        if (nptr [1]) {
                            // ninth digit
                            res = MULADD (res, *++nptr);
                        }
                    }
                }
            }

#  endif   // 4 < _RWSTD_LONG_SIZE

            // close brackets
            MULADD_4_DIGITS_END ();

#endif   // 2 < _RWSTD_LONG_SIZE

            
            for (unsigned long maxres = _RWSTD_ULONG_MAX / base; *++nptr; ) {

                const unsigned digit = __rw_digit_map [UChar (*nptr)];

                _RWSTD_ASSERT (digit < unsigned (base));

                // check for overflow
                if (maxres < res)
                    goto overflow;

                res *= base;

                if (_RWSTD_ULONG_MAX - res < digit)
                    goto overflow;

                res += digit;
            }
        }
    }

    typedef unsigned long ULong;

    if (neg) {
        if (res > ULong (_RWSTD_LONG_MIN)) {
            *errptr = ERANGE;
            return _RWSTD_LONG_MIN;
        }

        return -long (res);
    }

    if (res > ULong (_RWSTD_LONG_MAX)) {
        *errptr = ERANGE;
        return _RWSTD_LONG_MAX;
    }
   
    return res;

overflow:

    // set errptr to ERANGE on overflow and return LONG_MIN or _MAX
    *errptr = ERANGE;
    return ULong (neg ? _RWSTD_LONG_MIN : _RWSTD_LONG_MAX);
}


#ifdef _RWSTD_LONG_LONG


// for convenience
typedef _RWSTD_LONG_LONG          LLong;
typedef unsigned _RWSTD_LONG_LONG ULLong;


   // using LLONG_SIZE instead of ULLONG_MAX in the preprocessor
   // conditional below to work around a gcc 3.2 bug (PR #28595)
#  if (_RWSTD_LONG_SIZE < _RWSTD_LLONG_SIZE)

ULLong
__rw_strtoull (const char *nptr, int *errptr, int base) _THROWS (())
{
    _RWSTD_ASSERT (0 != nptr);
    _RWSTD_ASSERT (0 != errptr);
    _RWSTD_ASSERT ('+' == *nptr || '-' == *nptr);

    const bool neg = '-' == *nptr++;

    if (!*nptr) {
        *errptr = _RWSTD_EINVAL;
        return 0;
    }
        
    const int shift = __rw_base_bits [base];

    ULLong res = __rw_digit_map [UChar (*nptr)];

    _RWSTD_ASSERT (res < unsigned (base));

    if (shift) {

        // process subject sequence by shifting

        if (*++nptr) {

            // unroll loop w/o overflow checking for as many as
            // 8 (on ILP32) or 16 (on LP64) digits (max base is 16)

            // shift left and add second digit
            res = SHLOR (res, *nptr);

            if (*++nptr) {
                // third digit
                res = SHLOR (res, *nptr);

                if (*++nptr) {
                    // fourth digit
                    res = SHLOR (res, *nptr);

                    // parse up to 4 more digits w/o overflow checking
                    // (i.e., digits 5 through 8, inclusive)
                    SHLOR_4_DIGITS_BEGIN (res, nptr);

#    if 4 < _RWSTD_LLONG_SIZE

                    //               .2....:....1....:....0
                    // ULLONG_MAX >= 1777777777777777777777 (oct)
                    // ULLONG_MAX >=       ffffffffffffffff (hex)

                    // parse up to 8 more digits w/o overflow checking
                    // (i.e., digits 9 through 16, inclusive)
                    SHLOR_4_DIGITS_BEGIN (res, nptr);
                    SHLOR_4_DIGITS_BEGIN (res, nptr);

#    endif   // 4 < _RWSTD_LLONG_SIZE

                    // parse any remaining digits with overflow checking
                    // (i.e., digits 9 on ILP32 or 17 on LP64 and beyond)
                    while (*++nptr) {
                        const unsigned digit =
                            __rw_digit_map [UChar (*nptr)];

                        _RWSTD_ASSERT (digit < unsigned (base));

                        // check for overflow
                        const ULLong save = res;

                        res <<= shift;

                        if (res < save)
                            goto overflow;

                        // can't overflow
                        res += digit;
                    }

#    if 4 < _RWSTD_LLONG_SIZE

                    // close brackets
                    SHLOR_4_DIGITS_END ();
                    SHLOR_4_DIGITS_END ();

#    endif   // 4 < _RWSTD_LLONG_SIZE

                    // close brackets
                    SHLOR_4_DIGITS_END ();
                }
            }
        }
    }
    else {
        // process subject sequence by multiplication

        // unroll loop w/o overflow checking
        if (nptr [1]) {
            // multiply by base and add second digit
            res = MULADD (res, *++nptr);

            // digits 3 through 6, inclusive
            MULADD_4_DIGITS_BEGIN (res, nptr);

#    if 4 < _RWSTD_LLONG_SIZE

            //               ....:....1....:....0 (20 decimal digits)
            // ULLONG_MAX >= 18446744073709551615 (dec)
            // ULLONG_MAX >=        5g24a25twkwff (base-36)

            // digits 7 through 10, inclusive
            MULADD_4_DIGITS_BEGIN (res, nptr);

            if (nptr [1]) {
                // digit 11
                res = MULADD (res, *++nptr);

                if (base < 12) {

                    // unroll w/o overflow checking for bases up to 11

                    // digits 12 - 19
                    MULADD_4_DIGITS_BEGIN (res, nptr);
                    MULADD_4_DIGITS_BEGIN (res, nptr);

                    // close brackets
                    MULADD_4_DIGITS_END ();
                    MULADD_4_DIGITS_END ();
                }
            }

            // close brackets
            MULADD_4_DIGITS_END ();

#    else   // if 4 >= _RWSTD_LLONG_SIZE

            if (base < 12) {

                // unroll w/o overflow checking for bases up to 11

                if (nptr [1]) {
                    // seventh digit
                    res = MULADD (res, *++nptr);
                    if (nptr [1]) {
                        // eighth digit
                        res = MULADD (res, *++nptr);
                        if (nptr [1]) {
                            // ninth digit
                            res = MULADD (res, *++nptr);
                        }
                    }
                }
            }

#    endif   // 4 < _RWSTD_LLONG_SIZE

            // close brackets
            MULADD_4_DIGITS_END ();

            for (ULLong maxres = _RWSTD_ULLONG_MAX / base; *++nptr; ) {

                const unsigned digit = __rw_digit_map [UChar (*nptr)];

                _RWSTD_ASSERT (digit < unsigned (base));

                // check for overflow
                if (maxres < res)
                    goto overflow;

                res *= base;

                if (_RWSTD_ULLONG_MAX - res < digit)
                    goto overflow;

                res += digit;
            }
        }
    }

    return neg ? 0UL - res : res;

overflow:

    // set errptr to ERANGE on overflow and return ULLONG_MAX
    *errptr = ERANGE;
    return _RWSTD_ULLONG_MAX;
}


LLong
__rw_strtoll (const char *nptr, int *errptr, int base) _THROWS (())
{
    _RWSTD_ASSERT (0 != nptr);
    _RWSTD_ASSERT (0 != errptr);
    _RWSTD_ASSERT ('+' == *nptr || '-' == *nptr);

    const bool neg = '-' == *nptr++;

    if (!*nptr) {
        *errptr = _RWSTD_EINVAL;
        return 0;
    }

    const int shift = __rw_base_bits [base];

    ULLong res = __rw_digit_map [UChar (*nptr)];

    _RWSTD_ASSERT (res < unsigned (base));

    if (shift) {

        // process subject sequence by shifting

        if (*++nptr) {

            // unroll loop w/o overflow checking for as many as
            // 8 (on ILP32) or 16 (on LP64) digits (max base is 16)

            // shift left and add second digit
            res = SHLOR (res, *nptr);

            if (*++nptr) {
                // third digit
                res = SHLOR (res, *nptr);

                if (*++nptr) {
                    // fourth digit
                    res = SHLOR (res, *nptr);

                    // parse up to 4 more digits w/o overflow checking
                    // (i.e., digits 5 through 8, inclusive)
                    SHLOR_4_DIGITS_BEGIN (res, nptr);

#    if 4 < _RWSTD_LLONG_SIZE

                    //               .2....:....1....:....0
                    // ULLONG_MAX >= 1777777777777777777777 (oct)
                    // ULLONG_MAX >=       ffffffffffffffff (hex)

                    // parse up to 8 more digits w/o overflow checking
                    // (i.e., digits 9 through 16, inclusive)
                    SHLOR_4_DIGITS_BEGIN (res, nptr);
                    SHLOR_4_DIGITS_BEGIN (res, nptr);

#    endif   // 4 < _RWSTD_LLONG_SIZE

                    // parse any remaining digits with overflow checking
                    // (i.e., digits 9 on ILP32 or 17 on LP64 and beyond)
                    while (*++nptr) {
                        const unsigned digit =
                            __rw_digit_map [UChar (*nptr)];

                        _RWSTD_ASSERT (digit < unsigned (base));

                        // check for overflow
                        const ULLong save = res;

                        res <<= shift;

                        if (res < save)
                            goto overflow;

                        // can't overflow
                        res += digit;
                    }

#    if 4 < _RWSTD_LLONG_SIZE

                    // close brackets
                    SHLOR_4_DIGITS_END ();
                    SHLOR_4_DIGITS_END ();

#    endif   // 4 < _RWSTD_LLONG_SIZE

                    // close brackets
                    SHLOR_4_DIGITS_END ();
                }
            }
        }
    }
    else {

        // process subject sequence by multiplication

        // unroll loop w/o overflow checking
        if (*++nptr) {
            // multiply by base and add second digit
            res = MULADD (res, *nptr);

            // digits 3 through 6, inclusive
            MULADD_4_DIGITS_BEGIN (res, nptr);

#    if 4 < _RWSTD_LLONG_SIZE

            //               ....:....1....:....0 (20 decimal digits)
            // ULLONG_MAX >= 18446744073709551615 (dec)
            // ULLONG_MAX >=        5g24a25twkwff (base-36)

            // digits 7 through 10, inclusive
            MULADD_4_DIGITS_BEGIN (res, nptr);

            if (nptr [1]) {
                // digit 11
                res = MULADD (res, *++nptr);

                if (base < 12) {

                    // unroll w/o overflow checking for bases up to 11

                    // digits 12 - 19
                    MULADD_4_DIGITS_BEGIN (res, nptr);
                    MULADD_4_DIGITS_BEGIN (res, nptr);

                    // close brackets
                    MULADD_4_DIGITS_END ();
                    MULADD_4_DIGITS_END ();
                }
            }

            // close brackets
            MULADD_4_DIGITS_END ();

#    else   // if 4 >= _RWSTD_LLONG_SIZE

            if (base < 12) {

                // unroll w/o overflow checking for bases up to 11

                if (nptr [1]) {
                    // seventh digit
                    res = MULADD (res, *++nptr);
                    if (nptr [1]) {
                        // eighth digit
                        res = MULADD (res, *++nptr);
                        if (nptr [1]) {
                            // ninth digit
                            res = MULADD (res, *++nptr);
                        }
                    }
                }
            }

#    endif   // 4 < _RWSTD_LLONG_SIZE

            // close brackets
            MULADD_4_DIGITS_END ();

            for (ULLong maxres = _RWSTD_ULLONG_MAX / base; *++nptr; ) {

                const unsigned digit = __rw_digit_map [UChar (*nptr)];

                _RWSTD_ASSERT (digit < unsigned (base));

                // check for overflow
                if (maxres < res)
                    goto overflow;

                res *= base;

                if (_RWSTD_ULLONG_MAX - res < digit)
                    goto overflow;

                res += digit;
            }
        }
    }

    if (neg) {
        if (res > ULLong (_RWSTD_LLONG_MIN)) {
            *errptr = ERANGE;
            return _RWSTD_LLONG_MIN;
        }

        return -LLong (res);
    }

    if (res > ULLong (_RWSTD_LLONG_MAX)) {
        *errptr = ERANGE;
        return _RWSTD_LLONG_MAX;
    }
   
    return res;

overflow:

    // set errptr to ERANGE on overflow and return LLONG_MIN or _MAX
    *errptr = ERANGE;
    return ULLong (neg ? _RWSTD_LLONG_MIN : _RWSTD_LLONG_MAX);
}


#  endif   // _RWSTD_LLONG_SIZE < _RWSTD_LLONG_SIZE
#endif   // _RWSTD_LONG_LONG


}   // namespace __rw
