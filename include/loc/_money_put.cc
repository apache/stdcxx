/***************************************************************************
 *
 * _money_put.cc - definition of std::num_put members
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <streambuf>
#include <loc/_moneypunct.h>
#include <loc/_ctype.h>


_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_put_num (char**, _RWSTD_SIZE_T, unsigned, int, _RWSTD_STREAMSIZE,
              const void*, const char*);

_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_put_groups (char **, _RWSTD_SIZE_T, _RWSTD_SIZE_T,
                 const char*, const char*);

}   // namespace __rw


_RWSTD_NAMESPACE (std) { 


template <class _CharT, class _OutputIter>
_RW::__rw_facet_id money_put<_CharT, _OutputIter>::id;


// outlined to avoid generating a vtable in each translation unit
// that uses the class
template <class _CharT, class _OutputIter>
/* virtual */ money_put<_CharT, _OutputIter>::
~money_put () /* nothrow */
{
    // no-op
}


template <class _CharT, class _OutputIter>
/* private */ typename money_put<_CharT, _OutputIter>::iter_type
money_put<_CharT, _OutputIter>::
_C_put (iter_type __it, int __opts, ios_base &__flags, char_type __fill,
        const char_type *__s, _RWSTD_SIZE_T __n, int __fd,
        const char *__groups, _RWSTD_SIZE_T __ngroups) const
{
    // static const arrays of pointers to moneypunct<charT, Intl> members
    // to avoid "if (intl)" conditionals when retrieving punct values

    const bool __intl = 0 != (__opts & _C_intl);
    const bool __num  = 0 != (__opts & _C_ldbl);

    typedef moneypunct<_CharT, false> _Punct0;
    typedef moneypunct<_CharT, true>  _Punct1;

    const _Punct0* __pun0;
    const _Punct1* __pun1;

#ifdef __GNUG__
    // silence bogus gcc -Wuninitialized warning:
    //   object may be used uninitialized in this function
    // while avoiding bogus HP aCC/cadvise warning 20200:
    //   potential null pointer dereference 
    __pun0 = 0;
    __pun1 = 0;
#endif   // gcc

    if (__intl) {
        __pun1 = &_RWSTD_USE_FACET (_Punct1, __flags.getloc ());
    }
    else {
        __pun0 = &_RWSTD_USE_FACET (_Punct0, __flags.getloc ());
    }

    const ctype<_CharT> &__ctp =
        _RWSTD_USE_FACET (ctype<_CharT>,__flags.getloc ());

    const char __signchar = __ctp.narrow (*__s, '\0');

    money_base::pattern __fmat;   // negative or positive format
    string_type         __sign;   // negative or positive sign

    if ('-' == __signchar) {
        if (__intl) {
            __fmat = __pun1->neg_format ();
            __sign = __pun1->negative_sign ();
        }
        else {
            __fmat = __pun0->neg_format ();
            __sign = __pun0->negative_sign ();
        }
        ++__s;
        --__n;
    }
    else {
        if (__intl) {
            __fmat = __pun1->pos_format ();
            __sign = __pun1->positive_sign ();
        }
        else {
            __fmat = __pun0->pos_format ();
            __sign = __pun0->positive_sign ();
        }
        if ('+' == __signchar) {
            ++__s;
            --__n;
        }
    }

    // optional currency sumbol
    const string_type __curr = (__flags.flags () & _RWSTD_IOS_SHOWBASE) ?
          __intl ? __pun1->curr_symbol () : __pun0->curr_symbol ()
        : string_type ();

    // size of fractional and integral parts, respectively, to output
    long __dint = long (__n - __fd);

    // must have at least one group (even if there are no thousands_sep)
    _RWSTD_ASSERT (__ngroups > 0);

    // adjust padding by the sizes of each component, including the
    // number of thousands separators to be inserted
    long __pad = long (
          __flags.width () - __curr.size () - __sign.size ()
        - !__dint - !!__fd - (__dint > 0 ? __dint : -__dint) - __fd
        - (   money_base::space == __fmat.field [1]
           || money_base::space == __fmat.field [2])
        - (__ngroups - 1));

    const int __adj = __flags.flags () & _RWSTD_IOS_ADJUSTFIELD;

    if (__adj != _RWSTD_IOS_INTERNAL && __adj != _RWSTD_IOS_LEFT) {

        for (; __pad > 0; --__pad, ++__it)
            *__it = __fill;
    }

    for (_RWSTD_SIZE_T __i = 0, __j; __i != sizeof __fmat; ++__i) {

        switch (__fmat.field [__i]) {

        case money_base::symbol:   // exactly one occurrence required
            for (__j = 0; __j != __curr.size (); ++__j, ++__it)
                *__it = __curr [__j];
            break;

        case money_base::sign:   // exactly one occurrence required
            if (__sign.size ()) {
                // first character of sign only
                *__it = __sign [0];
                ++__it;
            }
            break;

        case money_base::space:   // optional (required if none not present)
            // 22.2.6.3, p1: the value space, if present,
            //               is neither first nor last.

            _RWSTD_REQUIRES (0 != __i && sizeof __fmat != __i - 1,
                             (_RWSTD_ERROR_RUNTIME_ERROR,
                              _RWSTD_FUNC ("std::num_put<>::do_put()")));

            // an ordinary space, not `fill', is required here
            // will precede any required padding
            *__it = __ctp.widen (' ');
            ++__it;

        case money_base::none:   // optional (required if space not present)
            // 22.2.6.3, p1: the value none, if present, is not first

            _RWSTD_REQUIRES (0 != __i,
                             (_RWSTD_ERROR_RUNTIME_ERROR,
                              _RWSTD_FUNC ("std::num_put<>::do_put()")));

            if (   (__adj & _RWSTD_IOS_ADJUSTFIELD) == _RWSTD_IOS_INTERNAL
                && sizeof __fmat != __i - 1)
                for (; __pad > 0; --__pad, ++__it)
                    *__it = __fill;

            break;

        case money_base::value: {   // exactly one occurrence required

            if (__dint < 0 || 0 == __dint && __fd > 0) {
                const char_type __zero = __ctp.widen ('0');

                // insert leading zero and decimal point
                *__it = __zero;
                ++__it;

                *__it = __intl ?
                    __pun1->decimal_point () : __pun0->decimal_point ();
                ++__it;

                // insert leading fractional zeros
                for (; __dint++; ++__it)
                    *__it = __zero;

                // prevent insertion of the decimal point below
                __dint = -1;
            }

            _RWSTD_SIZE_T __grplen = 0;

            for (const char_type *__e = __s + __n; __s != __e; ++__it, ++__s) {

                if (!__num && !__ctp.is (ctype_base::digit, *__s))
                    break;

                typedef unsigned char _UChar;

                if (0 < __dint) {

                    _RWSTD_ASSERT (0 != __groups);

                    if (*__groups && __grplen == _UChar (*__groups)) {
                        *__it = __intl ? __pun1->thousands_sep ()
                                       : __pun0->thousands_sep ();
                        __grplen = 0;
                        ++__groups;
                        ++__it;
                    }
                }
                else if (0 == __dint) {
                    *__it = __intl ? __pun1->decimal_point ()
                                   : __pun0->decimal_point ();
                    ++__it;
                }

                --__dint;
                ++__grplen;

                switch (*__s) {
                    // replace either the period or the comma with the
                    // decimal point in case a setlocale() call made by
                    // the program changed the default '.' to ','
                case '.':
                case ',':
                    *__it = __intl ? __pun1->decimal_point ()
                                   : __pun0->decimal_point ();
                    break;

                case ';':
                    *__it = __intl ? __pun1->thousands_sep ()
                                   : __pun0->thousands_sep ();
                    break;

                default:  *__it = *__s;
                }
            }

            break;
        }
        }
    }

    // output the remaining characters of sign (if any)
    for (_RWSTD_SIZE_T __k = 1; __k < __sign.size (); ++__k, ++__it)
        *__it = __sign [__k];

    // left ajustment
    for (; __pad > 0; --__pad, ++__it)
        *__it = __fill;

    // 22.2.6.2.2, p1: reset width()
    __flags.width (0);

    return __it;
}


