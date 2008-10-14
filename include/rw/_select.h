/***************************************************************************
 *
 * _select.h - Definitions of helper templates
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_SELECT_H_INCLUDED
#define _RWSTD_RW_SELECT_H_INCLUDED

#ifndef _RWSTD_RW_DEFS_H_INCLUDED
#  include <rw/_defs.h>
#endif   // _RWSTD_RW_DEFS_H_INCLUDED


_RWSTD_NAMESPACE (__rw) {

struct __rw_false_t { enum { _C_val }; };
struct __rw_true_t { enum { _C_val = 1 }; };


template <class _TypeT>
struct __rw_select_int
{
    typedef void* _SelectT;

#if defined (__SUNPRO_CC) && __SUNPRO_CC <= 0x530

    void* _C_selector;

#endif   // SunPro
};


#define _RWSTD_SPECIALIZE_IS_INT(T)    \
    _RWSTD_SPECIALIZED_CLASS           \
    struct __rw_select_int<T> {        \
        typedef int _SelectT;          \
        enum { _C_selector = 1 };      \
    }

#ifndef _RWSTD_NO_NATIVE_BOOL
_RWSTD_SPECIALIZE_IS_INT (bool);
#endif   // _RWSTD_NO_NATIVE_BOOL

_RWSTD_SPECIALIZE_IS_INT (char);
_RWSTD_SPECIALIZE_IS_INT (signed char);
_RWSTD_SPECIALIZE_IS_INT (unsigned char);
_RWSTD_SPECIALIZE_IS_INT (short);
_RWSTD_SPECIALIZE_IS_INT (unsigned short);
_RWSTD_SPECIALIZE_IS_INT (int);
_RWSTD_SPECIALIZE_IS_INT (unsigned int);
_RWSTD_SPECIALIZE_IS_INT (long);
_RWSTD_SPECIALIZE_IS_INT (unsigned long);

#ifdef _RWSTD_LONG_LONG
_RWSTD_SPECIALIZE_IS_INT (_RWSTD_LONG_LONG);
_RWSTD_SPECIALIZE_IS_INT (unsigned _RWSTD_LONG_LONG);
#endif   // _RWSTD_LONG_LONG

#ifndef _RWSTD_NO_NATIVE_WCHAR_T
_RWSTD_SPECIALIZE_IS_INT (wchar_t);
#endif   // _RWSTD_NO_NATIVE_WCHAR_T


#if !defined (__SUNPRO_CC) || __SUNPRO_CC > 0x530
#  define _RWSTD_DISPATCH(iter)   \
       (typename _RW::__rw_select_int< iter >::_SelectT (1L))

#else
   // working around a SunPro 5.3 a prior bug (PR #28739)
#  define _RWSTD_DISPATCH(iter) _RW::__rw_select_int< iter >()._C_selector
#endif


#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC

template <class _TypeT, class _TypeU>
struct __rw_same_type
{
    typedef __rw_false_t _C_type;
    enum { _C_val };
};

template <class _TypeT>
struct __rw_same_type<_TypeT, _TypeT>
{
    typedef __rw_true_t _C_type;
    enum { _C_val = 1 };
};

#else   // if defined (_RWSTD_NO_CLASS_PARTIAL_SPEC)

template <bool flag>
struct __rw_bool_t
{
    typedef __rw_false_t _C_type;
};

_RWSTD_SPECIALIZED_CLASS
struct __rw_bool_t<true>
{
    typedef __rw_true_t _C_type;
};

template <class _TypeT, class _TypeU>
struct __rw_same_type
{
    struct _C_yes {};
    struct _C_no { _C_yes no_ [2]; };
    template <class _TypeV>
    struct _C_Type {};

    static _C_yes test (_C_Type<_TypeT>, _C_Type<_TypeT>);
    static _C_no test (...);

    enum { _C_val = sizeof (test (_C_Type<_TypeT> (),
                                  _C_Type<_TypeU> ())) == sizeof (_C_yes) };

    typedef typename __rw_bool_t<_C_val>::_C_type _C_type;
};

#endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC


}   // namespace __rw


#  if defined (__IBMCPP__) && __IBMCPP__ <= 500

// working around an xlC 5.0 bug
_USING (__rw::__rw_select_int);

#  endif   // VisualAge 5.0


#endif   // _RWSTD_RW_SELECT_H_INCLUDED
