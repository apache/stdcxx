// -*- C++ -*-
/***************************************************************************
 *
 * _forward - forward/move helpers for <utility> header
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
 *
 * $Id: _forward.h 675429 2008-07-10 03:42:40Z sebor $
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

#ifndef _RWSTD_RW_FORWARD_INCLUDED
#  define _RWSTD_RW_FORWARD_INCLUDED

#  include <rw/_defs.h>
#  include <rw/_meta_ref.h>     // for __rw_remove_reference


_RWSTD_NAMESPACE (std) {


// 20.2.2, forward/move helpers:

template <class _TypeT>
struct identity
{
    typedef _TypeT  type;

    const type& operator() (const type& __x) const {
        return __x;
    }
};

#  define _RWSTD_IDENTITY(_TypeT)         _STD::identity<_TypeT>::type


#  if !defined _RWSTD_NO_RVALUE_REFERENCES

template <class _TypeT>
inline _TypeT&&
forward (typename identity<_TypeT>::type&& __x)
{
    return __x;
}


template <class _TypeT>
inline typename _RWSTD_REMOVE_REFERENCE(_TypeT)&&
move (_TypeT&& __x)
{
    return __x;
}

#    define _RWSTD_FORWARD(_TypeT, __x)   _STD::forward<_TypeT> (__x)
#    define _RWSTD_MOVE(__x)              _STD::move (__x)

#  else   // no rvalue references

#    define _RWSTD_FORWARD(_TypeT, __x)   (__x)
#    define _RWSTD_MOVE(__x)              (__x)

#  endif   // !defined _RWSTD_NO_RVALUE_REFERENCES


}   // namespace std


#endif   // _RWSTD_RW_FORWARD_INCLUDED
