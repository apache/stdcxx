// -*- C++ -*-
/***************************************************************************
 *
 * _tuple_traits.h - allocator traits for <tuple> header
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
 * Copyright 2008 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_TUPLE_TRAITS_INCLUDED
#  define _RWSTD_RW_TUPLE_TRAITS_INCLUDED

#  include <rw/_defs.h>

#  if !defined _RWSTD_NO_EXT_CXX_0X \
      && !defined _RWSTD_NO_VARIADIC_TEMPLATES

#    include <rw/_meta_help.h>  // for __rw_true_type


_RWSTD_NAMESPACE (std) {


// [20.3.1.1]  tuple traits

template <class _Type, class _Alloc>
struct uses_allocator;

template <class... _Types, class _Alloc> 
struct uses_allocator<tuple<_Types...>, _Alloc>
    : _RW::__rw_true_type
{
    // empty
}; 


template <class _Type>
struct constructible_with_allocator_prefix;

template <class... _Types> 
struct constructible_with_allocator_prefix<tuple<_Types...> > 
    : _RW::__rw_true_type
{
    // empty
}; 


}   // namespace std


#  endif   // !defined _RWSTD_NO_EXT_CXX_0X
           // && !defined _RWSTD_NO_VARIADIC_TEMPLATES

#endif   // _RWSTD_RW_TUPLE_TRAITS_INCLUDED
