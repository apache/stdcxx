/***************************************************************************
 *
 * podarray.h - definition of a fast array-like class template
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_PODARRAY_H_INCLUDED
#define _RWSTD_PODARRAY_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#include <string.h>
#include <rw/_traits.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) { 


// __rw_aligned_buffer specializations should be objects with static
// storage duration that must "survive" all other static objects and
// that do not need to be destroyed; this is a POD
template <class _TypeT>
struct __rw_aligned_buffer
{
    union {
#ifndef _RWSTD_NO_LONG_DOUBLE
        long double _C_pad;                     // guarantee suitable alignment
#else
        double      _C_pad;
#endif   // _RWSTD_NO_LONG_DOUBLE
        void       *_C_void_pad;
        void      (*_C_pfn_pad)();
        char        _C_data [sizeof (_TypeT)];  // data to back _TypeT up with 
    } _C_buf;

    void* _C_store () {
        return _C_buf._C_data;
    }

    _TypeT* _C_data () {
        return _RWSTD_REINTERPRET_CAST (_TypeT*, _C_buf._C_data);
    }
};


// typedef prevents an MSVC 6.0 ICE
typedef _RWSTD_SIZE_T _SizeT;

_EXPORT template <class _TypeT, _SizeT _Size>
class __rw_pod_array
{
    _SizeT  _C_len;
    _TypeT *_C_pbuf;
    _TypeT  _C_buffer [_Size];

public:

    __rw_pod_array (): _C_len (0), _C_pbuf (_C_buffer) {
        *_C_pbuf = _TypeT ();
    }

    explicit __rw_pod_array (const _TypeT *__a)
        : _C_len (_C_length (__a))
        // , _C_pbuf (_C_len < _Size ? _C_buffer : new _TypeT [_C_len + 1])
        {
            // initialze _C_pbuf here in order to prevent HP aCC 3.70
            // ICE: see http://issues.apache.org/jira/browse/STDCXX-276
            if (_C_len < _Size)
                _C_pbuf = _C_buffer;
            else
                _C_pbuf = new _TypeT [_C_len + 1];
            memcpy (_C_pbuf, __a, (_C_len + 1) * sizeof *__a);
        }

    __rw_pod_array (const _TypeT *__a, _SizeT __len)
        : _C_len (__len) {
        // initialze _C_pbuf here in order to prevent HP aCC 3.70
        // ICE: see http://issues.apache.org/jira/browse/STDCXX-276
        if (__len < _Size)
            _C_pbuf = _C_buffer;
        else
            _C_pbuf = new _TypeT [__len + 1];
        memcpy (_C_pbuf, __a, __len * sizeof *__a);
        _C_pbuf [_C_len] = _TypeT ();
    }

    __rw_pod_array (const __rw_pod_array &__rhs)
        : _C_len (__rhs._C_len) {
        // initialze _C_pbuf here in order to prevent HP aCC 3.70
        // ICE: see http://issues.apache.org/jira/browse/STDCXX-276
        if (_C_len < _Size)
            _C_pbuf = _C_buffer;
        else
            _C_pbuf = new _TypeT [_C_len + 1];
        memcpy (_C_pbuf, __rhs._C_pbuf,
                (_C_len + !!__rhs._C_pbuf) * sizeof *_C_pbuf);
    }

    ~__rw_pod_array () {
        if (_C_pbuf != _C_buffer)
            delete[] _C_pbuf;
    }

    _TypeT* data () {
        return _C_pbuf;
    }

    const _TypeT* data () const {
        return _C_pbuf;
    }

    _TypeT* acquire (_TypeT *__a, _SizeT __len) {
        _TypeT *__pbuf = _C_pbuf;
        _C_pbuf = __a;
        _C_len  = __len;
        return __pbuf;
    }

    _TypeT* acquire (_TypeT *__a) {
        return acquire (__a, _C_length (__a));
    }

    _TypeT* release () {
        _TypeT* const __pbuf = _C_pbuf;
        _C_pbuf = _C_buffer;
        return __pbuf;
    }

    static _SizeT max_size () {
        return _Size;
    }

    _SizeT size () const {
        return _C_len;
    }

    __rw_pod_array& append (const _TypeT*, _SizeT);

    __rw_pod_array& append (const _TypeT *__a) {
        _RWSTD_ASSERT (__a);
        return append (__a, _C_length (__a));
    }

    __rw_pod_array& operator+= (const _TypeT *__a) {
        _RWSTD_ASSERT (__a);
        return append (__a);
    }


    __rw_pod_array& assign (const _TypeT *__a, _SizeT __len) {
        return (_C_len = 0), append (__a, __len);
    }

    __rw_pod_array& assign (const _TypeT *__a) {
        _RWSTD_ASSERT (__a);
        return assign (__a, _C_length (__a));
    }

    __rw_pod_array& operator= (const _TypeT *__a) {
        _RWSTD_ASSERT (__a);
        return assign (__a);
    }

    __rw_pod_array& operator= (const __rw_pod_array &__rhs) {
        return assign (__rhs._C_pbuf, __rhs._C_len);
    }

private:

    static _RWSTD_SIZE_T _C_length (const _TypeT *__a) {
#ifndef _RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY
        return _STD::char_traits<_TypeT>::length (__a);
#else    // #ifdef _RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY
        _RWSTD_SIZE_T __len = 0;
        for (; _TypeT () != *__a; ++__a, ++__len) ;
        return __len;
#endif   // _RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY
    }
};


typedef __rw_pod_array<char, 256> __rw_chararray;


}   // namespace __rw


#if _RWSTD_DEFINE_TEMPLATE_FIRST (_POD_ARRAY)
#  include "podarray.cc"
#endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (_POD_ARRAY)


_RWSTD_NAMESPACE (__rw) { 

#if _RWSTD_INSTANTIATE (_POD_ARRAY, _CHAR)

// used throughout locale
_RWSTD_INSTANTIATE_2 (class __rw_pod_array<char, 256>);

#endif   // _RWSTD_INSTANTIATE (_POD_ARRAY, _CHAR)

#if _RWSTD_INSTANTIATE (_POD_ARRAY, _UINT)

// used in collate.cpp
_RWSTD_INSTANTIATE_2 (class __rw_pod_array<const unsigned int*, 1024>);

#endif   // _RWSTD_INSTANTIATE (_POD_ARRAY, _UINT)

}   // namespace __rw


#if _RWSTD_DEFINE_TEMPLATE_LAST (_POD_ARRAY)
#  include "podarray.cc"
#endif   // _RWSTD_DEFINE_TEMPLATE_LAST (_POD_ARRAY)


#endif   // _RWSTD_PODARRAY_H_INCLUDED
