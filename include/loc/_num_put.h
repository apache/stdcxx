/***************************************************************************
 *
 * _num_put.h - definition of the std::num_put class templates
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

#ifndef _RWSTD_LOC_NUM_PUT_H_INCLUDED
#define _RWSTD_LOC_NUM_PUT_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#include <streambuf>

#include <rw/_ioiter.h>
#include <loc/_facet.h>
#include <loc/_numpunct.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) {

// 22.2.2.2
_EXPORT
template <class _CharT, class _OutputIter = ostreambuf_iterator<_CharT> >
struct num_put: _RW::__rw_facet
{
    typedef _CharT      char_type;
    typedef _OutputIter iter_type;

    explicit num_put (_RWSTD_SIZE_T __refs = 0)
        : _RW::__rw_facet (__refs) { }

    virtual ~num_put ();

#ifndef _RWSTD_NO_BOOL

    iter_type put (iter_type __it, ios_base& __flags, char_type __fill,
                   bool __val) const {
        return do_put (__it, __flags, __fill, __val);
    }

#endif   // _RWSTD_NO_BOOL

    iter_type put (iter_type __it, ios_base &__flags, char_type __fill,
                   long __val) const {
        return do_put (__it, __flags, __fill, __val);
    }

    iter_type put (iter_type __it, ios_base &__flags, char_type __fill,
                   unsigned long __val) const {
        return do_put (__it, __flags, __fill, __val);
    }

    iter_type put (iter_type __it, ios_base &__flags, char_type __fill,
                   double __val) const {
        return do_put (__it, __flags, __fill, __val);
    }

#ifndef _RWSTD_NO_LONG_DOUBLE

    iter_type put (iter_type __it, ios_base &__flags, char_type __fill,
                   long double __val) const {
        return do_put (__it, __flags, __fill, __val);
    }

#endif   // _RWSTD_NO_LONG_DOUBLE

    iter_type put (iter_type __it, ios_base &__flags, char_type __fill,
                   const void* __val) const {
        return do_put (__it, __flags, __fill, __val);
    }

// prevent gcc warning: cast to pointer from integer of different size
#define _TO_VOID(x) \
  _RWSTD_REINTERPRET_CAST (const void*, _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, x))

#ifdef _RWSTD_LONG_LONG

    // extensions
    iter_type put (iter_type __it, ios_base &__flags, char_type __fill,
                   _RWSTD_LONG_LONG __val) const {
        return do_put (__it, __flags, __fill, __val);
    }

    iter_type put (iter_type __it, ios_base &__flags, char_type __fill,
                   unsigned _RWSTD_LONG_LONG __val) const {
        return do_put (__it, __flags, __fill, __val);
    }

#endif   // _RWSTD_LONG_LONG

    static _RW::__rw_facet_id id;

protected:

#ifndef _RWSTD_NO_BOOL

    virtual iter_type
    do_put (iter_type __it, ios_base &__flags, char_type __fill,
            bool __val) const {
        return _C_put (__it, __flags, __fill,
                          __flags.flags () & _RWSTD_IOS_BOOLALPHA
                          ? _C_bool : _C_int, _TO_VOID (__val));
    }

#endif   // _RWSTD_NO_BOOL

    virtual iter_type
    do_put (iter_type __it, ios_base &__flags, char_type __fill,
            long __val) const {
        return _C_put (__it, __flags, __fill, _C_long, _TO_VOID (__val));
    }

    virtual iter_type
    do_put (iter_type __it, ios_base &__flags, char_type __fill,
            unsigned long __val) const {
        return _C_put (__it, __flags, __fill, _C_ulong, _TO_VOID (__val));
    }

    virtual iter_type
    do_put (iter_type __it, ios_base &__flags, char_type __fill,
            double __val) const {
        return _C_put (__it, __flags, __fill, _C_double | _C_ptr, &__val);
    }

#ifndef _RWSTD_NO_LONG_DOUBLE

    virtual iter_type
    do_put (iter_type __it, ios_base &__flags, char_type __fill,
            long double __val) const {
        return _C_put (__it, __flags, __fill, _C_ldouble | _C_ptr, &__val);
    }

#endif   // _RWSTD_NO_LONG_DOUBLE

    virtual iter_type
    do_put (iter_type __it, ios_base &__flags, char_type __fill,
            const void *__val) const {
        return _C_put (__it, __flags, __fill, _C_pvoid, __val);
    }

#ifdef _RWSTD_LONG_LONG

    // extensions
    virtual iter_type
    do_put (iter_type __it, ios_base &__flags, char_type __fill,
            _RWSTD_LONG_LONG __val) const {
        return _C_put (__it, __flags, __fill, _C_llong, &__val);
    }

    virtual iter_type
    do_put (iter_type __it, ios_base &__flags, char_type __fill,
            unsigned _RWSTD_LONG_LONG __val) const {
        return _C_put (__it, __flags, __fill, _C_ullong, &__val);
    }

#endif   // _RWSTD_LONG_LONG

#undef _TO_VOID

private:

    iter_type _C_put (iter_type, ios_base&, char_type, int, const void*) const;
};


#ifndef _RWSTD_NO_SPECIALIZED_FACET_ID

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id num_put<char, ostreambuf_iterator<char> >::id;

#  ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id num_put<wchar_t, ostreambuf_iterator<wchar_t> >::id;

#  endif   // _RWSTD_NO_WCHAR_T
#endif   // _RWSTD_NO_SPECIALIZED_FACET_ID

}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE_FIRST (_NUM_PUT)
#  include <loc/_num_put.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (_NUM_PUT)


#ifdef _RWSTD_MSVC
#  pragma warning (push)
#  pragma warning (disable: 4231)
#endif   // _RWSTD_MSVC


_RWSTD_NAMESPACE (std) {

#if _RWSTD_INSTANTIATE (_NUM_PUT, _CHAR)

_RWSTD_INSTANTIATE_1 (struct _RWSTD_TI_EXPORT num_put<char>);

#endif   // _RWSTD_INSTANTIATE (_NUM_PUT, _CHAR)

#if _RWSTD_INSTANTIATE (_NUM_PUT, _WCHAR_T)

_RWSTD_INSTANTIATE_1 (struct _RWSTD_TI_EXPORT num_put<wchar_t>);

#endif   // _RWSTD_INSTANTIATE (_NUM_PUT, _WCHAR_T)

}   // namespace std


#ifdef _RWSTD_MSVC
#  pragma warning (pop)
#endif   // _RWSTD_MSVC


#if _RWSTD_DEFINE_TEMPLATE_LAST (_NUM_PUT)
#  include <loc/_num_put.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_LAST (_NUM_PUT)


#endif   // _RWSTD_LOC_NUM_PUT_H_INCLUDED
