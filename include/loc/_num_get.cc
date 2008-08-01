/***************************************************************************
 *
 * _num_get.cc - definition of std::num_get members
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


#include <loc/_ctype.h>
#include <loc/_locale.h>
#include <loc/_numpunct.h>
#include <rw/_basic_ios.h>


_RWSTD_NAMESPACE (__rw) { 

_RWSTD_EXPORT int
 __rw_get_num (void*, const char*, int, int,
               const char*, _RWSTD_SIZE_T, const char*, _RWSTD_SIZE_T);


// array of values of each base 2 through 36 digit, i.e., 0-9, A-Z,
// and a-z; elements with a value greater than 35 do not correspond
// to any valid digit
_RWSTD_EXPORT extern const unsigned char __rw_digit_map[];

// array of 1-based indices for each 8-bit character into an internal
// table of values of each roman digit, i.e., I, V, X, L, C, D, and M
// elements with a value greater than 7 do not correspond to a roman digit
_RWSTD_EXPORT extern const unsigned char __rw_roman_inxs[];

}   // namespace __rw


_RWSTD_NAMESPACE (std) { 


template <class _CharT, class _InputIter>
_RW::__rw_facet_id num_get<_CharT, _InputIter>::id;



#ifndef _RWSTD_NO_EXT_NUM_GET

template <class _CharT, class _InputIter>
typename num_get<_CharT, _InputIter>::iter_type
num_get<_CharT, _InputIter>::
get (iter_type __begin, iter_type __end, ios_base &__flags, 
     _RWSTD_IOSTATE &__err, short &__val) const
{
    _RWSTD_ASSERT_RANGE (__begin, __end);

    // preserve original value in case it's non-zero and
    // do_get() fails for reason other than bad grouping
    long __tmp = __val;

    __begin = do_get (__begin, __end, __flags, __err, __tmp);
    __val = __rw_check_overflow_short (__tmp, __flags.flags (), __err);

    return __begin;
}


template <class _CharT, class _InputIter>
typename num_get<_CharT, _InputIter>::iter_type
num_get<_CharT, _InputIter>::
get (iter_type __begin, iter_type __end, ios_base &__flags, 
     _RWSTD_IOSTATE &__err, int &__val) const
{
    _RWSTD_ASSERT_RANGE (__begin, __end);

    // preserve original value in case it's non-zero and
    // do_get() fails for reason other than bad grouping
    long __tmp = long (__val);

    __begin = do_get (__begin, __end, __flags, __err, __tmp);
    __val = __rw_check_overflow_int (__tmp, __flags.flags (), __err);

    return __begin;
}

#endif   // _RWSTD_NO_EXT_NUM_GET


#ifndef _RWSTD_NO_NATIVE_BOOL

template <class _CharT, class _InputIter /* = istreambuf_iterator<_CharT> */>
typename num_get<_CharT, _InputIter>::iter_type
num_get<_CharT, _InputIter>::
do_get (iter_type __begin, iter_type __end, ios_base &__flags,
        _RWSTD_IOSTATE &__err, bool &__val) const
{
    if (0 == (__flags.flags () & _RWSTD_IOS_BOOLALPHA)) {
        // initialize to an invalid value
        long __lval = -1L;

        // avoid calling overridden do_get() if it exists
        typedef num_get<_CharT, _InputIter> _This;
        __begin = _This::do_get (__begin, __end, __flags, __err, __lval);

        // set failbit in accordance with 22.2.2.1.2, p14 but without
        // testing for failbit in `err' since the bit could be set even
        // for valid `val' (e.g., when the positions of thousands_seps
        // in otherwise valid input do not match grouping)
        if (_RWSTD_STATIC_CAST (unsigned long, __lval) < 2)
            __val = 0L != __lval;
        else
            __err |= _RW::__rw_failbit;

        return __begin;
    }

    __err = _RW::__rw_goodbit;

    const numpunct<char_type> &__pun = 
        _RWSTD_USE_FACET (numpunct<char_type>, __flags.getloc ());

    // lwg issue 17
    typedef basic_string<char_type> string_type;

    const string_type __tnm = __pun.truename ();
    const string_type __fnm = __pun.falsename ();

    const char_type* const __names[] = {
        __fnm.c_str (), __tnm.c_str ()
    };

    const _RWSTD_SIZE_T __sizes[] = {
        __fnm.size (), __tnm.size ()
    };

    _RWSTD_SIZE_T __inx = 0;

    int __errtmp = 1;   // maximum number of allowed duplicates

    __begin = _RW::__rw_match_name (__begin, __end, __names, __sizes,
                                    sizeof __names / sizeof *__names,
                                    __inx, __errtmp, 0);

    if (_RWSTD_SIZE_MAX == __inx)
        __err |= _RW::__rw_failbit;
    else
        __val = !!__inx;

    __err |= _RWSTD_IOSTATE (__errtmp);

    return __begin;
}

