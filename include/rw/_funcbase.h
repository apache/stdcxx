// -*- C++ -*-
/***************************************************************************
 *
 * _funcbase.h - definitions of function objects base classes
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_FUNCBASE_H_INCLUDED
#define _RWSTD_RW_FUNCBASE_H_INCLUDED

#ifndef _RWSTD_RW_DEFS_H_INCLUDED
#  include <rw/_defs.h>
#endif   // _RWSTD_RW_DEFS_H_INCLUDED


_RWSTD_NAMESPACE (std) { 


// 20.3.1 - Base
template <class _Arg, class _Result>
struct unary_function
{
    typedef _Arg    argument_type;
    typedef _Result result_type;
};


#define _RWSTD_UNARY_FUNCTION_TYPES(T, U)                                    \
  typedef typename _STD::unary_function<T, U>::argument_type argument_type; \
  typedef typename _STD::unary_function<T, U>::result_type   result_type


template <class _Arg1, class _Arg2, class _Result>
struct binary_function
{
    typedef _Arg1   first_argument_type;
    typedef _Arg2   second_argument_type;
    typedef _Result result_type;
};


#define _RWSTD_BINARY_FUNCTION_TYPES(T, U, V)                              \
  typedef typename _STD::binary_function<T, U, V>::second_argument_type   \
          second_argument_type;                                            \
  typedef typename _STD::binary_function<T, U, V>::first_argument_type    \
          first_argument_type;                                             \
  typedef typename _STD::binary_function<T, U, V>::result_type            \
          result_type


// 20.3.3, p5
template <class _TypeT>
struct less: binary_function<_TypeT, _TypeT, bool>
{
    _RWSTD_BINARY_FUNCTION_TYPES (_TypeT, _TypeT, bool);

    result_type operator() (const first_argument_type  &__x,
                            const second_argument_type &__y) const {
        return __x < __y;
    }
};


}   // namespace std


#endif   // _RWSTD_RW_FUNCBASE_H_INCLUDED