template <class _CharT, class _OutputIter>
/* virtual */ typename money_put<_CharT, _OutputIter>::iter_type
money_put<_CharT, _OutputIter>::
do_put (iter_type __i, bool __intl, ios_base &__flags, char_type __fill,
        long double __val) const
{
    int __fd;
    string __grouping;

    if (__intl) {
        typedef moneypunct<_CharT, true> _Punct;
        const _Punct &__pun = _RWSTD_USE_FACET (_Punct, __flags.getloc ());

        __fd       = __pun.frac_digits ();
        __grouping = __pun.grouping ();
    }
    else {
        typedef moneypunct<_CharT, false> _Punct;
        const _Punct &__pun = _RWSTD_USE_FACET (_Punct, __flags.getloc ());

        __fd       = __pun.frac_digits ();
        __grouping = __pun.grouping ();
    }

    char       __buf [304];
    char_type __wbuf [sizeof __buf];

    char *__pbuf = __buf;

    // format a floating point number in fixed precision into narrow buffer
    // will insert thousands_sep placeholders (';') accroding to grouping
    const _RWSTD_SIZE_T __n =
        _RW::__rw_put_num (&__pbuf, sizeof __buf, _RWSTD_IOS_FIXED,
                           _C_ldouble | _C_ptr,
                           _RWSTD_STATIC_CAST (_RWSTD_STREAMSIZE, -__fd),
                           &__val, __grouping.c_str ());

    // widen narrow buffer (necessary even if char_type == char)
    const ctype<_CharT> &__ctp =
        _RWSTD_USE_FACET (ctype<_CharT>, __flags.getloc ());

    __ctp.widen (__buf, __buf + __n, __wbuf);

    // write the widened buffer out, replacing any commas
    // with the actual thousands_sep punct character
    return _C_put (__i, (__intl ? _C_intl : 0) | _C_ldbl,
                   __flags, __fill, __wbuf, __n, 0, "", 1);
}


