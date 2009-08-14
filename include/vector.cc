/***************************************************************************
 *
 * vector.cc - Non-inline definitions for the Standard Library vector class
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
vector<_TypeT, _Allocator>&
vector<_TypeT, _Allocator>::
operator= (const vector &__rhs)
{
#ifndef _RWSTD_NO_EXT_VECTOR_ASSIGN_IN_PLACE

    if (capacity () < __rhs.size ())
        vector (__rhs).swap (*this);
    else if (&__rhs != this)
        assign (__rhs.begin (), __rhs.end ());

#else   // if defined (_RWSTD_NO_EXT_VECTOR_ASSIGN_IN_PLACE)

    vector (__rhs).swap (*this);

#endif   // _RWSTD_NO_EXT_VECTOR_ASSIGN_IN_PLACE

    return *this;
}


template <class _TypeT, class _Allocator>
void
vector<_TypeT, _Allocator>::
_C_realloc (size_type __n)
{
    const size_type __max_size = max_size ();

    // compute the new capacity from the greater of the current size
    // and the requested size, but no greater than max_size()
    size_type __cap = _RWSTD_NEW_CAPACITY (vector, this, (max)(size (), __n));

    // do not exceed max_size()
    if (__max_size < __cap)
        __cap = __max_size;

    // create an empty temporary vector
    vector __tmp (get_allocator ());

    // allocate storage of requested capacity
    __tmp._C_alloc._C_begin = _C_alloc.allocate (__cap, this);

    // initialize pointers
    __tmp._C_alloc._C_end    = __tmp._C_alloc._C_begin;
    __tmp._C_alloc._C_bufend = __tmp._C_alloc._C_begin + __cap;

    // copy *this into the temporary one element at a time, as if
    // by calling std::unitialized_copy(), growing the temporary
    // at each iteration so that an exception thrown by the copy
    // ctor will cause the destruction of all already constructed
    // elements (by invoking the temporary's dtor)
    for (pointer __ptr = _C_alloc._C_begin; !(__ptr == _C_alloc._C_end);
         ++__ptr) {
        __tmp._C_push_back (*__ptr);
    }

    // swap *this with the temporary, having its dtor clean up
    swap (__tmp);
}


template <class _TypeT, class _Allocator>
void vector<_TypeT, _Allocator>::
_C_destroy (iterator __first)
{
    _RWSTD_ASSERT_RANGE (__first, end ());

    for (size_type __n = end () - __first; !(0 == __n); --__n)
        _C_alloc.destroy (--_C_alloc._C_end);
}


template <class _TypeT, class _Allocator>
void vector<_TypeT, _Allocator>::
_C_unsafe_swap (vector &__other)
{
    // called only from vector::swap() for unequal allocators
    _RWSTD_ASSERT (!(get_allocator () == __other.get_allocator ()));

    // avoid passing the whole object to other vector member
    // functions (i.e., assign()) in case they are implemented
    // in terms of swap(); use iterators instead

    vector __tmp (__other.get_allocator ());

    // assert that the copy of the allocator compares equal
    // to the original (otherwise the swap() below will cause
    // a recursive call)
    _RWSTD_ASSERT (__tmp.get_allocator () == __other.get_allocator ());

    __tmp.assign (begin (), end ());
    assign (__other.begin (), __other.end ());
    __other.swap (__tmp);
}


template <class _TypeT, class _Allocator>
void vector<_TypeT, _Allocator>::
_C_assign_n (size_type __n, const_reference __x)
{
#ifndef _RWSTD_NO_EXT_VECTOR_ASSIGN_IN_PLACE

    // FIXME: implement in-place assignment with repetition

    clear ();
    insert (begin (), __n, __x);   

#else   // if defined (_RWSTD_NO_EXT_VECTOR_ASSIGN_IN_PLACE)

    clear ();
    insert (begin (), __n, __x);   

#endif   // _RWSTD_NO_EXT_VECTOR_ASSIGN_IN_PLACE
}


template <class _TypeT, class _Allocator>
void vector<_TypeT, _Allocator>::
_C_insert_1 (const iterator &__it, const_reference __x)
{
    _RWSTD_ASSERT_RANGE (begin (), __it);

    if (size () < capacity ()) {

        if (__it < end ()) {

            const pointer __end = _C_alloc._C_end;

            // construct a copy of the last element in the range [it, end)
            // in the uninitialized slot just past the end of the range
            // and bump up end()
            _C_push_back (*(_C_alloc._C_end - difference_type (1)));

            // move the remaining elements from the range above one slot
            // toward the end starting with the last element
            _STD::copy_backward (__it, end () - 2, __end);

            // overwrite the element at the given position
            *__it = __x;
        }
        else {
            // construct a copy of the value to be inserted
            //  in the uninitialized slot
            _C_push_back (__x);
        }
    }
    else {
        _C_insert_n (__it, size_type (1), __x);
    }
}


template <class _TypeT, class _Allocator>
void vector<_TypeT, _Allocator>::
_C_insert_n (const iterator &__it, size_type __n, const_reference __x)
{
    _RWSTD_ASSERT_RANGE (begin (), __it);

    if (!__n)
        return;

    // 23.2.4.2, p5: calls to insert() must not reallocate storage
    // unless the new size of the container would exceed its capacity

    // compute the numbers of elements to be copied and inserted
    const size_type __size1 = _DISTANCE (begin (), __it, size_type);
    const size_type __size2 = size () + __n;

    if (capacity () < __size2) {

        // allocate a temporary vector large enough to hold all elements
        vector __tmp (get_allocator ());
        __tmp.reserve (__size2);

        _RWSTD_ASSERT (!(pointer () == __tmp._C_alloc._C_end));

        iterator __i;

        // copy the initial range prior to `it' as if by a call to
        // uninitialized_copy (begin (), __it, __tmp._C_alloc._C_begin);
        for (__i = begin (); !(__i == __it); ++__i) {

            _RWSTD_ASSERT (!(   __tmp._C_alloc._C_end
                             == __tmp._C_alloc._C_bufend));

            __tmp._C_push_back (*__i);
        }

        // construct `n' copies of `x' just past the initial range,
        // as if by a call to
        // uninitialized_fill_n (__tmp._C_aloc._C_begin + __size1, __n, __x);
        for ( ; __n; --__n) {

            _RWSTD_ASSERT (!(   __tmp._C_alloc._C_end
                             == __tmp._C_alloc._C_bufend));

            __tmp._C_push_back (__x);
        }

        // copy the final range of elements starting with `it'
        // as if by a call to
        // uninitialized_copy (__it, end (),
        //                     __tmp._C_alloc._C_begin + __size1 + __n);

        for (__i = __it; !(__i == end ()); ++__i) {

            _RWSTD_ASSERT (!(   __tmp._C_alloc._C_end
                             == __tmp._C_alloc._C_bufend));

            __tmp._C_push_back (*__i);
        }

        // swap the sequences controlled by the temporary vector and *this
        __tmp.swap (*this);

        return;
    }

    // compute the beginning of the range of elements in the sequence
    // controlled by *this that need to be moved (copy contructed past
    // the end of the end of the sequence or assigned over existing
    // elements)
    const pointer __movbeg = _C_alloc._C_begin + __size1;
    const pointer __movend = __movbeg + __n;

    _RWSTD_ASSERT (_C_make_iter (__movbeg) == __it);

    if (__movend <= _C_alloc._C_end) {

        // the end of the range of existing elements after being
        // moved to make room for the elements to be inserted is
        // before the current end of the sequence

        // compute the beginning of the range of elements whose copies
        // will be constructed just past the current end of the sequence
        const pointer __ucpbeg = _C_alloc._C_end - __n;
        const pointer __ucpend = _C_alloc._C_end;

        // construct copies of elements that will be moved beyond
        // the current end of the sequence controlled by *this
        _STD::uninitialized_copy (__ucpbeg, _C_alloc._C_end,
                                  _C_alloc._C_end, _C_alloc);

        // advance end to maintain consistent state
        _C_alloc._C_end += __n;

        // copy elements the will be overwritten below
        // over the range of elements moved above
        _STD::copy_backward (__movbeg, __ucpbeg, __ucpend);
    }
    else {

        // compute the length of the initial subsequence of the range
        // of element being inserted that overlaps the end of the
        // sequence being inserted into
        const size_type __n1 = size () - __size1;
        const size_type __n2 = __n - __n1;

        _STD::uninitialized_fill_n (_C_alloc._C_end, __n2, __x, _C_alloc);

        const pointer __end = _C_alloc._C_end;

        _C_alloc._C_end += __n2;

        // construct copies of the range of elements [pos, end)
        // past the end of the range of elements inserted above
        _STD::uninitialized_copy (__movbeg, __end, _C_alloc._C_end, _C_alloc);

        _C_alloc._C_end += __end - __movbeg;

        __n = __n1;
    }

    _STD::fill_n (__movbeg, __n, __x);
}


template<class _TypeT, class _Allocator>
template<class _InputIter>
void vector<_TypeT, _Allocator>::
_C_assign_range (_InputIter __first, _InputIter __last, input_iterator_tag)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

#ifndef _RWSTD_NO_EXT_VECTOR_ASSIGN_IN_PLACE

    // assign over existing elements for better efficiency
    // and exception safety

    // complexity: linear in distance(first, last)
    // -- exactly A=min(size(), distance(first, last)) calls
    //    to value_type's assignment operator
    // -- exactly (max(size(), distance(first, last)) - A)
    //    calls to value_type's copy ctor
    //
    // exception safety:
    // -- nothrow if size() >= distance(first, last), and value_type's
    //    assignment operator and iterator operations do not throw
    // -- basic otherwise

    const iterator __end = this->end ();

    for (iterator __it = this->begin (); __it != __end; ++__it, ++__first) {
        if (__first == __last) {
            this->erase (__it, __end);
            return;
        }
        *__it = *__first;
    }

    this->insert (__end, __first, __last);

#else   // if defined (_RWSTD_NO_EXT_VECTOR_ASSIGN_IN_PLACE)

    // follow the inefficient requirement in 23.2.4.1, p2

    // complexity: linear in distance(first, last)
    //    exactly distance(first, last) calls to value_type's copy ctor
    // exception safety: basic

    this->clear ();
    this->insert (this->begin (), __first, __last);

#endif   // _RWSTD_NO_EXT_VECTOR_ASSIGN_IN_PLACE

}


template <class _TypeT, class _Allocator>
template <class _FwdIter>
void vector<_TypeT, _Allocator>::
_C_assign_range (_FwdIter __first, _FwdIter __last, forward_iterator_tag)
{
    _RWSTD_ASSERT_RANGE (__first, __last);

#ifndef _RWSTD_NO_EXT_VECTOR_ASSIGN_IN_PLACE

    // assign over existing elements for better efficiency
    // and exception safety

    // complexity: linear in distance(first, last)
    // exception safety:
    // -- nothrow if size() >= distance(first, last), and value_type's
    //    assignment operator and iterator operations do not throw
    // -- strong if capacity() < size() + distance(first, last)
    // -- basic otherwise

    const size_type __size1 = _DISTANCE (__first, __last, size_type);
    const size_type __size2 = this->size () + __size1;

    if (this->capacity () < __size2) {

        // exception safety: strong

        vector<value_type, allocator_type> __tmp (this->get_allocator ());
        __tmp.reserve (__size2);

        // copy elements in the range [first, last) into the temporary
        // one element at a time, as if by calling
        // std::unitialized_copy(), growing the temporary at each
        // iteration so that an exception thrown by the copy ctor will
        // cause the destruction of all already constructed elements
        // (by invoking the temporary's dtor)
        for ( ; !(__first == __last); ++__first)
            __tmp._C_push_back (*__first);

        // swap *this with the temporary, having its dtor clean up
        this->swap (__tmp);
    }
    else {
        // exception safety: nothrow or basic

        const iterator __end = this->end ();

        for (iterator __i = this->begin (); __i != __end; ++__i, ++__first) {
            if (__first == __last) {
                this->erase (__i, __end);
                return;
            }
            *__i = *__first;
        }

        this->insert (__end, __first, __last);
    }

#else   // if defined (_RWSTD_NO_EXT_VECTOR_ASSIGN_IN_PLACE)

    // follow the inefficient requirement in 23.2.4.1, p2

    // complexity: linear in distance(first, last)
    // exception safety: basic

    this->clear ();
    this->insert (this->begin (), __first, __last);

#endif   // _RWSTD_NO_EXT_VECTOR_ASSIGN_IN_PLACE

}


template <class _TypeT, class _Allocator>
template <class _InputIter>
void vector<_TypeT, _Allocator>::
_C_insert_range (iterator __it, _InputIter __first, _InputIter __last,
                 input_iterator_tag)
{
    _RWSTD_ASSERT_RANGE (__it, end ());
    _RWSTD_ASSERT_RANGE (__first, __last);

#ifndef _RWSTD_NO_EXT_VECTOR_INSERT_IN_PLACE

    // note that this extension does not satisfy the strong exception
    // safety requirement in 23.2.4.3, p1

    // complexity: linear in distance(first, last)
    // exception safety: basic

    // append one element at a time to prevent the loss of data
    // from the input sequence in the case of an exception

    const size_type __size = this->size ();
    const size_type __inx  = _DISTANCE (this->begin (), __it, size_type);

    _RWSTD_ASSERT (__inx <= __size);

    for (; !(__first == __last); ++__first)
        this->push_back (*__first);

    if (__inx < __size) {
        // swap the inserted elements with the elements before which
        // they should be inserted, as if by calling
        // std::rotate (__beg, __mid, _C_alloc._C_end)
        const pointer __beg = this->_C_alloc._C_begin + __inx;
        const pointer __mid = this->_C_alloc._C_begin + __size;

        if (__beg < __mid) {
            for (pointer __p0 = __beg, __p1 = __mid; __p0 < --__p1; ++__p0)
                _STD::iter_swap (__p0, __p1);
        }

        if (__mid < this->_C_alloc._C_end) {
            for (pointer __p0 = __mid, __p1 = this->_C_alloc._C_end;
                 __p0 < --__p1; ++__p0)
                _STD::iter_swap (__p0, __p1);
        }

        if (__beg < this->_C_alloc._C_end) {
            for (pointer __p0 = __beg, __p1 = this->_C_alloc._C_end;
                 __p0 < --__p1; ++__p0)
                _STD::iter_swap (__p0, __p1);
        }
    }

#else   // if defined (_RWSTD_NO_EXT_VECTOR_INSERT_IN_PLACE)

    // 23.2.4.3, p1: If an exception is thrown other than by the copy
    // constructor or assignment operator of T there are no effects.
    // see also lwg issue 406

    // complexity: linear in distance(first, last)
    // exception safety:
    // -- strong if capacity() < size() + distance(first, last) and
    //    value_type's assignment operator and copy ctor do not throw
    // -- basic otherwise

    // insert input range into a temporary sequence rather than into *this
    // to coid modifying *this in case an exception (e.g., bad_alloc) is
    // thrown
    vector<value_type, allocator_type> __tmp (this->get_allocator ());

    for ( ; !(__first == __last); ++__first)
        __tmp.push_back (*__first);

    // insert into *this using a more efficient algorithm optimized
    // for BidirectionalIterator (and better)
    this->insert (__it, __tmp.begin (), __tmp.end ());

#endif   // _RWSTD_NO_EXT_VECTOR_INSERT_IN_PLACE

}


template <class _TypeT, class _Allocator>
template <class _FwdIter>
void vector<_TypeT, _Allocator>::
_C_insert_range (iterator __it, _FwdIter __first, _FwdIter __last,
                 forward_iterator_tag)
{
    _RWSTD_ASSERT_RANGE (__it, end ());
    _RWSTD_ASSERT_RANGE (__first, __last);

    // 23.2.4.2, p5: calls to insert() must not reallocate storage
    // unless the new size of the container would exceed its capacity

    // compute the sizes of the ranges of elements to be copied    
    const size_type __size1 = _DISTANCE (this->begin (), __it, size_type);
    const size_type __size2 = _DISTANCE (__first, __last, size_type);

    if (!__size2)
        return;

#ifndef _RWSTD_NO_EXT_VECTOR_INSERT_IN_PLACE
    const bool __insert_in_place =
        this->size () + __size2 <= this->capacity ();
#else   // if defined (_RWSTD_NO_EXT_VECTOR_INSERT_IN_PLACE)
    const bool __insert_in_place = false;
#endif   // _RWSTD_NO_EXT_VECTOR_INSERT_IN_PLACE

    if (__insert_in_place) {

        // compute the beginning an end of the range of elements
        // in the sequence controlled by *this that need to be moved
        // (copy contructed past the end of the end of the sequence
        // or assigned over existing elements)
        const pointer __movbeg = this->_C_alloc._C_begin + __size1;
        const pointer __movend = __movbeg + __size2;

        _RWSTD_ASSERT (this->_C_make_iter (__movbeg) == __it);

        const pointer __end = this->_C_alloc._C_end;

        if (__movend <= __end) {
            // compute the beginning of the range of elements whose copies
            // will be copy-constructed in the uninitialized space just past
            // the current end of the sequence
            const pointer __ucpbeg = __end - __size2;

            // construct copies of elements that will be moved beyond
            // the current end of the sequence controlled by *this
            for (pointer __p = __ucpbeg; !(__p == __end); ++__p)
                this->_C_push_back (*__p);

            // over the range of elements moved above
            for (pointer __q = __end; __movend < __q; ) {
                --__q;
                *__q = *(__q - __size2);
            }
        }
        else {
            // compute the length of the initial subsequence of the range
            // of elements being inserted that overlaps the end of the
            // sequence being inserted into
            const size_type __size2a = this->size () - __size1;
            _FwdIter __mid = __first;
            _STD::advance (__mid, __size2a);

            // construct copies of the trailing subsequence of the range
            // of elements being inserted, as if by a call to
            // std::uninitialized_copy (__mid, __last, _C_alloc._C_end);

            for (_FwdIter __m = __mid ; !(__m == __last); ++__m)
                this->_C_push_back (*__m);

            // construct copies of the range of elements [pos, end)
            // past the end of the range of elements inserted above,
            // as if by a call to 
            // std::uninitialized_copy (__movbeg, __end, _C_alloc._C_end);

            for (pointer __p = __movbeg; !(__p == __end); ++__p)
                this->_C_push_back (*__p);

            __last = __mid;
        }

        _STD::copy (__first, __last, __movbeg);
    }
    else {
        // 23.2.4.3, p1: If an exception is thrown other than by the copy
        // constructor or assignment operator of T there are no effects.

        // create a temporary vector and reserve sufficient space
        vector<value_type, allocator_type> __tmp (this->get_allocator ());

        __tmp.reserve (this->size () + __size2);

        // avoid using the name __i or __it below so as not to trigger
        // a (bogus) gcc 2.95.2 -Wshadow warning: declaration of `__i'
        // shadows previous local
        iterator __ix;

        // copy the initial subsequence of *this, [begin, it),  into
        // the temporary one element at a time, as if by calling
        // std::unitialized_copy(), growing the temporary at each
        // iteration so that an exception thrown by the copy ctor
        // will cause the destruction of all already constructed
        // elements (by invoking the temporary's dtor)
        for (__ix = this->begin (); __ix != __it; ++__ix) {
            __tmp._C_push_back (*__ix);
        }

        // append the sequence [first, last) to the temporary,
        // carefully increasing the size of tmp before performing
        // any operations on `first' and `last' in case they throw
        for (; !(__first == __last); ++__first) {
            __tmp._C_push_back (*__first);
        }

        // copy the remaining elements from *this
        for ( ; __ix != this->end (); ++__ix) {
            __tmp._C_push_back (*__ix);
        }

        // swap *this with the temporary, having its dtor clean up
        this->swap (__tmp);
    }
}

/***************************************************************************/