#endif   // _RWSTD_NO_NATIVE_BOOL


template <class _CharT, class _InputIter /* = istreambuf_iterator<_CharT> */>
typename num_get<_CharT, _InputIter>::iter_type
num_get<_CharT, _InputIter>::
_C_get (iter_type __begin, iter_type __end, ios_base &__flags,
        _RWSTD_IOSTATE &__err, int __type, void *__pval) const
{
    __err = _RW::__rw_goodbit;

    const int __fl = __flags.flags ();

    // boolalpha parsing not handled here
    _RWSTD_ASSERT (__type != _C_bool || !(__fl & _RWSTD_IOS_BOOLALPHA));

    const locale &__loc = __flags.getloc ();

    const numpunct<char_type> &__pun = 
        _RWSTD_USE_FACET (numpunct<char_type>, __loc);

    // 22.2.2.1.2, p8: Stage 2

    char __buf [128 + 2];                     // default buffer (128 bits + 2)
    char *__pbuf            = __buf;          // pointer to allocated buffer
    char *__pcur            = __buf;          // currently processed digit
    _RWSTD_SIZE_T __bufsize = sizeof __buf;   // size of allocated buffer

    _RWSTD_UNUSED (__bufsize);

    const ctype<char_type> &__ctp = _RWSTD_USE_FACET (ctype<char_type>, __loc);

    const _CharT __decimal_point = __pun.decimal_point ();
    const _CharT __thousands_sep = __pun.thousands_sep ();

    // grouping string and size are lazily initialized only
    // when the first thousands_sep() is encountered
    string __grouping;
    int    __grpsz = -1;

    // buffer containing the sizes of thousands_sep-separated
    // groups of digits and a pointer to the next grouping
    char __grpbuf [sizeof __buf];
    char *__pgrp = __grpbuf;

    const char *__grpbeg = 0;   // the beginning of the last group
    const char *__grpend = 0;   // the end of the last group

    int __base = unsigned (__fl) >> _RWSTD_IOS_BASEOFF;

    // switch to base-16 for pointer parsing
    if (_RW::__rw_facet::_C_pvoid == __type)
        __base = 16;
    else if (10 == __base && !(__fl & _RWSTD_IOS_BASEFIELD))
        __base = 0;

    int __subtype = __type;   // type of the number being parsed

    typedef unsigned char _UChar;

    // insert a plus sign (ovewrite it with a minus sign if found)
    // to avoid having to deal with the fact that it's optional
    *__pcur++ = '+';

    static const _UChar _HexX = _RW::__rw_digit_map [_UChar ('X')];
    static const _UChar _Sign = _RW::__rw_digit_map [_UChar ('+')];

    // 'x' or 'X' after a leading '0' (the same value for both),
    // any value outside the valid range [0, 36) is invalid and
    // will not match any character
    _UChar __hex_x = _HexX;

    // leading plus or minus sign (the same value for both)
    // any value outside the valid range [0, 36) is invalid
    _UChar __ld_sgn = _Sign;

    for ( ; ; ++__begin) {

        if (__begin == __end) {
            // 22.2.2.1, p13
            __err |= _RW::__rw_eofbit;
            break;
        }

        if (__pcur == __buf + sizeof __buf - 1) {
            // FIXME: handle long strings of digits
            __err |= _RW::__rw_failbit;
            break;
        }

        const _CharT __wc = *__begin;
        const char   __dp = __decimal_point == __wc ? '.' : '\0';

        if (   __thousands_sep == __wc
            && _RW::__rw_digit_map [_UChar (__pcur [-1])] < 36
            && (   __grpsz > 0
                || __grpsz < 0
                && (__grpsz = int((__grouping = __pun.grouping ()).size ())))) {

            // the current character is the thousands separator,
            // the previous character was a digit (and not a sign),
            // and grouping (lazily initialized above) is not empty

            if (__pcur == __pbuf + 1) {
                // 22.2.3.1, p2: thousands_sep not allowed to be first
                // non-fatal error, set failbit but continue parsing
                __err |= _RW::__rw_failbit;
            }

            if (__grpend) {
                // thousands_sep not allowed or recognized after the
                // end of the mantissa has been reached (i.e., after
                // the decimal point or after 'E' or 'e')
                break;
            }

            // add the length of the current group to the array groups
            // store UCHAR_MAX if group length exceeds the size of char
            const _RWSTD_PTRDIFF_T __len = __grpbeg ?
                __pcur - __grpbeg : __pcur - __pbuf - 1 - (_HexX != __hex_x);

            if (0 == __len) {
                // fatal error: thousands_sep characters must be separated
                // by groups of one or more digits
                __err |= _RW::__rw_failbit;                
                return __begin;
            }

            *__pgrp++ = char (__len < _UChar (_RWSTD_UCHAR_MAX) ? __len : -1);
            __grpbeg  = __pcur;
        }
        else if (!__dp) {

            const char __ch = __ctp.narrow (__wc, '\0');

            // get the digit value of the character; anything over
            // 35 is not a digit (43 is either the plus sign or
            // the minus sign, for efficiency); the value 99
            // indicates an invalid character
            const _UChar __digit = _RW::__rw_digit_map [_UChar (__ch)];

            // 22.2.2.1.2, p8: Stage 8 calls for widening of the sequence
            // of narrow digits ("0123456789abcdefABCDEFX+-") and looking
            // up the current wide character in the widened sequence, but
            // that may not possible unless the wide character type has
            // operator==() defined

            if (__subtype & _C_floating) {

                if (__digit < 10)
                    *__pcur++ = __ch;
                else if (__pcur == __pbuf + 1) {
                    // plus or minus only permitted (but not required)
                    // as the first character of the mantissa
                    if (__ld_sgn == __digit) {
                        // overwrite the default plus with the sign
                        __pcur [-1] = __ch;

                        // disable future recognition of the leading sign
                        __ld_sgn = 36;
                    }
                    else
                        break;
                }
                else {
                    if (/* 'E' or 'e' == __ch */
                        14 == _RW::__rw_digit_map [_UChar (__ch)]) {
                        const _RWSTD_PTRDIFF_T __diff = __pcur - __pbuf;

                        if (2 == __diff && '.' == __pcur [-1])
                            break;
                            
                        if (!__grpend)
                            __grpend = __pcur;
                        *__pcur++ = 'e';

                        if (__grpsz) {
                            const char __ts =
                                __ctp.narrow (__thousands_sep, '\0');
                            if (/* '+' or '-' == __ts */
                                _Sign == _RW::__rw_digit_map [_UChar (__ts)])
                                __grpsz = 0;
                        }
                    }
                    else if (   'e' == __pcur [-1]
                             && /* '+' or '-' == __ch */
                                _Sign == _RW::__rw_digit_map [_UChar (__ch)]) {
                        // '+' or '-' only permitted (but not required)
                        // as the first character after 'e' or 'E'

                        *__pcur++ = __ch;

                        // reached the exponent part of a floating point number
                        // switch to parsing a decimal integer
                        __base    = 10;
                        __subtype = _C_int;
                    }
                    else
                        break;  // invalid character terminates input
                }
            }
            else {
                if (__pcur == __pbuf + 1 && __digit == __ld_sgn) {
                    __pcur [-1] = __ch;   // overwrite the default '+'
                    __ld_sgn    = 36;     // disable leading sign
                }
                else if (16 == __base) {
                    if (__digit < 16)
                        *__pcur++ = __ch;
                    else if (   __digit == __hex_x
                             && __pcur == __pbuf + 2
                             && __pcur [-1] == '0') {
                        // don't append the 'x' part of the "0x" prefix
                        // and disable the recognition of any future 'x'
                        __hex_x = 36;
                    }
                    else
                        break;   // invalid character terminates input
                }
                else if (__base > 1) {
                    if (__digit < __base)
                        *__pcur++ = __ch;
                    else
                        break;   // invalid character terminates input
                }
                else if (0 == __base) {

                    // autodetect the base

                    if (__digit < 10) {
                        if (   __digit && __digit < 8
                            && *(__pcur - 1) == '0' && __pcur == __pbuf + 2)
                            __base = 8;
                        *__pcur++ = __ch;
                    }
                    else if (   __digit == __hex_x
                             && __pcur == __pbuf + 2
                             && __pcur [-1] == '0') {
                        __hex_x = 36;   // disable future 'x'
                        __base  = 16;   // set base-16
                        --__pcur;       // remove leading '0'
                    }
                    else
                        break;   // invalid character terminates input
                }
                else {
                    if (_RW::__rw_roman_inxs [_UChar (__ch)] <= 7)
                        *__pcur++ = __ch;
                    else if (__pcur == __pbuf + 1 && __digit < 10) {
                        *__pcur++ = __ch;
                        // if the first character is a digit
                        // switch to decimal (base-10) parsing
                        __base = 10;
                    }
                    else {
                        // invalid character terminates input
                        break;
                    }
                }
            }
        }
        else if (__subtype & _C_floating) {
            if (__grpend)
                break;

            __grpend  = __pcur;
            *__pcur++ = '.';
        }
        else
            break;
    }

    // add the length of the last group to the array of groups
    {
        // add the length of the current group to the array groups
        // store UCHAR_MAX if group length exceeds the size of char
        _RWSTD_PTRDIFF_T __len;

        if (__grpend) {

#if defined (_RWSTD_NO_STRTOLD) && defined (_RWSTD_NO_STRTOLD_IN_LIBC)

            // detect invalid formats in case strtold() is not
            // available (scanf() will not detect these)

            if (   'e' == __pcur [-1]
                || '-' == __pcur [-1] || '+' == __pcur [-1]) {
                __err |= _RW::__rw_failbit;
                return __begin;
            }

#endif   // _RWSTD_NO_STRTOLD && _RWSTD_NO_STRTOLD_IN_LIBC

        }
        else
            __grpend = __pcur;

        __len = __grpbeg ?
            __grpend - __grpbeg : __grpend - __pbuf - 1 - (_HexX != __hex_x);

        if (__grpbeg && 0 == __len) {
            // fatal error: according to the grammar in [locale.numpunct]
            // thousands_sep characters must be separated by groups of one
            // or more digits (i.e., valid input cannot start or end with
            // a thousands_sep); this needs to be clarified in the text
            __err |= _RW::__rw_failbit;
            return __begin;
        }

        *__pgrp++ = char (__len < _UChar (_RWSTD_UCHAR_MAX) ? __len : -1);
    }

    *__pgrp = '\0';
    *__pcur = '\0';

    // verify that the buffers haven't overflowed
    _RWSTD_ASSERT (__pgrp < __grpbuf + sizeof __grpbuf);
    _RWSTD_ASSERT (__pcur < __pbuf + __bufsize);

    // set the base determined above
    const unsigned __fl2 =
          __fl & ~_RWSTD_IOS_BASEFIELD
        & ~(   _RWSTD_STATIC_CAST (unsigned, _RWSTD_IOS_BASEMASK)
            << _RWSTD_IOS_BASEOFF)
        | __base << _RWSTD_IOS_BASEOFF;

    // 22.2.2.1.2, p11: Stage 3
    const int __errtmp =
        _RW::__rw_get_num (__pval, __pbuf, __type, __fl2,
                           __grpbuf, __pgrp - __grpbuf,
                           __grouping.data (), __grouping.size ());

    __err |= _RWSTD_IOSTATE (__errtmp);

    return __begin;
}


}   // namespace std
