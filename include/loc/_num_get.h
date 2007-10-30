/***************************************************************************
 *
 * _num_get.h - definition of the std::num_get class template
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

#ifndef _RWSTD_LOC_NUM_GET_H_INCLUDED
#define _RWSTD_LOC_NUM_GET_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#include <loc/_facet.h>
#include <rw/_ioiter.h>
#include <rw/_iosfwd.h>
#include <rw/_traits.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


// 22.2.2.1
_EXPORT
template <class _CharT, class _InputIter = istreambuf_iterator<_CharT> >
struct num_get: _RW::__rw_facet
{
    typedef _CharT     char_type;
    typedef _InputIter iter_type;

    static _RW::__rw_facet_id id;

    _EXPLICIT num_get (_RWSTD_SIZE_T __refs = 0)
        : _RW::__rw_facet (__refs) { }

#ifndef _RWSTD_NO_NATIVE_BOOL

    iter_type get (iter_type __begin, iter_type __end, ios_base &__flags, 
                   _RWSTD_IOSTATE &__err, bool &__val) const {
        _RWSTD_ASSERT_RANGE (__begin, __end);
        return do_get (__begin, __end, __flags, __err, __val);
    }

#endif   // _RWSTD_NO_NATIVE_BOOL

    iter_type get (iter_type __begin, iter_type __end, ios_base &__flags, 
                   _RWSTD_IOSTATE &__err,  unsigned short &__val) const {
        _RWSTD_ASSERT_RANGE (__begin, __end);
        return do_get (__begin, __end, __flags, __err, __val);
    }

#ifndef _RWSTD_NO_EXT_NUM_GET

    // extension
    iter_type
    get (iter_type, iter_type, ios_base&, _RWSTD_IOSTATE&,  short&) const;

    // extension
    iter_type
    get (iter_type, iter_type, ios_base&, _RWSTD_IOSTATE&, int&) const;

#endif   // _RWSTD_NO_EXT_NUM_GET

    iter_type get (iter_type __begin, iter_type __end, ios_base &__flags, 
                   _RWSTD_IOSTATE &__err, unsigned int &__val) const {
        _RWSTD_ASSERT_RANGE (__begin, __end);
        return do_get (__begin, __end, __flags, __err, __val);
    }

    iter_type get (iter_type __begin, iter_type __end, ios_base &__flags, 
                   _RWSTD_IOSTATE &__err, long &__val) const {
        _RWSTD_ASSERT_RANGE (__begin, __end);
        return do_get (__begin, __end, __flags, __err, __val);
    }

    iter_type get (iter_type __begin, iter_type __end, ios_base &__flags, 
                   _RWSTD_IOSTATE &__err, unsigned long &__val) const {
        _RWSTD_ASSERT_RANGE (__begin, __end);
        return do_get (__begin, __end, __flags, __err, __val);
    }

    iter_type get (iter_type __begin, iter_type __end, ios_base &__flags, 
                   _RWSTD_IOSTATE &__err, float &__val) const {
        _RWSTD_ASSERT_RANGE (__begin, __end);
        return do_get (__begin, __end, __flags, __err, __val);
    }

    iter_type get (iter_type __begin, iter_type __end, ios_base &__flags, 
                   _RWSTD_IOSTATE &__err, double &__val) const {
        _RWSTD_ASSERT_RANGE (__begin, __end);
        return do_get (__begin, __end, __flags, __err, __val);
    }

#ifndef _RWSTD_LONG_DOUBLE

    iter_type get (iter_type __begin, iter_type __end, ios_base &__flags, 
                   _RWSTD_IOSTATE &__err, long double &__val) const {
        _RWSTD_ASSERT_RANGE (__begin, __end);
        return do_get (__begin, __end, __flags, __err, __val);
    }

#endif   // _RWSTD_LONG_DOUBLE

    iter_type get (iter_type __begin, iter_type __end, ios_base &__flags, 
                   _RWSTD_IOSTATE &__err, void* &__val) const {
        _RWSTD_ASSERT_RANGE (__begin, __end);
        return do_get (__begin, __end, __flags, __err, __val);
    }


#ifdef _RWSTD_LONG_LONG

    // extension
    iter_type get (iter_type __begin, iter_type __end, ios_base &__flags,
                   _RWSTD_IOSTATE &__err, _RWSTD_LONG_LONG &__val) const {
        _RWSTD_ASSERT_RANGE (__begin, __end);
        return do_get (__begin, __end, __flags, __err, __val);
    }

    // extension
    iter_type get (iter_type __begin, iter_type __end, ios_base &__flags,
                   _RWSTD_IOSTATE &__err,
                   unsigned _RWSTD_LONG_LONG &__val) const {
        _RWSTD_ASSERT_RANGE (__begin, __end);
        return do_get (__begin, __end, __flags, __err, __val);
    }
#endif   // _RWSTD_LONG_LONG


protected:

#ifndef _RWSTD_NO_NATIVE_BOOL

    virtual iter_type
    do_get (iter_type, iter_type, ios_base&, _RWSTD_IOSTATE&, bool&) const;

#endif   // _RWSTD_NO_NATIVE_BOOL

    virtual iter_type
    do_get (iter_type __begin, iter_type __end, ios_base &__flags,
            _RWSTD_IOSTATE &__err,  unsigned short &__val) const {
        return _C_get (__begin, __end, __flags, __err, _C_ushort, &__val);
    }

    virtual iter_type
    do_get (iter_type __begin, iter_type __end, ios_base &__flags,
            _RWSTD_IOSTATE &__err, unsigned int &__val) const {
        return _C_get (__begin, __end, __flags, __err, _C_uint, &__val);
    }

    virtual iter_type
    do_get (iter_type __begin, iter_type __end, ios_base &__flags,
            _RWSTD_IOSTATE &__err, long &__val) const {
        return _C_get (__begin, __end, __flags, __err, _C_long, &__val);
    }

    virtual iter_type
    do_get (iter_type __begin, iter_type __end, ios_base &__flags,
            _RWSTD_IOSTATE &__err, unsigned long &__val) const {
        return _C_get (__begin, __end, __flags, __err, _C_ulong, &__val);
    }

    virtual iter_type
    do_get (iter_type __begin, iter_type __end, ios_base &__flags,
            _RWSTD_IOSTATE &__err, float &__val) const {
        return _C_get (__begin, __end, __flags, __err, _C_float, &__val);
    }

    virtual iter_type
    do_get (iter_type __begin, iter_type __end, ios_base &__flags,
            _RWSTD_IOSTATE &__err, double &__val) const {
        return _C_get (__begin, __end, __flags, __err, _C_double, &__val);
    }

#ifndef _RWSTD_LONG_DOUBLE

    virtual iter_type
    do_get (iter_type __begin, iter_type __end, ios_base &__flags,
            _RWSTD_IOSTATE &__err, long double &__val) const {
        return _C_get (__begin, __end, __flags, __err, _C_ldouble, &__val);
    }

#endif   // _RWSTD_LONG_DOUBLE

    virtual iter_type
    do_get (iter_type __begin, iter_type __end, ios_base &__flags,
            _RWSTD_IOSTATE &__err, void* &__val) const {
        return _C_get (__begin, __end, __flags, __err, _C_pvoid, &__val);
    }

#ifdef _RWSTD_LONG_LONG

    // extension
    virtual iter_type
    do_get (iter_type __begin, iter_type __end, ios_base &__flags,
            _RWSTD_IOSTATE &__err, _RWSTD_LONG_LONG &__val) const {
        return _C_get (__begin, __end, __flags, __err, _C_llong, &__val);
    }

    // extension
    virtual iter_type
    do_get (iter_type __begin, iter_type __end, ios_base &__flags,
            _RWSTD_IOSTATE &__err, unsigned _RWSTD_LONG_LONG &__val) const {
        return _C_get (__begin, __end, __flags, __err, _C_ullong, &__val);
    }


#endif   // _RWSTD_LONG_LONG

private:

    iter_type
    _C_get (iter_type, iter_type, ios_base&,
	    _RWSTD_IOSTATE&, int, void*) const;
};


#ifndef _RWSTD_NO_SPECIALIZED_FACET_ID

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id num_get<char, istreambuf_iterator<char> >::id;

#  ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id num_get<wchar_t, istreambuf_iterator<wchar_t> >::id;

#  endif   // _RWSTD_NO_WCHAR_T
#endif   // _RWSTD_NO_SPECIALIZED_FACET_ID

}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE_FIRST (_NUM_GET)
#  include <loc/_num_get.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (_NUM_GET)


_RWSTD_NAMESPACE (std) { 

#if _RWSTD_INSTANTIATE (_NUM_GET, _CHAR)

_RWSTD_INSTANTIATE_1 (struct _RWSTD_EXPORT num_get<char>);

#endif   // _RWSTD_INSTANTIATE (_NUM_GET, _CHAR)

#if _RWSTD_INSTANTIATE (_NUM_GET, _WCHAR_T)

_RWSTD_INSTANTIATE_1 (struct _RWSTD_EXPORT num_get<wchar_t>);

#endif   // _RWSTD_INSTANTIATE (_NUM_GET, _WCHAR_T)

}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE_LAST (_NUM_GET)
#  include <loc/_num_get.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_LAST (_NUM_GET)


#endif   // _RWSTD_LOC_NUM_GET_H_INCLUDED