#ifndef _RWSTD_NO_VECTOR_BOOL

#ifndef _RWSTD_NO_BOOL

// The body of this function is duplicated in src/vecbool.cpp and
// further down in this file as well.
#if !defined (_RWSTD_NO_CLASS_PARTIAL_SPEC) 
  template <class _Allocator>
  template<class _InputIter>
  void vector<bool, _Allocator >::insert 
#else
  template<class _InputIter>
  void vector<bool, allocator<bool> >::insert 
#endif // _RWSTD_NO_CLASS_PARTIAL_SPEC
  (iterator __it, _InputIter __first, _InputIter __last)

  {
    if (__first == __last) return;
    size_type __n = _DISTANCE (__first, __last, size_type);
    if (capacity() - size() >= __n)
    {
      copy_backward(__it, end(), _C_end + __n);
      copy(__first, __last, __it);
      _C_end += __n;
    }
    else
    {
      size_type __len = size() + (max)(size(), __n);

      unsigned int* __q = _C_bit_alloc(__len);
      iterator __i = copy(begin(), __it, iterator(__q, 0));
      __i = copy(__first, __last, __i);
      _C_end = copy(__it, end(), __i);
      _C_value_alloc_type (*this).
          deallocate((pointer)_C_begin._C_p,_C_bufend - _C_begin._C_p);
      _C_bufend = __q + (__len + _RWSTD_WORD_BIT - 1)/_RWSTD_WORD_BIT;
      _C_begin = iterator(__q, 0);
    }
  }

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC

