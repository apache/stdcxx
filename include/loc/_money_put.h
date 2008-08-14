/***************************************************************************
 *
 * _money_put.h - definition of the std::num_put class templates
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
 * Copyright 2001-2006 Rogue Wave Software.
 *
 **************************************************************************/

#ifndef _RWSTD_LOC_MONEY_PUT_H_INCLUDED
#define _RWSTD_LOC_MONEY_PUT_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#include <string>
#include <rw/_ioiter.h>
#include <loc/_facet.h>


_RWSTD_NAMESPACE (std) { 

struct _RWSTD_EXPORT ios_base;


// 22.2.6.2
_EXPORT
template <class _CharT, class _OutputIter = ostreambuf_iterator<_CharT> >
struct money_put: _RW::__rw_facet
{
    typedef _CharT      char_type;
    typedef _OutputIter iter_type;

    typedef
    basic_string<char_type, char_traits<char_type>, allocator<char_type> >
    string_type;

    explicit money_put (_RWSTD_SIZE_T __ref = 0)
        : _RW::__rw_facet (__ref) { }

    virtual ~money_put () _RWSTD_ATTRIBUTE_NOTHROW;

    // 22.2.6.2.1, p1
    iter_type put (iter_type __it, bool __intl, ios_base &__flags,
                   char_type __fill, long double __val) const {
        return do_put (__it, __intl, __flags, __fill, __val);
    }

    iter_type put (iter_type __it, bool __intl, ios_base &__flags,
                   char_type __fill, const string_type &__val) const {
        return do_put (__it, __intl, __flags, __fill, __val);
    }

    static _RW::__rw_facet_id id;

protected:

    // 22.2.6.2.2, p1
    virtual iter_type
    do_put (iter_type, bool, ios_base&, char_type, long double) const;

    virtual iter_type
    do_put (iter_type, bool, ios_base&, char_type, const string_type&) const;

private:

    enum { _C_intl = 0x1, _C_ldbl = 0x2 };

    iter_type _C_put (iter_type, int, ios_base&, char_type,
                      const char_type*, _RWSTD_SIZE_T, int,
                      const char*, _RWSTD_SIZE_T) const;
};


#ifndef _RWSTD_NO_SPECIALIZED_FACET_ID

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id money_put<char, ostreambuf_iterator<char> >::id;

#  ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id money_put<wchar_t, ostreambuf_iterator<wchar_t> >::id;

#  endif   // _RWSTD_NO_WCHAR_T
#endif   // _RWSTD_NO_SPECIALIZED_FACET_ID

}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE_FIRST (_MONEY_PUT)
#  include <loc/_money_put.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (_MONEY_PUT)


#ifdef _RWSTD_MSVC
#  pragma warning (push)
#  pragma warning (disable: 4231)
#endif   // _RWSTD_MSVC


_RWSTD_NAMESPACE (std) { 

#if _RWSTD_INSTANTIATE (_MONEY_PUT, _CHAR)

_RWSTD_INSTANTIATE_1 (struct _RWSTD_TI_EXPORT money_put<char>);

#endif   // _RWSTD_INSTANTIATE (_MONEY_PUT, _CHAR)

#if _RWSTD_INSTANTIATE (_MONEY_PUT, _WCHAR_T)

_RWSTD_INSTANTIATE_1 (struct _RWSTD_TI_EXPORT money_put<wchar_t>);

#endif   // _RWSTD_INSTANTIATE (_MONEY_PUT, _WCHAR_T)

}   // namespace std


#ifdef _RWSTD_MSVC
#  pragma warning (pop)
#endif   // _RWSTD_MSVC


#if _RWSTD_DEFINE_TEMPLATE_LAST (_MONEY_PUT)
#  include <loc/_money_put.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_LAST (_MONEY_PUT)


#endif   // _RWSTD_LOC_MONEY_PUT_H_INCLUDED
