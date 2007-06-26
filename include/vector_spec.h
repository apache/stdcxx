// -*- C++ -*-
/***************************************************************************
 *
 * vector_spec.h - vector <_TypeT*> partial specialization using vector<void*>
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

#ifndef __STD_VECTOR_SPEC__
#define __STD_VECTOR_SPEC__

#include <rw/_defs.h>

#ifndef __STD_VECTOR__
#include <vector>
#endif   // __STD_VECTOR__


_RWSTD_NAMESPACE (std) { 


template class vector <void*, allocator <void*> >;


template <class _TypeT>
class vector <_TypeT*, allocator <_TypeT*> >
{
    typedef allocator <_TypeT*>                   _Allocator;
    typedef _RWSTD_REBIND (_Allocator, void*) __imp_alloc_type;
    typedef vector <void*, __imp_alloc_type> __imp_type;

    __imp_type                                __imp;

public:

    typedef _TypeT*                                             value_type;
    typedef _Allocator                                      allocator_type;

    typedef _TYPENAME allocator_type::reference        reference;
    typedef _TYPENAME allocator_type::const_reference  const_reference;
    typedef _TYPENAME allocator_type::pointer          iterator;
    typedef _TYPENAME allocator_type::const_pointer    const_iterator;
    typedef _TYPENAME allocator_type::size_type        size_type;
    typedef _TYPENAME allocator_type::difference_type  difference_type;
    typedef _TYPENAME allocator_type::pointer          pointer;
    typedef _TYPENAME allocator_type::const_pointer    const_pointer;


#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC 

    typedef _STD::reverse_iterator <const_iterator> const_reverse_iterator;
    typedef _STD::reverse_iterator <iterator>       reverse_iterator;

#else   // if defined (_RWSTD_NO_CLASS_PARTIAL_SPEC)

    typedef _STD::reverse_iterator <const_iterator, 
        random_access_iterator_tag, value_type, 
        const_reference, const_pointer, difference_type>
    const_reverse_iterator;

    typedef _STD::reverse_iterator <iterator, 
        random_access_iterator_tag, value_type,
        reference, pointer, difference_type>
    reverse_iterator;

#endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC


public:
    _EXPLICIT vector (const _Allocator &alloc _RWSTD_DEFAULT_ARG (_Allocator ())) 
        : __imp (alloc) { }


#ifndef _RWSTD_NO_DEFAULT_TEMPLATE_ARGS

   _EXPLICIT vector (size_type n,
                     const_reference x _RWSTD_DEFAULT_ARG (value_type ()),
                     const _Allocator &alloc _RWSTD_DEFAULT_ARG (_Allocator ())) 
       : __imp (n, __imp_type::const_reference (x), alloc) { }

#else   // if defined (_RWSTD_NO_DEFAULT_TEMPLATE_ARGS)

    vector (): __imp () { }

    vector (size_type n, const_reference x) 
        : __imp (n, __imp_type::const_reference (x)) { }

    vector (size_type n, const_reference x, const _Allocator &alloc) 
        : __imp (n, __imp_type::const_reference (x), alloc) { }

#endif   // _RWSTD_NO_DEFAULT_TEMPLATE_ARGS


#ifndef _RWSTD_NO_MEMBER_TEMPLATES

    template <class InputIterator>
    vector (InputIterator first, InputIterator last,
            const _Allocator &alloc _RWSTD_DEFAULT_ARG (_Allocator ()))
        : __imp (first, last, alloc) { }

#else   // if defined (_RWSTD_NO_MEMBER_TEMPLATES)

    vector (const_iterator first, const_iterator last)
        : __imp (first, last) { }

#endif // _RWSTD_NO_MEMBER_TEMPLATES

    vector (const vector& x)
        : __imp (x.__imp) { }

    vector& operator= (const vector& x) {
        __imp = x.__imp;
        return *this;
    }


#ifndef _RWSTD_NO_MEMBER_TEMPLATES

    template <class InputIterator>
    void assign (InputIterator first, InputIterator last) {
        __imp.assign (first, last);
    }

#else   // if defined (_RWSTD_NO_MEMBER_TEMPLATES)

    void assign (const_iterator first, const_iterator last) {
        __imp.assign (__imp_type::const_iterator (first),
                     __imp_type::const_iterator (last));
    }

#endif // _RWSTD_NO_MEMBER_TEMPLATES


    void assign (size_type n, const_reference x) {
        __imp.assign (n, __imp_type::const_reference (x));
    }

    allocator_type get_allocator () const {
        return __imp.get_allocator ();
    }

    iterator begin () {
        return iterator (__imp.begin ());
    }

    const_iterator begin () const {
        return const_iterator (__imp.begin ());
    }

    iterator end () {
        return iterator (__imp.end ());
    }

    const_iterator end () const {
        return const_iterator (__imp.end ());
    }

    reverse_iterator rbegin () {
        return reverse_iterator (end ());
    }

    const_reverse_iterator rbegin () const {
        return const_reverse_iterator (end ());
    }

    reverse_iterator rend () {
        return reverse_iterator (begin ());
    }

    const_reverse_iterator rend () const {
        return const_reverse_iterator (begin ());
    }

    size_type size () const {
        return __imp.size ();
    }

    size_type max_size () const {
        return __imp.max_size ();
    }

    void resize (size_type new_size) {
        __imp.resize (new_size);
    }

    void resize (size_type new_size, const_reference x) {
        __imp.resize (new_size, __imp_type::const_reference (x));
    }

    size_type capacity () const {
        return __imp.capacity ();
    }

    bool empty () const {
        return __imp.empty ();
    }

    void reserve (size_type n) {
        __imp.reserve (n);
    }

    reference operator[] (size_type n) {
        return reference (__imp [n]);
    }

    const_reference operator[] (size_type n) const {
        return const_reference (__imp [n]);
    }

    reference at (size_type n) {
        return reference (__imp.at (n));
    }

    const_reference at (size_type n)  const {
        return const_reference (__imp.at (n));
    }

    reference front () {
        return reference (__imp.front ());
    }

    const_reference front () const {
        return const_reference (__imp.front ());
    }

    reference back () {
        return reference (__imp.back ());
    }

    const_reference back () const {
        return const_reference (__imp.back ());
    }

    void push_back (const_reference x) {
        __imp.push_back (__imp_type::const_reference (x));
    }

    void pop_back () {
        __imp.pop_back ();
    }

    iterator insert (iterator pos, const_reference x) {
          return iterator (__imp.insert (__imp_type::iterator (pos),
                                        __imp_type::const_reference (x)));
    }

    void insert (iterator pos, size_type n, const_reference x) {
        __imp.insert (__imp_type::iterator (pos), n, __imp_type::const_reference (x));
    }


#ifndef _RWSTD_NO_MEMBER_TEMPLATES

    template <class InputIterator>
    void insert (iterator pos, InputIterator first, InputIterator last) {
        __imp.insert (__imp_type::iterator (pos), first, last);
    }

#else   // defined (_RWSTD_NO_MEMBER_TEMPLATES)

    void insert (iterator pos, const_iterator first, const_iterator last) {
        __imp.insert (__imp_type::iterator (pos), __imp_type::const_iterator (first),
                     __imp_type::const_iterator (last));
    }

#endif // _RWSTD_NO_MEMBER_TEMPLATES


    iterator erase (iterator pos) {
        return iterator (__imp.erase (__imp_type::iterator (pos)));
    }

    iterator erase (iterator first, iterator last) {
        return iterator (__imp.erase (__imp_type::iterator (first),
                                     __imp_type::iterator (last)));
    }

    void swap (vector &x) {
        __imp.swap (x.__imp);
    }

    void clear () {
        __imp.clear ();
    }

#ifndef _RWSTD_NO_EXT_ALLOC_SIZE

    size_type allocation_size () {
        return __imp.allocation_size ();
    }

    size_type allocation_size (size_type new_size) const {
        return __imp.allocation_size (new_size);
    }

#endif  // _RWSTD_NO_EXT_ALLOC_SIZE
};


template <class _TypeT>
inline bool operator== (const vector<_TypeT*, allocator <_TypeT*> > &x,
                                              const vector<_TypeT*, allocator <_TypeT*> > &y)
{
    return x.size () == y.size () && equal (x.begin (), x.end (), y.begin ());
}

template <class _TypeT>
inline bool operator< (const vector<_TypeT*, allocator <_TypeT*> >&x,
                       const vector<_TypeT*, allocator <_TypeT*> >&y)
{
    return lexicographical_compare (x.begin(), x.end(), y.begin(), y.end());
}


#if !defined (_RWSTD_NO_NAMESPACE) || !defined (_RWSTD_NO_PART_SPEC_OVERLOAD)

template <class _TypeT>
inline bool operator!= (const vector<_TypeT*, allocator <_TypeT*> > &x,
                        const vector<_TypeT*, allocator <_TypeT*> > &y)
{
    return !(x == y);
}

template <class _TypeT>
inline bool operator<= (const vector<_TypeT*, allocator <_TypeT*> > &x,
                        const vector<_TypeT*, allocator <_TypeT*> > &y)
{
    return !(y < x);
}

template <class _TypeT>
inline bool operator>= (const vector<_TypeT*, allocator <_TypeT*> > &x,
                        const vector<_TypeT*, allocator <_TypeT*> > &y)
{
    return !(x < y);
}

template <class _TypeT>
inline bool operator> (const vector<_TypeT*, allocator <_TypeT*> > &x,
                       const vector<_TypeT*, allocator <_TypeT*> > &y)
{
    return y < x;
}

#endif // !defined (_RWSTD_NO_NAMESPACE) || !defined (_RWSTD_NO_PART_SPEC_OVERLOAD)


}   // namespace std


#endif //__STD_VECTOR_SPEC__
