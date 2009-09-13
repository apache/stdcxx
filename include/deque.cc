/***************************************************************************
 *
 * deque.cc - Non-iniline definitions for the Standard Library deque class
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
deque<_TypeT, _Allocator>&
deque<_TypeT, _Allocator>::
operator= (const deque &__rhs)
{
    _RWSTD_ASSERT (_C_is_valid ());
    _RWSTD_ASSERT (__rhs._C_is_valid ());

    const size_type __size = size ();

    if (__size < __rhs.size ()) {
        const const_iterator __mid = __rhs.begin () + __size;

        _STD::copy (__mid, __rhs.end (),
                    _STD::inserter (*this, _STD::copy (__rhs.begin (), __mid,
                                                       begin ())));
    }
    else
        erase (_STD::copy (__rhs.begin (), __rhs.end (), begin ()), end ());

    return *this;
}


template <class _TypeT, class _Allocator>
typename deque<_TypeT, _Allocator>::iterator 
deque<_TypeT, _Allocator>::
insert (iterator __it, const_reference __x)
{
    _RWSTD_ASSERT_RANGE (begin (), __it);
    _RWSTD_ASSERT (_C_is_valid ());

    if (__it == begin ()) {
        push_front (__x);
        return begin ();
    }

    if (__it == end ()) {
        push_back (__x);
        __it = end () - 1;
        return __it;
    }

    const size_type __inx = _DISTANCE (begin (), __it, size_type);

    _RWSTD_ASSERT (__inx < size ());

    if (__inx < size () - __inx) {
        push_front (front ());
        _STD::copy (begin () + 2, begin () + (__inx + 1), begin () + 1);
    }
    else {
        push_back (back ());

        _STD::copy_backward (begin () + __inx, end () - 2, end () - 1);
    }

    __it = begin () + __inx;

    *__it = __x;

    return __it;
}


template <class _TypeT, class _Allocator>
typename deque<_TypeT, _Allocator>::iterator 
deque<_TypeT, _Allocator>::
erase (iterator __it)
{
    // `it' must be a dereferenceable iterator into `*this'

    _RWSTD_ASSERT_RANGE (begin (), __it);
    _RWSTD_ASSERT (!(__it == end ()));
    _RWSTD_ASSERT (_C_is_valid (0 /* valid and non-empty */));

    const iterator __next = ++iterator (__it);

    if (__it - begin () < end () - __it) {

        _STD::copy_backward (begin (), __it, __next);

        pop_front ();

        return empty () ? end () : __next;
    }

    _STD::copy (__next, end (), __it);

    pop_back ();

    return __it;
}
    

template <class _TypeT, class _Allocator>
typename deque<_TypeT, _Allocator>::iterator 
deque<_TypeT, _Allocator>::
erase (iterator __first, iterator __last)
{
    // (`first', `last'] must be a valid range into `*this'

    _RWSTD_ASSERT_RANGE (__first, __last);
    _RWSTD_ASSERT_RANGE (begin (), __first);
    _RWSTD_ASSERT (_C_is_valid ());

    size_type __n = _DISTANCE (__first, __last, size_type);

    _RWSTD_ASSERT (__n <= size ());

    if (__first - begin () < end () - __last) {

        _STD::copy_backward (begin (), __first, __last);

        while (__n-- != 0)
            pop_front ();

        return __last;
    }

    _STD::copy (__last, end (), __first);

    while (__n-- != 0)
        pop_back ();

    return __first;
}


