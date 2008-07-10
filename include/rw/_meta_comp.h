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

#ifndef _RWSTD_RW_META_COMP_H_INCLUDED
#define _RWSTD_RW_META_COMP_H_INCLUDED

#include <rw/_defs.h>
#include <rw/_meta_cat.h>

_RWSTD_NAMESPACE (__rw) {

template <class _TypeT>
struct __rw_is_reference
    : __rw_integral_constant<bool,
             __rw_is_lvalue_reference<_TypeT>::value
          || __rw_is_rvalue_reference<_TypeT>::value>
{
};

#define _RWSTD_IS_REFERENCE(T) _RW::__rw_is_reference<T>::value

template <class _TypeT>
struct __rw_is_arithmetic
    : __rw_integral_constant<bool,
             __rw_is_integral<_TypeT>::value
          || __rw_is_floating_point<_TypeT>::value>
{
};

#define _RWSTD_IS_ARITHMETIC(T) _RW::__rw_is_arithmetic<T>::value

template <class _TypeT>
struct __rw_is_fundamental
    : __rw_integral_constant<bool,    __rw_is_void<_TypeT>::value
                                   || __rw_is_arithmetic<_TypeT>::value>
{
};

#define _RWSTD_IS_FUNDAMENTAL(T) _RW::__rw_is_fundamental<T>::value


template <class _TypeT>
struct __rw_is_object
    : __rw_integral_constant<bool,    !__rw_is_function<_TypeT>::value
                                   && !__rw_is_reference<_TypeT>::value
                                   && !__rw_is_void<_TypeT>::value>
{
};

#define _RWSTD_IS_OBJECT(T) _RW::__rw_is_object<T>::value


template <class _TypeT>
struct __rw_is_member_pointer
    : __rw_integral_constant<bool,
             __rw_is_member_function_pointer<_TypeT>::value
          || __rw_is_member_object_pointer<_TypeT>::value>
{
};

#define _RWSTD_IS_MEMBER_POINTER(T) _RW::__rw_is_member_pointer<T>::value

template <class _TypeT>
struct __rw_is_scalar
    : __rw_integral_constant<bool,    __rw_is_arithmetic<_TypeT>::value
                                   || __rw_is_enum<_TypeT>::value
                                   || __rw_is_pointer<_TypeT>::value
                                   || __rw_is_member_pointer<_TypeT>::value>
{
    // todo need to handle special case
    //
    // __rw_is_same<std::nullptr_t, __rw_remove_cv<_TypeT>::type>::value
};

#define _RWSTD_IS_SCALAR(T) _RW::__rw_is_scalar<T>::value


template <class _TypeT>
struct __rw_is_compound
    : __rw_integral_constant<bool,    __rw_is_array<_TypeT>::value
                                   || __rw_is_function<_TypeT>::value
                                   || __rw_is_pointer<_TypeT>::value
                                   || __rw_is_reference<_TypeT>::value
                                   || __rw_is_class<_TypeT>::value
                                   || __rw_is_union<_TypeT>::value
                                   || __rw_is_enum<_TypeT>::value
                                   || __rw_is_member_pointer<_TypeT>::value>
{
};

#define _RWSTD_IS_COMPOUND(T) _RW::__rw_is_compound<T>::value


} // namespace __rw


#endif   // _RWSTD_RW_META_COMP_H_INCLUDED

