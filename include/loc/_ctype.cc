/***************************************************************************
 *
 * _ctype .cc - definition of std::ctype members
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/


_RWSTD_NAMESPACE (std) { 


#ifndef _RWSTD_NO_EXT_CTYPE_PRIMARY

template <class _CharT>
_RW::__rw_facet_id ctype<_CharT>::id;


template <class _CharT> 
/* virtual */ const typename ctype<_CharT>::char_type*
ctype<_CharT>::
do_is (const char_type *__lo, const char_type *__hi, mask *__m) const
{
    _RWSTD_ASSERT (__lo <= __hi);
    _RWSTD_ASSERT (!__lo || __m);

    for (; __lo < __hi; ++__lo, ++__m)
        *__m = mask ();

    return __hi;
}


template <class _CharT> 
/* virtual */ const typename ctype<_CharT>::char_type*
ctype<_CharT>::
do_scan_is (mask __m, const char_type *__lo, const char_type *__hi) const
{
    _RWSTD_ASSERT (__lo <= __hi);

    for (; __lo < __hi; ++__lo) {
        if (is (__m, *__lo))
            return __lo;
    }

    return __hi;
}


template <class _CharT> 
/* virtual */ const typename ctype<_CharT>::char_type*
ctype<_CharT>::
do_scan_not (mask __m, const char_type *__lo, const char_type *__hi) const
{
    _RWSTD_ASSERT (__lo <= __hi);

    for (; __lo < __hi; ++__lo) {
        if (!is (__m, *__lo))
            return __lo;
    }

    return __hi;
}


template <class _CharT> 
/* virtual */ const typename ctype<_CharT>::char_type*
ctype<_CharT>::
do_toupper (char_type *__lo, const char_type *__hi) const
{
    _RWSTD_ASSERT (__lo <= __hi);

    for (; __lo < __hi; ++__lo) {
        *__lo = (toupper)(*__lo);
    }

    return __hi;
}


template <class _CharT> 
/* virtual */ const typename ctype<_CharT>::char_type*
ctype<_CharT>::
do_tolower (char_type *__lo, const char_type *__hi) const
{
    _RWSTD_ASSERT (__lo <= __hi);

    for (; __lo < __hi; ++__lo) {
        *__lo = (tolower)(*__lo);
    }

    return __hi;
}


template <class _CharT> 
/* virtual */ const char*
ctype<_CharT>::
do_widen (const char *__lo, const char *__hi, char_type *__dest) const
{
    _RWSTD_ASSERT (__lo <= __hi);

    for (; __lo < __hi; ++__lo, ++__dest)
        *__dest = widen (*__lo);

    return __hi;
}


template <class _CharT> 
/* virtual */ const typename ctype<_CharT>::char_type*
ctype<_CharT>::
do_narrow (const char_type *__lo, const char_type *__hi,
           char __dfault, char *__dest) const
{
    _RWSTD_ASSERT (__lo <= __hi);
    _RWSTD_ASSERT (!__lo || __dest);

    for (; __lo < __hi; ++__lo, ++__dest)
        *__dest = narrow (*__lo, __dfault);

    return __hi;
}

#endif   // _RWSTD_NO_EXT_CTYPE_PRIMARY


}   // namespace std
