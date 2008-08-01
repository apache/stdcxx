/***************************************************************************
 *
 * algorithm.cc - Non-inline definitions for the Standard Library algorithms
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
template <class _FwdIter1, class _FwdIter2, class _Dist>
_FwdIter1 __find_end (_FwdIter1 __first1, _FwdIter1 __last1,
                      _FwdIter2 __first2, _FwdIter2 __last2,
                      _Dist*)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    const _Dist __dist1 = _DISTANCE (__first2, __last2, _Dist);

    if (!__dist1)
        return __last1;

    _Dist __dist2 = _DISTANCE (__first1, __last1, _Dist);

    _FwdIter1 __res = __last1;

    while (__dist2 >= __dist1) {
        if (equal (__first2, __last2, __first1))
            __res  = __first1;

        __dist2 = _DISTANCE (++__first1, __last1, _Dist);
    }
    return __res;
}



_EXPORT
template <class _FwdIter1, class _FwdIter2,
          class _BinaryPredicate, class _Dist>
_FwdIter1 __find_end (_FwdIter1 __first1, _FwdIter1 __last1,
                      _FwdIter2 __first2, _FwdIter2 __last2,
                      _BinaryPredicate __pred, _Dist*)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    const _Dist __dist1 = _DISTANCE (__first2, __last2, _Dist);

    if (!__dist1)
        return __last1;

    _Dist __dist2 = _DISTANCE (__first1, __last1, _Dist);

    _FwdIter1 __res = __last1;

    while (__dist2 >= __dist1) {
        if (_STD::equal (__first2, __last2, __first1, __pred))
            __res = __first1;

        __dist2 = _DISTANCE (++__first1, __last1, _Dist);
    }
    return __res;
}


_EXPORT
template <class _FwdIter1, class _FwdIter2>
_FwdIter1 find_first_of (_FwdIter1 __first1, _FwdIter1 __last1,
                         _FwdIter2 __first2, _FwdIter2 __last2)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    if (__first2 == __last2)
        return __last1;

    for (_FwdIter1 __next = __first1; !(__next == __last1); ++__next)
        if (!(_STD::find (__first2, __last2, *__next) == __last2))
            return __next;

    return __last1;
}


_EXPORT
template <class _FwdIter1, class _FwdIter2,
          class _BinaryPredicate>
_FwdIter1 find_first_of (_FwdIter1 __first1, _FwdIter1 __last1,
                         _FwdIter2 __first2, _FwdIter2 __last2,
                         _BinaryPredicate __pred)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    if (__first2 == __last2)
        return __last1;

    for (_FwdIter1 __next = __first1; !(__next == __last1); ++__next)
        for (_FwdIter2 __iter = __first2; !(__iter == __last2); ++__iter)
            if (!(__pred (*__next, *__iter) == false))
                return __next;

    return __last1;
}


_EXPORT
template <class _FwdIter>
_FwdIter adjacent_find (_FwdIter __first, _FwdIter __last)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (__first == __last)
        return __last;

    for (_FwdIter __next = __first; !(++__next == __last); __first = __next)
        if (*__first == *__next)
            return __first;

    return __last;
}


_EXPORT
template <class _FwdIter, class _BinaryPredicate>
_FwdIter adjacent_find (_FwdIter __first, _FwdIter __last,
                        _BinaryPredicate __pred)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (__first == __last)
        return __last;

    for (_FwdIter __next = __first; !(++__next == __last); __first = __next)
        if (!(__pred (*__first, *__next) == false))
            return __first;

    return __last;
}


_EXPORT
template <class _FwdIter1, class _FwdIter2, class _Dist1, class _Dist2>
_FwdIter1 __search (_FwdIter1 __first1, _FwdIter1 __last1,
                    _FwdIter2 __first2, _FwdIter2 __last2,
                    _Dist1*, _Dist2*)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    _Dist1 __dist1 = _DISTANCE (__first1, __last1, _Dist1);
    _Dist2 __dist2 = _DISTANCE (__first2, __last2, _Dist2);

    if (__dist1 < __dist2)
        return __last1;

    _FwdIter1 __cur1 = __first1;
    _FwdIter2 __cur2 = __first2;

    while (!(__cur2 == __last2)) {
        if (!(*__cur1 == *__cur2)) {
            ++__cur1;
            ++__cur2;
            if (__dist1-- == __dist2)
                return __last1;

            __cur1 = ++__first1;
            __cur2 = __first2;
        }
        else {
            ++__cur1;
            ++__cur2;
        }
    }

    return (__cur2 == __last2) ? __first1 : __last1;
}


_EXPORT
template <class _FwdIter1, class _FwdIter2,
          class _BinaryPredicate, class _Dist1, class _Dist2>
_FwdIter1 __search (_FwdIter1 __first1, _FwdIter1 __last1,
                    _FwdIter2 __first2, _FwdIter2 __last2,
                    _BinaryPredicate __pred, _Dist1*, _Dist2*)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    _Dist1 __dist1 = _DISTANCE (__first1, __last1, _Dist1);
    _Dist2 __dist2 = _DISTANCE (__first2, __last2, _Dist2);

    if (__dist1 < __dist2) return __last1;

    _FwdIter1 __cur1 = __first1;
    _FwdIter2 __cur2 = __first2;

    while (!(__cur2 == __last2)) {
        if (__pred (*__cur1, *__cur2) == false) {
            ++__cur1;
            ++__cur2;
            if (__dist1-- == __dist2)
                return __last1;

            __cur1 = ++__first1;
            __cur2 = __first2;
        }
        else {
            ++__cur1;
            ++__cur2;
        }
    }

    return (__cur2 == __last2) ? __first1 : __last1;
}


_EXPORT
template <class _FwdIter, class _Dist, class _Size, class _TypeT>
_FwdIter __search_n (_FwdIter __first, _FwdIter __last,
                     _Dist*, _Size __count, const _TypeT& __val)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (__count <= 0)
        return __last;

    const _Dist __dist = _DISTANCE (__first, __last, _Dist);

    if (__dist < _Dist (__count))
        return __last;

    _Dist __span    = __dist - _Dist (__count);
    _Dist __matches = 0;

    _FwdIter __current = __first;

    while (!(__current == __last)) {
        if (!(*__current == __val)) {
            if (__span < __matches + 1)
                return __last;
            __span   -= __matches + 1;
            __matches = 0;
            __first   = ++__current;
        }
        else {
            if (++__matches == _Dist (__count))
                return __first;
            ++__current;
        }
    }

    return __last;
}


_EXPORT
template <class _FwdIter, class _Dist, class _Size, class _TypeT,
          class _BinaryPredicate>
_FwdIter __search_n (_FwdIter __first, _FwdIter __last,
                     _Dist*, _Size __count, const _TypeT& __val,
                     _BinaryPredicate __pred)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    const _Dist __dist = _DISTANCE (__first, __last, _Dist);

    const _RWSTD_PTRDIFF_T __int_count = __count;

    if (__dist < __int_count || __int_count <= 0)
        return __last;

    _Dist __span       = __dist - __int_count;
    _Dist __matches    = 0;
    _FwdIter __current = __first;

    while (!(__current == __last)) {
        if (__pred (*__current, __val) == false) {
            if (__span < __matches + 1)
                return __last;
            __span   -= __matches + 1;
            __matches = 0;
            __first   = ++__current;
        }
        else {
            if (++__matches == __count)
                return __first;
            ++__current;
        }
    }

    return __last;
}

//
// Modifying sequence operations.
//

_EXPORT
template <class _Iter, class _OutputIter, class _Predicate, class _TypeT>
_OutputIter replace_copy_if (_Iter __first, _Iter __last,
                             _OutputIter __res, _Predicate __pred,
                             const _TypeT& __new_value)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for (; !(__first == __last); ++__res, ++__first)
        *__res = __pred (*__first) == false ? *__first : __new_value;

    return __res;
}


_EXPORT
template <class _InputIter, class _OutputIter, class _TypeT>
_OutputIter remove_copy (_InputIter __first, _InputIter __last,
                         _OutputIter __res, const _TypeT& __val)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for (; !(__first == __last); ++__first)
        if (!(*__first == __val)) {
            *__res = *__first;
            ++__res;
        }
    return __res;
}


_EXPORT
template <class _InputIter, class _OutputIter, class _Predicate>
_OutputIter remove_copy_if (_InputIter __first, _InputIter __last,
                            _OutputIter __res, _Predicate __pred)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for (; !(__first == __last); ++__first)
        if (__pred (*__first) == false) {
            *__res = *__first;
            ++__res;
        }
    return __res;
}


_EXPORT
template <class _FwdIter>
_FwdIter
unique (_FwdIter __first, _FwdIter __last)
{
    // find the first of two equal elements
    __first = _STD::adjacent_find (__first, __last);

    if (__first == __last)
        return __first;

    // advance `first' to point at the copy (may be overwritten later)
    _FwdIter __next = ++__first;

    // skip over all subsequent elements that are equal to the first
    while (!(++__next == __last))
        if (!(*__next == *__first))
            break;

    // invoke unique_copy to copy over the original sequence
    // starting at one past the first element
    return __unique_copy (__next, __last, __first,
                          _RWSTD_ITERATOR_CATEGORY (_FwdIter, __first));
}


_EXPORT
template <class _FwdIter, class _BinaryPredicate>
_FwdIter
unique (_FwdIter __first, _FwdIter __last, _BinaryPredicate __pred)
{
    __first = _STD::adjacent_find (__first, __last, __pred);

    if (__first == __last)
        return __first;

    _FwdIter __next = ++__first;

    while (!(++__next == __last))
        if (__pred (*__next, *__first) == false)
            break;

    return __unique_copy (__next, __last, __first, __pred,
                          _RWSTD_ITERATOR_CATEGORY (_FwdIter, __first));
}


_EXPORT
template <class _InputIter, class _FwdIter>
_FwdIter __unique_copy (_InputIter __first, _InputIter __last,
                        _FwdIter __res, forward_iterator_tag)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (__first == __last)
        return __res;

    *__res = *__first;
    while (!(++__first == __last))
        if (!(*__res == *__first))
            *++__res = *__first;
    return ++__res;
}


_EXPORT
template <class _InputIter, class _OutputIter, class _TypeT>
_OutputIter __unique_copy (_InputIter __first, _InputIter __last,
                           _OutputIter __res, _TypeT*)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (__first == __last)
        return __res;

    _TypeT __val = *__first;

    for (*__res = __val; !(++__first == __last); )
        if (!(__val == *__first)) {
            __val = *__first;
            *++__res = __val;
        }

    return ++__res;
}


_EXPORT
template <class _InputIter, class _FwdIter, class _BinaryPredicate>
_FwdIter __unique_copy (_InputIter __first, _InputIter __last,
                        _FwdIter __res,
                        _BinaryPredicate __pred,
                        forward_iterator_tag)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (__first == __last)
        return __res;

    for (*__res = *__first; !(++__first == __last); )
        if (__pred (*__res, *__first) == false)
            *++__res = *__first;

    return ++__res;
}


_EXPORT
template <class _InputIter, class _OutputIter, class _BinaryPredicate,
          class _TypeT>
_OutputIter __unique_copy (_InputIter __first, _InputIter __last,
                           _OutputIter __res,
                           _BinaryPredicate __pred, _TypeT*)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (__first == __last)
        return __res;

    _TypeT __val = *__first;

    for (*__res = __val; !(++__first == __last); )
        if (__pred (__val, *__first) == false) {
            __val = *__first;
            *++__res = __val;
        }

    return ++__res;
}


_EXPORT
template <class _FwdIter, class _Dist>
void __rotate (_FwdIter __first, _FwdIter __middle,
               _FwdIter __last, _Dist*, forward_iterator_tag)
{
    _RWSTD_ASSERT_IN_RANGE (__middle, __first, __last); 

    for (_FwdIter __i = __middle; ; ) {
        _STD::iter_swap (__first, __i);
        ++__i;
        if (++__first == __middle) {
            if (__i == __last)
                return;
            __middle = __i;
        }
        else if (__i == __last)
            __i = __middle;
    }
}


_EXPORT
template <class _EuclideanRingElement>
_EuclideanRingElement __gcd (_EuclideanRingElement __m,
                             _EuclideanRingElement __n)
{
    while (!(__n == _EuclideanRingElement ())) {
        const _EuclideanRingElement __r = __m % __n;
        __m = __n;
        __n = __r;
    }
    return __m;
}


_EXPORT
template <class _RandomAccessIter, class _Dist, class _TypeT>
void __rotate_cycle (_RandomAccessIter __first, _RandomAccessIter __last,
                     _RandomAccessIter __first2, _Dist __shift, _TypeT*)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    const _TypeT __val = *__first2;

    _RandomAccessIter __it1 = __first2;

    for (_RandomAccessIter __it2 = __it1 + __shift; !(__it2 == __first2); ) {
        *__it1 = *__it2;
        __it1  = __it2;

        if (__last - __it2 > __shift)
            __it2 += __shift;
        else
            __it2 = __first + (__shift - (__last - __it2));
    }

    *__it1 = __val;
}


_EXPORT
template <class _RandomAccessIter, class _Dist>
void __rotate (_RandomAccessIter __first, _RandomAccessIter __middle,
               _RandomAccessIter __last, _Dist*,
               random_access_iterator_tag)
{
    _RWSTD_ASSERT_IN_RANGE (__middle, __first, __last);
  
    for (_Dist __n = __gcd (__last - __first, __middle - __first); __n--; )
        __rotate_cycle (__first, __last, __first + __n, __middle - __first,
                       _RWSTD_VALUE_TYPE (_RandomAccessIter));
}


_EXPORT
template <class _RandomAccessIter, class _RandomNumberGenerator>
void random_shuffle (_RandomAccessIter __first, _RandomAccessIter __last,
                     _RandomNumberGenerator &__rand)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (!(__first == __last)) {

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC
        typedef typename
            iterator_traits<_RandomAccessIter>::difference_type _DiffT;
#else   // if defined (_RWSTD_NO_CLASS_PARTIAL_SPEC)
        typedef _RWSTD_PTRDIFF_T _DiffT;
#endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC

        _DiffT __limit = 2;

        for (_RandomAccessIter __i = __first; !(++__i == __last); ++__limit) {

            // the argument to and the return value of the random number
            // generator's operator() is required to be convertinble from
            // and to the iterator's difference_type but nothing else
            const _DiffT __rndoff (__rand (__limit));

            // returned value must be non-negative and less than the limit
            _RWSTD_ASSERT (0 <= __rndoff && __rndoff < __limit);

            _STD::iter_swap (__i, __first + __rndoff);
        }
    }
}


_EXPORT
template <class _BidirIter, class _Predicate>
_BidirIter partition (_BidirIter __first, _BidirIter __last, _Predicate __pred)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for ( ; ; ++__first) {
        for ( ; ; ) {
            if (__first == __last)
                return __first;
            if (!(__pred (*__first) == false))
                ++__first;
            else
                break;
        }

        for (--__last; ; ) {
            if (__first == __last)
                return __first;
            if (__pred (*__last) == false)
                --__last;
            else
                break;
        }
        _STD::iter_swap (__first, __last);
    }
}


template <class _BidirIter, class _Predicate, class _Dist>
_BidirIter __inplace_stable_partition (_BidirIter __first, _BidirIter __last,
                                       _Predicate __pred, _Dist __dist)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (__dist == 1)
        return !(__pred (*__first) == false) ? __last : __first;

    _BidirIter __middle = __first;
    _STD::advance (__middle, __dist / 2);
    _BidirIter
        __first_cut = __inplace_stable_partition (__first, __middle, __pred,
                                                  __dist / 2);
    _BidirIter
        __second_cut = __inplace_stable_partition (__middle, __last, __pred,
                                                 __dist - __dist / 2);
    _STD::rotate (__first_cut, __middle, __second_cut);
    __dist = _DISTANCE (__middle, __second_cut, _Dist);
    _STD::advance (__first_cut, __dist);
    return __first_cut;
}


_EXPORT
template <class _BidirIter, class _Pointer, class _Predicate,
          class _Dist, class _TypeT>
_BidirIter __stable_partition_adaptive (_BidirIter __first, _BidirIter __last,
                                        _Predicate __pred, _Dist __dist,
                                        _Pointer __buf,
                                        _Dist __buf_size,
                                        _Dist& __fill_pointer, _TypeT*)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (__dist <= __buf_size)
    {
        __dist = 0;
        _BidirIter __res1 = __first;
        _Pointer __res2 = __buf;
        for (; !(__first == __last) && __dist < __fill_pointer; ++__first)
        {
            if (!(__pred (*__first) == false)) {
                *__res1 = *__first;
                ++__res1;
            }
            else {
                *__res2 = *__first;
                ++__res2;
                ++__dist;
            }
        }
        if (!(__first == __last))
        {
            raw_storage_iterator<_Pointer, _TypeT> __res3 (__res2);
            for (; !(__first == __last); ++__first)
            {
                if (!(__pred (*__first) == false)) {
                    *__res1 = *__first;
                    ++__res1;
                }
                else {
                    *__res3 = *__first;
                    ++__res3;
                    ++__dist;
                }
            }
            __fill_pointer = __dist;
        }
        _STD::copy (__buf, __buf + __dist, __res1);
        return __res1;
    }
    _BidirIter __middle = __first;
    _STD::advance (__middle, __dist / 2);

    // (__dist / 2)'s type may not be the same as that of __dist,
    // hence the seemingly redundant casts below
    _BidirIter __first_cut =
        __stable_partition_adaptive (__first, __middle, __pred,
                                     _Dist (__dist / 2),
                                     __buf, __buf_size,
                                     __fill_pointer, (_TypeT*)0);
    _BidirIter __second_cut =
        __stable_partition_adaptive (__middle, __last, __pred,
                                     _Dist (__dist - __dist / 2),
                                     __buf, __buf_size,
                                     __fill_pointer, (_TypeT*)0);

    _STD::rotate (__first_cut, __middle, __second_cut);
    __dist = _DISTANCE (__middle, __second_cut, _Dist);
    _STD::advance (__first_cut, __dist);
    return __first_cut;
}


_EXPORT
template <class _BidirIter, class _Predicate, class _TypeT, class _Dist>
_BidirIter __stable_partition (_BidirIter __first, _BidirIter __last,
                               _Predicate __pred, _TypeT*, _Dist*)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    const _Dist __dist = _DISTANCE (__first, __last, _Dist);

    if (!__dist)
        return __last;
    
    // call an extension of get_temporary_buffer<>() in case partial class
    // specialization (and iterator_traits<>) isn't supported by compiler
    pair<_TypeT*, _Dist> __pair =
        _STD::get_temporary_buffer (__dist, (_TypeT*)0);

    if (__pair.first == 0)
        return __inplace_stable_partition (__first, __last, __pred, __dist);

    _Dist __fill = 0;

    const _BidirIter __res =
        __stable_partition_adaptive (__first, __last, __pred, __dist,
                                     __pair.first, __pair.second,
                                     __fill, (_TypeT*)0);

    for (_TypeT *__ptr = __pair.first + __fill; !(__pair.first == __ptr--); )
        (*__ptr).~_TypeT ();

    _STD::return_temporary_buffer (__pair.first);

    return __res;
}

//
// Sorting and related operations.
//

template <class _RandomAccessIter, class _TypeT, class _Compare>
_RandomAccessIter __unguarded_partition (_RandomAccessIter __first,
                                         _RandomAccessIter __last,
                                         _TypeT __pivot, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for ( ; ; ++__first) {
        for (; __comp (*__first, __pivot); ++__first);

        while (__comp (__pivot, *--__last));

        if (!(__first < __last))
            return __first;

        _STD::iter_swap (__first, __last);
    }
}


template <class _TypeT, class _Compare>
inline const _TypeT& __median (const _TypeT& __a, const _TypeT& __b,
                               const _TypeT& __c, _Compare __comp)
{
    return __comp (__a, __b)
        ? __comp (__b, __c) ? __b : __comp (__a, __c) ? __c : __a
        : __comp (__a, __c) ? __a : __comp (__b, __c) ? __c : __b;
}


enum { __rw_threshold = 16 };

template <class _RandomAccessIter, class _TypeT, class _Compare>
void __quick_sort_loop_aux (_RandomAccessIter __first,
                            _RandomAccessIter __last, _TypeT*,
                            _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    while (__last - __first > __rw_threshold) {
        _RandomAccessIter __cut =
            __unguarded_partition (__first, __last,
                                   _TypeT (__median (*__first,
                                                     * (__first +
                                                        (__last - __first)/2),
                                                     * (__last - 1),
                                                     __comp)),
             __comp);
        if (__cut - __first >= __last - __cut) {
            __quick_sort_loop_aux (__cut, __last,
                                   _RWSTD_VALUE_TYPE (_RandomAccessIter),
                                   __comp);
            __last = __cut;
        }
        else {
            __quick_sort_loop_aux (__first, __cut,
                                   _RWSTD_VALUE_TYPE (_RandomAccessIter),
                                   __comp);
            __first = __cut;
        }
    }
}


template <class _RandomAccessIter, class _TypeT>
inline void __linear_insert (_RandomAccessIter __first, 
                             _RandomAccessIter __last, _TypeT*)
{
    const _TypeT __val = *__last;

    if (__val < *__first) {
        _STD::copy_backward (__first, __last, __last + 1);
        *__first = __val;
    }
    else
        __unguarded_linear_insert (__last, __val);
}


template <class _RandomAccessIter, class _TypeT, class _Compare>
inline void
__linear_insert (_RandomAccessIter __first, _RandomAccessIter __last, _TypeT*,
                 _Compare __comp)
{
    const _TypeT __val = *__last;

    if (__comp (__val, *__first)) {
        _STD::copy_backward (__first, __last, __last + 1);
        *__first = __val;
    }
    else
        __unguarded_linear_insert (__last, __val, __comp);
}


_EXPORT
template <class _RandomAccessIter, class _Compare>
void __insertion_sort (_RandomAccessIter __first,
                       _RandomAccessIter __last, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (! (__first == __last))
        for (_RandomAccessIter __i = __first + 1; !(__i == __last); ++__i)
            __linear_insert (__first, __i,
                             _RWSTD_VALUE_TYPE (_RandomAccessIter), __comp);
}

_EXPORT
template <class _RandomAccessIter, class _TypeT, class _Compare>
void __unguarded_linear_insert (_RandomAccessIter __last, _TypeT __val,
                                _Compare __comp)
{
    for (_RandomAccessIter __next = __last; __comp (__val, *--__next); ) {
        *__last = *__next;
        __last = __next;
    }
    *__last = __val;
}


_EXPORT
template <class _RandomAccessIter, class _Compare>
void __final_insertion_sort (_RandomAccessIter __first,
                             _RandomAccessIter __last, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (__last - __first > __rw_threshold) {
        __insertion_sort (__first, __first + __rw_threshold, __comp);
        __unguarded_insertion_sort (__first + __rw_threshold, __last, __comp);
    }
    else
        __insertion_sort (__first, __last, __comp);
}


template <class _RandomAccessIter1, class _RandomAccessIter2,
          class _Dist, class _Compare>
void __merge_sort_loop (_RandomAccessIter1 __first,
                        _RandomAccessIter1 __last,
                        _RandomAccessIter2 __res, _Dist __step,
                        _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    const _Dist __two_step = 2 * __step;

    while (__last - __first >= __two_step) {
        __res = _STD::merge (__first, __first + __step,
                             __first + __step, __first + __two_step, __res,
                             __comp);
        __first += __two_step;
    }
    __step = (_STD::min)(_Dist (__last - __first), __step);

    _STD::merge (__first, __first + __step, __first + __step, __last,
                 __res, __comp);
}


template <class _RandomAccessIter, class _Dist, class _Compare>
void __chunk_insertion_sort (_RandomAccessIter __first,
                             _RandomAccessIter __last,
                             _Dist __chunk_size, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    while (__last - __first >= __chunk_size)
    {
        __insertion_sort (__first, __first + __chunk_size, __comp);
        __first += __chunk_size;
    }
    __insertion_sort (__first, __last, __comp);
}


_EXPORT
template <class _RandomAccessIter, class _Pointer, class _Dist,
          class _Compare>
void __merge_sort_with_buffer (_RandomAccessIter __first,
                               _RandomAccessIter __last, _Pointer __buf,
                               _Dist*, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    const _Dist __dist  = __last - __first;
    const _Pointer __bufend = __buf + __dist;

    _Dist __step = 7;   // STL chunk size

    __chunk_insertion_sort (__first, __last, __step, __comp);

    while (__step < __dist) {
        __merge_sort_loop (__first, __last, __buf, __step, __comp);
        __step *= 2;
        __merge_sort_loop (__buf, __bufend, __first, __step, __comp);
        __step *= 2;
    }
}


template <class _BidirIter, class _Dist, class _Pointer, class _Compare>
void __merge_adaptive (_BidirIter, _BidirIter, _BidirIter,
                       _Dist, _Dist, _Pointer, _Dist, _Compare);

_EXPORT
template <class _RandomAccessIter, class _Pointer, class _Dist,
          class _TypeT, class _Compare>
void __stable_sort_adaptive (_RandomAccessIter __first,
                             _RandomAccessIter __last, _Pointer __buf,
                             _Dist __buf_size, _TypeT*,
                             _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    const _Dist __dist = (__last - __first + 1) / 2;
    const _RandomAccessIter __middle = __first + __dist;

    if (__dist > __buf_size) {
        __stable_sort_adaptive (__first, __middle, __buf, __buf_size,
                                (_TypeT*)0, __comp);
        __stable_sort_adaptive (__middle, __last, __buf, __buf_size,
                                (_TypeT*)0, __comp);
    }
    else {
        __merge_sort_with_buffer (__first, __middle, __buf,
                                 (_Dist*)0, __comp);
        __merge_sort_with_buffer (__middle, __last, __buf,
                                 (_Dist*)0, __comp);
    }
    __merge_adaptive (__first, __middle, __last,
                      _Dist (__middle - __first),
                     _Dist (__last - __middle), __buf, __buf_size, __comp);
}


_EXPORT
template <class _RandomAccessIter, class _TypeT, class _Dist, class _Compare>
void
__stable_sort (_RandomAccessIter __first, _RandomAccessIter __last,
               _TypeT*, _Dist*, _Compare __comp)
{
    // call an extension of get_temporary_buffer<>() in case partial class
    // specialization (and iterator_traits<>) isn't supported by compiler
    pair<_TypeT*, _Dist> __p =
        _STD::get_temporary_buffer (_Dist (__last - __first), (_TypeT*)0);

    if (__p.first == 0)
        __inplace_stable_sort (__first, __last, __comp);
    else {
        _Dist __len = (_STD::min)(_Dist (__p.second),
                                      _Dist (__last - __first));
        _STD::copy (__first, __first + __len,
                    raw_storage_iterator<_TypeT*, _TypeT>(__p.first));

        __stable_sort_adaptive (__first, __last, __p.first, __p.second,
                                (_TypeT*)0, __comp);

        for (_TypeT *__ptr = __p.first + __len; !(__p.first == --__ptr); )
            (*__ptr).~_TypeT ();

        _STD::return_temporary_buffer (__p.first);
    }
}


_EXPORT
template <class _RandomAccessIter, class _TypeT, class _Compare>
void __partial_sort (_RandomAccessIter __first, _RandomAccessIter __middle,
                     _RandomAccessIter __last, _TypeT*, _Compare __comp)
{
    _RWSTD_ASSERT_IN_RANGE (__middle, __first, __last);

    _STD::make_heap (__first, __middle, __comp);

    for (_RandomAccessIter __i = __middle; __i < __last; ++__i)
        if (__comp (*__i, *__first))
            __pop_heap (__first, __middle, __i, *__i, __comp,
                        _RWSTD_DIFFERENCE_TYPE (_RandomAccessIter));

    _STD::sort_heap (__first, __middle, __comp);
}


_EXPORT
template <class _InputIter, class _RandomAccessIter, class _Compare,
          class _Dist, class _TypeT>
_RandomAccessIter __partial_sort_copy (_InputIter __first,
                                       _InputIter __last,
                                       _RandomAccessIter __first2,
                                       _RandomAccessIter __last2,
                                       _Compare __comp,
                                       _Dist*, _TypeT*)
{
    _RWSTD_ASSERT_RANGE (__first, __last);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    if (__first2 == __last2)
        return __last2;

    _RandomAccessIter __res = __first2;

    for (; !(__first == __last) && !(__res == __last2); ++__first, ++__res)
        *__res = *__first;

    _STD::make_heap (__first2, __res, __comp);

    for (; !(__first == __last); ++__first) {
        if (__comp (*__first, *__first2))
            __adjust_heap (__first2, _Dist (), _Dist (__res - __first2),
                           *__first, __comp);
    }

    _STD::sort_heap (__first2, __res, __comp);

    return __res;
}


// David R. Musser's Introspective Sorting algorithm
// (see www.cs.rpi.edu/~musser/gp/introsort.ps)
// O(N * log (N)) worst case complexity
_EXPORT
template <class _RandomAccessIter, class _Dist, class _Compare>
void __introsort_loop (_RandomAccessIter __first, _RandomAccessIter __last,
                       _Dist __max_depth, _Compare __comp)
{
    for ( ; __rw_threshold < __last - __first; ) {
        if (0 == __max_depth) {
            __partial_sort (__first, __last, __last,
                            _RWSTD_VALUE_TYPE (_RandomAccessIter), __comp);
            break;
        }

        _RandomAccessIter __cut =
            __unguarded_partition (__first, __last,
                                   __median (*__first,
                                             *(__first + (__last - __first) /2),
                                             *(__last - 1), __comp), __comp);

        // limit the depth of the recursion tree to log2 (last - first)
        // where first and last are the initial values passed in from sort()
        __introsort_loop (__cut, __last, __max_depth /= 2, __comp);
        __last = __cut;
    }
}


_EXPORT
template <class _RandomAccessIter, class _TypeT, class _Compare>
void __nth_element (_RandomAccessIter __first, _RandomAccessIter __nth,
                    _RandomAccessIter __last, _TypeT*, _Compare __comp)
{
    _RWSTD_ASSERT_IN_RANGE (__nth, __first, __last);

    while (__last - __first > 3) {
        const _RandomAccessIter __cut =
            __unguarded_partition (__first, __last,
                                   _TypeT (__median (*__first,
                                                     * (__first +
                                                        (__last - __first)/2),
                                                     * (__last - 1),
                                                     __comp)),
             __comp);
        if (!(__nth < __cut))
            __first = __cut;
        else
            __last = __cut;
    }
    __insertion_sort (__first, __last, __comp);
}

//
// Binary search.
//

_EXPORT
template <class _FwdIter, class _TypeT, class _Compare, class _Dist>
_FwdIter __lower_bound (_FwdIter __first, _FwdIter __last,
                        const _TypeT& __val, _Compare __comp,
                        _Dist*, forward_iterator_tag)
{
    _RWSTD_ASSERT_RANGE (__first, __last);
    
    for (_Dist __dist = _DISTANCE (__first, __last, _Dist); __dist; ) {

        const _Dist __half = __dist / 2;
        _FwdIter __middle = __first;

        _STD::advance (__middle, __half);

        if (__comp (*__middle, __val)) {
            __first = __middle;
            ++__first;
            __dist = __dist - __half - 1;
        }
        else
            __dist = __half;
    }
    return __first;
}


_EXPORT
template <class _RandomAccessIter, class _TypeT, class _Compare,
          class _Dist>
_RandomAccessIter __lower_bound (_RandomAccessIter __first,
                                 _RandomAccessIter __last,
                                 const _TypeT& __val,
                                 _Compare __comp,
                                 _Dist*,
                                 random_access_iterator_tag)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for (_Dist __dist = __last - __first; __dist > 0; ) {

        const _Dist __half = __dist / 2;
        const _RandomAccessIter __middle = __first + __half;

        if (__comp (*__middle, __val)) {
            __first = __middle + 1;
            __dist  = __dist - __half - 1;
        }
        else
            __dist = __half;
    }

    return __first;
}


_EXPORT
template <class _FwdIter, class _TypeT, class _Compare, class _Dist>
_FwdIter __upper_bound (_FwdIter __first, _FwdIter __last,
                        const _TypeT& __val, _Compare __comp,
                        _Dist*, forward_iterator_tag)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for (_Dist __dist = _DISTANCE (__first, __last, _Dist); __dist; ) {

        const _Dist __half = __dist / 2;
        _FwdIter __middle = __first;

        _STD::advance (__middle, __half);

        if (__comp (__val, *__middle))
            __dist = __half;
        else {
            __first = __middle;
            ++__first;
            __dist = __dist - __half - 1;
        }
    }

    return __first;
}


_EXPORT
template <class _RandomAccessIter, class _TypeT, class _Compare, class _Dist>
_RandomAccessIter __upper_bound (_RandomAccessIter __first,
                                 _RandomAccessIter __last,
                                 const _TypeT& __val,
                                 _Compare __comp, _Dist*,
                                 random_access_iterator_tag)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for (_Dist __dist = __last - __first; __dist; ) {

        const _Dist __half = __dist / 2;
        const _RandomAccessIter __middle = __first + __half;

        if (__comp (__val, *__middle))
            __dist = __half;
        else {
            __first = __middle + 1;
            __dist = __dist - __half - 1;
        }
    }

    return __first;
}


_EXPORT
template <class _FwdIter, class _TypeT, class _Compare, class _Dist>
pair<_FwdIter, _FwdIter>
__equal_range (_FwdIter __first, _FwdIter __last, const _TypeT& __val,
               _Compare __comp, _Dist*, forward_iterator_tag)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for (_Dist __dist = _DISTANCE (__first, __last, _Dist); __dist; ) {

        const _Dist __half = __dist / 2;
        _FwdIter __middle = __first;

        _STD::advance (__middle, __half);

        if (__comp (*__middle, __val)) {
            __first = __middle;
            ++__first;
            __dist = __dist - __half - 1;
        }
        else if (__comp (__val, *__middle))
            __dist = __half;
        else {
            const _FwdIter __left =
                _STD::lower_bound (__first, __middle, __val, __comp);

            _STD::advance (__first, __dist);

            const _FwdIter __right =
                _STD::upper_bound (++__middle, __first, __val, __comp);

            return pair<_FwdIter, _FwdIter>(__left, __right);
        }
    }

    return pair<_FwdIter, _FwdIter>(__first, __first);
}


_EXPORT
template <class _RandomAccessIter, class _TypeT, class _Compare,
          class _Dist>
pair<_RandomAccessIter, _RandomAccessIter>
__equal_range (_RandomAccessIter __first, _RandomAccessIter __last,
               const _TypeT& __val, _Compare __comp, _Dist*,
               random_access_iterator_tag)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for (_Dist __dist = __last - __first; __dist; ) {

        const _Dist __half = __dist / 2;
        const _RandomAccessIter __middle = __first + __half;

        if (__comp (*__middle, __val)) {
            __first = __middle + 1;
            __dist  = __dist - __half - 1;
        }
        else if (__comp (__val, *__middle))
            __dist = __half;
        else {
            const _RandomAccessIter __left =
                _STD::lower_bound (__first, __middle, __val, __comp);

            const _RandomAccessIter __right =
                _STD::upper_bound (__middle + 1, __first + __dist,
                                   __val, __comp);

            return pair<_RandomAccessIter, _RandomAccessIter>(__left, __right);
        }
    }

    return pair<_RandomAccessIter, _RandomAccessIter>(__first, __first);
}

//
// Merge
//

_EXPORT
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter merge (_InputIter1 __first1, _InputIter1 __last1,
                   _InputIter2 __first2, _InputIter2 __last2,
                   _OutputIter __res, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    for (; !(__first1 == __last1) && !(__first2 == __last2); ++__res) {
        if (__comp (*__first2, *__first1)) {
            *__res = *__first2;
            ++__first2;
        }
        else {
            *__res = *__first1;
            ++__first1;
        }
    }
    return _STD::copy (__first2, __last2, _STD::copy(__first1, __last1, __res));
}


_EXPORT
template <class _BidirIter, class _Dist, class _Compare>
void __merge_without_buffer (_BidirIter __first,
                             _BidirIter __middle,
                             _BidirIter __last,
                             _Dist __dist1, _Dist __dist2,
                             _Compare __comp)
{
    _RWSTD_ASSERT_IN_RANGE (__middle, __first, __last);

    if (__dist1 == 0 || __dist2 == 0)
        return;

    if (__dist1 + __dist2 == 2) {
        if (__comp (*__middle, *__first))
            _STD::iter_swap (__first, __middle);
        return;
    }

    _BidirIter __first_cut = __first;
    _BidirIter __second_cut = __middle;
    _Dist __dist11;
    _Dist __dist22;

    if (__dist1 > __dist2) {
        __dist11 = __dist1 / 2;
        _STD::advance (__first_cut, __dist11);
        __second_cut = _STD::lower_bound (__middle, __last,
                                          *__first_cut, __comp);
        __dist22 = _DISTANCE (__middle, __second_cut, _Dist);
    }
    else {
        __dist22 = __dist2 / 2;
        _STD::advance (__second_cut, __dist22);
        __first_cut = _STD::upper_bound (__first, __middle,
                                            *__second_cut, __comp);
        __dist11 = _DISTANCE (__first, __first_cut, _Dist);
    }
    _STD::rotate (__first_cut, __middle, __second_cut);

    _BidirIter __middle2 = __first_cut;

    _STD::advance (__middle2, __dist22);

    __merge_without_buffer (__first, __first_cut, __middle2,
                            __dist11, __dist22, __comp);
    __merge_without_buffer (__middle2, __second_cut, __last,
                            __dist1 - __dist11, __dist2 - __dist22, __comp);
}

_EXPORT
template <class _BidirIter1, class _BidirIter2, class _Dist>
_BidirIter1 __rotate_adaptive (_BidirIter1 __first,
                               _BidirIter1 __middle,
                               _BidirIter1 __last,
                               _Dist __dist1, _Dist __dist2,
                               _BidirIter2 __buf,
                               _Dist __buf_size)
{
    _RWSTD_ASSERT_IN_RANGE (__middle, __first, __last);

    if (__dist1 > __dist2 && __dist2 <= __buf_size) {
        _BidirIter2 __buf_end = _STD::copy (__middle, __last, __buf);
        _STD::copy_backward (__first, __middle, __last);
        return _STD::copy (__buf, __buf_end, __first);
    }

    if (__dist1 <= __buf_size) {
        _BidirIter2 __buf_end = _STD::copy (__first, __middle, __buf);
        _STD::copy (__middle, __last, __first);
        return _STD::copy_backward (__buf, __buf_end, __last);
    }

    _STD::rotate (__first, __middle, __last);
    _STD::advance (__first, __dist2);
    return __first;
}


template <class _BidirIter1, class _BidirIter2,
          class _BidirIter3>
_BidirIter3 __merge_backward (_BidirIter1 __first1,
                              _BidirIter1 __last1,
                              _BidirIter2 __first2,
                              _BidirIter2 __last2,
                              _BidirIter3 __res)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    if (__first1 == __last1)
        return _STD::copy_backward (__first2, __last2, __res);

    if (__first2 == __last2)
        return _STD::copy_backward (__first1, __last1, __res);

    for (--__last1, --__last2; ; ) {
        if (*__last2 < *__last1) {
            *--__res = *__last1;
            if (__first1 == __last1)
                return _STD::copy_backward (__first2, ++__last2, __res);
            --__last1;
        }
        else {
            *--__res = *__last2;
            if (__first2 == __last2)
                return _STD::copy_backward (__first1, ++__last1, __res);
            --__last2;
        }
    }
}


_EXPORT
template <class _BidirIter1, class _BidirIter2,
          class _BidirIter3, class _Compare>
_BidirIter3 __merge_backward (_BidirIter1 __first1,
                              _BidirIter1 __last1,
                              _BidirIter2 __first2,
                              _BidirIter2 __last2,
                              _BidirIter3 __res,
                              _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    if (__first1 == __last1)
        return _STD::copy_backward (__first2, __last2, __res);
    if (__first2 == __last2)
        return _STD::copy_backward (__first1, __last1, __res);

    for (--__last1, --__last2; ; ) {
        if (__comp (*__last2, *__last1)) {
            *--__res = *__last1;
            if (__first1 == __last1)
                return _STD::copy_backward (__first2, ++__last2, __res);
            --__last1;
        }
        else {
            *--__res = *__last2;
            if (__first2 == __last2)
                return _STD::copy_backward (__first1, ++__last1, __res);
            --__last2;
        }
    }
}


template <class _BidirIter, class _Dist, class _Pointer, class _Compare>
void __merge_adaptive (_BidirIter __first, _BidirIter __middle,
                       _BidirIter __last,
                       _Dist __dist1, _Dist __dist2,
                       _Pointer __buf, _Dist __buf_size,
                       _Compare __comp)
{
    _RWSTD_ASSERT_IN_RANGE (__middle, __first, __last);
    
    if (__dist1 <= __dist2 && __dist1 <= __buf_size)
    {
        _Pointer __buf_end = _STD::copy (__first, __middle, __buf);
        _STD::merge (__buf, __buf_end, __middle, __last, __first, __comp);
    }
    else if (__dist2 <= __buf_size)
    {
        _Pointer __buf_end = _STD::copy (__middle, __last, __buf);
        __merge_backward (__first, __middle, __buf, __buf_end, __last,
                          __comp);
    }
    else
    {
        _BidirIter __first_cut = __first;
        _BidirIter __second_cut = __middle;
        _Dist __dist11;
        _Dist __dist22;
        if (__dist1 > __dist2)
        {
            __dist11 = __dist1 / 2;
            _STD::advance (__first_cut, __dist11);
            __second_cut = _STD::lower_bound (__middle, __last, *__first_cut,
                                              __comp);
            __dist22 = _DISTANCE (__middle, __second_cut, _Dist);
        }
        else
        {
            __dist22 = __dist2 / 2;
            _STD::advance (__second_cut, __dist22);
            __first_cut = _STD::upper_bound (__first, __middle, *__second_cut,
                                                __comp);
            __dist11 = _DISTANCE (__first, __first_cut, _Dist);
        }
        _BidirIter __middle2 =
            __rotate_adaptive (__first_cut, __middle, __second_cut,
                               __dist1 - __dist11, __dist22,
                               __buf, __buf_size);
        __merge_adaptive (__first, __first_cut, __middle2,
                          __dist11, __dist22,
                          __buf, __buf_size, __comp);
        __merge_adaptive (__middle2, __second_cut, __last,
                          __dist1 - __dist11, __dist2 - __dist22,
                          __buf, __buf_size, __comp);
    }
}


_EXPORT
template <class _BidirIter, class _Dist, class _TypeT, class _Compare>
void __inplace_merge (_BidirIter __first, _BidirIter __middle,
                      _BidirIter __last, _Dist*, _TypeT*, _Compare __comp)
{
    _Dist __dist1 = _DISTANCE (__first, __middle, _Dist);
    _Dist __dist2 = _DISTANCE (__middle, __last, _Dist);
 
    // call an extension of get_temporary_buffer<>() in case partial class
    // specialization (and iterator_traits<>) isn't supported by compiler
    pair<_TypeT*, _Dist> __pair =
        _STD::get_temporary_buffer (__dist1 + __dist2, (_TypeT*)0);

    if (__pair.first == 0) {
        __merge_without_buffer (__first, __middle, __last, __dist1, __dist2,
                                __comp);
    }
    else {
        _Dist __dist = (_STD::min)(__pair.second, __dist1 + __dist2);
        
        _STD::fill_n (raw_storage_iterator<_TypeT*, _TypeT> (__pair.first),
                      __dist, *__first);
        __merge_adaptive (__first, __middle, __last, __dist1, __dist2,
                          __pair.first, __pair.second, __comp);

        for (_TypeT *__ptr = __pair.first + __dist;
             !(__pair.first == --__ptr); )
            (*__ptr).~_TypeT ();

        _STD::return_temporary_buffer (__pair.first);
    }
}

//
// Set operations. Assume sorted sequences.
//


// 25.3.5.1 - returns true iff every (not necessarily distinct) element
// in [first2, last2) occurs (at least as many times) in [first1, last1)
_EXPORT
template <class _InputIter1, class _InputIter2, class _Compare>
bool includes (_InputIter1 __first1, _InputIter1 __last1,
               _InputIter2 __first2, _InputIter2 __last2,
               _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    for (; !(__first1 == __last1) && !(__first2 == __last2); ++__first1) {
        if (__comp (*__first2, *__first1))
            return false;

        if (__comp (*__first1, *__first2) == false)
            ++__first2;
    }

    return __first2 == __last2;
}


_EXPORT
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter set_union (_InputIter1 __first1, _InputIter1 __last1,
                       _InputIter2 __first2, _InputIter2 __last2,
                       _OutputIter __res, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    for (; !(__first1 == __last1) && !(__first2 == __last2); ++__res) {
        if (__comp (*__first1, *__first2)) {
            *__res = *__first1;
            ++__first1;
        }
        else if (__comp (*__first2, *__first1)) {
            *__res = *__first2;
            ++__first2;
        }
        else {
            *__res = *__first1;
            ++__first1;
            ++__first2;
        }
    }
    return _STD::copy (__first2, __last2,
                       _STD::copy (__first1, __last1, __res));
}


_EXPORT
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter set_intersection (_InputIter1 __first1, _InputIter1 __last1,
                              _InputIter2 __first2, _InputIter2 __last2,
                              _OutputIter __res, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    while (!(__first1 == __last1) && !(__first2 == __last2)) {
        if (__comp (*__first1, *__first2))
            ++__first1;
        else if (__comp (*__first2, *__first1))
            ++__first2;
        else {
            *__res = *__first1;
            ++__res;
            ++__first1;
            ++__first2;
        }
    }

    return __res;
}


_EXPORT
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter set_difference (_InputIter1 __first1, _InputIter1 __last1,
                            _InputIter2 __first2, _InputIter2 __last2,
                            _OutputIter __res, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    while (!(__first1 == __last1) && !(__first2 == __last2)) {
        if (__comp (*__first1, *__first2)) {
            *__res = *__first1;
            ++__res;
            ++__first1;
        }
        else if (__comp (*__first2, *__first1))
            ++__first2;
        else {
            ++__first1;
            ++__first2;
        }
    }

    return _STD::copy (__first1, __last1, __res);
}


_EXPORT
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter set_symmetric_difference (_InputIter1 __first1,
                                      _InputIter1 __last1,
                                      _InputIter2 __first2,
                                      _InputIter2 __last2,
                                      _OutputIter __res,
                                      _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    while (!(__first1 == __last1) && !(__first2 == __last2)) {
        if (__comp (*__first1, *__first2)) {
            *__res = *__first1;
            ++__res;
            ++__first1;
        }
        else if (__comp (*__first2, *__first1)) {
            *__res = *__first2;
            ++__res;
            ++__first2;
        }
        else {
            ++__first1;
            ++__first2;
        }
    }

    return _STD::copy (__first2, __last2, _STD::copy(__first1, __last1, __res));
}

//
// Minimum and maximum.
//


_EXPORT
template <class _FwdIter, class _Compare>
_FwdIter min_element (_FwdIter __first, _FwdIter __last, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    _FwdIter __res = __first;

    if (!(__first == __last)) {
        while (!(++__first == __last)) {
            if (__comp (*__first, *__res))
                __res = __first;
        }
    }

    return __res;
}


_EXPORT
template <class _FwdIter, class _Compare>
_FwdIter max_element (_FwdIter __first, _FwdIter __last, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    _FwdIter __res = __first;

    if (!(__first == __last)) {
        while (!(++__first == __last)) {
            if (__comp (*__res, *__first))
                __res = __first;
        }
    }

    return __res;
}

//
// Permutations.
//

_EXPORT
template <class _BidirIter, class _Compare>
bool next_permutation (_BidirIter __first, _BidirIter __last, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (__first == __last)
        return false;

    _BidirIter __i = __first;

    if (++__i == __last)
        return false;

    bool __res;

    for (__i = __last, --__i; ; ) {
        const _BidirIter __ii = __i;

        if (__comp (*--__i, *__ii)) {

            _BidirIter __j = __last;

            while (__comp (*__i, *--__j) == false);

            _STD::iter_swap (__i, __j);

            __first = __ii;
            __res   = true;
            break;
        }

        if (__i == __first) {
            __res = false;
            break;
        }
    }

    _STD::reverse (__first, __last);

    return __res;
}


_EXPORT
template <class _BidirIter, class _Compare>
bool prev_permutation (_BidirIter __first, _BidirIter __last, _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (__first == __last)
        return false;

    _BidirIter __i = __first;

    if (++__i == __last)
        return false;

    bool __res;

    for (__i = __last, --__i; ; ) {
        const _BidirIter __ii = __i;

        if (__comp (*__ii, *--__i)) {

            _BidirIter __j = __last;

            while (__comp (*--__j, *__i) == false);

            _STD::iter_swap (__i, __j);

            __first = __ii;
            __res   = true;
            break;
        }

        if (__i == __first) {
            __res = false;
            break;
        }
    }

    _STD::reverse (__first, __last);

    return __res;
}


}   // namespace std
