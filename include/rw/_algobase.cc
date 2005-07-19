/***************************************************************************
 *
 * _algobase.cc - Definitions of out-of-line C++ Standard Library algorithms
 *
 * $Id: //stdlib/dev/include/rw/_algobase.cc#14 $
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
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/


_RWSTD_NAMESPACE (std) { 


_EXPORT
template <class _InputIter1, class _InputIter2>
bool lexicographical_compare (_InputIter1 __first1, _InputIter1 __last1,
                              _InputIter2 __first2, _InputIter2 __last2)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    for ( ; ; ++__first2, ++__first1) {

        if (__first1 == __last1)
            return !(__first2 == __last2);

        if (__first2 == __last2 || *__first2 < *__first1)
            break;

        if (*__first1 < *__first2)
            return true;
    }
    return false;
}


_EXPORT
template <class _InputIter1, class _InputIter2, class _Compare>
bool lexicographical_compare (_InputIter1 __first1, _InputIter1 __last1,
                              _InputIter2 __first2, _InputIter2 __last2,
                              _Compare __comp)
{
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    for ( ; ; ++__first2, ++__first1) {

        if (__first1 == __last1)
            return !(__first2 == __last2);

        if (__first2 == __last2 || !(__comp (*__first2, *__first1) == false))
            break;

        if (!(__comp (*__first1, *__first2) == false))
            return true;
    }

    return false;
}


}   // namespace std