template <class _TypeT, class _Allocator>
void deque<_TypeT, _Allocator>::
swap (deque<_TypeT, _Allocator> &__rhs)
{
    _RWSTD_ASSERT (_C_is_valid ());
    _RWSTD_ASSERT (__rhs._C_is_valid ());

    if (get_allocator () == __rhs.get_allocator ()) {

        _STD::swap (_C_beg,       __rhs._C_beg);
        _STD::swap (_C_end,       __rhs._C_end);
        _STD::swap (_C_nodes,     __rhs._C_nodes);
        _STD::swap (_C_node_size, __rhs._C_node_size);

        if (pointer() == _C_beg._C_cur) {
            _RWSTD_ASSERT (pointer() == _C_end._C_cur);
            _RWSTD_ASSERT (   _C_beg._C_node == &__rhs._C_end._C_cur
                           && _C_end._C_node == &__rhs._C_end._C_cur);

            _C_beg._C_node =
            _C_end._C_node = &_C_end._C_cur;
        }
        else {
            _RWSTD_ASSERT (pointer() != _C_end._C_cur);
        }

        if (pointer() == __rhs._C_beg._C_cur) {
            _RWSTD_ASSERT (pointer() == __rhs._C_end._C_cur);
            _RWSTD_ASSERT (   __rhs._C_beg._C_node == &_C_end._C_cur
                           && __rhs._C_end._C_node == &_C_end._C_cur);

            __rhs._C_beg._C_node =
            __rhs._C_end._C_node = &__rhs._C_end._C_cur;
        }
        else {
            _RWSTD_ASSERT (pointer() != __rhs._C_end._C_cur);
        }
    }
    else {
        deque __tmp (*this);
        *this = __rhs;
        __rhs = __tmp;
    }
}


template <class _TypeT, class _Allocator>
void deque<_TypeT, _Allocator>::
_C_push (bool __at_back, const_reference __x)
{
    _RWSTD_ASSERT (_C_is_valid ());

    const size_type __bufsize = _C_bufsize ();

    _C_value_alloc_type __alloc (*this);

    // allocate a new buffer to store elements in
    const pointer __buf = __alloc.allocate (__bufsize, pointer ());

    const bool __empty = empty ();

    if (   __empty
        || (__at_back && _C_end._C_node == _C_nodes + _C_node_size - 1)
        || (!__at_back && _C_beg._C_node == _C_nodes)) {

        _C_node_alloc_type __node_alloc (*this);

        _RWSTD_ASSERT (_C_beg._C_node <= _C_end._C_node);

        // compute the current number of used nodes
        // and the new number of nodes

        const _C_node_pointer __end = _C_end._C_node + !__empty;

        const size_type __nodes = size_type (__end - _C_beg._C_node);

        // obtain the next greater number of nodes for the new vector
        const size_type __nvecsize = _C_vecsize (__nodes);

        _RWSTD_ASSERT (__nvecsize > __nodes);

        // compute the beginning of the old (current) vector
        const _C_node_pointer __ovec = _C_nodes - !__empty;
        _C_node_pointer __nvec;

        _TRY {
            // allocate enough for two extra unused nodes
            __nvec = __node_alloc.allocate (__nvecsize + 2, __ovec);

            _RWSTD_ASSERT (!(_C_node_pointer () == __nvec));

            const pointer __where = __buf
                + (  __empty ? __bufsize / 2 - !__at_back
                   : __at_back ? 0 : __bufsize - 1);

            _TRY {
                __alloc.construct (__where, __x);
            }
            _CATCH (...) {
                __node_alloc.deallocate (__nvec, __nvecsize + 2);
                _RETHROW;
            }

            // reserve the first and last nodes (must always be 0)
            ++__nvec;
        }
        _CATCH (...) {
            // deallocate buffer and rethrow
            __alloc.deallocate (__buf, __bufsize);
            _RETHROW;
        }

        // compute the new indices into the newly allocated vector
        // of the first and last occuppied node, respectively
        const size_type __beg_inx = __nvecsize / (__empty ? 2 : 4);
        const size_type __end_inx = __beg_inx + __nodes;

        // copy existing nodes into the new buffer
        _STD::copy (_C_beg._C_node, __end, __nvec + (__beg_inx + !__at_back));

        // deallocate the current buffer only when non-0 to satisfy
        // the requirement that deallocate() argument be non-zero
        if (!(__ovec == _C_node_pointer ()))
            __node_alloc.deallocate (__ovec, _C_node_size + 2);

        // set the pointer and size
        _C_nodes     = __nvec;
        _C_node_size = __nvecsize;

        // insert the fixed-size array into the vector of nodes...
        if (__at_back) {

            // ...at the end
            _C_nodes [__end_inx] = __buf;

            _C_beg._C_node = _C_nodes + __beg_inx;
            _C_end = _C_deque_iter (__buf, _C_nodes + __end_inx);

        }
        else {
            // ...at the beginning
            _C_nodes [__beg_inx] = __buf;

            _C_beg = _C_deque_iter (__buf + __bufsize, _C_nodes + __beg_inx);
            _C_end._C_node = _C_nodes + __end_inx;
        }

        if (__empty) {
            // set the beginning and end pointers both
            // to point at the same (future) element
            _C_beg._C_cur =
            _C_end._C_cur = __buf + __bufsize / 2;
        }
    }
    else {
        _RWSTD_ASSERT (_C_is_valid (0 /* valid and non-empty */));

        _TRY {

            if (__at_back) {
                __alloc.construct (__buf, __x);

                // append the newly allocated buffer to the end of the vector
                // of pointers and increment `end' to point at the first
                // element in it; zero out the following node
                *++_C_end._C_node  = __buf;
                _C_end._C_cur      = __buf;
            }
            else {
                __alloc.construct (__buf + (__bufsize - 1), __x);

                // prepend the newly allocated buffer to the beginning of
                // the vector of pointers and decrement `begin' to point
                // at the last element in it; zero out the preceding node
                *--_C_beg._C_node   = __buf;
                _C_beg._C_cur       = __buf + __bufsize;
            }
        }
        _CATCH (...) {
            __alloc.deallocate (__buf, __bufsize);

            _RETHROW;
        }
    }

    _C_end._C_node [+1] = pointer ();
    _C_beg._C_node [-1] = pointer ();
}


