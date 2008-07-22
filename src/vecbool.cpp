/***************************************************************************
 *
 * vecbool.cpp - source for the C++ Standard Library vector<bool>
 *               specialization
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
 * License, Version  2.0 (the  License); you may  not use  this file
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

#define _RWSTD_LIB_SRC

#include <vector>
#include <rw/_relops.h>
#include <rw/_defs.h>


#if !defined (_RWSTD_NO_BOOL) && !defined (_RWSTD_NO_VECTOR_BOOL)

#  if defined (_RWSTD_NO_CLASS_PARTIAL_SPEC)

  // Duplicates of the following functions exist in include/vector.cc.
  // Which set is used depends on the availability of partial specialization.

_RWSTD_NAMESPACE (std) { 


void vector<bool, allocator<bool> >::flip ()
{
    _USING (namespace rel_ops);

    for (iterator __i = begin(); __i != end(); __i++)
        *__i = !*__i;
}


void vector<bool, allocator<bool> >::
swap (vector<bool, allocator<bool> >::reference __x, 
      vector<bool, allocator<bool> >::reference __y)
{
    const bool __tmp = __x;
     __x = __y;
     __y = __tmp;
}


void vector<bool, allocator<bool> >::
_C_insert (iterator pos, bool __x)
{
    if (_C_end._C_p != _C_bufend) {
        _C_copy_backward(pos, _C_end - 1, _C_end);
        *pos = __x;
        ++_C_end;
    }
    else
    {
        size_type __len = size() ? 2 * size() : _RWSTD_WORD_BIT;
        unsigned int* __q = _C_bit_alloc(__len);
        iterator __i = _C_copy(begin(), pos, iterator(__q, 0));
        *__i++ = __x;
        _C_end = _C_copy(pos, end(), __i);
        _C_value_alloc_type (*this).
            deallocate(_C_begin._C_p,_C_bufend-_C_begin._C_p);
        _C_bufend = __q + (__len + _RWSTD_WORD_BIT - 1)/_RWSTD_WORD_BIT;
        _C_begin = iterator(__q, 0);
    }
}


void vector<bool, allocator<bool> >::
insert (iterator pos, size_type __n, const bool& __x)
{
    if (__n == 0)
        return;

    if (capacity() - size() >= __n) {
        _C_copy_backward(pos, end(), _C_end + __n);
        _C_fill(pos, pos + __n, __x);
        _C_end += __n;
    }
    else {
        size_type __len = size() + max(size(), __n);
        unsigned int* __q = _C_bit_alloc(__len);
        iterator __i = _C_copy(begin(), pos, iterator(__q, 0));
        _C_fill_n(__i, __n, __x);
        _C_end = _C_copy(pos, end(), __i + __n);
        _C_value_alloc_type (*this).
            deallocate(_C_begin._C_p,_C_bufend-_C_begin._C_p);
        _C_bufend = __q + (__n + _RWSTD_WORD_BIT - 1)/_RWSTD_WORD_BIT;
        _C_begin = iterator(__q, 0);
    }
}


void vector<bool, allocator<bool> >::
resize (size_type __new_size, bool __c)
{
    if (__new_size > size())
        insert(end(), __new_size - size(), __c);
    else if (__new_size < size())
        erase(begin() + __new_size, end());
}


}   // namespace std


#  endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC
#endif   //  !defined (_RWSTD_NO_BOOL) && !defined (_RWSTD_NO_VECTOR_BOOL)
