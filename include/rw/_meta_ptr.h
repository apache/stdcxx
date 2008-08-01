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

#ifndef _RWSTD_RW_META_PTR_H_INCLUDED
#define _RWSTD_RW_META_PTR_H_INCLUDED

#include <rw/_defs.h>
#include <rw/_meta_ref.h>

_RWSTD_NAMESPACE (__rw) {

template <class _TypeT>
struct __rw_remove_pointer
{
    typedef _TypeT type;
};

template <class _TypeT>
struct __rw_remove_pointer<_TypeT*>
{
    typedef _TypeT type;
};

#define _RWSTD_REMOVE_POINTER(T) _RW::__rw_remove_pointer<T>::type

template <class _TypeT>
struct __rw_add_pointer
{
    typedef typename
    __rw_remove_reference<_TypeT>::type* type;
};

#define _RWSTD_ADD_POINTER(T) _RW::__rw_add_pointer<T>::type

} // namespace __rw


#endif   // _RWSTD_RW_META_PTR_H_INCLUDED

