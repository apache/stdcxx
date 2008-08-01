/***************************************************************************
 *
 * _num_put.cc - definition of std::num_put members
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <loc/_locale.h>   // for use_facet
#include <loc/_ctype.h>    // for ctype

_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_put_num (char**, _RWSTD_SIZE_T, unsigned, int, _RWSTD_STREAMSIZE,
              const void*, const char*);

_RWSTD_EXPORT extern const unsigned char
__rw_digit_map[];


template <class _OutputIter>
inline bool
__rw_iter_failed (const _OutputIter&) { return false; }

// const commented to resolve ambiguity on MSVC 7.0:
// error C2667: '__rw_iter_failed' : none of 2 overloads have a best conversion
template <class _CharT, class _Traits>
inline bool
__rw_iter_failed (/*const */_STD::ostreambuf_iterator<_CharT, _Traits> &__it)
{
    return __it.failed ();
}

}   // namespace __rw


_RWSTD_NAMESPACE (std) { 


template <class _CharT, class _OutputIter>
_RW::__rw_facet_id num_put<_CharT, _OutputIter>::id;


template <class _CharT, class _OutputIter /* = ostreambuf_iterator<_CharT> */>
/* virtual */ num_put<_CharT, _OutputIter>::~num_put ()
{
    // no-op
}


template <class _CharT, class _OutputIter /* = ostreambuf_iterator<_CharT> */>
typename num_put<_CharT, _OutputIter>::iter_type
num_put<_CharT, _OutputIter>::
_C_put (iter_type __it, ios_base &__flags, char_type __fill, int __type,
        const void *__pval) const
{
    const numpunct<char_type> &__np =
        _RWSTD_USE_FACET (numpunct<char_type>, __flags.getloc ());

    char __buf [_RWSTD_DBL_MAX_10_EXP];

    // will grow as necessary and may need to be deleted
    char *__pbuf = __buf;

    const string            __grouping = __np.grouping ();
    const char* const       __grp      = __grouping.c_str ();
    const _RWSTD_STREAMSIZE __prec     = __flags.precision ();

    const int __f = __flags.flags ();

    // format value, ignore C++ decimal_point and thousands_sep
    _RWSTD_SIZE_T __res =
        _RW::__rw_put_num (&__pbuf, sizeof __buf, __f, __type,
                           __prec, __pval, __grp);

    // for bool values and boolalpha bit set, retrieve the "truth" string
    basic_string<char_type, char_traits<char_type>, allocator<char_type> >
        __boolstr;

    if (!__res) {
        // avoid an IBM VAC++ 7.0 code generation bug that causes
        // local string objects to get incorrectly destroyed (see
        // bug #546)
        // __boolstr = __pval ? __np.truename () : __np.falsename ();

        if (__pval)
            __boolstr = __np.truename ();
        else
            __boolstr = __np.falsename ();

        __res = __boolstr.size ();
    }

    // number of fill chars to pad with
    streamsize __pad = __flags.width () - streamsize (__res);

#ifdef _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE

    // unconditionally reset width before inserting anything
    // in case the insertion causes an exception to be thrown
    __flags.width (0);

#endif   // _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE

    // adjustfield bits (left, internal, or the default right)
    const int __adj = __f & _RWSTD_IOS_ADJUSTFIELD;

    if (__pad > 0 && !(__adj & (_RWSTD_IOS_LEFT | _RWSTD_IOS_INTERNAL))) {

        // right justify if either no justification or
        // right justification specified in adjustfield

        for (; __pad--; ++__it)
            *__it = __fill;
    }

    const ctype<char_type> &__ctp =
        _RWSTD_USE_FACET (ctype<char_type>, __flags.getloc ());

    const char *__p = __pbuf;

    if (__pad > 0 && _RWSTD_IOS_INTERNAL == __adj) {

        typedef unsigned char _UChar;

        // internal padding only between the leading "0x"
        // prefix or the leading sign and the number

        if (43 /* '+' or '-' */ == _RW::__rw_digit_map [_UChar (*__p)])
            *__it++ = __ctp.widen (*__p++);
        else if (   '0' == *__p && __res > 2
                 && 33 == /* 'X' */ _RW::__rw_digit_map [_UChar (__p [1])]) {
            *__it++ = __ctp.widen (*__p++);
            *__it++ = __ctp.widen (*__p++);
        }

        // right justify if not internally padded
        for (; __pad--; ++__it)
            *__it = __fill;
    }

    if (__type & _RW::__rw_facet::_C_integral && !*__grp && __prec >= 0) {
        // optimized for integer output, no grouping
        for (const char *__end = __pbuf + __res; __p != __end; ++__p, ++__it)
            *__it = __ctp.widen (*__p);
    }
    else if (_RW::__rw_facet::_C_bool == __type) {
        // boolalpha output
        for (_RWSTD_SIZE_T __i = 0; __i != __res; ++__i, ++__it)
            *__it = __boolstr [__i];
    }
    else {
        // all other numeric output
        for (const char *__end = __pbuf + __res; __p != __end; ++__p, ++__it) {
            switch (*__p) {
                // replace either the period or the comma with the
                // decimal point in case a setlocale() call made by
                // the program changed the default '.' to ','
            case '.':
            case ',': *__it = __np.decimal_point (); break;
            case ';': *__it = __np.thousands_sep (); break;
            default: *__it = __ctp.widen (*__p); break;
            }
        }
    }

    // delete if the buffer was dynamically allocated
    if (__pbuf != __buf)
        delete[] __pbuf;

    // left justify if necessary
    for (; __pad > 0; ++__it, --__pad)
        *__it = __fill;

#ifndef _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE

    // reset width only if the insertion has been successful
    // (i.e., no exception and the iterator has not failed)
    if (!_RW::__rw_iter_failed (__it))
        __flags.width (0);

#endif   // _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE

    return __it;
}


}   // namespace std
