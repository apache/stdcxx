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

#ifndef _RWSTD_RW_META_HELP_H_INCLUDED
#define _RWSTD_RW_META_HELP_H_INCLUDED

#include <rw/_defs.h>

_RWSTD_NAMESPACE (__rw) {

/**
 * @internal
 * A compile-time integral constant wrapper.  This class template
 * encapsulates an integer constant value for metaprogramming.
 */
template <class _TypeT, _TypeT _Value>
struct __rw_integral_constant
{
    /**
     * Describes the type of this integral_constant.
     */
    typedef __rw_integral_constant<_TypeT,_Value> type;

    /**
     * Describes the type of the value defined by this integral_constant.
     */
    typedef _TypeT value_type;

    /**
     * The actual integral constant value.
     */
    static const _TypeT value = _Value;
};

#ifndef _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION

template<class _TypeT, _TypeT _Value>
const _TypeT __rw_integral_constant<_TypeT, _Value>::value;

#endif    // _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION

/**
 * @internal
 * Synonym for __rw_integral_constant<bool, true> type.
 */
typedef __rw_integral_constant<bool, true>  __rw_true_type;

/**
 * @internal
 * Synonym for __rw_integral_constant<bool, false> type.
 */
typedef __rw_integral_constant<bool, false> __rw_false_type;

} // namespace __rw


#endif   // _RWSTD_RW_META_HELP_H_INCLUDED

