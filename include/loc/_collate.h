/***************************************************************************
 *
 * _collate.h - definition of the std::collate class templates
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
 * Copyright 1994-2006 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _RWSTD_LOC_COLLATE_H_INCLUDED
#define _RWSTD_LOC_COLLATE_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#include <string>
#include <loc/_facet.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


_EXPORT
template <class _CharT>
class collate;


#ifndef _RWSTD_NO_EXT_COLLATE_PRIMARY

_EXPORT
template <class _CharT>
class collate: public _RW::__rw_facet
{
public:
    typedef _CharT char_type;
    typedef basic_string<char_type, char_traits<char_type>,
                         allocator<char_type> >
    string_type;

    explicit collate (_RWSTD_SIZE_T __refs = 0)
        : _RW::__rw_facet (__refs) { }

    virtual ~collate () _RWSTD_ATTRIBUTE_NOTHROW;

    int
    compare (const char_type* __low1, const char_type* __high1,
             const char_type* __low2, const char_type* __high2) const {
        return do_compare (__low1, __high1, __low2, __high2);
    }

    string_type
    transform (const char_type* __low, const char_type* __high) const {
        return do_transform (__low, __high);
    }

    long
    hash (const char_type* __low, const char_type* __high) const {
        return do_hash (__low, __high);
    }

    static _RW::__rw_facet_id id;

protected:

    // protected virtual members are not defined
    virtual int
    do_compare (const char_type*, const char_type*,
                const char_type*, const char_type*) const;
        
    virtual string_type
    do_transform (const char_type*, const char_type*) const;

    virtual long
    do_hash (const char_type*, const char_type*) const;
};

#endif   // _RWSTD_NO_EXT_COLLATE_PRIMARY


_RWSTD_SPECIALIZED_CLASS
class _RWSTD_EXPORT collate<char>: public _RW::__rw_facet
{
public:
    typedef char char_type;
    typedef basic_string<char_type, char_traits<char_type>,
                         allocator<char_type> >
    string_type;

    explicit collate (_RWSTD_SIZE_T __refs = 0)
        : _RW::__rw_facet (__refs) { }

    virtual ~collate () _RWSTD_ATTRIBUTE_NOTHROW;

    int
    compare (const char_type* __low1, const char_type* __high1,
             const char_type* __low2, const char_type* __high2) const {
        return do_compare (__low1, __high1, __low2, __high2);
    }

    string_type
    transform (const char_type* __low, const char_type* __high) const {
        return do_transform (__low, __high);
    }

    long
    hash (const char_type* __low, const char_type* __high) const {
        return do_hash (__low, __high);
    }

    static _RW::__rw_facet_id id;

protected:

    virtual int
    do_compare (const char_type*, const char_type*,
                const char_type*, const char_type*) const;
        
    virtual string_type
    do_transform (const char_type*, const char_type*) const;

    virtual long
    do_hash (const char_type*, const char_type*) const;
};


#ifndef   _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
class _RWSTD_EXPORT collate<wchar_t>: public _RW::__rw_facet
{
public:
    typedef wchar_t char_type;
    typedef basic_string<char_type, char_traits<char_type>,
                         allocator<char_type> >
    string_type;

    explicit collate (_RWSTD_SIZE_T __refs = 0) _THROWS (())
        : _RW::__rw_facet (__refs) { }

    virtual ~collate () _RWSTD_ATTRIBUTE_NOTHROW;

    int
    compare (const char_type* __low1, const char_type* __high1,
             const char_type* __low2, const char_type* __high2) const {
        return do_compare (__low1, __high1, __low2, __high2);
    }

    string_type
    transform (const char_type* __low, const char_type* __high) const {
        return do_transform (__low, __high);
    }

    long
    hash (const char_type* __low, const char_type* __high) const {
        return do_hash (__low, __high);
    }

    static _RW::__rw_facet_id id;

protected:

    virtual int
    do_compare (const char_type*, const char_type*,
                const char_type*, const char_type*) const;

    virtual string_type
    do_transform (const char_type*, const char_type*) const;

    virtual long
    do_hash (const char_type*, const char_type*) const;
};

#endif   // _RWSTD_NO_WCHAR_T


template <class _CharT>
class collate_byname: public collate<_CharT>
{
    char _C_namebuf [32];

public:

    explicit collate_byname (const char *__name, _RWSTD_SIZE_T __ref = 0)
        : collate<_CharT>(__ref) {
        this->_C_set_name (__name, _C_namebuf, sizeof _C_namebuf);
    }
};


_RWSTD_SPECIALIZED_CLASS
class _RWSTD_EXPORT collate_byname<char>: public collate<char>
{
    char _C_namebuf [32];
public:

    explicit collate_byname (const char* __name, _RWSTD_SIZE_T __ref = 0)
        :  collate<char_type>(__ref){ 
        this->_C_set_name (__name, _C_namebuf, sizeof _C_namebuf);
    }

    virtual ~collate_byname () _RWSTD_ATTRIBUTE_NOTHROW;

protected:

    virtual int
    do_compare (const char_type*, const char_type*,
                const char_type*, const char_type*) const;

    virtual string_type
    do_transform (const char_type*, const char_type*) const;
};


#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
class _RWSTD_EXPORT collate_byname<wchar_t>: public collate<wchar_t>
{
    char _C_namebuf [32];
public:

    explicit collate_byname (const char* __name, _RWSTD_SIZE_T __ref = 0)
        :  collate<char_type>(__ref){ 
        this->_C_set_name (__name, _C_namebuf, sizeof _C_namebuf);
    }

    virtual ~collate_byname () _RWSTD_ATTRIBUTE_NOTHROW;

protected:

    virtual int
    do_compare (const char_type*, const char_type*,
                const char_type*, const char_type*) const;

    virtual string_type
    do_transform (const char_type*, const char_type*) const;
};

#endif   // _RWSTD_NO_WCHAR_T

}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE (_COLLATE)
#  include <loc/_collate.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE (_COLLATE)


#endif   // _RWSTD_LOC_COLLATE_H_INCLUDED