template <class _TypeT, class _Allocator>
void deque<_TypeT, _Allocator>::
_C_free_at_begin ()
{
    const size_type __bufsize = _C_bufsize ();

    _RWSTD_VALUE_ALLOC (_C_value_alloc_type, *this,
                        deallocate (*_C_beg._C_node, __bufsize));
    if (empty ()) {
        _C_node_alloc_type (*this).
            deallocate (_C_nodes - 1, _C_node_size + 2);
        _C_init ();
    }
    else {
        *_C_beg._C_node++ = pointer ();
        _C_beg = _C_deque_iter (*_C_beg._C_node, _C_beg._C_node);
    }
}


template <class _TypeT, class _Allocator>
void deque<_TypeT, _Allocator>::
_C_free_at_end ()
{
    const size_type __bufsize = _C_bufsize ();

    _RWSTD_VALUE_ALLOC (_C_value_alloc_type, *this,
                        deallocate (*_C_end._C_node, __bufsize));
    if (empty ()) {
        _C_node_alloc_type (*this).
            deallocate (_C_nodes - 1, _C_node_size + 2);
        _C_init ();
    }
    else {
        *_C_end._C_node-- = pointer ();
        _C_end = _C_deque_iter (*_C_end._C_node + __bufsize, _C_end._C_node);
    }
}


