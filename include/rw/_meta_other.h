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

#ifndef _RWSTD_META_OTHER_H_INCLUDED
#define _RWSTD_META_OTHER_H_INCLUDED

#include <rw/_defs.h>
#include <rw/_meta_cat.h>
#include <rw/_meta_ref.h>
#include <rw/_meta_ptr.h>

_RWSTD_NAMESPACE (__rw) {

template <_RWSTD_SIZE_T _Align>
struct __rw_aligned_storage_impl;

template <>
struct __rw_aligned_storage_impl<1>
{
    _RWSTD_UINT_LEAST8_T _C_type;
};

template <>
struct __rw_aligned_storage_impl<2>
{
    _RWSTD_UINT_LEAST16_T _C_type;
};

template <>
struct __rw_aligned_storage_impl<3>
{
    _RWSTD_UINT_LEAST32_T _C_type;
};

template <>
struct __rw_aligned_storage_impl<4>
{
    _RWSTD_UINT_LEAST32_T _C_type;
};

template <_RWSTD_SIZE_T _Len, _RWSTD_SIZE_T _Align = 4>
struct __rw_aligned_storage
{
    // the member typedef _C_type shall be a pod-type suitable for
    // use as uninitialized storage for any object whose size is at
    // most _Len and whose alignment is a divisor of _Align.
    //
    // i think they mean divisible by _Align. i.e. if _Align is 4, we
    // could return 4, 8 or 16 byte aligned data, but we could not
    // return 1 byte aligned data.        
    union {
        unsigned char __data [_Len];
        typename __rw_aligned_storage_impl<_Align>::_C_type __align;
    } type;
};


#ifndef _RWSTD_NO_VARIADIC_TEMPLATES

template <_RWSTD_SIZE_T _Len, class... Types>
struct __rw_aligned_union_impl;

template <_RWSTD_SIZE_T _Len, class Type, class... Types>
struct __rw_aligned_union_impl<_Len, Type, Types...>
{
    union {
        unsigned char __pad [_Len];
        typename __rw_aligned_union<_Len, Types...>::_C_type __align;
    } _C_type;
};

template <_RWSTD_SIZE_T _Len>
struct __rw_aligned_union_impl<_Len>
{
    union {
        unsigned char __pad [_Len];
    } _C_type;
};

template <_RWSTD_SIZE_T _Len, class... Types>
struct __rw_aligned_union
{
    typedef typename
    __rw_aligned_union_impl<_Len, Types...>::_C_type type;
};

#else // _RWSTD_NO_VARIADIC_TEMPLATES

struct __rw_empty { };

template <_RWSTD_SIZE_T _Len,
          class _Type1 = __rw_empty, class _Type2 = __rw_empty,
          class _Type3 = __rw_empty, class _Type4 = __rw_empty,
          class _Type5 = __rw_empty, class _Type6 = __rw_empty,
          class _Type7 = __rw_empty, class _Type8 = __rw_empty>
struct __rw_aligned_union
{
    union {
        unsigned char __pad [_Len];
        _Type1 __object1;
        _Type2 __object2;
        _Type3 __object3;
        _Type4 __object4;
        _Type5 __object5;
        _Type6 __object6;
        _Type7 __object7;
        _Type8 __object8;
    } type;
};

#endif // !_RWSTD_NO_VARIADIC_TEMPLATES

/**
 * Conditional primitive that provides a member typedef type that is
 * _TypeT if _Enable is true, otherwise there will be no such typedef.
 *
 * The primary template provides an implementation for the case that
 * _Enable is true.
 */
template <bool _Enable, class _TypeT = void>
struct __rw_enable_if
{
    typedef _TypeT type;
};

/**
 * Conditional primitive that provides a member typedef type that is
 * _TypeT if _Enable is true, otherwise there will be no such typedef.
 *
 * The specialization provides an implementation for the case that _Enable
 * is false.
 */
template <class _TypeT>
struct __rw_enable_if<false, _TypeT>
{
};

//#define _RWSTD_ENABLE_IF(C,T) _RW::__rw_enable_if<C,T>::type

/**
 * Conditional primitive that provides a member typedef type that is
 * _TypeT if _Enable is false, otherwise there will be no such typedef.
 *
 * The primary template provides an implementation for the case that
 * _Enable is false.
 */
template <bool _Enable, class _TypeT = void>
struct __rw_disable_if
{
    typedef _TypeT type;
};

/**
 * Conditional primitive that provides a member typedef type that is
 * _TypeT if _Enable is false, otherwise there will be no such typedef.
 *
 * The specialization provides an implementation for the case that _Enable
 * is false.
 */
template <class _TypeT>
struct __rw_disable_if<true, _TypeT>
{
};

//#define _RWSTD_DISABLE_IF(C,T) _RW::__rw_disable_if<C,T>::type

/**
 * Metaprogramming conditional primitive that provides a member typedef
 * _C_type that is _TypeT if _Select is true, otherwise is _TypeU.
 *
 * The primary template is used when _Select is true.
 */
template <bool _Select, class _TypeT, class _TypeU>
struct __rw_conditional
{
    typedef _TypeT type;
};

/**
 * Metaprogramming conditional primitive that provides a member typedef
 * type is _TypeT if _Select is true, otherwise is _TypeU.
 *
 * This specialization if used when _Select is false.
 */
template <class _TypeT, class _TypeU>
struct __rw_conditional<false, _TypeT, _TypeU>
{
    typedef _TypeU type;
};

//#define _RWSTD_CONDITIONAL(C,T,U) _RW::__rw_conditional<C,T,U>::type

/**
 * TransformationTrait that implements compile time array-to-pointer
 * conversions and function-to-pointer conversions for the given type
 * _TypeT.
 *
 * Let _TypeU be __rw_remove_ref<_TypeT>::type. If _TypeT is an
 * array type, the member typedef type shall equal to the type of
 * __rw_remove_extent<_TypeU>::type*. If _TypeT is a function type,
 * the member typedef type shall be __rw_add_ptr<_TypeU>::type.
 * Otherwise type will be __rw_remove_cv<_TypeU>::type.
 */
template <class _TypeT>
struct __rw_decay
{
private:
    typedef typename
    __rw_remove_reference<_TypeT>::type _TypeU;

    typedef typename
    __rw_conditional<__rw_is_function<_TypeU>::value,
                     typename __rw_add_pointer<_TypeU>::type,
                     typename __rw_remove_cv<_TypeU>::type
                    >::type _TypeV;

public:
    typedef typename
    __rw_conditional<__rw_is_array<_TypeU>::value,
                     typename __rw_remove_extent<_TypeU>::type*,
                     _TypeV
                    >::type type;
};

//#define _RWSTD_DECAY(T) _RW::__rw_decay<T>::type

} // namespace __rw


#endif   // _RWSTD_META_OTHER_H_INCLUDED
