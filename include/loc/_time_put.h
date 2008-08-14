/***************************************************************************
 *
 * _time_put.h - definition of the std::time_put class templates
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

#ifndef _RWSTD_LOC_TIME_PUT_H_INCLUDED
#define _RWSTD_LOC_TIME_PUT_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#include <loc/_facet.h>
#include <rw/_ioiter.h>
#include <rw/_defs.h>

#include _RWSTD_CTIME


_RWSTD_NAMESPACE (std) {

struct _RWSTD_EXPORT ios_base;


// 22.2.5.3
_EXPORT
template <class _CharT, class _OutputIter = ostreambuf_iterator<_CharT> >
struct time_put: _RW::__rw_facet
{
    typedef _CharT      char_type;
    typedef _OutputIter iter_type;

    explicit time_put (_RWSTD_SIZE_T __refs = 0)
        : _RW::__rw_facet (__refs) { }

    virtual ~time_put () _RWSTD_ATTRIBUTE_NOTHROW;

    // 22.2.5.3.1, p1
    iter_type put (iter_type, ios_base&, char_type, const tm*,
                   const char_type*, const char_type*) const;

    iter_type put (iter_type __it, ios_base& __flags, char_type __fill, 
                   const tm* __tmb, char __fmt, char __modifier = 0) const {
        return do_put (__it, __flags, __fill, __tmb, __fmt, __modifier); 
    }

    static _RW::__rw_facet_id id;

protected:

    // 22.2.5.3.2, p1
    virtual iter_type
    do_put (iter_type, ios_base&, char_type, const tm*, char, char) const;
};


#ifndef _RWSTD_NO_SPECIALIZED_FACET_ID

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id time_put<char, ostreambuf_iterator<char> >::id;

#  ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id time_put<wchar_t, ostreambuf_iterator<wchar_t> >::id;

#  endif   // _RWSTD_NO_WCHAR_T
#endif   // _RWSTD_NO_SPECIALIZED_FACET_ID


// 22.2.5.2
template <class _CharT, class _OutputIter = ostreambuf_iterator<_CharT> >
class time_put_byname: public time_put<_CharT, _OutputIter>
{
    char _C_namebuf [32];

public:

    explicit time_put_byname (const char *__name, _RWSTD_SIZE_T __ref = 0)
        : time_put<_CharT, _OutputIter>(__ref) {
        this->_C_set_name (__name, _C_namebuf, sizeof _C_namebuf);
    }

    virtual ~time_put_byname () _RWSTD_ATTRIBUTE_NOTHROW;
};

}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE_FIRST (_TIME_PUT)
#  include <loc/_time_put.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (_TIME_PUT)


#ifdef _RWSTD_MSVC
#  pragma warning (push)
#  pragma warning (disable: 4231)
#endif   // _RWSTD_MSVC


_RWSTD_NAMESPACE (std) { 

#if _RWSTD_INSTANTIATE (_TIME_PUT, _CHAR)

_RWSTD_INSTANTIATE_1 (struct _RWSTD_TI_EXPORT time_put<char>);

#endif   // _RWSTD_INSTANTIATE (_TIME_PUT, _CHAR)

#if _RWSTD_INSTANTIATE (_TIME_PUT, _WCHAR_T)

_RWSTD_INSTANTIATE_1 (struct _RWSTD_TI_EXPORT time_put<wchar_t>);

#endif   // _RWSTD_INSTANTIATE (_TIME_PUT, _WCHAR_T)

}   // namespace std


#ifdef _RWSTD_MSVC
#  pragma warning (pop)
#endif   // _RWSTD_MSVC


#if _RWSTD_DEFINE_TEMPLATE_LAST (_TIME_PUT)
#  include <loc/_time_put.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_LAST (_TIME_PUT)


#endif   // _RWSTD_LOC_TIME_PUT_H_INCLUDED
