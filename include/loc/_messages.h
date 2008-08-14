/***************************************************************************
 *
 * _messages.h - definition of the std::messages class templates
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
 * Copyright 1994-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _RWSTD_LOC_MESSAGES_H_INCLUDED
#define _RWSTD_LOC_MESSAGES_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#include <string>

#include <loc/_facet.h>
#include <loc/_codecvt.h>
#include <loc/_locale.h>

#include <rw/_defs.h> 


_RWSTD_NAMESPACE (__rw) {

int __rw_cat_open (const _STD::string&, const _STD::locale&);

const char* __rw_get_message (int, int, int);

const _STD::locale& __rw_get_locale (int);

void __rw_cat_close (int);

}   // namespace __rw


_RWSTD_NAMESPACE (std) { 


struct _RWSTD_EXPORT messages_base
{
    typedef int catalog;
};


// 22.2.7.1
_EXPORT
template <class _CharT>
class messages: public _RW::__rw_facet,
                public messages_base
{
public:
    typedef _CharT char_type;

    typedef basic_string<char_type, char_traits<char_type>,
                         allocator<char_type> > string_type;

    explicit messages (_RWSTD_SIZE_T __refs = 0)
        : _RW::__rw_facet (__refs) { }

    virtual ~messages () _RWSTD_ATTRIBUTE_NOTHROW;

    catalog open (const string& __fun, const locale& __loc) const {
        return do_open (__fun, __loc);
    }

    string_type get (catalog __c, int __set, int __msgid,
                     const string_type& __df) const {
        return do_get (__c, __set, __msgid, __df);
    }

    void close (catalog __c) const {
        do_close (__c);
    }

    static _RW::__rw_facet_id id;

protected:

    virtual catalog do_open (const string&, const locale&) const;

    virtual string_type do_get (catalog, int, int, const string_type&) const;

    virtual void do_close (catalog) const;
};


#ifndef _RWSTD_NO_SPECIALIZED_FACET_ID

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id messages<char>::id;

#  ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id messages<wchar_t>::id;

#  endif   // _RWSTD_NO_WCHAR_T
#endif   // _RWSTD_NO_SPECIALIZED_FACET_ID


// 22.2.7.2
template <class _CharT>
class messages_byname: public messages<_CharT>
{
    char _C_namebuf [32];

public:

    explicit messages_byname (const char *__name, _RWSTD_SIZE_T __refs = 0)
        : messages<_CharT>(__refs) {
        this->_C_set_name (__name, _C_namebuf, sizeof _C_namebuf);
    }

    virtual ~messages_byname () _RWSTD_ATTRIBUTE_NOTHROW;
};

}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE_FIRST (_MESSAGES)
#  include <loc/_messages.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (MESSAGES)


#ifdef _RWSTD_MSVC
#  pragma warning (push)
#  pragma warning (disable: 4231)
#endif   // _RWSTD_MSVC


_RWSTD_NAMESPACE (std) { 

#if _RWSTD_INSTANTIATE (_MESSAGES, _CHAR)

_RWSTD_INSTANTIATE_1 (class _RWSTD_TI_EXPORT messages<char>);

#endif   // _RWSTD_INSTANTIATE (_MESSAGES, _CHAR)

#if _RWSTD_INSTANTIATE (_MESSAGES, _WCHAR_T)

_RWSTD_INSTANTIATE_1 (class _RWSTD_TI_EXPORT messages<wchar_t>);

#endif   // _RWSTD_INSTANTIATE (_MESSAGES, _WCHAR_T)

}   // namespace std


#ifdef _RWSTD_MSVC
#  pragma warning (pop)
#endif   // _RWSTD_MSVC


#if _RWSTD_DEFINE_TEMPLATE_LAST (_MESSAGES)
#  include <loc/_messages.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_LAST (MESSAGES)


#endif   // _RWSTD_LOC_MESSAGES_H_INCLUDED
