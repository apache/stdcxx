/***************************************************************************
 *
 * bitset.cpp - definitions of bitset operations
 *
 * $Id: //stdlib/dev/source/stdlib/bitset.cpp#28 $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <bitset>
#include <string.h>       // for memmove(), memset(), strlen()
#include <rw/_traits.h>   // for char_traits<wchar_t>

#include <rw/_defs.h>

#include <stdio.h>

_RWSTD_NAMESPACE (__rw) {

#if 2 == _RWSTD_LONG_SIZE
static const int           __rw_wbasebits =  1;
static const _RWSTD_SIZE_T __rw_wbasemask = 15;
#elif 4 == _RWSTD_LONG_SIZE
static const int           __rw_wbasebits =  2;
static const _RWSTD_SIZE_T __rw_wbasemask = 31;
#elif 8 == _RWSTD_LONG_SIZE
static const int           __rw_wbasebits =  3;
static const _RWSTD_SIZE_T __rw_wbasemask = 63;
#else   // assume 16 == sizeof (long)
static const int           __rw_wbasebits =   4;
static const _RWSTD_SIZE_T __rw_wbasemask = 127;
#endif   // _RWSTD_LONG_SIZE

// overloads of __rw_bitset() implement:
//
// 23.3.5.1 bitset constructors                            [lib.bitset.cons]
//
// bitset<N>::
// bitset(basic_string<charT, Traits, Allocator>& str,
//        typename basic_string<charT, Traits, Allocator>::size_type pos,
//        typename basic_string<charT, Traits, Allocator>::size_type n);
//
// -5- Effects: Determines the effective length rlen of the initializing
//     string as the smaller of n and str.size() - pos.
//     The function then throws invalid_argument if any of the rlen
//     characters in str beginning at position pos is other than 0 or 1.
//     Otherwise, the function constructs an object of class bitset<N>,
//     initializing the first M bit positions to values determined from
//     the corresponding characters in the string str. M is the smaller
//     of N and rlen.
// -6- An element of the constructed string has value zero if
//     the corresponding character in str, beginning at position pos,
//     is 0. Otherwise, the element has the value one. Character
//     position pos + M - 1 corresponds to bit position zero.
//     Subsequent decreasing character positions correspond to
//     increasing bit positions.
// -7- If M < N, remaining bit positions are initialized to zero.

_RWSTD_SPECIALIZED_FUNCTION _RWSTD_EXPORT void
__rw_bitset (unsigned long *bits, _RWSTD_SIZE_T maxbits,
             const char *str, _RWSTD_SIZE_T slen,
             const _STD::char_traits<char>*, char b0, char b1,
             _RWSTD_SIZE_T pos, _RWSTD_SIZE_T n,
             const char *file, const char *fun)
{
    if (_RWSTD_SIZE_MAX == slen)
        slen = strlen (str);

    // verify that `pos' is a valid offset into `str'
    _RWSTD_REQUIRES (pos <= slen,
                     (_RWSTD_ERROR_OUT_OF_RANGE, file, fun, pos, slen));

    // compute the number of characters in `str' past the offset `pos'
    const _RWSTD_SIZE_T nchars = n < (slen - pos) ? n : slen - pos;

    // compute the number of bits to initialize from `str'
    const _RWSTD_SIZE_T nbits = nchars < maxbits ? nchars : maxbits;

    str += pos;

    // compute the number of non-zero bytes occupied by `bits'
    _RWSTD_SIZE_T nbytes = ((maxbits | 1) >> 3) + (0 != (maxbits & 7));

    // round the number up to a multiple of sizeof(long)
    nbytes = (  (nbytes >> __rw_wbasebits)
              + (0 != (nbytes & (__rw_wbasemask >> 3)))) << __rw_wbasebits;

    memset (bits, 0, nbytes);

    // set all bits but also check any extra characters as required
    for (_RWSTD_SIZE_T i = 0; i != nchars; ++i) {

        if (b1 == str [i]) {
            if (i < nbits) {
                const _RWSTD_SIZE_T bitno = nbits - i - 1;

                // efficiently compute the value of the expression below
                // (i.e., without relying on slow division and modulo)
                // bits [bitno / wordbits] |= 1UL << bitno % wordbits;
                bits [bitno >> (3 + __rw_wbasebits)]
                    |= 1UL << (bitno & __rw_wbasemask);
            }
        }
        else {
            // verify that the character is valid
            _RWSTD_REQUIRES (b0 == str [i],
                             (_RWSTD_ERROR_INVALID_ARGUMENT, file, fun));
        }
    }
}


#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_FUNCTION _RWSTD_EXPORT void
__rw_bitset (unsigned long *bits, _RWSTD_SIZE_T maxbits,
             const wchar_t *str, _RWSTD_SIZE_T slen,
             const _STD::char_traits<wchar_t>*, wchar_t b0, wchar_t b1,
             _RWSTD_SIZE_T pos, _RWSTD_SIZE_T n,
             const char *file, const char *fun)
{
    if (_RWSTD_SIZE_MAX == slen)
        slen = _STD::char_traits<wchar_t>::length (str);

    // verify that `pos' is a valid offset into `str'
    _RWSTD_REQUIRES (pos <= slen,
                     (_RWSTD_ERROR_OUT_OF_RANGE, file, fun, pos, slen));

    // compute the number of characters in `str' past the offset `pos'
    const _RWSTD_SIZE_T nchars = n < (slen - pos) ? n : slen - pos;

    // compute the number of bits to initialize from `str'
    const _RWSTD_SIZE_T nbits = nchars < maxbits ? nchars : maxbits;

    str += pos;

    // compute the number of non-zero bytes occupied by `bits'
    _RWSTD_SIZE_T nbytes = ((maxbits | 1) >> 3) + (0 != (maxbits & 7));

    // round the number up to a multiple of sizeof(long)
    nbytes = (  (nbytes >> __rw_wbasebits)
              + (0 != (nbytes & (__rw_wbasemask >> 3)))) << __rw_wbasebits;

    memset (bits, 0, nbytes);

    // set all bits but also check any extra characters as required
    for (_RWSTD_SIZE_T i = 0; i != nchars; ++i) {

        if (b1 == str [i]) {
            if (i < nbits) {
                const _RWSTD_SIZE_T bitno = nbits - i - 1;
                bits [bitno >> (3 + __rw_wbasebits)]
                    |= 1UL << (bitno & __rw_wbasemask);
            }
        }
        else {
            // verify that the character is valid
            _RWSTD_REQUIRES (b0 == str [i],
                             (_RWSTD_ERROR_INVALID_ARGUMENT, file, fun));
        }
    }
}

#endif   // _RWSTD_NO_WCHAR_T


// count the number of 1 bits in bitset `bits' of `size' elements
_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_bit_count (const unsigned long *bits,
                _RWSTD_SIZE_T        size) _THROWS (())
{
    _RWSTD_SIZE_T sum = 0;

#if 4 == _RWSTD_LONG_SIZE

    // works if sizeof (unsigned long) * CHAR_BIT < 63
    for (_RWSTD_SIZE_T i = 0; i != size; i++) {

        if (bits [i]) {

            const unsigned long n = bits [i];

            unsigned long t = n - ((n >> 1) & 033333333333UL)
                                - ((n >> 2) & 011111111111UL);

            t = (t + (t >> 3)) & 030707070707UL;

            t = (t & 07700770077UL) + ((t >> 6) & 07700770077UL);
            t = ((t >> 12) + (t >> 24) + t) & 0777UL;
            t = (t >> 6) + (t & 077UL) + 1UL;

            sum += (t >> 6) + (t & 077UL) - 1UL;
        }
    }

#else   // if 4 != _RWSTD_LONG_SIZE

    // the more naive implementation that always works
    for (_RWSTD_SIZE_T i = 0; i != size; i++) {
        for (unsigned long n = bits [i]; n; n &= n - 1UL) {
            ++sum;
        }
    }

#endif   // _RWSTD_LONG_SIZE

    return sum;
}


// shift bitset `bits' of `size' elements left by `n' positions
_RWSTD_EXPORT void
__rw_shl (unsigned long *bits,
          _RWSTD_SIZE_T size, _RWSTD_SIZE_T n) _THROWS (())
{
    const _RWSTD_SIZE_T wordbits = sizeof *bits * _RWSTD_CHAR_BIT;

    if (n >= size * wordbits) {
        memset (bits, 0, size * sizeof *bits);
        return;
    }

    if (!n)
        return;

    const _RWSTD_SIZE_T shlw = n / wordbits;    // word shift left
    const _RWSTD_SIZE_T shlb = n % wordbits;    // bit shift left

    if (shlb) {
        const _RWSTD_SIZE_T shrb = wordbits - shlb; // bit shift right

        // shift one word at a time, spanning word boundaries
        for (_RWSTD_SIZE_T i = size - 1; i > shlw; --i)
            bits [i] =   bits [i - shlw]     << shlb
                       | bits [i - shlw - 1] >> shrb;

        // shift the last word
        bits [shlw] = bits [0] << shlb;
    }
    else {
        // no shifting necessary, just copy 
        memmove (bits + shlw, bits, (size - shlw) * sizeof *bits);
    }

    // fill remainder with zeros
    memset (bits, 0, shlw * sizeof *bits);
}


// shift bitset `bits' of `size' elements right by `n' positions
_RWSTD_EXPORT void
__rw_shr (unsigned long *bits,
          _RWSTD_SIZE_T size, _RWSTD_SIZE_T n) _THROWS (())
{
    const _RWSTD_SIZE_T wordbits = sizeof *bits * _RWSTD_CHAR_BIT;

    if (n >= size * wordbits) {
        memset (bits, 0, size * sizeof *bits);
        return;
    }

    if (!n)
        return;

    const _RWSTD_SIZE_T shrw = n / wordbits;    // word shift right
    const _RWSTD_SIZE_T shrb = n % wordbits;    // bit shift right

    // max word index
    const _RWSTD_SIZE_T maxw = size - shrw - 1;

    if (shrb) {
        const _RWSTD_SIZE_T shlb = wordbits - shrb; // bit shift left

        // shift one word at a time, spanning word boundaries
        for (_RWSTD_SIZE_T i = 0; i < maxw;  ++i)
            bits [i] =   bits [i + shrw]     >> shrb
                       | bits [i + shrw + 1] << shlb;

        // shift the last word
        bits [maxw] = bits [size - 1] >> shrb;
    }
    else {
        // no shifting necessary, just copy
        memmove (bits, bits + shrw, (maxw + 1) * sizeof *bits);
    }

    // fill remainder with zeros
    memset (bits + (maxw + 1), 0, (size - (maxw + 1)) * sizeof *bits);
}


}   // namespace __rw
