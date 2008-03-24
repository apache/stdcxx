/***************************************************************************
 *
 * _array.h - Declarations for the Standard Library __rw_array
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
 * Copyright 2000-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_ARRAY_H_INCLUDED
#define _RWSTD_RW_ARRAY_H_INCLUDED

#ifndef _RWSTD_RW_SPECIALIZED_H_INCLUDED
#  include <rw/_specialized.h>
#endif   // _RWSTD_RW_SPECIALIZED_H_INCLUDED


_RWSTD_NAMESPACE (__rw) { 


template <class _TypeT>
class __rw_array
{
public:

    typedef _RWSTD_SIZE_T     size_type;
    typedef _TypeT            value_type;
    typedef value_type*       pointer;
    typedef value_type&       reference;
    typedef const value_type* const_pointer;
    typedef const value_type& const_reference;

    __rw_array ()
        : _C_size (0),
          _C_data (0) { }

    // allocate but do not initialize
    __rw_array (size_type);

    // allocate and initialize from value
    __rw_array (const_reference, size_type);

    // allocate and initialize from an array
    __rw_array (const_pointer, size_type);

    __rw_array (const __rw_array&);

    ~__rw_array ();

    __rw_array& operator= (const __rw_array&);

    size_type size () const {
        return _C_size;
    }

    const_reference operator[] (size_type __inx) const {
        _RWSTD_ASSERT (__inx < size ());
        return _C_data [__inx];
    }

    reference operator[] (size_type __inx) {
        _RWSTD_ASSERT (__inx < size ());
        return _C_data [__inx];
    }

    void swap (__rw_array&);

    pointer begin () {
        return _C_data;
    }

    const_pointer begin () const {
        return _C_data;
    }

    pointer end () {
        return _C_data + _C_size;
    }

    const_pointer end () const {
        return _C_data + _C_size;
    }

    void resize (size_type, const_reference = value_type ());

private:
    size_type _C_size;   // number of elements
    pointer   _C_data;   // array of elements
};


template <class _TypeT>
inline __rw_array<_TypeT>::__rw_array (size_type __n)
{
    if (__n)
        _C_data = _RWSTD_STATIC_CAST (pointer,
                                      ::operator new (__n * sizeof *_C_data));
    else
        _C_data = 0;

    _C_size = __n;
}


template <class _TypeT>
inline __rw_array<_TypeT>::__rw_array (const_reference __val, size_type __n)
{
    if (__n) {
        _C_data = _RWSTD_STATIC_CAST (pointer,
                                      ::operator new (__n * sizeof *_C_data));
        _STD::uninitialized_fill_n (_C_data, __n, __val);
    }
    else
        _C_data = 0;

    _C_size = __n;
}


template <class _TypeT>
inline __rw_array<_TypeT>::__rw_array (const_pointer __data, size_type __n)
{ 
    if (__n) {
        _C_data = _RWSTD_STATIC_CAST (pointer,
                                      ::operator new (__n * sizeof *_C_data));
        _STD::uninitialized_copy (__data, __data + __n, _C_data);
    }
    else
        _C_data = 0;

    _C_size = __n;
}


template <class _TypeT>
inline __rw_array<_TypeT>::__rw_array (const __rw_array<_TypeT> &__rhs)
    : _C_size (__rhs.size ())
{ 
    if (__rhs.size ()) {
        _C_data = _RWSTD_STATIC_CAST (pointer,
                                    ::operator new (_C_size * sizeof *_C_data));
        _STD::uninitialized_copy (__rhs.begin (), __rhs.end (), begin ());
    }
    else
        _C_data = 0;
}


template <class _TypeT>
inline __rw_array<_TypeT>&
__rw_array<_TypeT>::operator= (const __rw_array<_TypeT> &__rhs)
{
    if (__rhs.size () == size ())
        // do not allocate, just copy for efficiency
        _STD::copy (__rhs.begin (), __rhs.end (), begin ());
    else
        // allocate and copy into a temporary, then swap
        __rw_array <_TypeT> (__rhs).swap (*this);

    return *this;
}


template <class _TypeT>
inline
__rw_array<_TypeT>::
~__rw_array ()
{
    __rw_destroy (_C_data, _C_data + _C_size);
    ::operator delete (_C_data);
}


template <class _TypeT>
inline void __rw_array<_TypeT>::swap (__rw_array<_TypeT> &__rhs)
{
    pointer __tmp_data   = begin ();
    size_type __tmp_size = size ();

    _C_data       = __rhs.begin ();
    _C_size       = __rhs.size ();
    __rhs._C_data = __tmp_data;
    __rhs._C_size = __tmp_size;
}


template <class _TypeT>
inline void __rw_array<_TypeT>::
resize (size_type __size, const_reference __val /* = value_type () */)
{
    if (_C_data) {
        __rw_destroy (_C_data, _C_data + _C_size);
        ::operator delete (_C_data);
        _C_data = 0;
    }

    if ((_C_size = __size)) {
        _C_data = _RWSTD_STATIC_CAST (pointer,
                              ::operator new (_C_size * sizeof *_C_data));
        _STD::uninitialized_fill_n (_C_data, _C_size, __val);
    }
}


}   // namespace __rw


#endif   // _RWSTD_RW_ARRAY_H_INCLUDED
