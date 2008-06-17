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

#ifndef _RWSTD_RW_STATIC_ASSERT_H_INCLUDED
#define _RWSTD_RW_STATIC_ASSERT_H_INCLUDED

#include <rw/_defs.h>

_RWSTD_NAMESPACE (__rw) {

/**
 * Template used to perform compile time assertions at class or global scope.
 */
template <bool _Cond>
struct __rw_static_assert
{
};

template <bool _Cond>
struct __rw_assert_failed;

template <>
struct __rw_assert_failed<true>
{
};

} // namespace __rw

#ifndef _RWSTD_NO_STATIC_ASSERT
#  define _RWSTD_STATIC_ASSERT(Cond,Mesg) static_assert(Cond, Mesg)
#else
#  define _RWSTD_STATIC_ASSERT(Cond,Mesg)                                  \
   typedef                                                                 \
   _RW::__rw_static_assert<sizeof (_RW::__rw_assert_failed<(Cond)>) != 0>  \
       _RWSTD_PASTE(__static_assert,__LINE__)
#endif   // _RWSTD_NO_STATIC_ASSERT

#endif   // _RWSTD_RW_STATIC_ASSERT_H_INCLUDED
