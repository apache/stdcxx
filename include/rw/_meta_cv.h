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

#ifndef _RWSTD_RW_META_CV_H_INCLUDED
#define _RWSTD_RW_META_CV_H_INCLUDED

#include <rw/_defs.h>
#include <rw/_meta_cat.h>
#include <rw/_meta_comp.h>

_RWSTD_NAMESPACE (__rw) {

/**
 * TransformationTrait strips any top level const-qualifier from _TypeT.
 *
 * The primary template is for non-const types.
 */
template <class _TypeT>
struct __rw_remove_const
{
    typedef _TypeT type;
};

/**
 * TransformationTrait strips any top level const-qualifier from _TypeT.
 *
 * This specialization is for const types.
 */
template <class _TypeT>
struct __rw_remove_const<const _TypeT>
{
    typedef _TypeT type;
};

#define _RWSTD_REMOVE_CONST(T) _RW::__rw_remove_const<T>::type


/**
 * TransformationTrait strips any top level volatile-qualifier from _TypeT.
 *
 * The primary template is for non-volatile types.
 */
template <class _TypeT>
struct __rw_remove_volatile
{
    typedef _TypeT type;
};

/**
 * TransformationTrait strips any top level volatile-qualifier from _TypeT.
 *
 * This specialization is for volatile types.
 */
template <class _TypeT>
struct __rw_remove_volatile<volatile _TypeT>
{
    typedef _TypeT type;
};

#define _RWSTD_REMOVE_VOLATILE(T) _RW::__rw_remove_volatile<T>::type

/**
 * TransformationTrait strips top level cv-qualifiers from _TypeT.
 */
template <class _TypeT>
struct __rw_remove_cv
{
    typedef typename __rw_remove_const<
        typename __rw_remove_volatile<_TypeT>::type
    >::type type;
};

#define _RWSTD_REMOVE_CV(T) _RW::__rw_remove_cv<T>::type


template <class _TypeT, bool =   __rw_is_function<_TypeT>::value
                              || __rw_is_reference<_TypeT>::value>
struct __rw_add_const_impl
{
    typedef _TypeT _C_type;
};

template <class _TypeT>
struct __rw_add_const_impl<_TypeT, false>
{
    typedef const _TypeT _C_type;
};

/**
 * TransformationTrait adds a top level const qualifier to _TypeT.
 *
 * If _TypeT is a reference, function, or top-level const-qualified
 * type, then type shall name the same type as _TypeT otherwise it
 * shall name const _TypeT.
 */
template <class _TypeT>
struct __rw_add_const
{
    typedef typename __rw_add_const_impl<_TypeT>::_C_type type;
};

#define _RWSTD_ADD_CONST(T) _RW::__rw_add_const<T>::type


template <class _TypeT, bool =   __rw_is_function<_TypeT>::value
                              || __rw_is_reference<_TypeT>::value>
struct __rw_add_volatile_impl
{
    typedef _TypeT _C_type;
};

template <class _TypeT>
struct __rw_add_volatile_impl<_TypeT, false>
{
    typedef volatile _TypeT _C_type;
};

/**
 * TransformationTrait adds a top level volatile qualifier to _TypeT.
 *
 * If _TypeT is a reference, function, or top-level volatile-qualified
 * type, then type shall name the same type as _TypeT otherwise it
 * shall name volatile _TypeT.
 */
template <class _TypeT>
struct __rw_add_volatile
{
    typedef typename __rw_add_volatile_impl<_TypeT>::_C_type type;
};

#define _RWSTD_ADD_VOLATILE(T) _RW::__rw_add_volatile<T>::type


/**
 * Trait adds top level cv-qualifiers to _TypeT.
 */
template <class _TypeT>
struct __rw_add_cv
{
    typedef typename
    __rw_add_const<
        typename __rw_add_volatile<_TypeT>::type
    >::type type;
};

#define _RWSTD_ADD_CV(T) _RW::__rw_add_cv<T>::type

} // namespace __rw


#endif   // _RWSTD_RW_META_CV_H_INCLUDED