template <class _TypeT, class _Allocator>
void deque<_TypeT, _Allocator>::
_C_assign_n (size_type __n, const_reference __x)
{
    _RWSTD_ASSERT (_C_is_valid ());

#ifndef _RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE

    // assign over existing elements for better efficiency
    // and exception safety

    // complexity: linear in n
    // exception safety:
    // -- nothrow if size() >= n, and value_type's assignment operator
    //    and iterator operations do not throw
    // -- basic otherwise

    const iterator __end = end ();

    // avoid using the name __i or __it below so as not to trigger
    // a (bogus) gcc 2.95.2 -Wshadow warning: declaration of `__i'
    // shadows previous local
    for (iterator __ix = begin (); !(__ix == __end); ++__ix, --__n) {
        if (size_type () == __n) {
            erase (__ix, __end);
            return;
        }
        *__ix = __x;
    }

    insert (__end, __n, __x);

#else   // if defined (_RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE)

    // follow the inefficient requirement in 23.2.1.1, p8

    // complexity: linear in n
    // exception safety: basic

    clear ();
    insert (begin (), __n, __x);

#endif   // _RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE

}


template <class _TypeT, class _Allocator>
void deque<_TypeT, _Allocator>::
_C_insert_n (const iterator &__it, size_type __n, const_reference __x)
{
    _RWSTD_ASSERT_RANGE (begin (), __it);
    _RWSTD_ASSERT (_C_is_valid ());

    const size_type __inx = _DISTANCE (begin (), __it, size_type);
    const size_type __rem = size () - __inx;

    _RWSTD_ASSERT (__inx <= size ());

    if (__inx < __rem) {

        // inserting `x' into the first half of the container

        if (__inx < __n) {
            
            for (size_type __i = __n - __inx; __i; --__i)
                push_front (__x);

            for (size_type __j = __inx; __j; --__j)
                push_front (*(begin () + (__n - 1)));

            _STD::fill (begin () + __n, begin () + (__n + __inx), __x);
        }
        else if (__n) {

            for (size_type __i = __n; __i; --__i)
                push_front (*(begin () + __n - 1));

            const iterator __tmp = begin () + __n;

            _STD::copy (__tmp + __n, __tmp + __inx, __tmp);

            _STD::fill (begin () + __inx, __tmp + __inx, __x);
        }
    }
    else {

        // inserting `x' into the second half of the container

        const size_type __size0 = size ();

        if (__rem < __n) {

            for (size_type __i = __n - __rem; __i; --__i)
                push_back (__x);

            for (size_type __j = 0; __j != __rem; ++__j)
                push_back (*(begin () + (__inx + __j)));

            _STD::fill (begin () + __inx, begin () + __size0, __x);
        }
        else if (__n) {

            for (size_type __i = 0;__i != __n; ++__i)
                push_back (*(begin () + __size0 - __n + __i));

            const iterator __tmp = begin () + __inx;

            _STD::copy_backward (__tmp, begin () + __size0 - __n, 
                                 begin () + __size0);

            _STD::fill (__tmp, __tmp + __n, __x);
        }
    }
}


template <class _TypeT, class _Allocator>
template <class _InputIter>
void deque<_TypeT, _Allocator>::
_C_assign_range (_InputIter __first, _InputIter __last, input_iterator_tag)
{
    typedef deque _Deque;

    _RWSTD_ASSERT_RANGE (__first, __last);
    _RWSTD_ASSERT (this->_C_is_valid ());

#ifndef _RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE

    // assign over existing elements for better efficiency
    // and exception safety

    // complexity: linear in distance(first, last)
    // exception safety:
    // -- nothrow if size() >= distance(first, last), and value_type's
    //    assignment operator and iterator operations do not throw
    // -- basic otherwise

    const iterator __end = this->end ();

    // avoid using the name __i or __it below so as not to trigger
    // a (bogus) gcc 2.95.2 -Wshadow warning: declaration of `__i'
    // shadows previous local
    for (iterator __ix = this->begin (); !(__ix == __end);
         ++__ix, ++__first) {
        if (__first == __last) {
            this->erase (__ix, __end);
            return;
        }
        *__ix = *__first;
    }

    this->insert (__end, __first, __last);

#else   // if defined (_RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE)

    // follow the inefficient requirement in 23.2.1.1, p7

    // complexity: linear in distance(first, last)
    // exception safety: basic

    this->clear ();
    this->insert (this->begin (), __first, __last);

#endif   // _RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE

}


