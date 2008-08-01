/***************************************************************************
 *
 * _algobase.h - Declarations and inline definitions of frequently used
 *               C++ Standard Library algorithms
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

#ifndef _RWSTD_RW_ALGOBASE_H_INCLUDED
#define _RWSTD_RW_ALGOBASE_H_INCLUDED


#include <rw/_pair.h>       // for pair
#include <rw/_iterbase.h>   // for iterator_traits, __rw_debug_iter


_RWSTD_NAMESPACE (std) { 


// 25.3.7, p1
template <class _TypeT>
inline const _TypeT& (min)(const _TypeT& __a, const _TypeT& __b)
{
    return __b < __a ? __b : __a;
}

template <class _TypeT, class _Compare>
inline const _TypeT& (min)(const _TypeT& __a, const _TypeT& __b, _Compare __cmp)
{
    return __cmp (__b, __a) ? __b : __a;
}


// 25.3.7, p3
template <class _TypeT>
inline const _TypeT& (max)(const _TypeT& __a, const _TypeT& __b)
{
    return  __a < __b ? __b : __a;
}


template <class _TypeT, class _Compare>
inline const _TypeT&
(max)(const _TypeT& __a, const _TypeT& __b, _Compare __cmp)
{
    return !(__cmp (__a, __b) == false) ? __b : __a;
}


// 25.2.1, p1
template <class _InputIter, class _OutputIter>
inline _OutputIter
copy (_InputIter __first, _InputIter __last, _OutputIter __res)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for (; !(__first == __last); ++__first,++__res)
        *__res = *__first;
    return __res;
}


// 25.2.1, p5
template <class _BidirIter1, class _BidirIter2>
inline _BidirIter2
copy_backward (_BidirIter1 __first, _BidirIter1 __last, _BidirIter2 __res)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    while (!(__first == __last))
        *--__res = *--__last;
    return __res;
}


// 25.2.5
template <class _FwdIter, class _TypeT>
inline void fill (_FwdIter __first, _FwdIter __last, const _TypeT &__val)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

    for ( ; !(__first == __last); ++__first)
        *__first = __val;
}


template <class _OutputIter, class _Size, class _TypeT>
inline void fill_n (_OutputIter __first, _Size __n, const _TypeT &__val)
{
    // Size must be convertible to integral type but need not itself be one
    // Complexity:
    // Exactly n if n is positive, or 0 otherwise, assignments.
    // (see lwg issue 426 for the complexity when n is not positive)
    for (_RWSTD_PTRDIFF_T __inx = __n; 0 < __inx; --__inx, ++__first)
        *__first = __val;
}


}   // namespace std

_RWSTD_NAMESPACE (__rw) {

// `less than' function object - used by non-predicate forms
// of algorithms to invoke the predicate forms for code reuse
struct __rw_lt
{
    template <class _TypeT, class _TypeU>
    bool operator() (const _TypeT &__lhs, const _TypeU &__rhs) const {
        // cast away constness in case operator<
        // takes a non-const argument
        return   _RWSTD_CONST_CAST (_TypeT&, __lhs)
               < _RWSTD_CONST_CAST (_TypeU&, __rhs);
    }
};

#  define _RWSTD_LESS(ignore) _RW::__rw_lt ()


// swaps values of 2 (possibly distinct) types
template <class _TypeT, class _TypeU>
inline void __rw_swap (_TypeT& __a, _TypeU& __b)
{
    // _TypeT must satisfy Assignable and CopyConstructible
    _TypeT __tmp = __a;
    __a = __b;
    __b = __tmp;
}


}   // namespace __rw


_RWSTD_NAMESPACE (std) { 


// 25.2.2, p1
template <class _TypeT>
inline void swap (_TypeT& __a, _TypeT& __b)
{
    // lwg issue 227 - _TypeT must satisfy Assignable and CopyConstructible
    _TypeT __tmp = __a;
    __a = __b;
    __b = __tmp;
}


// 25.2.2, p7
template <class _FwdIter1, class _FwdIter2>
inline void iter_swap (_FwdIter1 __a, _FwdIter2 __b)
{
    // std::iterator_traits<_FwdIter[12]>::value_type must satisfy
    // both Assignable and CopyConstructible

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC

    // accommodate _FwdIter::operator*() that returns an rvalue that
    // doesn't bind to a non-const reference e.g., vector<bool>::reference

    typename iterator_traits<_FwdIter1>::reference __ref1 (*__a);
    typename iterator_traits<_FwdIter2>::reference __ref2 (*__b);

    _RW::__rw_swap (__ref1, __ref2);

#else   // if defined (_RWSTD_NO_CLASS_PARTIAL_SPEC)

    // the return type of std::iterator_traits<_FwdIter>::operator*()
    // must be (convertible to) a true reference (i.e., value_type&)
    _RW::__rw_swap (*__a, *__b);

#endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC
}


// 25.1.7 - Mismatch
template <class _InputIter1, class _InputIter2>
inline pair<_InputIter1, _InputIter2>
mismatch (_InputIter1 __first1, _InputIter1 __last1, _InputIter2 __first2)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);

    while (!(__first1 == __last1) && *__first1 == *__first2) {
        ++__first1;
        ++__first2;
    }
    return pair<_InputIter1, _InputIter2> (__first1, __first2);
}


template <class _InputIter1, class _InputIter2, class _BinaryPredicate>
inline pair<_InputIter1, _InputIter2>
mismatch (_InputIter1 __first1, _InputIter1 __last1,
          _InputIter2 __first2, _BinaryPredicate __pred)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);

    while (   !(__first1 == __last1)
           && !(__pred (*__first1, *__first2) == false)) {
        ++__first1;
        ++__first2;
    }
    return pair<_InputIter1, _InputIter2> (__first1, __first2);
}


// 25.1.8 - Equal
template <class _InputIter1, class _InputIter2>
inline bool
equal (_InputIter1 __first1, _InputIter1 __last1, _InputIter2 __first2)
{
    return __last1 == _STD::mismatch (__first1, __last1, __first2).first;
}


template <class _InputIter1, class _InputIter2, class _BinaryPredicate>
inline bool equal (_InputIter1 __first1, _InputIter1 __last1,
                   _InputIter2 __first2, _BinaryPredicate __pred)
{
    return __last1 == _STD::mismatch (__first1, __last1,
                                      __first2, __pred).first;
}


// 25.3.8 - Lexicographical Comparison
_EXPORT
template <class _InputIter1, class _InputIter2>
bool lexicographical_compare (_InputIter1 __first1, _InputIter1 __last1,
                              _InputIter2 __first2, _InputIter2 __last2);


_EXPORT
template <class _InputIter1, class _InputIter2, class _Compare>
bool lexicographical_compare(_InputIter1 __first1, _InputIter1 __last1,
                             _InputIter2 __first2, _InputIter2 __last2,
                             _Compare __comp);

}   // namespace std


#ifdef _RWSTD_NO_IMPLICIT_INCLUSION
#  include <rw/_algobase.cc>
#endif   // _RWSTD_NO_IMPLICIT_INCLUSION


#endif   // _RWSTD_RW_ALGOBASE_H_INCLUDED
