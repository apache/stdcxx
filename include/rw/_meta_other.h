/***************************************************************************
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
 * Copyright 2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#ifndef _RWSTD_RW_META_OTHER_H_INCLUDED
#define _RWSTD_RW_META_OTHER_H_INCLUDED

#include <rw/_defs.h>

#include <rw/_meta_cat.h>
#include <rw/_meta_ref.h>
#include <rw/_meta_prop.h>
#include <rw/_meta_ptr.h>
#include <rw/_static_assert.h>

#include <rw/_static_assert.h>

_RWSTD_NAMESPACE (__rw) {

template <bool _Select, class _TypeT, class _TypeU>
struct __rw_conditional
{
    typedef _TypeT type;
};

template <class _TypeT, class _TypeU>
struct __rw_conditional<false, _TypeT, _TypeU>
{
    typedef _TypeU type;
};

#define _RWSTD_CONDITIONAL(C,T,U) _RW::__rw_conditional<C,T,U>::type


#ifndef _RWSTD_NO_ALIGN_TRAITS

// Helper for __rw_default_alignment. The member value will evaluate
// to the nearest power of two that is a valid alignment value that
// is less than or equal to _Size.
template <_RWSTD_SIZE_T _Size, _RWSTD_SIZE_T _N,
          bool =    (_RWSTD_TT_MAX_ALIGNMENT <= _N * 2)
                 || (_Size < _N * 2)>
struct __rw_default_alignment_impl
{
    enum { value = __rw_default_alignment_impl<_Size, _N * 2>::value };
};

// Helper for __rw_default_alignment. The member value will evaluate
// to the nearest power of two that is less than or equal to _Size.
// This specialization is used to terminate recursion. It is only used
// when when _Done in the primary template evaluates is true.
template <_RWSTD_SIZE_T _Size, _RWSTD_SIZE_T _N>
struct __rw_default_alignment_impl<_Size, _N, true>
{
    enum { value = _N };
};


// Helper for __rw_aligned_storage. The value member shall be the most
// most stringent alignment requirement for any C++ object whose size
// is no greater than _Size. This implementation will set value to be
// the nearest power of two value that is less than or equal to _Size.
template <_RWSTD_SIZE_T _Size>
struct __rw_default_alignment
{
    enum { value = __rw_default_alignment_impl<_Size, 1>::value };
};

#ifdef __GNUG__

template <_RWSTD_SIZE_T _Size,
          _RWSTD_SIZE_T _Align = __rw_default_alignment<_Size>::value>
struct __rw_aligned_storage
{
    typedef struct {
        _RWSTD_TT_ALIGNED_POD (_Align) _C_align;
        unsigned char _C_size [_Size];
    } type;
};

#else // !__GNUG__

// Helper for __rw_aligned_storage.
template <_RWSTD_SIZE_T _Align>
struct __rw_aligned_storage_impl;

#define _RWSTD_ALIGNED_STORAGE_SPEC(N)              \
  template <> struct __rw_aligned_storage_impl<N> { \
      typedef _RWSTD_TT_ALIGNED_POD (N) _C_type;  };

_RWSTD_ALIGNED_STORAGE_SPEC(1)
_RWSTD_ALIGNED_STORAGE_SPEC(2)
_RWSTD_ALIGNED_STORAGE_SPEC(4)
_RWSTD_ALIGNED_STORAGE_SPEC(8)
_RWSTD_ALIGNED_STORAGE_SPEC(16)

#if (32 <= _RWSTD_TT_MAX_ALIGNMENT)
  _RWSTD_ALIGNED_STORAGE_SPEC(32)
#endif

#if (64 <= _RWSTD_TT_MAX_ALIGNMENT)
  _RWSTD_ALIGNED_STORAGE_SPEC(64)
#endif

#if (128 <= _RWSTD_TT_MAX_ALIGNMENT)
  _RWSTD_ALIGNED_STORAGE_SPEC(128)
#endif

#if (256 <= _RWSTD_TT_MAX_ALIGNMENT)
  _RWSTD_ALIGNED_STORAGE_SPEC(256)
#endif

#if (512 <= _RWSTD_TT_MAX_ALIGNMENT)
  _RWSTD_ALIGNED_STORAGE_SPEC(512)
#endif

#if (1024 <= _RWSTD_TT_MAX_ALIGNMENT)
  _RWSTD_ALIGNED_STORAGE_SPEC(1024)
#endif

#if (2048 <= _RWSTD_TT_MAX_ALIGNMENT)
  _RWSTD_ALIGNED_STORAGE_SPEC(2048)
#endif 

#if (4096 <= _RWSTD_TT_MAX_ALIGNMENT)
  _RWSTD_ALIGNED_STORAGE_SPEC(4096)
#endif  

#if (8192 <= _RWSTD_TT_MAX_ALIGNMENT)
  _RWSTD_ALIGNED_STORAGE_SPEC(8192)
#endif

template <_RWSTD_SIZE_T _Size,
          _RWSTD_SIZE_T _Align = __rw_default_alignment<_Size>::value>
struct __rw_aligned_storage
{
    typedef union {
        typename
        __rw_aligned_storage_impl<_Align>::_C_type _C_align;

        unsigned char _C_size [_Size];
    } type;
};

#endif // !__GNUG__


#ifndef _RWSTD_NO_VARIADIC_TEMPLATES

// Helper for __rw_aligned_union. Provides a typedef type that
// is the largest type in the sequence of provided types.
template <class... _Types>
struct __rw_biggest;

template <class _TypeT, class... _Types>
struct __rw_biggest<_TypeT, _Types...>
{
    enum { _C_values = __rw_biggest<_Types...>::_C_value,
           _C_value  =   sizeof (_TypeT) < _C_values
                       ? _C_values 
                       : sizeof (_TypeT) };
};

template <class _TypeT>
struct __rw_biggest<_TypeT>
{
    enum { _C_value = sizeof (_TypeT) };
};

// Helper for __rw_aligned_union.
template <class... _Types>
struct __rw_strictest;

template <class _TypeT, class... _Types>
struct __rw_strictest<_TypeT, _Types...>
{
    enum { _C_values = __rw_strictest<_Types...>::_C_value,
           _C_value =   __rw_alignment_of<_TypeT>::value < _C_values
                      ? _C_values
                      : __rw_alignment_of<_TypeT>::value };
};

template <class _TypeT>
struct __rw_strictest<_TypeT>
{
    enum { _C_value = __rw_alignment_of<_TypeT>::value };
};

template <_RWSTD_SIZE_T _Len, class... _Types>
struct __rw_aligned_union;

template <_RWSTD_SIZE_T _Len, class _TypeT, class... _Types>
struct __rw_aligned_union<_Len, _TypeT, _Types...>
{
    enum { _C_len_value   = _Len,
           _C_size_value  = __rw_biggest<_TypeT, _Types...>::_C_value,
           _C_align_value = __rw_strictest<_TypeT, _Types...>::_C_value };

    typedef typename
    __rw_aligned_storage<  _C_len_value < _C_size_value
                         ? _C_size_value : _C_len_value,
                         _C_align_value>::type type;
};

#else // _RWSTD_NO_VARIADIC_TEMPLATES

// Helper. Gives the largest of a series of values.
template <unsigned _A    , unsigned _B = 0,
          unsigned _C = 0, unsigned _D = 0,
          unsigned _E = 0, unsigned _F = 0,
          unsigned _G = 0, unsigned _H = 0>
struct __rw_max_uint
{
    enum {
            _C_ab = _A < _B ? _B : _A,
            _C_cd = _C < _D ? _D : _C,
            _C_abcd = _C_ab < _C_cd ? _C_cd : _C_ab,

            _C_ef = _E < _F ? _F : _E,
            _C_gh = _G < _H ? _H : _G,
            _C_efgh = _C_ef < _C_gh ? _C_gh : _C_ef,

            _C_value = _C_abcd < _C_efgh ?  _C_efgh : _C_abcd
    };
};

struct __rw_empty { };

// Helper for __rw_aligned_union.
template <class _Type1             , class _Type2 = __rw_empty,
          class _Type3 = __rw_empty, class _Type4 = __rw_empty,
          class _Type5 = __rw_empty, class _Type6 = __rw_empty,
          class _Type7 = __rw_empty, class _Type8 = __rw_empty>
struct __rw_biggest
{
    enum { _C_value =
        __rw_max_uint<sizeof (_Type1), sizeof (_Type2),
                      sizeof (_Type3), sizeof (_Type4),
                      sizeof (_Type5), sizeof (_Type6),
                      sizeof (_Type7), sizeof (_Type8)>::_C_value
    };
};

// Helper for __rw_aligned_union.
template <class _Type1             , class _Type2 = __rw_empty,
          class _Type3 = __rw_empty, class _Type4 = __rw_empty,
          class _Type5 = __rw_empty, class _Type6 = __rw_empty,
          class _Type7 = __rw_empty, class _Type8 = __rw_empty>
struct __rw_strictest
{
    enum { _C_value =
        __rw_max_uint<__rw_alignment_of<_Type1>::value,
                      __rw_alignment_of<_Type2>::value,
                      __rw_alignment_of<_Type3>::value,
                      __rw_alignment_of<_Type4>::value,
                      __rw_alignment_of<_Type5>::value,
                      __rw_alignment_of<_Type6>::value,
                      __rw_alignment_of<_Type7>::value,
                      __rw_alignment_of<_Type8>::value>::_C_value
        };
};

template <_RWSTD_SIZE_T _Len,
          class _Type1             , class _Type2 = __rw_empty,
          class _Type3 = __rw_empty, class _Type4 = __rw_empty,
          class _Type5 = __rw_empty, class _Type6 = __rw_empty,
          class _Type7 = __rw_empty, class _Type8 = __rw_empty>
struct __rw_aligned_union
{
    enum { _C_len_value   = _Len,
           _C_size_value  =
               __rw_biggest<_Type1, _Type2, _Type3, _Type4,
                            _Type5, _Type6, _Type7, _Type8>::_C_value,
           _C_align_value =
               __rw_strictest<_Type1, _Type2, _Type3, _Type4,
                              _Type5, _Type6, _Type7, _Type8>::_C_value
    };

    typedef typename
    __rw_aligned_storage<  _C_size_value < _C_len_value
                         ? _C_len_value : _C_size_value,
                         _C_align_value>::type type;
};

#endif // !_RWSTD_NO_VARIADIC_TEMPLATES

#endif // !_RWSTD_NO_ALIGN_TRAITS


template <bool _Enable, class _TypeT = void>
struct __rw_enable_if
{
    typedef _TypeT type;
};

template <class _TypeT>
struct __rw_enable_if<false, _TypeT>
{
};

#define _RWSTD_ENABLE_IF(C,T) _RW::__rw_enable_if<C,T>::type


template <bool _Enable, class _TypeT = void>
struct __rw_disable_if
{
    typedef _TypeT type;
};

template <class _TypeT>
struct __rw_disable_if<true, _TypeT>
{
};

#define _RWSTD_DISABLE_IF(C,T) _RW::__rw_disable_if<C,T>::type


template <class _TypeT>
struct __rw_decay
{
private:
    typedef typename
    __rw_remove_reference<_TypeT>::type _TypeU;

    typedef typename
    __rw_conditional<__rw_is_function<_TypeU>::value,
                     typename __rw_add_pointer<_TypeU>::type,
                     typename __rw_remove_cv<_TypeU>::type
                    >::type _TypeV;

public:
    typedef typename
    __rw_conditional<__rw_is_array<_TypeU>::value,
                     typename __rw_remove_extent<_TypeU>::type*,
                     _TypeV
                    >::type type;
};

#define _RWSTD_DECAY(T) _RW::__rw_decay<T>::type


} // namespace __rw


#endif   // _RWSTD_RW_META_OTHER_H_INCLUDED

