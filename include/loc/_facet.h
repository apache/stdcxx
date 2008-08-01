/***************************************************************************
 *
 * _facet.h - definition of the std::locale::facet and locale::id classes
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

#ifndef _RWSTD_LOC_FACET_H_INCLUDED
#define _RWSTD_LOC_FACET_H_INCLUDED


#include <rw/_mutex.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 

struct _RWSTD_CLASS_EXPORT locale;

}   // namespace std


_RWSTD_NAMESPACE (__rw) { 

class _RWSTD_EXPORT __rw_locale;

struct __rw_access;

struct _RWSTD_EXPORT __rw_facet: __rw_synchronized
{
    // the type of a "constructor" function used to create facet objects
    // of standard types by the library
    typedef __rw_facet* (_C_ctor_t)(_RWSTD_SIZE_T, const char*);

#if !defined (_MSC_VER) || 5 <= _RWSTD_VER_MAJOR

    // disabled for binary compatibility with stdcxx 4.1.x for MSVC
    // (the compiler mangles access specifiers into class members)
protected:

#endif   // !MSVC || 5 <= stdcxx version

    explicit __rw_facet (_RWSTD_SIZE_T = 0);

    virtual ~__rw_facet ();

public:

    // types of standard facets; only globally managed facets (i.e.,
    // standard facets created internally) set this member, user-created
    // facet objects of any types set their type to _C_unknown

    // NOTE: the values and order of all the enumerators below are important
    //       any changes may affect the binary compatibility of the library

    enum _C_facet_type {
        /* -1 */ _C_invalid = -1,   // marked e.g., when deleted
        /*  0 */ _C_unknown =  0,   // user-defined

        // narrow character specializations

        // classic versions           // byname versions
        //
        /* id */ // <-- facet id, i.e. the value of Facet::id._C_val

        // collate category
        /*  1 */ _C_collate,          _C_collate_byname,
        // ctype category
        /*  3 */ _C_codecvt,          _C_codecvt_byname,
        /*  5 */ _C_ctype,            _C_ctype_byname,
        // monetary category
        /*  7 */ _C_moneypunct,       _C_moneypunct_byname,
        /*  9 */ _C_moneypunct_intl,  _C_moneypunct_intl_byname,
        /* 11 */ _C_money_get,        _C_nonexistent_0,
        /* 13 */ _C_money_put,        _C_nonexistent_1,
        // numeric category
        /* 15 */ _C_numpunct,         _C_numpunct_byname,
        /* 17 */ _C_num_get,          _C_nonexistent_2,
        /* 19 */ _C_num_put,          _C_nonexistent_3,
        // time category
        /* 21 */ _C_time_get,         _C_time_get_byname,
        /* 23 */ _C_time_put,         _C_time_put_byname,
        // messages category
        /* 25 */ _C_messages,         _C_messages_byname,

        // wide character specializations

        // classic versions           // byname versions
        /* 27 */ _C_wcollate,         _C_wcollate_byname,
        /* 29 */ _C_wcodecvt,         _C_wcodecvt_byname,
        /* 31 */ _C_wctype,           _C_wctype_byname,
        /* 33 */ _C_wmoneypunct,      _C_wmoneypunct_byname,
        /* 35 */ _C_wmoneypunct_intl, _C_wmoneypunct_intl_byname,
        /* 37 */ _C_wmoney_get,       _C_nonexistent_4,
        /* 39 */ _C_wmoney_put,       _C_nonexistent_5,
        /* 41 */ _C_wnumpunct,        _C_wnumpunct_byname,
        /* 43 */ _C_wnum_get,         _C_nonexistent_6,
        /* 45 */ _C_wnum_put,         _C_nonexistent_7,
        /* 47 */ _C_wtime_get,        _C_wtime_get_byname,
        /* 49 */ _C_wtime_put,        _C_wtime_put_byname,
        /* 51 */ _C_wmessages,        _C_wmessages_byname,

        // the value of each standard facet's id is computed
        // as (facet_type + 1) / 2; the value is stored in
        // each facet's *__rw_facet::_C_pid member

        // the value of the first available (non-standard)
        // facet's id is computed as (_C_last_type / 2 + 1)

        // note that this value determines the binary footprint of each facet
        // changing it affects the binary compatibility of the localization
        // library
        /* 52 */ _C_last_type = _C_wmessages_byname
    };

    // unique fundamental type id
    enum _C_type_id {
        _C_signed   =  0x100, _C_ptr = 0x200, _C_cont = 0x400,
        _C_integral = 0x800, _C_floating = 0x1000, _C_typemask = 0x00ff,
        _C_bool    =  0,
        _C_char    =  1 | _C_integral,
        _C_uchar   =  2 | _C_integral, _C_schar = _C_uchar  | _C_signed,
        _C_ushort  =  3 | _C_integral, _C_short = _C_ushort | _C_signed,
        _C_uint    =  4 | _C_integral, _C_int   = _C_uint   | _C_signed,
        _C_ulong   =  5 | _C_integral, _C_long  = _C_ulong  | _C_signed,
        _C_ullong  =  6 | _C_integral, _C_llong = _C_ullong | _C_signed,
        _C_float   =  7 | _C_floating,
        _C_double  =  8 | _C_floating,
        _C_ldouble =  9 | _C_floating,
        _C_void    = 10, _C_pvoid = _C_void | _C_ptr,
        _C_wchar_t = 11
    };

    enum {
        // SunOS prepends the separator charater to the beginning
        // of locale names, e.g., "/en_US/de_DE/fr_FR/..."
        _C_prepend_cat_sep = 0x01,

        // Linux uses the names of LC_XXX constants in combined
        // locale names,
        // e.g., "LC_CYPE=en_US;LC_COLLATE=de_DE;LC_MONETARY=fr_FR..."
        _C_use_cat_names = 0x02,

        // HP-UX, for instance, keeps even trivial locale names
        // fully expanded
        // (e.g., setlocale ("C") will return "C C C C C C")
        // and doesn't include category names in (combined or others)
        // locale names
        _C_condensed_name = 0x04,

        // bits to determine whether and when libc locale will be
        // used as an implementation of C++ locale functionality
        //
        // libc libstd effect
        //   0      0   use own implementation, fall back on libc if that fails
        //   0      1   use own implementation only, never libc
        //   1      0   use libc implementation only, never own
        //   1      1   try libc implementation first, own if that fails
        _C_use_libc   = 0x08,
        _C_use_libstd = 0x10
    };

    // global options controlling the behavior of locale and facets
    static int _C_opts;

    // returns a pointer to the facet's implementation data
    // if it exists, 0 otherwise
    const void* _C_data () const {
        return _C_impsize ? _C_impdata
            : _RWSTD_CONST_CAST (__rw_facet*, this)->_C_get_data ();
    }

    const char* _C_get_name () const {
        return _C_name ? _C_name : "C";
    }

