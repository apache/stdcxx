/***************************************************************************
 *
 * _moneypunct.h - definition of the std::moneypunct class templates
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

#ifndef _RWSTD_LOC_MONEYPUNCT_H_INCLUDED
#define _RWSTD_LOC_MONEYPUNCT_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#include <string>
#include <loc/_facet.h>
#include <loc/_punct.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


// 22.2.6.3
struct money_base
{
    enum part { none, space, symbol, sign, value };
    struct pattern { char field [4]; };
};


// 22.2.6.3
_EXPORT
template <class _CharT, bool _Intl = false>
struct moneypunct: _RW::__rw_facet, public money_base
{
    typedef _CharT char_type;
    typedef
    basic_string<char_type, char_traits<char_type>, allocator<char_type> >
    string_type;

    explicit moneypunct (_RWSTD_SIZE_T __refs = 0)
        : _RW::__rw_facet (__refs) { }

    virtual ~moneypunct () _RWSTD_ATTRIBUTE_NOTHROW;

    char_type decimal_point () const {
        return do_decimal_point ();
    }

    char_type thousands_sep () const {
        return do_thousands_sep ();
    }

    string grouping () const {
        return do_grouping ();
    }

    string_type curr_symbol () const {
        return do_curr_symbol ();
    }

    string_type positive_sign () const {
        return do_positive_sign ();
    }

    string_type negative_sign () const {
        return do_negative_sign ();
    }

    int frac_digits () const {
        return do_frac_digits ();
    }

    pattern pos_format () const {
        return do_pos_format ();
    }

    pattern neg_format () const {
        return do_neg_format ();
    }

    static _RW::__rw_facet_id id;

    _RWSTD_STATIC_CONST (bool, intl = _Intl);

protected:

    // 22.2.6.3.1, p1
    virtual char_type do_decimal_point () const {
        return *_C_get (this, _RW::__rw_dp);
    }

    // 22.2.6.3.1, p2
    virtual char_type do_thousands_sep () const {
        return *_C_get (this, _RW::__rw_ts);
    }

    // 22.2.6.3.1, p3
    virtual string do_grouping () const {
        return
            _RWSTD_REINTERPRET_CAST (const char*, _C_get (this, _RW::__rw_gr));
    }

    // 22.2.6.3.1, p4
    virtual string_type do_curr_symbol () const {
        return _C_get (this, _RW::__rw_cs);
    }

    // 22.2.6.3.1, p5
    virtual string_type do_positive_sign () const {
        return _C_get (this, _RW::__rw_ps);
    }

    // 22.2.6.3.1, p5
    virtual string_type do_negative_sign () const {
        return _C_get (this, _RW::__rw_ns);
    }

    // 22.2.6.3.1, p6
    virtual int do_frac_digits () const {
        return _RWSTD_STATIC_CAST(int, _RWSTD_REINTERPRET_CAST (_RWSTD_SIZE_T, 
                                           _C_get (this, _RW::__rw_fd)));
    }

    // 22.2.6.3.1, p7
    virtual pattern do_pos_format () const {
        return *_RWSTD_REINTERPRET_CAST (const pattern*,
                                         _C_get (this, _RW::__rw_pf));
    }

    // 22.2.6.3.1, p5
    virtual pattern do_neg_format () const {
        return *_RWSTD_REINTERPRET_CAST (const pattern*,
                                         _C_get (this, _RW::__rw_nf));
    }

private:

    static const char_type* _C_get (const _RW::__rw_facet*, int);
};


#ifndef _RWSTD_NO_SPECIALIZED_FACET_ID

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id moneypunct<char, false>::id;

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id moneypunct<char, true>::id;

#  ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id moneypunct<wchar_t, false>::id;

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id moneypunct<wchar_t, true>::id;

#  endif   // _RWSTD_NO_WCHAR_T
#endif   // _RWSTD_NO_SPECIALIZED_FACET_ID


template <class _CharT, bool _Intl /* = false */>
inline const _CharT*
moneypunct<_CharT, _Intl>::_C_get (const _RW::__rw_facet *__pf, int __fl)
{
    if (intl)
        __fl |= _RW::__rw_intl;

    __fl |= _RW::__rw_mon;

    return _RWSTD_STATIC_CAST (const char_type*,
                               _RW::__rw_get_punct(__pf, __fl, char_type()));
}


// 22.2.6.4
template <class _CharT, bool _Intl = false>
class moneypunct_byname: public moneypunct<_CharT, _Intl>
{
    char _C_namebuf [32];

public:

    explicit moneypunct_byname (const char *__name, _RWSTD_SIZE_T __ref = 0)
        : moneypunct<_CharT, _Intl>(__ref) {
            this->_C_set_name (__name, _C_namebuf, sizeof _C_namebuf);
        }

    virtual ~moneypunct_byname () _RWSTD_ATTRIBUTE_NOTHROW;
};


}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE_FIRST (_MONEYPUNCT)
#  include <loc/_moneypunct.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (_MONEYPUNCT)


#ifdef _RWSTD_MSVC
#  pragma warning (push)
#  pragma warning (disable: 4231)
#endif   // _RWSTD_MSVC


_RWSTD_NAMESPACE (std) { 

#if _RWSTD_INSTANTIATE (_MONEYPUNCT, _CHAR)

_RWSTD_INSTANTIATE_2 (struct _RWSTD_TI_EXPORT moneypunct<char, true>);
_RWSTD_INSTANTIATE_2 (struct _RWSTD_TI_EXPORT moneypunct<char, false>);

#endif   // _RWSTD_INSTANTIATE (_MONEYPUNCT, _CHAR)

#if _RWSTD_INSTANTIATE (_MONEYPUNCT, _WCHAR_T)

_RWSTD_INSTANTIATE_2 (struct _RWSTD_TI_EXPORT moneypunct<wchar_t, true>);
_RWSTD_INSTANTIATE_2 (struct _RWSTD_TI_EXPORT moneypunct<wchar_t, false>);

#endif   // _RWSTD_INSTANTIATE (_MONEYPUNCT, _WCHAR_T)

}   // namespace std


#ifdef _RWSTD_MSVC
#  pragma warning (pop)
#endif   // _RWSTD_MSVC


#if _RWSTD_DEFINE_TEMPLATE_LAST (_MONEYPUNCT)
#  include <loc/_moneypunct.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_LAST (_MONEYPUNCT)


#endif   // _RWSTD_LOC_MONEYPUNCT_H_INCLUDED