template<class _TypeT, class _Allocator>
template <class _InputIter>
void deque<_TypeT, _Allocator>::
_C_insert_range (iterator __it,
                 _InputIter __first, _InputIter __last,
                 input_iterator_tag)
{
    typedef deque _Deque;

    _RWSTD_ASSERT_RANGE (__it, this->end ());
    _RWSTD_ASSERT_RANGE (__first, __last);

#ifndef _RWSTD_NO_EXT_DEQUE_INSERT_IN_PLACE

    // insert one element at a time to prevent a loss of data
    // from the input sequence in the case of an exception

    for ( ; !(__first == __last); ++__it, ++__first)
        __it = this->insert (__it, *__first); 

#else   // if defined (_RWSTD_NO_EXT_DEQUE_INSERT_IN_PLACE)

    // 23.2.1.3, p2: if an exception is thrown other than by the copy
    // constructor or assignment operator of T there are no effects.

    _Deque __tmp (this->begin (), __it, this->get_allocator ());

    for (; !(__first == __last); ++__first)
        __tmp.push_back (*__first);

    __tmp.insert (__tmp.end (), __it, this->end ());

    this->swap (__tmp);
    
#endif   // _RWSTD_NO_EXT_DEQUE_INSERT_IN_PLACE

}


// template<class _TypeT, class _Allocator>
// template <class _FwdIter>
// void deque<_TypeT, _Allocator>::
// _C_insert_range (iterator __it,
//                  _FwdIter __first, _FwdIter __last,
//                  forward_iterator_tag)
// {
//     typedef deque _Deque;

//     // implemented in terms of the Input Iterator overload
//     _RWSTD_INSERT_RANGE (__it, __first, __last, input_iterator_tag ());
// }


template <class _TypeT, class _Allocator>
template <class _BidirIter>
void deque<_TypeT, _Allocator>::
_C_insert_range (iterator __it,
                 _BidirIter __first, _BidirIter __last,
                 bidirectional_iterator_tag)
{
    typedef deque _Deque;

    _RWSTD_ASSERT_RANGE (this->begin (), __it);
    _RWSTD_ASSERT_RANGE (__first, __last);
    _RWSTD_ASSERT (this->_C_is_valid ());

    // compute the distance of `it' from the beginning of the container,
    // the distance of `it' to the end of the container, and the number
    // of elements to be inserted
    const size_type __sz0 = this->size ();
    const size_type __inx = _DISTANCE (this->begin (), __it, size_type);
    const size_type __rem = __sz0 - __inx;
    const size_type __n = _DISTANCE (__first, __last, size_type);

    _RWSTD_ASSERT (__inx <= __sz0);

    if (__inx < __rem) {

        // inserting into the first half of the container

        if (__inx < __n) {

            // the number of elements in the range controlled by *this,
            // [begin, it), is less than the number of elements in the
            // range [first, last)

            _BidirIter __from (__last);

            _STD::advance (__from, -difference_type (__inx));

            const _BidirIter __mid (__from);

            while (!(__from == __first))
                this->push_front (*--__from);

            for (size_type __i = __inx; __i; --__i)
                this->push_front (*(this->begin () + (__n - 1)));

            _STD::copy (__mid, __last, this->begin () + __n);
        }
        else {

            // the number of elements in the range to be inserted,
            // [first, last), is less than or equal to the number
            // of elements in the range  [begin, it)

            for (size_type __i = __n; __i; --__i)
                this->push_front (*(this->begin () + (__n - 1)));

            const iterator __to = this->begin () + __n;
            
            _STD::copy (__to + __n, __to + __inx, __to);
            _STD::copy (__first, __last, this->begin () + __inx);
        }
    }
    else {
        // inserting into the second half of the container

        if (__rem < __n) {

            _BidirIter __from (__first);

            _STD::advance (__from, __rem);

            const _BidirIter __to (__from);

            for ( ; !(__from == __last); ++__from)
                this->push_back (*__from);

            for (size_type __i = 0; __i != __rem; ++__i)
                this->push_back (*(this->begin () + (__inx + __i)));

            _STD::copy (__first, __to, this->begin () + __inx);
        }
        else {
            for (size_type __i = 0; __i != __n; ++__i)
                this->push_back (*(this->begin () + (__sz0 - __n + __i)));

            const iterator __to       = this->begin () + __sz0;
            const iterator __from     = this->begin () + __inx;
            const iterator __from_end = this->begin () + (__sz0 - __n);

            _STD::copy_backward (__from, __from_end, __to);
            _STD::copy (__first, __last, __from);
        }
    }
}


