/***************************************************************************
 *
 * _bitmask.h - helper definitions for bitmask types
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

#ifndef _RWSTD_RW_IOSFWD_H_INCLUDED
#define _RWSTD_RW_IOSFWD_H_INCLUDED

#ifndef _RWSTD_RW_BITMASK_H_INCLUDED
#  include <rw/_bitmask.h>
#endif   // _RWSTD_RW_BITMASK_H_INCLUDED


_RWSTD_NAMESPACE (std) {

template<class _CharT>
struct char_traits;

_RWSTD_SPECIALIZED_CLASS
struct char_traits<char>;

#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
struct char_traits<wchar_t>;

#endif   // _RWSTD_NO_WCHAR_T


struct _RWSTD_EXPORT ios_base;

_EXPORT
template <class _CharT, class _Traits>
class basic_ios;

_EXPORT
template <class _CharT, class _Traits>
class basic_streambuf;

// 27.2, p4
_EXPORT
template <class _CharT, class _Traits>
class basic_istream;

// 27.2, p5
_EXPORT
template <class _CharT, class _Traits>
class basic_ostream;

// 27.2, p6
_EXPORT
template <class _CharT, class _Traits>
class basic_iostream;

template <class _CharT, class _Traits>
struct ostreambuf_iterator;

template <class _CharT, class _Traits>
struct istreambuf_iterator;

}   // namespace std


// used in money_get and num_get facets
#define _RWSTD_IOSTATE  _RWSTD_BITMASK_ENUM (_RW::__rw_iostate)
#define _RWSTD_FMTFLAGS _RWSTD_BITMASK_ENUM (_RW::__rw_fmtflags)


_RWSTD_NAMESPACE (__rw) {

// for convenience
typedef _STD::istreambuf_iterator<char, _STD::char_traits<char> >
    __rw_istreambuf_iterator;

#ifndef _RWSTD_NO_WCHAR_T

typedef _STD::istreambuf_iterator<wchar_t, _STD::char_traits<wchar_t> >
    __rw_wistreambuf_iterator;

#endif   // _RWSTD_NO_WCHAR_T


enum __rw_fmtflags {
    __rw_boolalpha   = _RWSTD_IOS_BOOLALPHA,
    __rw_dec         = _RWSTD_IOS_DEC,
    __rw_fixed       = _RWSTD_IOS_FIXED,
    __rw_hex         = _RWSTD_IOS_HEX,
    __rw_internal    = _RWSTD_IOS_INTERNAL,
    __rw_left        = _RWSTD_IOS_LEFT,
    __rw_oct         = _RWSTD_IOS_OCT,
    __rw_right       = _RWSTD_IOS_RIGHT,
    __rw_scientific  = _RWSTD_IOS_SCIENTIFIC,
    __rw_showbase    = _RWSTD_IOS_SHOWBASE,
    __rw_showpoint   = _RWSTD_IOS_SHOWPOINT,
    __rw_showpos     = _RWSTD_IOS_SHOWPOS,
    __rw_skipws      = _RWSTD_IOS_SKIPWS,
    __rw_unitbuf     = _RWSTD_IOS_UNITBUF,
    __rw_uppercase   = _RWSTD_IOS_UPPERCASE,
    __rw_bin         = _RWSTD_IOS_BIN,
    __rw_basefield   = _RWSTD_IOS_BASEFIELD,
    __rw_adjustfield = _RWSTD_IOS_ADJUSTFIELD,
    __rw_floatfield  = _RWSTD_IOS_FLOATFIELD,
    __rw_nolock      = _RWSTD_IOS_NOLOCK,
    __rw_nolockbuf   = _RWSTD_IOS_NOLOCKBUF,
    __rw_sync_stdio  = _RWSTD_IOS_SYNC_STDIO
};

_RWSTD_DEFINE_BITMASK_OPERATORS (__rw_fmtflags);


enum __rw_iostate {
    __rw_goodbit = _RWSTD_IOS_GOODBIT,
    __rw_badbit  = _RWSTD_IOS_BADBIT,
    __rw_eofbit  = _RWSTD_IOS_EOFBIT,
    __rw_failbit = _RWSTD_IOS_FAILBIT,

    // for debugging
    __rw_bad_eof      = _RWSTD_IOS_BADBIT | _RWSTD_IOS_EOFBIT,
    __rw_bad_fail     = _RWSTD_IOS_BADBIT | _RWSTD_IOS_FAILBIT,
    __rw_bad_eof_fail =   _RWSTD_IOS_BADBIT | _RWSTD_IOS_EOFBIT
                        | _RWSTD_IOS_FAILBIT,
    __rw_eof_fail     = _RWSTD_IOS_EOFBIT | _RWSTD_IOS_FAILBIT,


    __rw_nothrow = _RWSTD_IOS_NOTHROW,
    __rw_rethrow = _RWSTD_IOS_RETHROW
};

_RWSTD_DEFINE_BITMASK_OPERATORS (__rw_iostate);


enum __rw_openmode {
    __rw_app       = _RWSTD_IOS_APP,
    __rw_binary    = _RWSTD_IOS_BINARY,
    __rw_in        = _RWSTD_IOS_IN,
    __rw_out       = _RWSTD_IOS_OUT,
    __rw_in_out    = _RWSTD_IOS_IN | _RWSTD_IOS_OUT,
    __rw_trunc     = _RWSTD_IOS_TRUNC,
    __rw_ate       = _RWSTD_IOS_ATE,
    __rw_nocreate  = _RWSTD_IOS_NOCREATE,
    __rw_noreplace = _RWSTD_IOS_NOREPLACE,
    __rw_stdio     = _RWSTD_IOS_STDIO,
    __rw_native    = _RWSTD_IOS_NATIVE
};

_RWSTD_DEFINE_BITMASK_OPERATORS (__rw_openmode);


enum __rw_seekdir {
    __rw_beg = _RWSTD_SEEK_SET,
    __rw_cur = _RWSTD_SEEK_CUR,
    __rw_end = _RWSTD_SEEK_END
};

}   // namespace __rw


#endif   // _RWSTD_RW_IOSFWD_H_INCLUDED