// Duplicates of the following functions exist in src/stl/vecbool.cpp.
// Which set is used depends on the availability of partial specialization.

  template <class _Allocator>
  void vector<bool,_Allocator >::flip ()
  {
    for (iterator __i = begin(); !(__i == end()); __i++)
      *__i = !(*__i);
  }

  template <class _Allocator>
  void vector<bool,_Allocator >::swap (reference __x, reference __y)
  {
    bool __tmp = __x; __x = __y; __y = __tmp;
  }


template <class _Allocator>
void vector<bool,_Allocator >::
_C_insert (iterator __it, bool __x)
{
    _RWSTD_ASSERT_RANGE (begin (), __it);

    if (size () < capacity ()) {

        const iterator __end = _C_end;

        // move elements in the range [it, end) before the insert
        // one slot toward the end
        // i.e., copy [it, end) over [end - (end - it), end)
        // or, given N = distance(it, end) before the insert,
        //       copy [it, end) over [end - N, end)
        _STD::copy_backward (__it, __end, ++_C_end);

        // overwrite the element at the given position
        *__it = __x;
    }
    else
    {
      size_type __len = size() ? 2 * size() : _RWSTD_WORD_BIT;
      unsigned int* __q = _C_bit_alloc(__len);
      iterator __i = _C_copy(begin(), __it, iterator(__q, 0));
      *__i++ = __x;
      _C_end = _C_copy(__it, end(), __i);
      _C_value_alloc_type (*this).
          deallocate((pointer)_C_begin._C_p,_C_bufend - _C_begin._C_p);
      _C_bufend = __q + (__len + _RWSTD_WORD_BIT - 1)/_RWSTD_WORD_BIT;
      _C_begin = iterator(__q, 0);
    }
}


  template <class _Allocator>
  void vector<bool,_Allocator >::insert (iterator __it, size_type __n,
                                         const bool& __x)
  {
    if (__n == 0) return;
    if (capacity() - size() >= __n)
    {
      _C_copy_backward(__it, end(), _C_end + __n);
      _C_fill(__it, __it + __n, __x);
      _C_end += __n;
    }
    else
    {
      size_type __len = size() + (max)(size(), __n);
      unsigned int* __q = _C_bit_alloc(__len);
      iterator __i = _C_copy(begin(), __it, iterator(__q, 0));
      _C_fill_n(__i, __n, __x);
      _C_end = _C_copy(__it, end(), __i + __n);
      _C_value_alloc_type (*this).
          deallocate((pointer)_C_begin._C_p,_C_bufend - _C_begin._C_p);
      _C_bufend = __q + (__len + _RWSTD_WORD_BIT - 1)/_RWSTD_WORD_BIT;
      _C_begin = iterator(__q, 0);
    }
  }


  template <class _Allocator>
  void vector<bool,_Allocator >::resize (size_type __new_size, bool __c)
  {
    if (__new_size > size())
      insert(end(), __new_size - size(), __c);             
    else if (__new_size < size())
      erase(begin() + __new_size, end());
  }

#endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC

#endif   // _RWSTD_NO_BOOL

#endif   // _RWSTD_NO_VECTOR_BOOL

}   // namespace std
