// -*- C++ -*-
/***************************************************************************
 *
 * _allocator.h - definition of the class template allocator
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
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

#ifndef _RWSTD_RW_ALLOCATOR_H_INCLUDED
#define _RWSTD_RW_ALLOCATOR_H_INCLUDED

#ifndef _RWSTD_SPECIALIZED_H_INCLUDED
#  include <rw/_specialized.h>
#endif   // _RWSTD_SPECIALIZED_H_INCLUDED


_RWSTD_NAMESPACE (__rw) {

// [de]allocate storage (in bytes)
_RWSTD_EXPORT void* __rw_allocate (_RWSTD_SIZE_T, int = 0);
_RWSTD_EXPORT void  __rw_deallocate (void*, _RWSTD_SIZE_T, int = 0);

}   // namespace __rw


_RWSTD_NAMESPACE (std) {


#ifndef _RWSTD_NO_EXT_CXX_0X

struct allocator_arg_t { };

const allocator_arg_t allocator_arg = allocator_arg_t ();

#endif


template <class _TypeT> class
allocator;


_RWSTD_SPECIALIZED_CLASS
class allocator<void>
{
public:
    typedef void              value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
   

#ifdef _RWSTD_ALLOCATOR

    template <class _TypeU> 
    struct rebind {
        typedef allocator<_TypeU> other;
    };

#endif   // _RWSTD_ALLOCATOR
    
};


template <class _TypeT>
class allocator
{
public:
    typedef _RWSTD_SIZE_T       size_type;
    typedef _RWSTD_PTRDIFF_T    difference_type;
    typedef _TypeT              value_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;

    allocator () _THROWS (()) { }

    allocator (const allocator &__rhs) _THROWS (()) {
        // working around an HP aCC warning 431
        _RWSTD_UNUSED (__rhs);
    }

#ifdef _RWSTD_ALLOCATOR

    template <class _TypeU> 
    struct rebind {
        typedef allocator<_TypeU> other;
    };

    template <class _TypeU>
    allocator (const allocator<_TypeU>&) _THROWS (()) { }

    template <class _TypeU>
    allocator&
    operator= (const allocator<_TypeU>&) _THROWS (()) { 
        return *this; 
    }

#endif   // _RWSTD_ALLOCATOR


    pointer address (reference __x) const {
        // lwg issue 350
        return _RWSTD_REINTERPRET_CAST (pointer,
                   &_RWSTD_REINTERPRET_CAST (char&, __x));
    }

    const_pointer address (const_reference __x) const {
        // lwg issue 350
        return _RWSTD_REINTERPRET_CAST (const_pointer,
                   &_RWSTD_REINTERPRET_CAST (const char&, __x));
    }

    pointer allocate (size_type __n, allocator<void>::const_pointer = 0) {

#ifdef _RWSTD_ALLOCATOR
        return _RWSTD_STATIC_CAST (pointer,
                   _RW::__rw_allocate (__n * sizeof (value_type)));
#else
        return _RWSTD_STATIC_CAST (pointer, _RW::__rw_allocate (__n));
#endif   // _RWSTD_ALLOCATOR
    }

#ifdef _RWSTD_ALLOCATOR        
    void deallocate (pointer __p, size_type __n)
#else
    void deallocate (void* __p, size_type __n)
#endif   // _RWSTD_ALLOCATOR
    {
        _RW::__rw_deallocate (__p, __n);
    }

    // 20.4.1.1, p11 - the largest N for which allocate (N) might succeed
    size_type max_size () const _THROWS (()) { 
        return size_type (_RWSTD_SIZE_MAX) / sizeof (value_type)
            ? size_type (size_type (_RWSTD_SIZE_MAX) / sizeof (value_type))
            : size_type (1);
    }

    void construct (pointer __p, const_reference __val) {
        _RW::__rw_construct (__p, __val);
    }
    
    void destroy (pointer __p) {
        _RWSTD_ASSERT (0 != __p);
        __p->~_TypeT ();
    }
};


#if    !defined (_RWSTD_NO_CLASS_PARTIAL_SPEC) \
    && !defined (_RWSTD_NO_EXT_CONST_ALLOCATOR)

// extension: allocates/constructs/destroys const elements

template <class _TypeT>
class allocator<const _TypeT>
{
public:
    typedef _RWSTD_SIZE_T       size_type;
    typedef _RWSTD_PTRDIFF_T    difference_type;
    typedef const _TypeT        value_type;
    typedef const value_type*   pointer;
    typedef const value_type*   const_pointer;
    typedef const value_type&   reference;
    typedef const value_type&   const_reference;

    allocator () _THROWS (()) { }

    allocator (const allocator &__rhs) _THROWS (()) {
        // working around an HP aCC warning 431
        _RWSTD_UNUSED (__rhs);
    }

#ifdef _RWSTD_ALLOCATOR

    template <class _TypeU> 
    struct rebind {
        typedef allocator<_TypeU> other;
    };

    template <class _TypeU>
    allocator (const allocator<_TypeU>&) _THROWS (()) { }

    template <class _TypeU>
    allocator& operator= (const allocator<_TypeU>&) _THROWS (()) { 
        return *this; 
    }

#endif   // _RWSTD_ALLOCATOR

    const_pointer address (const_reference __x) const { 
        // lwg issue 350
        return _RWSTD_REINTERPRET_CAST (const_pointer,
                   &_RWSTD_REINTERPRET_CAST (const char&, __x));
    }

    // avoid the use of allocator<void>::const_pointer below
    // to work around a SunPro 5.3 (and prior) bug
    const_pointer allocate (size_type __n, const void* = 0) { 

#ifdef _RWSTD_ALLOCATOR
        return _RWSTD_STATIC_CAST (const_pointer,
                   _RWSTD_CONST_CAST (const void*,
                       _RW::__rw_allocate (__n * sizeof (value_type))));
#else
        return _RWSTD_STATIC_CAST (const_pointer,
                   _RWSTD_CONST_CAST (const void*,
                       _RW::__rw_allocate (__n)));
#endif   // _RWSTD_ALLOCATOR

    }

#ifdef _RWSTD_ALLOCATOR        
    void deallocate (const_pointer __p, size_type __nelems) {
        _RW::__rw_deallocate (_RWSTD_CONST_CAST (_TypeT*, __p), __nelems);
    }
#else
    void deallocate (const void* __p, size_type __nbytes) {
        _RW::__rw_deallocate (_RWSTD_CONST_CAST (void*, __p), __nbytes);
    }
#endif   // _RWSTD_ALLOCATOR

    // 20.4.1.1, p11 - the largest N for which allocate (N) might succeed
    size_type max_size () const _THROWS (()) { 
        return size_type (_RWSTD_SIZE_MAX) / sizeof (value_type)
            ? size_type (size_type (_RWSTD_SIZE_MAX) / sizeof (value_type))
            : size_type (1);
    }

    void construct (const_pointer __p, const_reference __val) {
        _RW::__rw_construct (_RWSTD_CONST_CAST (_TypeT*, __p), __val);
    }
    
    void destroy (const_pointer __p) {
        _RWSTD_ASSERT (0 != __p);
        __p->~_TypeT ();
    }
};

#endif   // !_RWSTD_NO_CLASS_PARTIAL_SPEC && !_RWSTD_NO_EXT_CONST_ALLOCATOR)


// allocator_interface provides all types and typed functions.  Memory
// allocated as raw bytes using the class provided by the Allocator
// template parameter.  allocator_interface casts appropriately.
//
// Multiple allocator_interface objects can attach to a single 
// allocator, thus allowing one allocator to allocate all storage
// for a container, regardless of how many types are involved.
//
// The only real restriction is that pointer and reference are
// hard coded as _TypeT* and _TypeT&.  Partial specialization would 
// get around this.
//
#ifndef _RWSTD_ALLOCATOR

template <class _Allocator, class _TypeT>
class allocator_interface 
{
public:
    typedef _Allocator                                allocator_type;
    typedef _TypeT                                    value_type;
    typedef value_type*                               pointer;
    typedef const value_type*                         const_pointer;
    typedef value_type&                               reference;
    typedef const value_type&                         const_reference;
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;

protected:
    allocator_type _C_alloc;

public:
    allocator_interface()  _THROWS (())  { }

    allocator_interface (const allocator_type &__alloc)  _THROWS (())
    : _C_alloc (__alloc) { }

    operator allocator_type& () {
        return _C_alloc;
    }

    pointer address (reference __x) const {
        // lwg issue 350
        return _RWSTD_REINTERPRET_CAST (pointer,
                   &_RWSTD_REINTERPRET_CAST (char&, __x));
    }
  
    const_pointer address (const_reference __x) const {
        // lwg issue 350
        return _RWSTD_REINTERPRET_CAST (const_pointer,
                   &_RWSTD_REINTERPRET_CAST (const char&, __x));
    }

    size_type max_size () const { 
        return _C_alloc.max_size () / sizeof (value_type);
    }

    pointer allocate (size_type __n, const void* __p = 0) {
        return (pointer)_C_alloc.allocate (__n * sizeof (value_type), __p);
    }

    void deallocate (pointer __p, size_type __n) {
        _C_alloc.deallocate (__p, __n);
    }

    void construct (pointer __p, const_reference __val) {
        _RW::__rw_construct(__p, __val);
    }

    void destroy (pointer __p) const {
        _RWSTD_ASSERT (0 != __p);
        __p->~_TypeT ();
    }
};


_RWSTD_SPECIALIZED_CLASS
class allocator_interface<allocator<void>, void> 
{
public:
    typedef allocator<void> allocator_type;
    typedef void*           pointer;
    typedef const void*     const_pointer;      
    typedef void            value_type;

protected:
    allocator_type _C_alloc;

public:
    allocator_interface () _THROWS (()) { }

    allocator_interface (const allocator<void>& __rhs) _THROWS (())
    : _C_alloc (__rhs) { }

};


template <class _TypeT, class _TypeU, class _TypeV, class _TypeW>
inline bool
operator== (const allocator_interface<_TypeT, _TypeU>&, 
            const allocator_interface<_TypeV, _TypeW>&) _THROWS (())
{
    return true;
}

#endif  // _RWSTD_ALLOCATOR


template <class _TypeT, class _TypeU>
inline bool
operator== (const allocator<_TypeT>&, const allocator<_TypeU>&)  _THROWS (())
{
    return true;
}


#ifndef _RWSTD_NO_NAMESPACE

template <class _TypeT, class _TypeU>
inline bool
operator!= (const allocator<_TypeT>& __x,
            const allocator<_TypeU>& __y)  _THROWS (())
{
    return !(__x == __y);
}

#endif   // _RWSTD_NO_NAMESPACE


}   // namespace std


#endif   // _RWSTD_RW_ALLOCATOR_H_INCLUDED
