/***************************************************************************
 *
 * _money_get.cc - definition of std::money_get members
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

#include <streambuf>
#include <loc/_ctype.h>
#include <loc/_moneypunct.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT int
__rw_get_num (void*, const char*, int, int,
              const char*, _RWSTD_SIZE_T, const char*, _RWSTD_SIZE_T);

_RWSTD_EXPORT int
__rw_check_grouping (const char*, _RWSTD_SIZE_T,
                     const char*, _RWSTD_SIZE_T);

}   // namespace __rw


_RWSTD_NAMESPACE (std) { 


template <class _CharT, class _InputIter>
_RW::__rw_facet_id money_get<_CharT, _InputIter>::id;


template <class _CharT, class _InputIter>
typename money_get<_CharT, _InputIter>::iter_type
money_get<_CharT, _InputIter>::
_C_get (iter_type __it, iter_type __end, bool __intl, ios_base &__flags,
        _RWSTD_IOSTATE &__err, void *__pval, string_type *__pstr) const
{
    typedef moneypunct<_CharT, false> _Punct0;
    typedef moneypunct<_CharT, true> _Punct1;

    const _Punct0 &__pun = __intl ? 
          _RWSTD_REINTERPRET_CAST (const _Punct0&,
                                   _RWSTD_USE_FACET (_Punct1,
                                                     __flags.getloc ()))
        : _RWSTD_USE_FACET (_Punct0, __flags.getloc ());

    // retrieve positive and negative sign, currency symbol,
    // the grouping string and the pattern format
    const string_type __ps = __pun.positive_sign ();
    const string_type __ns = __pun.negative_sign ();
    const string_type __cs = __pun.curr_symbol ();
    const string      __gr = __pun.grouping ();
    const money_base::pattern __pat = __pun.neg_format ();

    _RWSTD_IOSTATE __ebits = _RW::__rw_goodbit;

    bool __needws = false;   // whitespace required?
    bool __seendp = false;   // seen decimal point in input

    const ctype<_CharT> &__ctp = 
        _RWSTD_USE_FACET (ctype<_CharT>, __flags.getloc ());

    char __buf [304];
    char *__pcur = __buf;

    typedef typename string_type::traits_type _Traits;

    char __grpbuf [sizeof __buf];    // holds sizes of discovered groups
    char       *__pgrp = __grpbuf;   // current group
    const char *__grpstart = 0;      // the start of the last group
    const char *__grpend   = 0;      // the end of the last group


    int __sign = 0;    // the sign of the result if detected (-1, 0, or +1)

    // buffer must always start with a sign (__rw_get_num requirement)
    // use a '+' and overwrite it with a '-' if necessary
    *__pcur++ = '+';

    const int __fl = __flags.flags ();

    typedef _RWSTD_SIZE_T _SizeT;

    for (_SizeT __i = 0; !__ebits && __i != sizeof __pat.field; ++__i) {

        switch (__pat.field [__i]) {

        case /* '\1' */ money_base::space:
            __needws = true;
            // fall through...

        case /* '\0' */ money_base::none: {

            // optional or even required whitespace is consumed only if
            // other characters may be required to complete the format
            if (   sizeof __pat.field == __i + 1
                || 2 == __i && money_base::symbol == __pat.field [3]
                && (!(__fl & _RW::__rw_showbase) || 0 == __cs.size ())
                && !(   __sign < 0 && __ns.size () > 1
                     || __sign > 0 && __ps.size () > 1)
                || 1 == __i && money_base::value == __pat.field [0]
                && 0 == __ns.size () && 0 == __ps.size ()
                && (0 == __cs.size () || !(__fl & _RW::__rw_showbase)))
                break;

            _SizeT __nc = 0;

            while (__it != __end && __ctp.is (ctype_base::space, *__it)) {
                ++__it;
                ++__nc;
            }

            if (__needws && !__nc)
                __ebits |= _RW::__rw_failbit;
            break;
        }

        case /* '\2' */ money_base::symbol: {

            // if optional, currency symbol is not extracted unless
            // it is consumed only if it is followed by characters
            // required to complete the pattern (see example in
            // 22.2.6.1.2, p3)
            if (   __fl & _RW::__rw_showbase
                || __i < 2
                || 2 == __i && (money_base::none != __pat.field [3])
                || __sign < 0 && __ns.size () > 1
                || __sign > 0 && __ps.size () > 1) {

                for (_SizeT __nc = 0; __nc != __cs.size ();
                     ++__nc, ++__it) {
                    if (__it == __end || !_Traits::eq (*__it, __cs [__nc])) {

                        // 22.2.6.1.2, p2: unless showbase is set,
                        //                 curr_symbol is optional
                        if (__nc || __fl & _RW::__rw_showbase)
                            __ebits |= _RW::__rw_failbit;
                        break;
                    }
                }
            }
            break;
        }

        case /* '\3' */ money_base::sign: {

            if (__it == __end) {
                if (__ps.size () && __ns.size ())
                    __ebits |= _RW::__rw_failbit;
                else
                    __sign = __ps.empty () - 1;
                break;
            }

            const char_type __c = *__it;

            // 22.2.6.1.2, p3: if the first character of positive
            // and negative sign is the same, the result is positive

            if (__ps.size () && _Traits::eq (__c, __ps [0])) {
                __sign = 1;
                ++__it;
            }
            else if (__ns.size () && _Traits::eq (__c, __ns [0])) {
                __sign = -1;
                ++__it;
            }

            break;
        }

        case /* '\4' */ money_base::value: {

            const char_type __ts =
                __gr.size () && __gr [0] && __gr [0] != _RWSTD_CHAR_MAX ?
                __pun.thousands_sep () : __ctp.widen ('0');

            int __fd = __pun.frac_digits ();

            for (; __it != __end; ++__it) {

                // read and narrow a character (note that narrow() may
                // yield the same narrow char for more than one wide
                // character; e.g., there may be two sets of digits)
                const char_type __c  = *__it;
                const char      __ch = __ctp.narrow (__c, '\0');

                if (__ch >= '0' && __ch <= '9') {
                    if (!__seendp || __fd-- > 0)
                        *__pcur++ = __ch;
                }
                else if (   !__seendp && __fd > 0
                         && _Traits::eq (__c, __pun.decimal_point ())) {
                    __grpend = __pcur;
                    __seendp = true;
                }
                else if (!__seendp && _Traits::eq (__c, __ts)) {
                    // add the length of the current group to the array groups
                    // store UCHAR_MAX if group length exceeds the size of char
                    _RWSTD_PTRDIFF_T __len;

                    if (__grpstart)
                        __len = __pcur - __grpstart;
                    else {
                        __grpstart = __pcur;
                        __len      = __pcur - __buf - 1;
                    }

                    typedef unsigned char _UChar;

                    *__pgrp++  = char (__len < _UChar (-1) ? __len : -1);
                    __grpstart = __pcur;
                }
                else
                    break;
            }

            if (__pcur - __buf > 1) {
                // append zeros to a non-empty string of digits
                // up to the number of frac_digits
                while (__fd-- > 0)
                    *__pcur++ = '0';
            }

            *__pcur = '\0';
            break;
        }

        default:
            __ebits = _RW::__rw_failbit;
            break;

        }
    }

    if (!(__ebits & _RW::__rw_failbit)) {

        if (__buf [1]) {

            // process the remainder of a multicharacter sign
            const _SizeT __sizes [] = {
                __ps.size () ? __ps.size () -1 : 0,
                __ns.size () ? __ns.size () -1 : 0
            };

            const char_type* const __names [] = {
                __ps.data () + !!__sizes [0],
                __ns.data () + !!__sizes [1]
            };

            if (__sign && (__sizes [0] || __sizes [1])) {

                // if the first character of a multi-character sign
                // has been seen, try to extract the rest of the sign

                _SizeT __inx = 0;

                int __errtmp = 1;   // no duplicates allowed

                __it = _RW::__rw_match_name (__it, __end, __names, __sizes,
                                             sizeof __names / sizeof *__names,
                                             __inx, __errtmp, 0);

                if (1 < __inx && __sizes [__sign < 0]) {

                    // failed to match the remainder of the sign to
                    // the input  and the first character of the sign
                    // does not form the complete (positive or negative)
                    // sign
                    __ebits = _RW::__rw_failbit;
                    __buf [1]  = '\0';
                }
                else if (!_Traits::eq (*__ps.data (), *__ns.data ())) {

                    // if the first extracted character of the sign string
                    // forms the complete sign whose first character doesn't
                    // form an initial substring of the other sign string
                    // the resulting sign is determined by the one-character
                    // sign string
                    if (1 < __inx)
                        __inx = 0 < __sizes [__sign < 0];

                    // if both signs begin with the same character,
                    // the result is positive (22.2.6.1.2, p3)
                    *__buf = __inx ? '-' : '+';
                    __sign = -int (__inx);
                }
            }
            else if (__sign < 0) {
                *__buf = '-';
            }

            if (__pstr && !(__ebits & _RW::__rw_failbit)) {
                // skip over the leading sign and any redundant zeros after it
                const char *__start = __buf + 1;
                for (; '0' == *__start && '0' == __start [1]; ++__start);

                // invert the sign if negative
                __sign = __sign < 0;

                // widen narrow digits optionally preceded by the minus sign
                // into the basic_string object as required by 22.2.6.1.2, p1
                __pstr->resize ((__pcur - __start) + _SizeT (__sign));

                if (__sign)
                    _Traits::assign ((*__pstr)[0], __ctp.widen ('-'));

                __ctp.widen (__start, __pcur, &(*__pstr)[_SizeT (__sign)]);
            }

            const char *__grs = "";
            _SizeT      __grn = 0;

            // 22.2.6.1.2, p1: thousands separators are optional

            // add the length of the last group to the array of groups
            // store UCHAR_MAX if group length exceeds the size of char
            if (__grpstart) {
                const _RWSTD_PTRDIFF_T __len =
                    (__grpend ? __grpend : __pcur) - __grpstart;

                typedef unsigned char _UChar;

                *__pgrp++ = char (__len < _UChar (-1) ? __len : -1);
                *__pgrp   = '\0';
                __grs     = __gr.data ();
                __grn     = __gr.size ();
            }

            if (__pval) {
                // produce a number from the extracted string of digits
                // and (optionally) check grouping

                const int __errtmp =
                    _RW::__rw_get_num (__pval, __buf, _C_ldouble, 0,
                                       __grpbuf, __pgrp - __grpbuf,
                                       __grs, __grn);

                __ebits |= _RWSTD_IOSTATE (__errtmp);
            }
            else if (0 > _RW::__rw_check_grouping (__grpbuf, __pgrp - __grpbuf,
                                                   __grs, __grn))
                __ebits |= _RW::__rw_failbit;
        }
        else
            __ebits |= _RW::__rw_failbit;
    }

    if (__it == __end)
        __ebits |= _RW::__rw_eofbit;

    __err |= __ebits;

    return __it;
}


}   // namespace std
