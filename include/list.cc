/***************************************************************************
 *
 * list.cc - Non-nline list definitions for the Standard Library
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

template <class _TypeT, class _Allocator>
list<_TypeT, _Allocator>::
~list ()
{
    if (_C_node) {
        clear ();
        _C_put_node (_C_node);
        _RWSTD_NODE_LIST_FREE ();
    }
}


template <class _TypeT, class _Allocator>
typename list<_TypeT, _Allocator>::iterator 
list<_TypeT, _Allocator>::erase (iterator __first, iterator __last)
{
    _RWSTD_ASSERT_RANGE (begin (), __first);
    _RWSTD_ASSERT_RANGE (__first, __last);

    while (!(__first == __last)) {
        __first = erase (__first);
    }
      
    return __first;
}


#if !defined (_RWSTD_NO_LIST_NODE_BUFFER)

template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::_C_add_buffer (bool __is_list_empty)
{
    // empty list gets one node
    _RWSTD_SIZE_T __next_buffer_size = 1;
    if(!__is_list_empty) {
        if ((void*)_C_buflist) {
            __next_buffer_size = 
                _RWSTD_NEW_CAPACITY (list, this, _C_buflist->_C_bufsize);
        }
        else {
            __next_buffer_size = _RWSTD_NEW_CAPACITY (list, this, 0);
        }          
     }
    _C_buf_pointer __tmp = 
        _C_buf_alloc_type (*this).allocate (1, (void*)_C_buflist);

    _TRY {
        __tmp->_C_buffer = 
           _C_node_alloc_type (*this).allocate (__next_buffer_size,
                                                (void*)_C_last);
    }
    _CATCH (...) {
        _C_buf_alloc_type (*this).deallocate (__tmp, 1);
        _RETHROW;
    }

    __tmp->_C_next_buf = _C_buflist;
    __tmp->_C_bufsize  = __next_buffer_size;

    _C_buflist    = __tmp;
    _C_next_avail = _C_buflist->_C_buffer; 
    _C_last       = _C_next_avail + __next_buffer_size;
}


template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::_C_free_buffers ()
{
    while ((void*)_C_buflist) {
        _C_buf_pointer __tmp = _C_buflist;
        _C_buflist = _C_buflist->_C_next_buf;
        _C_node_alloc_type (*this).deallocate (__tmp->_C_buffer, 
                                               __tmp->_C_bufsize);
        _C_buf_alloc_type (*this).deallocate (__tmp, 1);
    }
    _C_free_list  = 0;
    _C_next_avail = 0;
    _C_last       = 0;
}

#endif // defined (_RWSTD_NO_LIST_NODE_BUFFER)


template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::_C_transfer (iterator __it, iterator __first, 
                                           iterator __last, list& __x)
{
    _RWSTD_ASSERT_RANGE (begin (), __it);
    _RWSTD_ASSERT_RANGE (__first, __last);

    if (this == &__x) {
        (*(_C_link_type ((*_ITER_NODE (__last))._C_prev)))._C_next =
            _ITER_NODE (__it);
        (*(_C_link_type ((*_ITER_NODE (__first))._C_prev)))._C_next =
            _ITER_NODE (__last);
        (*(_C_link_type ((*_ITER_NODE (__it))._C_prev)))._C_next =
            _ITER_NODE (__first);
        
        _C_link_type __tmp = _C_link_type ((*_ITER_NODE (__it))._C_prev);
        
        (*_ITER_NODE (__it))._C_prev    = (*_ITER_NODE (__last))._C_prev;
        (*_ITER_NODE (__last))._C_prev  = (*_ITER_NODE (__first))._C_prev; 
        (*_ITER_NODE (__first))._C_prev = __tmp;
    }
    else {
        insert (__it, __first, __last);
        __x.erase (__first, __last);
    }
}


template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::resize (size_type __size, _TypeT __val)
{
    if (__size > size ())
        insert (end (), __size - size (), __val);
    else if (__size < size ()) {
        iterator __tmp = begin ();
        advance (__tmp, __size);
        erase (__tmp, end ());
    }
}


template <class _TypeT, class _Allocator>
list<_TypeT, _Allocator>&
list<_TypeT, _Allocator>::operator= (const list<_TypeT, _Allocator>& __rhs)
{
    if (this != &__rhs) {
        iterator       __first1 = begin ();
        iterator       __last1  = end ();
        const_iterator __first2 = __rhs.begin ();
        const_iterator __last2  = __rhs.end ();

        for ( ; !(__first1 == __last1) && !(__first2 == __last2);
              ++__first1, ++__first2)
            *__first1 = *__first2;

        if (__first2 == __last2)
            erase (__first1, __last1);
        else
            insert (__last1, __first2, __last2);
    }
    return *this;
}


template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::splice (iterator __it, list &__x)
{
    // assert 23.2.2.4, p3
    _RWSTD_ASSERT (&__x != this);
    _RWSTD_ASSERT_RANGE (begin (), __it);


#if !defined (_RWSTD_NO_LIST_NODE_BUFFER)
    // linear complexity
    insert (__it, __x.begin (), __x.end ());
    __x.clear ();
#else
    if (get_allocator () == __x.get_allocator ()) {

        // constant complexity

        // relink `it' and the head of `x'
        (*(_C_link_type ((*_ITER_NODE (__it))._C_prev)))._C_next =
            __x._C_node->_C_next;

        __x._C_node->_C_next->_C_prev = (*_ITER_NODE (__it))._C_prev;
        (*_ITER_NODE (__it))._C_prev  = __x._C_node->_C_prev;
        __x._C_node->_C_prev->_C_next = _ITER_NODE (__it);

        // empty out `x'
        __x._C_node->_C_next =
        __x._C_node->_C_prev = __x._C_node;

        // adjust sizes
        _C_length     += __x._C_length;
        __x._C_length  = 0;
    }
    else {
        // linear complexity
        insert (__it, __x.begin (), __x.end ());
        __x.clear ();
    }
#endif // _RWSTD_NO_LIST_NODE_BUFFER
}


template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::splice (iterator __i, list &__x, iterator __j)
{
    _RWSTD_ASSERT_RANGE (begin (), __i);
    _RWSTD_ASSERT_RANGE (__x.begin (), __j);
    
    if (   !(__x.end () == __j)
        && !(__i == __j) && !(__i == ++iterator (__j))) {

#if !defined (_RWSTD_NO_LIST_NODE_BUFFER)
        // linear complexity
        insert (__i, *__j);
        __x.erase (__j);
#else
        if (get_allocator () == __x.get_allocator ()) {

            // constant complexity

            // relink `i' and `j'
            (*(_C_link_type ((*_ITER_NODE (__i))._C_prev)))._C_next =
                _ITER_NODE (__j);

            // save position before `i'
            _C_link_type __tmp = (*_ITER_NODE (__i))._C_prev;

            (*_ITER_NODE (__i))._C_prev = _ITER_NODE (__j);
            (*_ITER_NODE (__j))._C_prev->_C_next = (*_ITER_NODE (__j))._C_next;
            (*_ITER_NODE (__j))._C_next->_C_prev = (*_ITER_NODE (__j))._C_prev;

            (*_ITER_NODE (__j))._C_next = _ITER_NODE (__i);
            (*_ITER_NODE (__j))._C_prev = __tmp;

            // adjust sizes
            ++_C_length;
            --__x._C_length;
        }
        else {
            // linear complexity
            insert (__i, *__j);
            __x.erase (__j);
        }
#endif  //  _RWSTD_NO_LIST_NODE_BUFFER
    }
}


template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::
splice (iterator __i, list &__x, iterator __j, iterator __k)
{
    _RWSTD_ASSERT_RANGE (begin (), __i);
    _RWSTD_ASSERT_RANGE (__x.begin (), __j);
    _RWSTD_ASSERT_RANGE (__j, __k);

    // 23.2.2.4, p12 - check undefined behavior
    _RWSTD_ASSERT (&__x != this || (__i != __j && __i != __k));

    if (__j == __k)
        return;

#if !defined (_RWSTD_NO_LIST_NODE_BUFFER)
        insert (__i, __j, __k);
        __x.erase (__j, __k);
#else
    if (get_allocator () == __x.get_allocator ()) {

# ifdef _RWSTDDEBUG

        for (iterator __it = __j; __it != __k; ++__it) {
            // 23.2.2.4, p12 - check undefined behavior (linear time)
            _RWSTD_ASSERT (&__x != this || __i != __j);

            ++_C_length;
            --__x._C_length;
        }

# else   // if !defined (_RWSTDDEBUG)

        if (&__x != this) {
            // adjust sizes first
            for (iterator __it = __j; __it != __k; ++__it) {
                ++_C_length;
                --__x._C_length;
            }
        }

# endif   // _RWSTDDEBUG

        (*(_C_link_type ((*_ITER_NODE (__k))._C_prev)))._C_next =
            _ITER_NODE (__i);
        (*(_C_link_type ((*_ITER_NODE (__j))._C_prev)))._C_next =
            _ITER_NODE (__k);
        (*(_C_link_type ((*_ITER_NODE (__i))._C_prev)))._C_next =
            _ITER_NODE (__j);
        
        _C_link_type __tmp = _C_link_type ((*_ITER_NODE (__i))._C_prev);
        
        (*_ITER_NODE (__i))._C_prev = (*_ITER_NODE (__k))._C_prev;
        (*_ITER_NODE (__k))._C_prev = (*_ITER_NODE (__j))._C_prev; 
        (*_ITER_NODE (__j))._C_prev = __tmp;
    }
    else {
        insert (__i, __j, __k);
        __x.erase (__j, __k);
    }
#endif  //  _RWSTD_NO_LIST_NODE_BUFFER
}

template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::remove (const_reference __val)
{
    for (iterator __first = begin (), __last = end (); !(__first == __last); ) {
        iterator __next = __first;
        ++__next;
        if (*__first == __val)
            erase (__first);
        __first = __next;
    }
}


template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::unique ()
{
    iterator __first = begin ();
    iterator __last  = end ();

    if (__first == __last)
        return;

    iterator __next = __first;

    while (!(++__next == __last)) {
        if (*__first == *__next)
            erase (__next);
        else
            __first = __next;
        __next = __first;
    }
}


template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::merge (list<_TypeT, _Allocator>& __x)
{
    if (&__x == this)
        return;
    iterator __first1 = begin ();
    iterator __last1  = end ();
    iterator __first2 = __x.begin ();
    iterator __last2  = __x.end ();

    while (!(__first1 == __last1) && !(__first2 == __last2)) {
      if (*__first2 < *__first1) {
          iterator __next = __first2;
          _C_transfer (__first1, __first2, ++__next, __x);
          __first2 = __next;
      }
      else
        ++__first1;
    }

    if (!(__first2 == __last2))
        _C_transfer (__last1, __first2, __last2, __x);
}


template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::reverse ()
{
    if (size () < 2)
        return;

    for (iterator __first = ++begin (); !(__first == end ()); ) {
        iterator __tmp = __first;
        _C_transfer (begin (), __tmp, ++__first, *this);
    }
}

// sorts list by moving nodes within list; preserves iterators pointing to
// elements of the list.
template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::sort ()
{
    for (size_type __n = 1; __n < size (); __n *= 2) {
        iterator __i1 = begin (),
                 __i2 = begin (),
                 __i3 = begin ();

        _C_advance (__i2, (difference_type)__n, end ());
        _C_advance (__i3, (difference_type)(2 * __n), end ());

        for (size_type __m = 0;
             __m < (size () + (2 * __n)) / (__n * 2); __m++) {

            if (!(__i1 == end ()) && !(__i2 == end ())) {
                _C_adjacent_merge (__i1, __i2, __i3);
                __i1 = __i2 = __i3;
                _C_advance (__i2, (difference_type) __n, end ());
                _C_advance (__i3, (difference_type) 2 * __n, end ());
            }
        }
    }
}


template <class _TypeT, class _Allocator>
void list<_TypeT, _Allocator>::
_C_adjacent_merge (iterator __first1, iterator __last1, iterator __last2)
{
    difference_type __n = _DISTANCE (__first1, __last1, difference_type);

    for (iterator __first2 = __last1; __n >= 0 && !(__first2 == __last2); ) {
        if (*__first2 < *__first1) {
            iterator __next = __first2;
            _C_transfer (__first1, __first2, ++__next, *this);
            __first2 = __next;
        }
        else {
            ++__first1;
            --__n;
        }
    }
}


template <class _TypeT, class _Allocator>
template<class _Compare>
void list<_TypeT, _Allocator>::
_C_adjacent_merge (iterator __first1, iterator __last1, iterator __last2,
                   _Compare __cmp)
{
    difference_type __n = _DISTANCE (__first1, __last1, difference_type);

    for (iterator __first2 = __last1; __n >= 0 && !(__first2 == __last2); ) {
        if (__cmp (*__first2, *__first1)) {
            iterator __next = __first2;
            _C_transfer (__first1, __first2, ++__next, *this);
            __first2 = __next;
        }
        else {
            ++__first1;
            --__n;
        }
    }
}


template<class _TypeT, class _Allocator>
template<class _Predicate>
void list<_TypeT, _Allocator>::remove_if (_Predicate __pred)
{
    iterator __first = begin ();
    iterator __last = end ();

    while (!(__first == __last)) {
        iterator __next = __first;
        ++__next;
        if (__pred (*__first))
            erase (__first);
        __first = __next;
    }
}


template<class _TypeT, class _Allocator>
template<class _BinaryPredicate>
void list<_TypeT, _Allocator>::unique (_BinaryPredicate __pred)
{
    iterator __first = begin ();
    iterator __last = end ();

    if (__first == __last)
        return;

    iterator __next = __first;
    while (!(++__next == __last)) {
        if (__pred (*__first, *__next))
            erase (__next);
        else
            __first = __next;
        __next = __first;
    }
}


template<class _TypeT, class _Allocator>
template<class _Compare>
void list<_TypeT, _Allocator>::
merge (list<_TypeT, _Allocator>& __x, _Compare __cmp)
{
    if (&__x == this)
        return;
    iterator __first1 = begin ();
    iterator __last1  = end ();
    iterator __first2 = __x.begin ();
    iterator __last2  = __x.end ();

    while (!(__first1 == __last1) && !(__first2 == __last2)) {
      if (__cmp (*__first2, *__first1)) {
          iterator __next = __first2;
          _C_transfer (__first1, __first2, ++__next, __x);
          __first2 = __next;
      }
      else
          ++__first1;
    }

    if (!(__first2 == __last2))
        _C_transfer (__last1, __first2, __last2, __x);
}


template <class _TypeT, class _Allocator>
template<class _Compare>
void list<_TypeT, _Allocator>::sort (_Compare __cmp)
{
    for (size_type __n = 1; __n < size (); __n *= 2) {

        iterator __it1 = begin (), 
                 __it2 = begin (), 
                 __it3 = begin ();

        _C_advance (__it2, __n, end ());
        _C_advance (__it3, 2 * __n, end ());

        for (size_type __m = 0;
             __m != (size () + (2 * __n)) / (__n * 2); ++__m) {
            
            if (!(__it1 == end ()) && !(__it2 == end ())) {
                _C_adjacent_merge (__it1, __it2, __it3, __cmp);
                __it1 = __it3;
                __it2 = __it3;
                _C_advance (__it2, __n, end ());
                _C_advance (__it3, 2 * __n, end ());
            }
        }
    }
}


}   // namespace std
