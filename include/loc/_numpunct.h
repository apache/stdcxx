/***************************************************************************
 *
 * _numpunct.h - definition of the std::numpunct class templates
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

#ifndef _RWSTD_LOC_NUMPUNCT_H_INCLUDED
#define _RWSTD_LOC_NUMPUNCT_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#include <string>
#include <loc/_facet.h>
#include <loc/_punct.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


// FIXME: provide just required specializations when #defined
// #ifndef _RWSTD_NO_EXT_NUMPUNCT_PRIMARY

// 22.2.3.1
_EXPORT
template <class _CharT>
struct numpunct: _RW::__rw_facet
{
    typedef _CharT char_type;
    typedef
    basic_string<char_type, char_traits<char_type>, allocator<char_type> >
    string_type;

    explicit numpunct (_RWSTD_SIZE_T __ref = 0)
        : _RW::__rw_facet (__ref), _C_flags (0) { }

    virtual ~numpunct ();

    // 22.2.3.1.1, p1
    char_type decimal_point () const;

    // 22.2.3.1.1, p2
    char_type thousands_sep () const;

    // 22.2.3.1.1, p3
    string grouping () const;

    // 22.2.3.1.1, p4
    string_type truename () const;

    // 22.2.3.1.1, p4
    string_type falsename () const;

    static _RW::__rw_facet_id id;

protected:

    // 22.2.3.1.2, p1
    virtual char_type do_decimal_point () const {
        return *_RW::__rw_get_punct (this, _RW::__rw_dp, char_type ());
    }

    // 22.2.3.1.2, p2
    virtual char_type do_thousands_sep () const {
        return *_RW::__rw_get_punct (this, _RW::__rw_ts, char_type ());
    }

    // 22.2.3.1.2, p3
    virtual string do_grouping () const {
        return _RWSTD_REINTERPRET_CAST (const char*,
                   _RW::__rw_get_punct (this, _RW::__rw_gr, char_type ()));
    }

    // 22.2.3.1.2, p4
    virtual string_type do_truename () const {
        return _RW::__rw_get_punct (this, _RW::__rw_tn, char_type ());
    }

    // 22.2.3.1.2, p4
    virtual string_type do_falsename () const {
        return _RW::__rw_get_punct (this, _RW::__rw_fn, char_type ());
    }

private:

    int         _C_flags;           // bitmap of "cached data valid" flags
    string      _C_grouping;        // cached results of virtual members
    string_type _C_truename;
    string_type _C_falsename;
    char_type   _C_decimal_point;
    char_type   _C_thousands_sep;
};


#ifndef _RWSTD_NO_SPECIALIZED_FACET_ID

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id numpunct<char>::id;

#  ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id numpunct<wchar_t>::id;

#  endif   // _RWSTD_NO_WCHAR_T
#endif   // _RWSTD_NO_SPECIALIZED_FACET_ID


template <class _CharT>
inline typename numpunct<_CharT>::char_type
numpunct<_CharT>::decimal_point () const
{
    if (!(_C_flags & _RW::__rw_dp)) {

        numpunct* const __self = _RWSTD_CONST_CAST (numpunct*, this);

        // [try to] get the decimal point first (may throw)
        // then set a flag to avoid future initializations
        __self->_C_decimal_point  = do_decimal_point ();
        __self->_C_flags         |= _RW::__rw_dp;
    }

    return _C_decimal_point;
}


template <class _CharT>
inline typename numpunct<_CharT>::char_type
numpunct<_CharT>::thousands_sep () const
{
    if (!(_C_flags & _RW::__rw_ts)) {

        numpunct* const __self = _RWSTD_CONST_CAST (numpunct*, this);

        // [try to] get the thousands_sep first (may throw)
        // then set a flag to avoid future initializations
        __self->_C_thousands_sep  = do_thousands_sep ();
        __self->_C_flags         |= _RW::__rw_ts;
    }

    return _C_thousands_sep;
}


template <class _CharT>
inline string numpunct<_CharT>::grouping () const
{
    if (!(_C_flags & _RW::__rw_gr)) {

        numpunct* const __self = _RWSTD_CONST_CAST (numpunct*, this);

        // [try to] get the grouping first (may throw)
        // then set a flag to avoid future initializations
        __self->_C_grouping  = do_grouping ();
        __self->_C_flags    |= _RW::__rw_gr;
    }

    return _C_grouping;
}


template <class _CharT>
inline typename numpunct<_CharT>::string_type
numpunct<_CharT>::truename () const
{
    if (!(_C_flags & _RW::__rw_tn)) {

        numpunct* const __self = _RWSTD_CONST_CAST (numpunct*, this);

        // [try to] get the true name first (may throw)
        // then set a flag to avoid future initializations
        __self->_C_truename  = do_truename ();
        __self->_C_flags    |= _RW::__rw_tn;
    }

    return _C_truename;
}


template <class _CharT>
inline typename numpunct<_CharT>::string_type
numpunct<_CharT>::falsename () const
{
    if (!(_C_flags & _RW::__rw_fn)) {

        numpunct* const __self = _RWSTD_CONST_CAST (numpunct*, this);

        // [try to] get the false name first (may throw)
        // then set a flag to avoid future initializations
        __self->_C_falsename  = do_falsename ();
        __self->_C_flags     |= _RW::__rw_fn;
    }

    return _C_falsename;
}

// #endif _RWSTD_NO_EXT_NUMPUNCT_PRIMARY


// 22.2.3.2
template <class _CharT>
class numpunct_byname: public numpunct<_CharT>
{
    char _C_namebuf [32];

public:

    explicit numpunct_byname (const char *__name, _RWSTD_SIZE_T __refs = 0)
        : numpunct<_CharT>(__refs) {
        this->_C_set_name (__name, _C_namebuf, sizeof _C_namebuf);
    }

    virtual ~numpunct_byname () _RWSTD_ATTRIBUTE_NOTHROW;
};

}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE_FIRST (_NUMPUNCT)
#  include <loc/_numpunct.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (_NUMPUNCT)


#ifdef _RWSTD_MSVC
#  pragma warning (push)
#  pragma warning (disable: 4231)
#endif   // _RWSTD_MSVC


_RWSTD_NAMESPACE (std) { 

#if _RWSTD_INSTANTIATE (_NUMPUNCT, _CHAR)

_RWSTD_INSTANTIATE_1 (struct _RWSTD_TI_EXPORT numpunct<char>);

#endif   // _RWSTD_INSTANTIATE (_NUMPUNCT, _CHAR)

#if _RWSTD_INSTANTIATE (_NUMPUNCT, _WCHAR_T)

_RWSTD_INSTANTIATE_1 (struct _RWSTD_TI_EXPORT numpunct<wchar_t>);

#endif   // _RWSTD_INSTANTIATE (_NUMPUNCT, _WCHAR_T)

}   // namespace std


#ifdef _RWSTD_MSVC
#  pragma warning (pop)
#endif   // _RWSTD_MSVC


#if _RWSTD_DEFINE_TEMPLATE_LAST (_NUMPUNCT)
#  include <loc/_numpunct.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_LAST (_NUMPUNCT)


#endif   // _RWSTD_LOC_NUMPUNCT_H_INCLUDED
