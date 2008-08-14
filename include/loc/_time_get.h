/***************************************************************************
 *
 * _time_get.h - definition of the std::time_get class templates
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

#ifndef _RWSTD_LOC_TIME_GET_H_INCLUDED
#define _RWSTD_LOC_TIME_GET_H_INCLUDED
 

#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#include <loc/_facet.h>
#include <rw/_ioiter.h>
#include <rw/_iosfwd.h>
#include <rw/_defs.h>

#include _RWSTD_CTIME


_RWSTD_NAMESPACE (__rw) {

const void* __rw_get_timepunct (const _RW::__rw_facet*, int, _RWSTD_SIZE_T);

}   // namespace __rw


_RWSTD_NAMESPACE (std) { 


// 22.2.5.1
struct _RWSTD_EXPORT time_base
{
   enum dateorder { no_order, dmy, mdy, ymd, ydm };
};


// 22.2.5.1
_EXPORT
template <class _CharT, class _InputIter = istreambuf_iterator<_CharT> >
class time_get: public _RW::__rw_facet, public time_base
{
public:
    typedef _CharT                         char_type;
    typedef _InputIter                     iter_type;

    explicit time_get (_RWSTD_SIZE_T __ref = 0)
        : _RW::__rw_facet (__ref) { }

    virtual ~time_get () _RWSTD_ATTRIBUTE_NOTHROW;

    dateorder date_order() const { 
        return do_date_order(); 
    }

    iter_type
    get_time (iter_type __start, iter_type __end, ios_base &__flags,
              _RWSTD_IOSTATE &__err, tm *__time) const {
        return do_get_time (__start, __end, __flags, __err, __time); 
    }

    iter_type
    get_date (iter_type __start, iter_type __end, ios_base &__flags,
              _RWSTD_IOSTATE &__err, tm *__time) const {
        return do_get_date(__start, __end, __flags, __err, __time); 
    }

    iter_type
    get_weekday (iter_type __start, iter_type __end, ios_base &__flags,
                 _RWSTD_IOSTATE &__err, tm *__time) const {
        return do_get_weekday (__start, __end, __flags, __err, __time); 
    }

    iter_type
    get_monthname (iter_type __start, iter_type __end, ios_base &__flags, 
                   _RWSTD_IOSTATE &__err, tm *__time) const {
        return do_get_monthname (__start, __end, __flags, __err, __time);
    }

    iter_type
    get_year (iter_type __start, iter_type __end, ios_base &__flags,
              _RWSTD_IOSTATE &__err, tm *__time) const { 
        return do_get_year (__start, __end, __flags, __err, __time); 
    }


    static _RW::__rw_facet_id id;


#ifdef _RWSTD_NO_EXT_TIME_GET

private:

#endif   // _RWSTD_NO_EXT_TIME_GET

    // extension
    iter_type
    get (iter_type, iter_type, ios_base&, _RWSTD_IOSTATE&, tm*,
         const char_type*, const char_type*) const;

    // extension
    iter_type
    get (iter_type __it, iter_type __end, ios_base &__flags,
         _RWSTD_IOSTATE &__err, tm *__time,
         char __fmt, char __modifier = '\0') const {
        return do_get (__it, __end, __flags, __err, __time, __fmt, __modifier);
    }

protected:

    virtual dateorder do_date_order()  const {
        return dateorder ();
    }

    virtual iter_type
    do_get_time (iter_type __it, iter_type __end, ios_base &__fl,
                 _RWSTD_IOSTATE &__err, tm *__tmb) const {
        return do_get (__it, __end, __fl, __err, __tmb, 'X');
    }

    virtual iter_type
    do_get_date (iter_type __it, iter_type __end, ios_base &__fl,
                 _RWSTD_IOSTATE &__err, tm *__tmb) const {
        return do_get (__it, __end, __fl, __err, __tmb, 'x');
    }

    virtual iter_type
    do_get_weekday (iter_type __it, iter_type __end, ios_base &__fl,
                    _RWSTD_IOSTATE &__err, tm *__tmb) const {
        return do_get (__it, __end, __fl, __err, __tmb, 'a');
    }

    virtual iter_type
    do_get_monthname (iter_type __it, iter_type __end, ios_base &__fl,
                      _RWSTD_IOSTATE &__err, tm *__tmb) const {
        return do_get (__it, __end, __fl, __err, __tmb, 'b');
    }

    virtual iter_type
    do_get_year (iter_type __it, iter_type __end, ios_base &__fl,
                 _RWSTD_IOSTATE &__err, tm *__tmb) const {
        return do_get (__it, __end, __fl, __err, __tmb, 'Y');
    }

#ifdef _RWSTD_NO_EXT_TIME_GET

private:

#endif   // _RWSTD_NO_EXT_TIME_GET

    // extension
    virtual iter_type
    do_get (iter_type, iter_type, ios_base&, _RWSTD_IOSTATE&, tm*,
            char, char = '\0') const;
};


#ifndef _RWSTD_NO_SPECIALIZED_FACET_ID

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id time_get<char, istreambuf_iterator<char> >::id;

#  ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id time_get<wchar_t, istreambuf_iterator<wchar_t> >::id;

#  endif   // _RWSTD_NO_WCHAR_T
#endif   // _RWSTD_NO_SPECIALIZED_FACET_ID


// 22.2.5.2
template <class _CharT, class _InputIter = istreambuf_iterator<_CharT> >
class time_get_byname: public time_get<_CharT, _InputIter>
{
    char _C_namebuf [32];

public:

    explicit time_get_byname (const char *__name, _RWSTD_SIZE_T __ref = 0)
        : time_get<_CharT, _InputIter>(__ref) {
        this->_C_set_name (__name, _C_namebuf, sizeof _C_namebuf);
    }

    virtual ~time_get_byname () _RWSTD_ATTRIBUTE_NOTHROW;
};

}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE_FIRST (_TIME_GET)
#  include <loc/_time_get.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (_TIME_GET)


#ifdef _RWSTD_MSVC
#  pragma warning (push)
#  pragma warning (disable: 4231)
#endif   // _RWSTD_MSVC


_RWSTD_NAMESPACE (std) { 

#if _RWSTD_INSTANTIATE (_TIME_GET, _CHAR)

_RWSTD_INSTANTIATE_1 (class _RWSTD_TI_EXPORT time_get<char>);

#endif   // _RWSTD_INSTANTIATE (_TIME_GET, _CHAR)

#if _RWSTD_INSTANTIATE (_TIME_GET, _WCHAR_T)

_RWSTD_INSTANTIATE_1 (class _RWSTD_TI_EXPORT time_get<wchar_t>);

#endif   // _RWSTD_INSTANTIATE (_TIME_GET, _WCHAR_T)

}   // namespace std


#ifdef _RWSTD_MSVC
#  pragma warning (pop)
#endif   // _RWSTD_MSVC


#if _RWSTD_DEFINE_TEMPLATE_LAST (_TIME_GET)
#  include <loc/_time_get.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_LAST (_TIME_GET)


#endif   // _RWSTD_LOC_TIME_GET_H_INCLUDED
