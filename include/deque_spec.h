// -*- C++ -*-
/***************************************************************************
 *
 * deque_spec.h - deque <_TypeT*> partial specialization
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

#ifndef __STD_DEQUE_SPEC__
#define __STD_DEQUE_SPEC__

#include <rw/_defs.h>


#ifndef __STD_DEQUE__
#include <deque>
#endif   // __STD_DEQUE__


_RWSTD_NAMESPACE (std) { 


template class deque <void*, allocator <void*> >;


template <class _TypeT>
class deque <_TypeT*, allocator <_TypeT*> >
{
    typedef allocator <_TypeT*>                   _Allocator;
    typedef _RWSTD_REBIND (_Allocator, void*) __imp_alloc_type;
    typedef deque <void*, __imp_alloc_type>  __imp_type;

    __imp_type                                __imp;

public:

    typedef _TypeT*                               value_type;
    typedef _Allocator                        allocator_type;

    class        iterator;
    class        const_iterator;
    friend class iterator;
    friend class const_iterator;

    typedef _TYPENAME allocator_type::reference       reference;
    typedef _TYPENAME allocator_type::const_reference const_reference;
    typedef _TYPENAME allocator_type::size_type       size_type;
    typedef _TYPENAME allocator_type::difference_type difference_type;
    typedef _TYPENAME allocator_type::pointer         pointer;
    typedef _TYPENAME allocator_type::const_pointer   const_pointer;


protected:

    typedef _RWSTD_REBIND (_Allocator, pointer)   __map_alloc_type;
    typedef _TYPENAME __map_alloc_type::pointer  __map_pointer;

    typedef _STD::iterator <random_access_iterator_tag, value_type,
        difference_type, pointer,reference> __it;

    typedef _STD::iterator <random_access_iterator_tag, value_type, 
        difference_type, const_pointer, const_reference> __cit;

public:

    class const_iterator;

    class iterator : public __it
    {
        typedef _TYPENAME __imp_type::iterator __imp_iter;

        __imp_iter __imp;

        friend class deque<_TypeT, allocator <_TypeT> >;
        friend class const_iterator;

          // implicit conversions from and to implementation-specific iterator
          // used only internally
        iterator (const __imp_iter &imp)
            : __imp (imp) {}

        operator __imp_iter () {
            return __imp;
        }

    public:

        iterator () {}

        __iterator_base::reference operator* () const {
            return __iterator_base::reference (*__imp);
        }


#ifndef _RWSTD_NO_NONCLASS_ARROW_RETURN

        __iterator_base::pointer operator-> () const {
            return __iterator_base::pointer (&*__imp);
        }

#endif   // _RWSTD_NO_NONCLASS_ARROW_RETURN


        iterator& operator++ () { 
            ++__imp;
            return *this;
        }

        iterator operator++ (int) {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        iterator& operator-- () {
            --__imp;
            return *this;
        }

        iterator operator-- (int) {
            iterator tmp = *this;
            --*this;
            return tmp;
        }

        iterator& operator+= (__iterator_base::difference_type n) {
            __imp += n;
            return *this;
        }

        iterator& operator-= (__iterator_base::difference_type n) {
            __imp -= n;
            return *this;
        }

        __iterator_base::difference_type operator- (const iterator& x) const {
            return __imp - x.__imp;
        }

        iterator operator- (__iterator_base::difference_type n) {
            return __imp - n;
        }

        iterator operator+ (__iterator_base::difference_type n) {
            return __imp + n;
        }

        __iterator_base::reference operator[] (__iterator_base::difference_type n) {
            return __imp [n];
        }

        bool operator== (const iterator &x) const {
            return __imp == x.__imp;
        }

        bool operator< (const iterator &x) const {
            return __imp < x.__imp;
        }

        bool operator> (const iterator &x) const {
            return x.__imp < *this;
        }

        bool operator!= (const iterator &x) const {
            return !(*this == x.__imp);
        }

        bool operator<= (const iterator &x) const {
            return !(*x.__imp < *this);
        }

        bool operator>= (const iterator &x) const {
            return !(x.__imp < *this);
        }
    };


    class const_iterator : public __cit
    {
        typedef _TYPENAME __imp_type::const_iterator __imp_iter;

        __imp_iter __imp;

        friend class deque<_TypeT, allocator <_TypeT> >;

        const_iterator (const __imp_iter &imp): __imp (imp) {}
    
    public:
      
        const_iterator () {}

        const_iterator (const iterator &x) : __imp (x.__imp) {}

        const_reference operator* () const {
            return const_reference (*__imp);
        }


#ifndef _RWSTD_NO_NONCLASS_ARROW_RETURN

        const_pointer operator-> () const {
            return const_pointer (&*__imp);
        }

#endif   //  _RWSTD_NO_NONCLASS_ARROW_RETURN


        const_iterator& operator++ () { 
            ++__imp;
            return *this;
        }

        const_iterator operator++ (int) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        const_iterator& operator-- () {
            --__imp;
            return *this;
        }

        const_iterator operator-- (int) {
            const_iterator tmp = *this;
            --*this;
            return tmp;
        }

        const_iterator& operator+= (__iterator_base::difference_type n) {
            __imp += n;
            return *this;
        }

        const_iterator& operator-= (__iterator_base::difference_type n) {
            __imp -= n;
            return *this;
        }

        __iterator_base::difference_type operator- (const const_iterator& x) const {
            return __imp - x.__imp;
        }

        const_iterator operator- (__iterator_base::difference_type n) {
            return __imp - n;
        }

        const_iterator operator+ (__iterator_base::difference_type n) {
            return __imp + n;
        }

        const_reference operator[] (__iterator_base::difference_type n) {
            return __imp [n];
        }

        bool operator== (const const_iterator &x) const {
            return __imp == x.__imp;
        }

        bool operator< (const const_iterator &x) const {
            return __imp < x.__imp;
        }

        bool operator> (const const_iterator &x) const {
            return x.__imp < *this;
        }

        bool operator!= (const const_iterator &x) const {
            return !(*this == x.__imp);
        }

        bool operator<= (const const_iterator &x) const {
            return !(*x.__imp < *this);
        }

        bool operator>= (const const_iterator &x) const {
            return !(x.__imp < *this);
        }
    };


#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC 
    typedef _STD::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef _STD::reverse_iterator<iterator>       reverse_iterator;
#else
    typedef _STD::reverse_iterator<const_iterator, 
        random_access_iterator_tag, value_type, 
        const_reference, const_pointer, difference_type>
    const_reverse_iterator;

    typedef _STD::reverse_iterator<iterator, 
        random_access_iterator_tag, value_type,
        reference, pointer, difference_type>
    reverse_iterator;
#endif

public:

    _EXPLICIT deque (const _Allocator &alloc _RWSTD_DEFAULT_ARG (_Allocator ())) 
        : __imp (alloc) { }


#ifdef _RWSTD_NO_DEFAULT_TEMPLATE_ARGS

    deque () 
        : __imp () { }

    deque (size_type n, const_reference x)
        : __imp (n, x) { }

#endif // _RWSTD_NO_DEFAULT_TEMPLATE_ARGS


    _EXPLICIT deque (size_type n)
        : __imp (n) { }


    deque (size_type n, const_reference x,
           const _Allocator& alloc _RWSTD_DEFAULT_ARG (_Allocator()))
        : __imp (n, x, alloc) { }


#ifndef _RWSTD_NO_MEMBER_TEMPLATES

    template <class InputIterator>
    deque (InputIterator first, InputIterator last,
           const _Allocator& alloc _RWSTD_DEFAULT_ARG (_Allocator ()))
        : __imp (first, last, alloc) { }

#else   // if defined (_RWSTD_NO_MEMBER_TEMPLATES)

    deque (const_iterator first, const_iterator last,
           const _Allocator& alloc _RWSTD_DEFAULT_ARG(_Allocator()))
        : __imp (first, last, alloc) { }
    
    deque (const_pointer first, const_pointer last,
           const _Allocator& alloc _RWSTD_DEFAULT_ARG(_Allocator()))
        : __imp (first, last, alloc) { }


#ifdef _RWSTD_NO_DEFAULT_TEMPLATE_ARGS

    deque (const_iterator first, const_iterator last)
        : __imp (first, last) { }
    
    deque (const_pointer first, const_pointer last)
        : __imp (first, last) { }

#endif // _RWSTD_NO_DEFAULT_TEMPLATE_ARGS
#endif // _RWSTD_NO_MEMBER_TEMPLATES


    deque (const deque &x)
        : __imp (x.__imp) { }

    deque& operator= (const deque &x) {
        __imp = x.__imp;
        return *this;
    }


#ifndef _RWSTD_NO_MEMBER_TEMPLATES

    template<class InputIterator>
    void assign (InputIterator first, InputIterator last) {
        __imp.assign (first, last);
    }

#else   // if defined (_RWSTD_NO_MEMBER_TEMPLATES)

    void assign (const_iterator first, const_iterator last) {
        __imp.assign (first, last);
    }

    void assign (const_pointer first, const_pointer last) {
        __imp.assign (first, last);
    }

#endif // _RWSTD_NO_MEMBER_TEMPLATES


    void assign (size_type n, const_reference x) {
        __imp.assign (n, x);
    }

    allocator_type get_allocator () const
    {
      return __imp.get_allocator ();
    }

    iterator begin () {
        return __imp.begin ();
    }

    const_iterator begin () const {
        return __imp.begin ();
    }

    iterator end () {
        return __imp.end ();
    }

    const_iterator end () const {
        return __imp.end ();
    }

    reverse_iterator rbegin () {
        return reverse_iterator (end ());
    }

    const_reverse_iterator rbegin () const {
        return reverse_iterator (end ());
    }

    reverse_iterator rend () {
        return reverse_iterator (begin ());
    }

    const_reverse_iterator rend () const {
        return reverse_iterator (begin ());
    }

    bool empty () const {
        return __imp.empty ();
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
        __imp.resize (new_size, x);
    }

    reference operator[] (size_type n) {
        return reference (__imp [n]);
    }

    const_reference operator[] (size_type n) const {
        return const_reference (__imp [n]);
    }

    const_reference at (size_type n) const {
        return __imp.at (n);
    }

    reference at (size_type n) {
        return __imp.at (n);
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

    void push_front (const_reference x) {
        __imp.push_front (x);
    }

    void push_back (const_reference x) {
        __imp.push_back (x);
    }

    void pop_front () {
        __imp.pop_front ();
    }

    void pop_back () {
        __imp.pop_back ();
    }

    iterator insert (iterator pos, const_reference x) {
        return __imp.insert (pos, x);
    }


#ifndef _RWSTD_NO_MEMBER_TEMPLATES

    template <class InputIterator>
    void insert (iterator pos, InputIterator first, InputIterator last) {
        __imp.insert (pos, first, last);
    }

#else   // if defined (_RWSTD_NO_MEMBER_TEMPLATES)

    void insert (iterator pos, const_iterator first, const_iterator last) {
        __imp.insert (pos, first, last);
    }        

#endif // _RWSTD_NO_MEMBER_TEMPLATES


    void insert (iterator pos, size_type n, const_reference x) {
        __imp.insert (pos, n, x);
    }

    iterator erase (iterator pos) {
        return __imp.erase (pos);
    }

    iterator erase (iterator first, iterator last) {
        return __imp.erase (first, last);
    }

    void swap (deque &x) {
        __imp.swap (x.__imp);
    }

    void clear () {
        __imp.clear ();
    }
};

template <class _TypeT>
inline bool operator== (const deque <_TypeT*, allocator <_TypeT*> > &x,
                        const deque <_TypeT*, allocator <_TypeT*> > &y)
{
    return x.size () == y.size () && equal (x.begin (), x.end (), y.begin ());
}

template <class _TypeT>
inline bool operator< (const deque <_TypeT*, allocator <_TypeT*> > &x,
                       const deque <_TypeT*, allocator <_TypeT*> > &y)
{
    return lexicographical_compare (x.begin (), x.end (), y.begin (), y.end ());
}


#if !defined (_RWSTD_NO_NAMESPACE) || !defined (_RWSTD_NO_PART_SPEC_OVERLOAD)
template <class _TypeT>
inline bool operator!= (const deque <_TypeT*, allocator <_TypeT*> > &x,
                        const deque <_TypeT*, allocator <_TypeT*> > &y)
{
    return !(x == y);
}

template <class _TypeT>
inline bool operator> (const deque <_TypeT*, allocator <_TypeT*> > &x,
                       const deque <_TypeT*, allocator <_TypeT*> > &y)
{
    return y < x;
}

template <class _TypeT>
inline bool operator>= (const deque <_TypeT*, allocator <_TypeT*> > &x,
                        const deque <_TypeT*, allocator <_TypeT*> > &y)
{
    return !(x < y);
}

template <class _TypeT>
inline bool operator<= (const deque <_TypeT*, allocator <_TypeT*> > &x,
                        const deque <_TypeT*, allocator <_TypeT*> > &y)
{
    return !(y <  x);
}

template <class _TypeT>
inline void swap (deque <_TypeT*, allocator <_TypeT*> > &a,
                  deque <_TypeT*, allocator <_TypeT*> > &b)
{
    a.swap(b);
}

#endif // !defined(_RWSTD_NO_NAMESPACE) || !defined(_RWSTD_NO_PART_SPEC_OVERLOAD)


}   // namespace std


#endif /*__STD_DEQUE_SPEC__*/
