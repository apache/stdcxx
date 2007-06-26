// -*- C++ -*-
/***************************************************************************
 *
 * _heap.h - declarations and inline definitions of the C++ Standard
 *           Library Heap operations
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
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

#ifndef _RWSTD_RW_HEAP_H_INCLUDED
#define _RWSTD_RW_HEAP_H_INCLUDED

#ifndef _RWSTD_RW_ALGOBASE_H_INCLUDED
#  include <rw/_algobase.h>
#endif   // _RWSTD_RW_ALGOBASE_H_INCLUDED

#ifndef _RWSTD_RW_ITERBASE_H_INCLUDED
#  include <rw/_iterbase.h>
#endif   // _RWSTD_RW_ITERBASE_H_INCLUDED


_RWSTD_NAMESPACE (std) {


// 25.3.6 - Heap operations

// helper to work around the lack of iterator_traits
_EXPORT
template <class _RandomAccessIter, class _Dist, class _TypeT,
          class _Compare>
void __push_heap (_RandomAccessIter, _Dist, _Dist, _TypeT, _Compare);


template <class _RandomAccessIter, class _Dist, class _Compare>
inline void
__push_heap (_RandomAccessIter __first, _RandomAccessIter __last,
             _Dist*, _Compare __comp)
{
    __push_heap (__first, _Dist (__last - __first), _Dist (), *__last, __comp);
}

// 25.3.6.1
template <class _RandomAccessIter, class _Compare>
inline void push_heap (_RandomAccessIter __first, _RandomAccessIter __last,
                       _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (!(__first == __last))
        __push_heap (__first, --__last,
                     _RWSTD_DIFFERENCE_TYPE (_RandomAccessIter), __comp);
}


// 25.3.6.1
template <class _RandomAccessIter>
inline void push_heap (_RandomAccessIter __first, _RandomAccessIter __last)
{
    _STD::push_heap (__first, __last, _RWSTD_LESS (_RandomAccessIter));
}


_EXPORT
template <class _RandomAccessIter, class _Dist, class _TypeT,
          class _Compare>
void __adjust_heap (_RandomAccessIter, _Dist, _Dist, _TypeT, _Compare);


// helper to work around the lack of iterator_traits
template <class _RandomAccessIter, class _TypeT, class _Compare, class _Dist>
inline void
__pop_heap (_RandomAccessIter __first, _RandomAccessIter __last,
            _RandomAccessIter __res, _TypeT __val, _Compare __cmp, _Dist*)
{
    *__res = *__first;
    __adjust_heap (__first, _Dist (), _Dist (__last - __first), __val, __cmp);
}


// 25.3.6.2
template <class _RandomAccessIter, class _Compare>
inline void
pop_heap (_RandomAccessIter __first, _RandomAccessIter __last, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (!(__first == __last)) {
        --__last;
        __pop_heap (__first, __last, __last, *__last, __comp,
                    _RWSTD_DIFFERENCE_TYPE (_RandomAccessIter));
    }
}


// 25.3.6.2
template <class _RandomAccessIter>
inline void pop_heap (_RandomAccessIter __first, _RandomAccessIter __last)
{
    _STD::pop_heap (__first, __last, _RWSTD_LESS (_RandomAccessIter));
}


_EXPORT
template <class _RandomAccessIter, class _Compare, class _Dist>
void __make_heap (_RandomAccessIter, _RandomAccessIter, _Compare, _Dist*);


// 25.3.6.3
template <class _RandomAccessIter, class _Compare>
inline void make_heap (_RandomAccessIter __first, _RandomAccessIter __last,
                       _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (!(__last - __first < 2))
        __make_heap (__first, __last, __comp,
                     _RWSTD_DIFFERENCE_TYPE (_RandomAccessIter));
}


// 25.3.6.3
template <class _RandomAccessIter>
inline void make_heap (_RandomAccessIter __first, _RandomAccessIter __last)
{
    _STD::make_heap (__first, __last, _RWSTD_LESS (_RandomAccessIter));
}


// 25.3.6.4
template <class _RandomAccessIter, class _Compare>
inline void sort_heap (_RandomAccessIter __first, _RandomAccessIter __last,
                       _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for (; __last - __first > 1; --__last)
        _STD::pop_heap (__first, __last, __comp);
}


// 25.3.6.4
template <class _RandomAccessIter>
inline void sort_heap (_RandomAccessIter __first, _RandomAccessIter __last)
{
    _STD::sort_heap (__first, __last, _RWSTD_LESS (_RandomAccessIter));
}


}   // namespace std


#ifdef _RWSTD_NO_IMPLICIT_INCLUSION
#  include <rw/_heap.cc>
#endif


#endif   // _RWSTD_RW_HEAP_H_INCLUDED
