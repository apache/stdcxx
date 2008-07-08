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

#ifndef _RWSTD_RW_META_REF_H_INCLUDED
#define _RWSTD_RW_META_REF_H_INCLUDED

#include <rw/_defs.h>
#include <rw/_meta_cat.h>
#include <rw/_meta_comp.h>

_RWSTD_NAMESPACE (__rw) {

template <class _TypeT>
struct __rw_remove_reference
{
    typedef _TypeT type;
};

template <class _TypeT>
struct __rw_remove_reference<_TypeT&>
{
    typedef _TypeT type;
};

#ifndef _RWSTD_NO_RVALUE_REFERENCES

template <class _TypeT>
struct __rw_remove_reference<_TypeT&&>
{
    typedef _TypeT type;
};

#endif   // _RWSTD_NO_RVALUE_REFERENCES

#define _RWSTD_REMOVE_REFERENCE(T) _RW::__rw_remove_reference<T>::type


template <class _TypeT, bool =    !__rw_is_void<_TypeT>::value
                               && !__rw_is_reference<_TypeT>::value,
                        bool = __rw_is_rvalue_reference<_TypeT>::value>
struct __rw_add_lvalue_reference_impl
{
    // for other cases _C_type shall be _TypeT.
    typedef _TypeT _C_type;
};

template <class _TypeT>
struct __rw_add_lvalue_reference_impl<_TypeT, true, false>
{
    // for object and function types _C_type shall become _TypeT&
    // for other cases _C_type shall be _TypeT.
    typedef _TypeT& _C_type;
};

template <class _TypeT>
struct __rw_add_lvalue_reference_impl<_TypeT, false, true>
{
    // for rval reference types, _C_type shall become the corresponding
    // lvalue reference type
    typedef typename __rw_remove_reference<_TypeT>::type& _C_type;
};

template <class _TypeT>
struct __rw_add_lvalue_reference
{
    typedef typename
    __rw_add_lvalue_reference_impl<_TypeT>::_C_type type;
};

#define _RWSTD_ADD_LVALUE_REFERENCE(T) \
    _RW::__rw_add_lvalue_reference<T>::type


template <class _TypeT, bool =    __rw_is_object<_TypeT>::value
                               || __rw_is_function<_TypeT>::_C_type>
struct __rw_add_rvalue_reference_impl
{
    // else it is _TypeT
    typedef _TypeT _C_type;
};

#ifndef _RWSTD_NO_RVALUE_REFERENCES

template <class _TypeT>
struct __rw_add_rvalue_reference_impl<_TypeT, true>
{
    // for object and function types, _C_type is the same as _TypeT&&
    typedef _TypeT&& _C_type;
};

#endif   // _RWSTD_NO_RVALUE_REFERENCES

/**
 * Trait makes a rval reference from the input type _TypeT.
 */
template <class _TypeT>
struct __rw_add_rvalue_reference
{
    typedef typename
    __rw_add_rvalue_reference_impl<_TypeT>::_C_type type;
};

#define _RWSTD_ADD_RVALUE_REFERENCE(T) \
    _RW::__rw_add_rvalue_reference<T>::type

} // namespace __rw


#endif   // _RWSTD_RW_META_REF_H_INCLUDED

