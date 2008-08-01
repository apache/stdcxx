// -*- C++ -*-
/***************************************************************************
 *
 * _typetraits.h - definitions of type traits helper templates
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

#ifndef _RWSTD_TYPETRAITS_H_INCLUDED
#define _RWSTD_TYPETRAITS_H_INCLUDED


#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) { 


template <class _TypeT>
struct __rw_is_const
{
    typedef _TypeT _C_type;
    enum { _C_val = 0 };
};

template <class _TypeT>
struct __rw_is_const<const _TypeT>
{
    typedef _TypeT _C_type;
    enum { _C_val = 1 };
};


template <class _TypeT>
struct __rw_is_volatile
{
    typedef _TypeT _C_type;
    enum { _C_val = 0 };
};

template <class _TypeT>
struct __rw_is_volatile<volatile _TypeT>
{
    typedef _TypeT _C_type;
    enum { _C_val = 1 };
};


template <class _TypeT>
struct __rw_is_cv_qualified
{
    typedef _TypeT _C_type;
    enum { _C_val = 0 };
};

template <class _TypeT>
struct __rw_is_cv_qualified<const _TypeT>
{
    typedef _TypeT _C_type;
    enum { _C_val = 1 };
};

template <class _TypeT>
struct __rw_is_cv_qualified<volatile _TypeT>
{
    typedef _TypeT _C_type;
    enum { _C_val = 1 };
};

template <class _TypeT>
struct __rw_is_cv_qualified<const volatile _TypeT>
{
    typedef _TypeT _C_type;
    enum { _C_val = 1 };
};


template <class _TypeT>
struct __rw_is_pointer
{
    typedef _TypeT _C_type;
    enum { _C_val = 0 };
};

template <class _TypeT>
struct __rw_is_pointer<_TypeT*>
{
    typedef _TypeT _C_type;
    enum { _C_val = 1 };
};


template <class _TypeT>
struct __rw_is_reference
{
    typedef _TypeT _C_type;
    enum { _C_val = 0 };
};

template <class _TypeT>
struct __rw_is_reference<_TypeT&>
{
    typedef _TypeT _C_type;
    enum { _C_val = 1 };
};


template <class _TypeT>
struct __rw_is_fundamental
{
    enum { _C_val = __rw_is_pointer<_TypeT>::_C_val };
};

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<unsigned char> { enum { _C_val = 1 }; };

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<signed char> { enum { _C_val = 1 }; };

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<char> { enum { _C_val = 1 }; };

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<unsigned short> { enum { _C_val = 1 }; };

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<signed short> { enum { _C_val = 1 }; };

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<unsigned int> { enum { _C_val = 1 }; };

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<signed int> { enum { _C_val = 1 }; };

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<unsigned long> { enum { _C_val = 1 }; };

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<signed long> { enum { _C_val = 1 }; };

#ifdef _RWSTD_LONG_LONG

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<unsigned _RWSTD_LONG_LONG> { enum { _C_val = 1 }; };

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<signed _RWSTD_LONG_LONG> { enum { _C_val = 1 }; };

#endif   // _RWSTD_LONG_LONG

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<float> { enum { _C_val = 1 }; };

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<double> { enum { _C_val = 1 }; };

#ifndef _RWSTD_NO_LONG_DOUBLE

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<long double> { enum { _C_val = 1 }; };

#endif   // _RWSTD_NO_LONG_DOUBLE

#ifndef _RWSTD_NO_BOOL

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<bool> { enum { _C_val = 1 }; };

#endif   // _RWSTD_NO_BOOL

#ifndef _RWSTD_NO_NATIVE_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<wchar_t> { enum { _C_val = 1 }; };

#endif   // _RWSTD_NO_NATIVE_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
struct __rw_is_fundamental<void> { enum { _C_val = 1 }; };


template <class _TypeT>
struct __rw_is_enum
{
    struct _C_no { };
    struct _C_yes { int _C_dummy [2]; };

    struct _C_indirect {
        // prevent classes with user-defined conversions from matching

        // use double to prevent float->int gcc conversion warnings
        _C_indirect (double);
    };

    // nested struct gets rid of bogus gcc errors
    struct _C_nest {
        // supply first argument to prevent HP aCC warnings
        static _C_no _C_is (int, ...);
        static _C_yes _C_is (int, _C_indirect);

        static _TypeT _C_make_T ();
    };

#if !defined (__HP_aCC) || __HP_aCC > 33000

    enum {
        _C_val =       sizeof (_C_yes)
                    == sizeof (_C_nest::_C_is (0, _C_nest::_C_make_T ()))
                 && !__rw_is_fundamental<_TypeT>::_C_val
    };

#else

    // prevent references from causing an error
    typedef typename __rw_is_reference<_TypeT>::_C_type _C_type;

    // working around an HP aCC bug (see PR #25347)
    // NOTE: this fails for classes due to another bug (PR #25384)
    static const bool
    _C_val =    sizeof (_C_yes) == sizeof (_C_nest::_C_is (0, _C_type ()))
             && !__rw_is_fundamental<_TypeT>::_C_val;

#endif   // __HP_aCC > 33000

};


#if defined (__HP_aCC) && __HP_aCC <= 33000

template <class _TypeT>
const bool __rw_is_enum<_TypeT>::_C_val;

#endif   // __HP_aCC <= 33000


_RWSTD_SPECIALIZED_CLASS
struct __rw_is_enum<void>
{
    enum { _C_val = 0 };
};


template <class _TypeT>
struct __rw_type_traits
{
    typedef typename __rw_is_const<_TypeT>::_C_type     _C_non_const_type;
    typedef typename __rw_is_volatile<_TypeT>::_C_type  _C_non_volatile_type;
    typedef typename __rw_is_pointer<_TypeT>::_C_type   _C_non_pointer_type;
    typedef typename __rw_is_reference<_TypeT>::_C_type _C_non_reference_type;

    typedef typename __rw_is_cv_qualified<_TypeT>::_C_type
    _C_non_cv_qualified_type;

    enum {
        _C_is_const        = __rw_is_const<_TypeT>::_C_val,
        _C_is_volatile     = __rw_is_volatile<_TypeT>::_C_val,
        _C_is_cv_qualified = __rw_is_cv_qualified<_TypeT>::_C_val,
        _C_is_pointer      = __rw_is_pointer<_TypeT>::_C_val,
        _C_is_reference    = __rw_is_reference<_TypeT>::_C_val,
        _C_is_fundamental  = __rw_is_fundamental<_TypeT>::_C_val,
        _C_is_enum         = __rw_is_enum<_TypeT>::_C_val,
        _C_is_class        =    !_C_is_pointer
                             && !_C_is_reference
                             && !_C_is_fundamental
                             && !_C_is_enum
    };
};


}   // namespace __rw


#endif   // _RWSTD_TYPETRAITS_H_INCLUDED