template<class _TypeT, class _Allocator>
bool deque<_TypeT, _Allocator>::
_C_is_valid (int __emptyfl) const 
{
#ifdef _RWSTDDEBUG

    if (__emptyfl < 0 /* don't care */)
        return _C_is_valid (0 + empty ());

    // the nodes just prior the first valid node and
    // just past the last valid node must always be 0
    if (_C_node_pointer () != _C_nodes) {
        _RWSTD_ASSERT (pointer () == _C_beg._C_node [-1]);
        _RWSTD_ASSERT (pointer () == _C_end._C_node [+1]);
    }

    if (__emptyfl) {
        // container must be empty
        _RWSTD_ASSERT (empty ());

        // both `current' pointers must be 0
        _RWSTD_ASSERT (pointer () == _C_beg._C_cur);
        _RWSTD_ASSERT (pointer () == _C_end._C_cur);

        // node pointers must be dereferenceable...
        _RWSTD_ASSERT (_C_node_pointer () != _C_beg._C_node);
        _RWSTD_ASSERT (_C_node_pointer () != _C_end._C_node);

        // ...and both must dereference the same pointer
        _RWSTD_ASSERT (*_C_beg._C_node == *_C_end._C_node);

        return true;
    }

    const size_type __bufsize = _C_bufsize ();

    // container must be non-empty
    _RWSTD_ASSERT (!empty ());

    // current pointers must be non-0
    _RWSTD_ASSERT (pointer () != _C_beg._C_cur);
    _RWSTD_ASSERT (pointer () != _C_end._C_cur);

    // `begin' must be less than `end'
    _RWSTD_ASSERT (_C_beg._C_node <= _C_end._C_node);

    // `begin' must point into the vector of nodes
    _RWSTD_ASSERT (_C_nodes <= _C_beg._C_node);
    _RWSTD_ASSERT (_C_beg._C_node <= _C_nodes + _C_node_size);

    // `begin' must point to a non-empty array of elements
    _RWSTD_ASSERT (pointer () != *_C_beg._C_node);

    // `begin' must point into an array of `bufsize' elements
    _RWSTD_ASSERT (_C_beg._C_cur >= *_C_beg._C_node);
    _RWSTD_ASSERT (_C_beg._C_cur <= *_C_beg._C_node + __bufsize);

    // `end' must point into the vector of nodes
    _RWSTD_ASSERT (_C_nodes <= _C_end._C_node);
    _RWSTD_ASSERT (_C_end._C_node <= _C_nodes + _C_node_size);

    // `end' must point to a non-empty array of elements
    _RWSTD_ASSERT (pointer () != *_C_end._C_node);

    // `end' must point into an array of `bufsize' elements
    _RWSTD_ASSERT (_C_end._C_cur >= *_C_end._C_node);
    _RWSTD_ASSERT (_C_end._C_cur <= *_C_end._C_node + __bufsize);

#else   // if !defined (_RWSTDDEBUG)

    _RWSTD_UNUSED (__emptyfl);

#endif   // _RWSTDDEBUG

    return true;
}


}   // namespace std