template <class _CharT, class _OutputIter>
/* virtual */ typename money_put<_CharT, _OutputIter>::iter_type
money_put<_CharT, _OutputIter>::
do_put (iter_type __i, bool __intl, ios_base &__flags, char_type __fill,
        const string_type &__str) const
{
    // fractional part does not undergo grouping and will be removed
    int __fd;

    string __grouping;

    if (__intl) {
        typedef moneypunct<_CharT, true> _Punct;

        const _Punct &__pun = _RWSTD_USE_FACET (_Punct, __flags.getloc ());

        __fd       = __pun.frac_digits ();
        __grouping = __pun.grouping ();
    }
    else {
        typedef moneypunct<_CharT, false> _Punct;

        const _Punct &__pun = _RWSTD_USE_FACET (_Punct, __flags.getloc ());

        __fd       = __pun.frac_digits ();
        __grouping = __pun.grouping ();
    }

    _RWSTD_SIZE_T  __ngroups = 1;          // always at least one group
    _RWSTD_SIZE_T  __strdigs = 0;          // number of digits in `str'

    typedef string::traits_type                   _Traits;
    typedef typename string_type::const_iterator _StringIter;

    char        __buf [304];            // buffer for groups
    static char __nul    = '\0';        // `groups' must be non-0
    char       *__groups = __grouping.size () ? __buf : &__nul;

    const ctype<_CharT> &__ctp =
        _RWSTD_USE_FACET (ctype<_CharT>, __flags.getloc ());

    // narrow digits into a temporary buffer to determine grouping
    for (_StringIter __it = __str.begin (); __it != __str.end (); ++__it) {
        if (__ctp.is (ctype_base::digit, *__it)) {
            if (__groups != &__nul)
                _Traits::assign (*__groups++, __ctp.narrow (*__it, '\0'));
        }
        else if (__str.begin () != __it || '-' != __ctp.narrow (*__it, '\0'))
            break;
        ++__strdigs;
    }

    if (__groups != &__nul && __fd >= 0 && __fd < __groups - __buf) {
        __groups -= __fd;

        // compute the size of each group relative to the formatted number
        // e.g., with `str' of "9876543210" and `grouping' of "\1\2\3",
        // `groups' will point at "\1\3\3\2\1" since `str' will be
        // formatted as "9,876,543,21,0" with thousands_sep's inserted

        char *__pbuf = __buf;

        *__pbuf = '\0';

        __ngroups =
            _RW::__rw_put_groups (&__pbuf, __groups - __pbuf, sizeof __buf,
                                  0, __grouping.c_str ());
        __pbuf [__ngroups] = '\0';
        __groups           = __pbuf;
    }

    return _C_put (__i, __intl ? _C_intl : 0,
                   __flags, __fill, __str.c_str (), __strdigs,
                   __fd, __groups, __ngroups);
}


}   // namespace std
