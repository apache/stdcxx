/***************************************************************************
 *
 * _heap.cc - Non-inline definitions for the C++ Standard Library
 *            Heap operations
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


_RWSTD_NAMESPACE (std) {


_EXPORT
template <class _RandomAccessIter, class _Dist, class _TypeT, class _Compare>
void __push_heap (_RandomAccessIter __first, _Dist __holeIndex,
                  _Dist __topIndex, _TypeT __val, _Compare __comp)
{
    for (_Dist __parent = (__holeIndex - 1) / 2; 
            __holeIndex > __topIndex
         && !(__comp (*(__first + __parent), __val) == false);
         __parent = ((__holeIndex = __parent) - 1) / 2) {
        *(__first + __holeIndex) = *(__first + __parent);
    }

    *(__first + __holeIndex) = __val;
}


_EXPORT
template <class _RandomAccessIter, class _Dist, class _TypeT, class _Compare>
void __adjust_heap (_RandomAccessIter __first, _Dist __holeIndex,
                    _Dist __dist, _TypeT __val, _Compare __comp)
{
    const _Dist __topIndex = __holeIndex;

    _Dist __2ndChild;

    while ((__2ndChild = 2 * __holeIndex + 2) < __dist) {
        if (__comp (*(__first + __2ndChild), *(__first + (__2ndChild - 1))))
            --__2ndChild;

        *(__first + __holeIndex) = *(__first + __2ndChild);
        __holeIndex = __2ndChild;
    }

    if (__2ndChild == __dist) {
        *(__first + __holeIndex) = *(__first + (__2ndChild - 1));
        __holeIndex = __2ndChild - 1;
    }

    __push_heap (__first, __holeIndex, __topIndex, __val, __comp);
}


_EXPORT
template <class _RandomAccessIter, class _Compare, class _Dist>
void __make_heap (_RandomAccessIter __first, _RandomAccessIter __last,
                  _Compare __comp, _Dist*)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    const _Dist __dist = __last - __first;

    for (_Dist __parent = __dist / 2; 0 < __parent; ) {
        --__parent;
        __adjust_heap (__first, __parent, __dist, *(__first + __parent),
                       __comp);
    }
}


}   // namespace std
