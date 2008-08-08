/***************************************************************************
 *
 * _punct.h - definition of the __rw_punct class template
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
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

#ifndef _RWSTD_LOC_PUNCT_H_INCLUDED
#define _RWSTD_LOC_PUNCT_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#include <rw/_defs.h>
#include <rw/_ioiter.h>
#include <rw/_traits.h>


_RWSTD_NAMESPACE (std) {

struct _RWSTD_EXPORT ios_base;

}   // namespace std


_RWSTD_NAMESPACE (__rw) {

enum {
    // bitset elements representing decimal_point, thousands_sep, grouping,
    // truename, falsename, currency_sym, positive_sign, negative_sign,
    // frac_digits, pos_format, neg_format

    // values are bitset elements since they are used to indicate which
    // members have already been cached by std::moneypunct and std::numpunct
    __rw_dp  = 0x0001, __rw_ts   = 0x0002, __rw_gr = 0x0004, __rw_tn = 0x0008,
    __rw_fn  = 0x0010, __rw_cs   = 0x0020, __rw_ps = 0x0040, __rw_ns = 0x0080,
    __rw_fd  = 0x0100, __rw_pf   = 0x0200, __rw_nf = 0x0400,

    // bitset elements used by std::time_get and std::time_put
    __rw_abday = 0x0001, __rw_day   = 0x0002, __rw_abmon      = 0x0004,
    __rw_month = 0x0008, __rw_ampm  = 0x0010, __rw_d_t_fmt    = 0x0020,
    __rw_d_fmt = 0x0040, __rw_t_fmt = 0x0080, __rw_t_fmt_ampm = 0x0100,

    // bitset elements used by std::time_get and std::time_put
    // for alternative date/time representions and eras
    __rw_era_d_t_fmt = 0x0200, __rw_era_d_fmt = 0x0400, __rw_era_t_fmt = 0x0800,
    __rw_era_names   = 0x1000, __rw_era_fmts  = 0x2000,
    __rw_alt_digits  = 0x4000,

    // other flags
    __rw_wide = 0x100000, __rw_mon = 0x200000, __rw_intl = 0x400000
};

struct __rw_facet;

template <class _CharT>
inline const _CharT*
__rw_get_punct (const _RW::__rw_facet*, int, _CharT)
{
    // body provided to work around bugs in compilers such as IBM
    // VAC++ 7.0 (see bug #562) that instantiate the function when
    // e.g., use_facet<numpunct<int> >(locale) is called
    _RWSTD_ASSERT (!"must not be called");
    return 0;
}

// use overloading instead of specialization below to prevent
// linker unsats when using Intel C++ on Windows (PR #29178)
/* _RWSTD_SPECIALIZED_FUNCTION */ _RWSTD_EXPORT
const char* __rw_get_punct (const _RW::__rw_facet*, int, char);

#ifndef _RWSTD_NO_WCHAR_T

/* _RWSTD_SPECIALIZED_FUNCTION */ _RWSTD_EXPORT
const wchar_t* __rw_get_punct (const _RW::__rw_facet*, int, wchar_t);

#endif   // _RWSTD_NO_WCHAR_T

// extracts as many values from the given range as necessary to
// (not necessarily unambiguously) match one or more names (in
// case of duplicates) in the array of sequences given by the
// third and fourth arguments
// case insensitive comparison (characters are compared after
// converting to uppercase using std::ctype<charT>::toupper())
// is performed if the last argument is nonzero
// retuns the iterator pointing one past the last extracted value,
// fills 6th argument with the index of the matched name and sets
// the 7th argument to eofbit if the end of input has been reached
// while trying to match a name (note that the returned iterator
// may be equal to the end iterator and eofbit may not be set)
_EXPORT
template <class _CharT, class _InputIter>
_InputIter __rw_match_name (_InputIter, _InputIter,
                            const _CharT* const*, const _RWSTD_SIZE_T*,
                            _RWSTD_SIZE_T, _RWSTD_SIZE_T&, int&,
                            _STD::ios_base*);

}   // namespace __rw


#if _RWSTD_DEFINE_TEMPLATE_FIRST (_PUNCT)
#  include <loc/_punct.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (_PUNCT)


#ifdef _RWSTD_MSVC
#  pragma warning (push)
#  pragma warning (disable: 4231)
#endif   // _RWSTD_MSVC


_RWSTD_NAMESPACE (__rw) {

#define _RWSTD_ARG_LIST(charT)                                          \
        _RWSTD_ISTREAMBUF_ITER (charT), _RWSTD_ISTREAMBUF_ITER (charT), \
        const charT* const*, const _RWSTD_SIZE_T*, _RWSTD_SIZE_T,       \
        _RWSTD_SIZE_T&, int&, _STD::ios_base*

#if _RWSTD_INSTANTIATE (_PUNCT, _CHAR)

_RWSTD_INSTANTIATE_FUN_1 (_RWSTD_TI_EXPORT _RW::__rw_istreambuf_iterator
                          __rw_match_name (_RWSTD_ARG_LIST (char)));

#endif   // _RWSTD_INSTANTIATE (_PUNCT, _CHAR)

#if _RWSTD_INSTANTIATE (_PUNCT, _WCHAR_T)

_RWSTD_INSTANTIATE_FUN_1 (_RWSTD_TI_EXPORT _RW::__rw_wistreambuf_iterator
                          __rw_match_name (_RWSTD_ARG_LIST (wchar_t)));

#endif   // _RWSTD_INSTANTIATE (_PUNCT, _WCHAR_T)

#undef _RWSTD_ARG_LIST

}   // namespace __rw


#ifdef _RWSTD_MSVC
#  pragma warning (pop)
#endif   // _RWSTD_MSVC


#if _RWSTD_DEFINE_TEMPLATE_LAST (_PUNCT)
#  include <loc/_punct.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_LAST (_PUNCT)


#endif   // _RWSTD_LOC_PUNCT_H_INCLUDED
