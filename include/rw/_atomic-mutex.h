/***************************************************************************
 *
 * _atomic-mutex.h - definitions of inline functions for atomic
 *                   operations with using mutex object
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

#ifndef _RWSTD_RW_ATOMIC_MUTEX_H_INCLUDED
#define _RWSTD_RW_ATOMIC_MUTEX_H_INCLUDED


#ifndef _RWSTD_RW_MUTEX_H_INCLUDED
#  include <rw/_mutex.h>
#endif   // _RWSTD_RW_MUTEX_H_INCLUDED


_RWSTD_NAMESPACE (__rw) {
    
// helper functions for atomic value [in|de]crement and exchange
// the functions are atomic with respect to each other as long as
// they are passed the same mutex by the callers
template <class _TypeT>
inline
_TypeT __rw_atomic_preincrement (_TypeT &__t, __rw_mutex_base &__mutex)
{
    _RWSTD_MT_GUARD (__mutex);

    return ++__t;
}


template <class _TypeT>
inline
_TypeT __rw_atomic_predecrement (_TypeT &__t, __rw_mutex_base &__mutex)
{
    _RWSTD_MT_GUARD (__mutex);

    return --__t;
}


template <class _TypeT, class _TypeU>
inline
_TypeT __rw_atomic_exchange (_TypeT &__t, const _TypeU &__u,
                             __rw_mutex_base &__mutex)
{
    _RWSTD_MT_GUARD (__mutex);

    _TypeT __tmp = __t;

#ifndef _WIN64
    __t = __u;
#else
    // silence MSVC conversion warnings (cast breaks SunPro 5.3 and prior)
    __t = _RWSTD_STATIC_CAST (_TypeT, __u);
#endif

    return __tmp;
}


// for use on class statics or on namespace-scope variables
// the unused argument is only here so that all functions
// can be called from the same set of macros

template <class _TypeT>
inline
_TypeT __rw_atomic_preincrement (_TypeT &__t, bool)
{
    return __rw_atomic_preincrement (__t,
                                     __rw_get_static_mutex ((_TypeT*)0));
}


template <class _TypeT>
inline
_TypeT __rw_atomic_predecrement (_TypeT &__t, bool)
{
    return __rw_atomic_predecrement (__t,
                                     __rw_get_static_mutex ((_TypeT*)0));
}


template <class _TypeT, class _TypeU>
inline
_TypeT __rw_atomic_exchange (_TypeT &__t, const _TypeU &__u, bool)
{
    return __rw_atomic_exchange (__t, __u,
                                 __rw_get_static_mutex ((_TypeT*)0));
}

}   // namespace __rw

#endif   // _RWSTD_RW_ATOMIC_MUTEX_H_INCLUDED
