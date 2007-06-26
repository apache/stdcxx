// -*- C++ -*-
/***************************************************************************
 *
 * _pair.h - definition of std::pair
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
 * Copyright 1994-2005 Rogue Wave Software.
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
 **************************************************************************/

#ifndef _RWSTD_RW_PAIR_H_INCLUDED
#define _RWSTD_RW_PAIR_H_INCLUDED

#ifndef _RWSTD_RW_FUNCBASE_H_INCLUDED
#  include <rw/_funcbase.h>   // for less
#endif   // _RWSTD_RW_FUNCBASE_H_INCLUDED


_RWSTD_NAMESPACE (std) { 


// 20.2.2
template <class _TypeT, class _TypeU>
struct pair
{
    typedef _TypeT first_type;
    typedef _TypeU second_type;

    first_type  first;
    second_type second;

    // 20.2.2, p2
    pair ()
#ifndef _RWSTD_NO_EMPTY_MEM_INITIALIZER
        : first (/* lwg issue 265 */), second () { }
#else
        // assumes types satisfy the CopyConstructible requirements
        : first (first_type ()), second (second_type ()) { }
#endif   // _RWSTD_NO_EMPTY_MEM_INITIALIZER

    // 20.2.2, p3
    pair (const first_type &__x, const second_type &__y)
        : first (__x), second (__y) { }

    // 20.2.2, p4
    template <class _TypeX, class _TypeY>
    pair (const pair <_TypeX, _TypeY> &__rhs) 
        : first (__rhs.first), second (__rhs.second) { }

    // lwg issue 353
    template <class _TypeX, class _TypeY>
    pair<_TypeT, _TypeU>& operator= (const pair <_TypeX, _TypeY> &__rhs) {
        return first = __rhs.first, second = __rhs.second, *this;
    }

};


// 20.2.2, p5
template <class _TypeT, class _TypeU>
inline bool
operator== (const pair<_TypeT, _TypeU>& __x, const pair<_TypeT, _TypeU>& __y)
{ 
    return __x.first == __y.first && __x.second == __y.second; 
}


template <class _TypeT, class _TypeU>
inline bool
operator!= (const pair<_TypeT, _TypeU>& __x, const pair<_TypeT, _TypeU>& __y)
{ 
    return !(__x == __y);
}


// 20.2.2, p6
template <class _TypeT, class _TypeU>
inline bool
operator< (const pair<_TypeT, _TypeU>& __x, const pair<_TypeT, _TypeU>& __y)
{
    _STD::less<_TypeT> __lessT;

    // follows resolution of lwg issue 348
    return    __lessT (__x.first, __y.first)
           || (   !__lessT (__y.first, __x.first)
               &&  _STD::less<_TypeU>()(__x.second, __y.second));
}


template <class _TypeT, class _TypeU>
inline bool
operator> (const pair<_TypeT, _TypeU>& __x, const pair<_TypeT, _TypeU>& __y)
{ 
    return __y < __x;
}


template <class _TypeT, class _TypeU>
inline bool
operator>= (const pair<_TypeT, _TypeU>& __x, const pair<_TypeT, _TypeU>& __y)
{ 
    return !(__x < __y);
}


template <class _TypeT, class _TypeU>
inline bool
operator<= (const pair<_TypeT, _TypeU>& __x, const pair<_TypeT, _TypeU>& __y)
{ 
    return !(__y < __x);
}


// 20.2.2, p7, signature follows lwg issue 181
template <class _TypeT, class _TypeU>
inline pair<_TypeT, _TypeU>
make_pair (_TypeT __x, _TypeU __y)
{
    return pair<_TypeT, _TypeU>(__x, __y);
}


}   // namespace std


#endif   // _RWSTD_RW_PAIR_H_INCLUDED
