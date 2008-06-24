/***************************************************************************
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
 * Copyright 2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#ifndef _RWSTD_RW_META_CAT_H_INCLUDED
#define _RWSTD_RW_META_CAT_H_INCLUDED

#include <rw/_defs.h>
#include <rw/_meta_help.h>

_RWSTD_NAMESPACE (__rw) {

#define _RWSTD_TRAIT_SPEC_0_CV(Trait,Type,Cond)                         \
    template <> struct Trait<Type>                                      \
      : __rw_integral_constant<bool, Cond> { };                         \
    template <> struct Trait<Type const>                                \
      : __rw_integral_constant<bool, Cond> { };                         \
    template <> struct Trait<Type volatile>                             \
      : __rw_integral_constant<bool, Cond> { };                         \
    template <> struct Trait<Type const volatile>                       \
      : __rw_integral_constant<bool, Cond> { }

#define _RWSTD_TRAIT_SPEC_1_CV(Trait,Type,Cond)                         \
    template <class _TypeT> struct Trait<Type>                          \
      : __rw_integral_constant<bool, Cond> { };                         \
    template <class _TypeT> struct Trait<Type const>                    \
      : __rw_integral_constant<bool, Cond> { };                         \
    template <class _TypeT> struct Trait<Type volatile>                 \
      : __rw_integral_constant<bool, Cond> { };                         \
    template <class _TypeT> struct Trait<Type const volatile>           \
      : __rw_integral_constant<bool, Cond> { }

#define _RWSTD_TRAIT_SPEC_2_CV(Trait,Type,Cond)                         \
    template <class _TypeT, class _ClassT> struct Trait<Type>           \
      : __rw_integral_constant<bool, Cond> { };                         \
    template <class _TypeT, class _ClassT> struct Trait<Type const>     \
      : __rw_integral_constant<bool, Cond> { };                         \
    template <class _TypeT, class _ClassT> struct Trait<Type volatile>  \
      : __rw_integral_constant<bool, Cond> { };                         \
    template <class _TypeT, class _ClassT>                              \
    struct Trait<Type const volatile>                                   \
      : __rw_integral_constant<bool, Cond> { }

template <class _TypeT>
struct __rw_is_void : __rw_false_type
{
};

_RWSTD_TRAIT_SPEC_0_CV(__rw_is_void, void, true);

#define _RWSTD_IS_VOID(T) _RW::__rw_is_void<T>::value


template <class _TypeT>
struct __rw_is_integral : __rw_false_type
{
};

_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral, bool, true);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral, char, true);

#if 0
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral, char16_t, true);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral, char32_t, true);
#endif   // 0

#ifndef _RWSTD_NO_NATIVE_WCHAR_T
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral, wchar_t, true);
#endif   // _RWSTD_NO_NATIVE_WCHAR_T

_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral,    signed char, true);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral,  unsigned char, true);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral,   signed short, true);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral, unsigned short, true);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral,     signed int, true);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral,   unsigned int, true);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral,    signed long, true);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral,  unsigned long, true);

#ifndef _RWSTD_NO_LONG_LONG
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral,   signed long long, true);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_integral, unsigned long long, true);
#endif   // _RWSTD_NO_LONG_LONG

#define _RWSTD_IS_INTEGRAL(T) _RW::__rw_is_integral<T>::value

template <class _TypeT>
struct __rw_is_floating_point : __rw_false_type
{
};

_RWSTD_TRAIT_SPEC_0_CV(__rw_is_floating_point, float, true);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_floating_point, double, true);

#ifndef _RWSTD_NO_LONG_DOUBLE
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_floating_point, long double, true);
#endif    // _RWSTD_NO_LONG_DOUBLE

#define _RWSTD_IS_FLOATING_POINT(T) _RW::__rw_is_floating_point<T>::value


template <class _TypeT>
struct __rw_is_array : __rw_false_type
{
};

template <class _TypeT, _RWSTD_SIZE_T _Size>
struct __rw_is_array<_TypeT [_Size]> : __rw_true_type
{
};

template <class _TypeT>
struct __rw_is_array<_TypeT []> : __rw_true_type
{
};

#define _RWSTD_IS_ARRAY(T) _RW::__rw_is_array<T>::value

template <class _TypeT>
struct __rw_is_pointer : __rw_false_type
{
};

_RWSTD_TRAIT_SPEC_1_CV(__rw_is_pointer, _TypeT*, true);

#define _RWSTD_IS_POINTER(T) _RW::__rw_is_pointer<T>::value


template <class _TypeT>
struct __rw_is_lvalue_reference : __rw_false_type
{
};

template <class _TypeT>
struct __rw_is_lvalue_reference<_TypeT&> : __rw_true_type
{
};

#define _RWSTD_IS_LVALUE_REFERENCE(T) \
    _RW::__rw_is_lvalue_reference<T>::value


template <class _TypeT>
struct __rw_is_rvalue_reference : __rw_false_type
{
};


#ifndef _RWSTD_NO_RVALUE_REFERENCES

template <class _TypeT>
struct __rw_is_rvalue_reference<_TypeT&&> : __rw_true_type
{
};

#endif   // _RWSTD_NO_RVALUE_REFERENCES

#define _RWSTD_IS_RVALUE_REFERENCE(T) \
    _RW::__rw_is_rvalue_reference<T>::value


template <class _TypeT>
struct __rw_is_enum
    : __rw_integral_constant<bool, _RWSTD_TT_IS_ENUM(_TypeT)>
{
};

#define _RWSTD_IS_ENUM(T) _RW::__rw_is_enum<T>::value

template <class _TypeT>
struct __rw_is_union
    : __rw_integral_constant<bool, _RWSTD_TT_IS_UNION(_TypeT)>
{
};

#define _RWSTD_IS_UNION(T) _RW::__rw_is_union<T>::value


template <class _TypeT>
struct __rw_is_class
    : __rw_integral_constant<bool, _RWSTD_TT_IS_CLASS(_TypeT)>
{
};

#define _RWSTD_IS_CLASS(T) _RW::__rw_is_class<T>::value


template <class _TypeT, bool =    __rw_is_void<_TypeT>::value
                               || __rw_is_array<_TypeT>::value
                               || __rw_is_lvalue_reference<_TypeT>::value
                               || __rw_is_rvalue_reference<_TypeT>::value>
struct __rw_is_function_impl
{
    enum { _C_value = 0 };
};

template <class _TypeT>
struct __rw_is_function_impl<_TypeT, false>
{
    typedef _TypeT& _TypeT_Ref;
    typedef _TypeT* _TypeT_Ptr;

    struct _C_no  { };
    struct _C_yes { _C_no __pad [2]; };

    static _C_yes _C_is (int, _TypeT_Ptr);
    static _C_no  _C_is (int, ...);

    static _TypeT_Ref _C_make ();

    enum { _C_value = sizeof (_C_yes) == sizeof (_C_is (0, _C_make ())) };
};

template <class _TypeT>
struct __rw_is_function
#ifdef _RWSTD_TT_IS_FUNCTION
    : __rw_integral_constant<bool, _RWSTD_TT_IS_FUNCTION(_TypeT)>
#else
    : __rw_integral_constant<bool, __rw_is_function_impl<_TypeT>::_C_value>
#endif   // _RWSTD_TT_IS_FUNCTION
{
};

#define _RWSTD_IS_FUNCTION(T) _RW::__rw_is_function<T>::value

template <class _TypeT>
struct __rw_is_member_object_pointer : __rw_false_type
{
};

_RWSTD_TRAIT_SPEC_2_CV(__rw_is_member_object_pointer,
                       _TypeT _ClassT::*,
                       !__rw_is_function<_TypeT>::value);

#define _RWSTD_IS_MEMBER_OBJECT_PTR(T) \
    _RW::__rw_is_member_object_pointer<T>::value


template <class _TypeT>
struct __rw_is_member_function_pointer : __rw_false_type
{
};

_RWSTD_TRAIT_SPEC_2_CV(__rw_is_member_function_pointer,
                       _TypeT _ClassT::*,
                       __rw_is_function<_TypeT>::value);

#define _RWSTD_IS_MEMBER_FUNCTION_PTR(T) \
    _RW::__rw_is_member_function_pointer<T>::value

#undef _RWSTD_TRAIT_SPEC_2_CV
#undef _RWSTD_TRAIT_SPEC_1_CV
#undef _RWSTD_TRAIT_SPEC_0_CV

} // namespace __rw


#endif   // _RWSTD_RW_META_CAT_H_INCLUDED