protected:

    // set facet locale name (will allocate if bufsize is too small)
    void _C_set_name (const char*, char*, _RWSTD_SIZE_T);

    const char    *_C_name;       // name of locale associated with facet
    char          *_C_buf;        // character buffer name may be stored in
    const void    *_C_impdata;    // implementation data (e.g., punct)
    _RWSTD_SIZE_T  _C_impsize;    // size of implementation data

private:

    _C_facet_type  _C_type;       // facet type (see above)
    _RWSTD_SIZE_T  _C_ref;        // reference count (> 0)
    _RWSTD_SIZE_T *_C_pid;        // pointer to the static Facet::id._C_id

    // standard facets' id's are computed as (_C_type + 1) / 2
    // standard facet's base type is computed as 2 * Facet::id._C_id - 1

    __rw_facet (const __rw_facet&);       // not defined
    void operator= (const __rw_facet&);   // not defined

    const void* _C_get_data ();

    static __rw_facet*
    _C_manage (__rw_facet*, _C_facet_type, const char*, _C_ctor_t*);

    friend struct _STD::locale;
    friend class __rw_locale;
    friend struct __rw_facet_id;
    friend struct __rw_access;
};


struct _RWSTD_EXPORT __rw_facet_id
{
#ifdef _RWSTD_NO_SPECIALIZED_FACET_ID

    __rw_facet_id () {
        // _C_id member zero-initialized (i.e., invalidated) by the system
        // for __rw_facet_id objects with static storage duration, the only
        // ones locale ever references
    }

#else   // if !defined (_RWSTD_NO_SPECIALIZED_FACET_ID)

    enum _C_init_id { _C_Init };

    __rw_facet_id (_C_init_id = _C_Init) {
        // same as above except that Facet::id is explicitly specialized
        // for all standard facet base classes in order to guarantee one
        // instance of each in a program; the definition of the explicit
        // specialization is distinguished from the declaration by the
        // presence of an initializer
    }

#endif   // _RWSTD_NO_SPECIALIZED_FACET_ID

private:

    __rw_facet_id (const __rw_facet_id&);    // not defined
    void operator= (const __rw_facet_id&);   // not defined

    // initialize id to the given value if within the valid range
    // otherwise to the next value generated by the id generator
    _RWSTD_SIZE_T _C_init () const;

    _MUTABLE _RWSTD_SIZE_T _C_id;           // unique id > 0

    friend class __rw_locale;
    friend struct _STD::locale;
    friend struct __rw_access;
};


// does the necessary path calculations and mapping of locale databases
const void*
__rw_get_facet_data (int, _RWSTD_SIZE_T&, const char*, const char* = 0);

// its counterpart - does the database unmapping
void __rw_release_facet_data (const void*, _RWSTD_SIZE_T);

}   // namespace __rw


#endif   // _RWSTD_LOC_FACET_H_INCLUDED
