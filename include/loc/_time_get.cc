/***************************************************************************
 *
 * _time_get.cc - definition of std::time_get members
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
 * Copyright 1994-2006 Rogue Wave Software, Inc.
 * 
 **************************************************************************/


#include <streambuf>
#include <loc/_ctype.h>
#include <loc/_punct.h>
#include <loc/_locale.h>


_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_get_timepunct (const __rw_facet *pfacet,
                    int data [4], _RWSTD_C::tm *tmb, int **pmem,
                    const void *names[], _RWSTD_SIZE_T sizes[]);

}   // namespace __rw


_RWSTD_NAMESPACE (std) { 


template <class _CharT, class _InputIter>
_RW::__rw_facet_id time_get<_CharT, _InputIter>::id;


// outlined to avoid generating a vtable in each translation unit
// that uses the class
template <class _CharT, class _InputIter>
/* virtual */ time_get<_CharT, _InputIter>::
~time_get ()
{
    // no-op
}


template <class _CharT, class _InputIter>
typename time_get<_CharT, _InputIter>::iter_type
time_get<_CharT, _InputIter>::
get (iter_type __it, iter_type __end, ios_base &__fl, _RWSTD_IOSTATE &__err,
     tm *__tmb, const char_type *__pat, const char_type *__pat_end) const
{
    _RWSTD_ASSERT (0 != __pat);
    _RWSTD_ASSERT (__pat <= __pat_end);

    const ctype<char_type> &__ctp =
        _RWSTD_USE_FACET (ctype<char_type>, __fl.getloc ());

    // extension: `tmb' may be 0
    tm __tmp = __tmb ? *__tmb : tm ();

    _RWSTD_IOSTATE __errtmp = _RW::__rw_goodbit;

    char __fmt [] = { '\0', '\0' };

    for (; __pat != __pat_end; ++__pat) {

        if (__errtmp) {
            // failed to match the whole pattern
            __errtmp |= _RW::__rw_failbit;
            break;
        }

        const char __ch = __ctp.narrow (*__pat, '\0');

        if (__fmt [0]) {
            if ('E' == __ch || 'O' == __ch)
                __fmt [1] = __ch;
            else if ('.' == __ch || __ch >= '0' && __ch <= '9') {
                // FIXME: handle the "%.[0-9]*" HP-UX extension
            }
            else {
                __it = do_get (__it, __end, __fl, __errtmp, &__tmp, __ch,
                               __fmt [1]);
                __fmt [0] = __fmt [1] = '\0';   // reset
            }
        }
        else if ('%' == __ch) {
            __fmt [0] = __ch;
        }
        else if (__ctp.is (ctype_base::space, *__pat)) {
            // skip all whitespace characters
            while (__it != __end && __ctp.is (ctype_base::space, *__it))
                ++__it;
        }
        else {
            if (__it != __end && __ch == __ctp.narrow (*__it, '\0'))
                ++__it;
            else {
                // mismatch between pattern and input
                __errtmp |= _RW::__rw_failbit;
                break;
            }
        }
    }

    // store value only on success
    if (!(__errtmp & ~_RW::__rw_eofbit) && __tmb)
        *__tmb = __tmp;

    __err |= __errtmp;

    return __it;
}


template <class _CharT, class _InputIter>
/* virtual */ typename time_get<_CharT, _InputIter>::iter_type
time_get<_CharT, _InputIter>::
do_get (iter_type __it, iter_type __end, ios_base &__fl,
        _RWSTD_IOSTATE &__err, tm *__tmb,
        char __fmt, char __modifier /* = '\0' */) const
{
    // enough pointers for 100 alternative numeric symbols and their sizes
    const char_type* __names [100];
    _RWSTD_SIZE_T    __sizes [sizeof __names / sizeof *__names];

    const void** __pv =
        _RWSTD_REINTERPRET_CAST (const void**, __names);

    tm __tmp = __tmb ? *__tmb : tm ();

    int *__pmem = 0;   // pointer to std::tm member

    _RWSTD_SIZE_T __dup;   // number of allowed duplicates

    const ctype<char_type> &__ctp =
        _RWSTD_USE_FACET (ctype<char_type>, __fl.getloc ());

    switch (__fmt) {

    case 'A': case 'a': case 'B': case 'b': case 'h':
        // names of days and months can have duplicates (i.e.,
        // it can be either a full name aor an abbreviation)
        __dup = 2;
        break;

    case 'n': case 't': {   // any whitespace
        
        for ( ; ; ++__it) {

            if (__it == __end) {
                __err |= _RW::__rw_eofbit;
                break;
            }

            if (!__ctp.is (ctype_base::space, *__it))
                break;
        }

        return __it;
    }

    default: __dup = 1;
    }

    int __data [4] = {
        // array of input data for __rw_get_timepunct will on output
        // contain data needed to interpret values parsed below
        __fmt, __modifier, sizeof (char_type) > sizeof (char), 0
    };

    // get the number of punctuator (or format) string(s) and their values
    const _RWSTD_SIZE_T __cnt =
        _RW::__rw_get_timepunct (this, __data, &__tmp,
                                 &__pmem, __pv, __sizes);

    if (1 == __cnt) {
        _RWSTD_ASSERT (0 != __names [0]);

        // single punctuator string stored in `names [0]'
        // e.g., "%m/%d/%y" for the format of "%D" (`fmt' == 'D')
        return get (__it, __end, __fl, __err, __tmb,
                    __names [0], __names [0] + __sizes [0]);
    }

    _RWSTD_IOSTATE __errtmp = _RW::__rw_goodbit;

    if (__cnt > 1) {

        // parse string on input

        // set `inx' to 0 to allow numeric input, SIZE_MAX otherwise
        _RWSTD_SIZE_T __inx = 'O' == __modifier ? 0 : _RWSTD_SIZE_MAX;

        // on function input, `interr' contains the maximum number
        // of duplicates; on output, it contains an iostate value
        // and `inx' contains the index of the parsed name within
        // the `names' array
        int __interr = int (__dup);

        __it = _RW::__rw_match_name (__it, __end, __names, __sizes,
                                     __cnt, __inx, __interr, &__fl);

        __errtmp = _RWSTD_IOSTATE (__interr);

        if (__inx == _RWSTD_SIZE_MAX) {

            // if no name matches, and the first name is the empty
            // (i.e., alt_digits 0 is not defined), try to match
            // and extract all zeros
            if (   !__errtmp && 0 == __sizes [0]
                && __it != __end && '0' == __ctp.narrow (*__it, '\0')) {

                while ('0' == __ctp.narrow (*__it, '\0')) {

                    if (++__it == __end) {
                        __errtmp |= _RW::__rw_eofbit;
                        break;
                    }
                }
            }
            else {
                __errtmp |= _RW::__rw_failbit;
            }
        }
        else if ('p' == __fmt) {
            if (__pmem) {
                if (!__inx && 11 == *__pmem)
                    *__pmem = 0;
                else if (*__pmem < 12) {
                    if (!__inx || 11 != *__pmem)
                        *__pmem += int (__inx * 12) + 1;
                    else
                        *__pmem += 1;
                }
                else {
                    // invalid data
                    __errtmp |= _RW::__rw_failbit;
                }
            }
            else {
                // invalid (or unimplemented) format
                __errtmp |= _RW::__rw_failbit;
            }
        }
        else if (__pmem) {
            _RWSTD_ASSERT (0 != __pmem);

            if (__inx == __cnt && !__sizes [0] || __inx >= 2 * __cnt)
                *__pmem = int (__inx - __cnt);
            else if (__inx < __cnt)
                *__pmem = int (__inx % (__cnt / __dup));
            else
                __errtmp |= _RW::__rw_failbit;

            const int __adj = __data [0];   // value to increment result by
            const int __fac = __data [1];   // value to multiply result by

            *__pmem = *__pmem * __fac + __adj;

            const int __lob = __data [2];   // lower bound on valid input
            const int __hib = __data [3];   // upper bound on valid input

            // validate input value
            if (*__pmem < __lob || *__pmem > __hib)
                __errtmp |= _RW::__rw_failbit;
        }
        else {
            // invalid (or unimplemented) format
            __errtmp |= _RW::__rw_failbit;
        }
    }
    else if (__pmem) {

        // parse numeric input

        _RWSTD_SIZE_T __nread = 0;

        int __mem = 0;

        for ( ; ; ++__it, ++__nread) {
            if (__it == __end) {
                __err |= _RW::__rw_eofbit;
                break;
            }

            const char_type __c = *__it;
            if (!__ctp.is (ctype_base::digit, __c))
                break;

            const char __ch = __ctp.narrow (__c, '\0');
            if (__ch < '0' || __ch > '9')
                break;

            __mem = 10 * __mem + __ch - '0';
        }

        if (__nread) {

            const int __adj = __data [0];   // value to increment result by
            const int __fac = __data [1];   // value to multiply result by

            int __lob = __data [2];   // lower bound on valid input
            int __hib = __data [3];   // upper bound on valid input

            if ('y' == __fmt) {

                // IEEE Std 1003.1-2001: When a century is not otherwise
                // specified, values in the range [69,99] shall refer to
                // years 1969 to 1999 inclusive, and values in the range
                // [00,68] shall refer to years 2000 to 2068 inclusive.

                if (__mem > 99) {
                    __lob  = -1900;
                    __hib  = _RWSTD_INT_MAX - 1900;
                    __mem -= 1900;
                }
                else {
                    __lob =  69;
                    __hib = 168;

                    if (__mem < 69)
                        __mem += 100;
                }
            }

            *__pmem = __mem * __fac + __adj;

            // validate input value
            if (*__pmem < __lob || *__pmem > __hib)
                __errtmp |= _RW::__rw_failbit;
        }
        else {
            // invalid data
            __errtmp |= _RW::__rw_failbit;
        }
    }
    else {
        // invalid (or unimplemented) format
        __errtmp |= _RW::__rw_failbit;
    }

    if (!(__errtmp & ~_RW::__rw_eofbit) && __tmb)
        *__tmb = __tmp;

    __err |= __errtmp;

    return __it;
}


// outlined to avoid generating a vtable in each translation unit
// that uses the class
template <class _CharT, class _InputIter>
/* virtual */ time_get_byname<_CharT, _InputIter>::
~time_get_byname ()
{
    // no-op
}


}   // namespace std
