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


// Helper for __rw_aligned_storage. Specializations define a member type
// that is aligned on power of two boundaries.
template <_RWSTD_SIZE_T _Align>
struct __rw_aligned_storage_impl;

#define _RWSTD_ALIGNED_STORAGE_SPEC(N)              \
  template <> struct __rw_aligned_storage_impl<N> { \
      typedef _RWSTD_TT_ALIGNED_POD(N) _C_type;     \
}

_RWSTD_ALIGNED_STORAGE_SPEC(1);
_RWSTD_ALIGNED_STORAGE_SPEC(2);
_RWSTD_ALIGNED_STORAGE_SPEC(4);
_RWSTD_ALIGNED_STORAGE_SPEC(8);
_RWSTD_ALIGNED_STORAGE_SPEC(16);
_RWSTD_ALIGNED_STORAGE_SPEC(32);
_RWSTD_ALIGNED_STORAGE_SPEC(64);
_RWSTD_ALIGNED_STORAGE_SPEC(128);
_RWSTD_ALIGNED_STORAGE_SPEC(256);
_RWSTD_ALIGNED_STORAGE_SPEC(512);
_RWSTD_ALIGNED_STORAGE_SPEC(1024);
_RWSTD_ALIGNED_STORAGE_SPEC(2048);
_RWSTD_ALIGNED_STORAGE_SPEC(4096);
_RWSTD_ALIGNED_STORAGE_SPEC(8192);

// Helper for __rw_default_alignment. The member value will evaluate
// to the nearest power of two that is a valid alignment value that
// is less than or equal to _Size.
template <_RWSTD_SIZE_T _Size, _RWSTD_SIZE_T _N,
          bool _Done =    (_RWSTD_TT_MAX_ALIGNMENT <= _N * 2)
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


template <_RWSTD_SIZE_T _Size,
          _RWSTD_SIZE_T _Align = __rw_default_alignment<_Size>::value>
struct __rw_aligned_storage
{
    _RWSTD_STATIC_ASSERT (_Size != 0,
                          "Unsupported size");
    
    _RWSTD_STATIC_ASSERT ((_Align & (_Align - 1)) == 0 || _Align == 0,
                          "Unsupported alignment"); // expect power of 2
    
    _RWSTD_STATIC_ASSERT (_Align <= _RWSTD_TT_MAX_ALIGNMENT,
                          "Unsupported alignment"); // expect less than max

    typedef union {
        unsigned char __size [_Size];

        typename
        __rw_aligned_storage_impl<_Align>::_C_type __align;
    } type;
};


#ifndef _RWSTD_NO_VARIADIC_TEMPLATES

// Helper for __rw_aligned_union. Provides a typedef type that
// is the largest type in the sequence of provided types.
template <class... _Types>
struct __rw_biggest;

template <class _TypeT, class... _Types>
struct __rw_biggest<_TypeT, _Types...>
{
    typedef typename
    __rw_biggest<_Types...>::type _TypeU;

    typedef typename
    __rw_conditional<sizeof (_TypeT) < sizeof (_TypeU),
                     _TypeU, _TypeT>::type type;
};

template <class _TypeT>
struct __rw_biggest<_TypeT>
{
    typedef _TypeT type;
};

// Helper for __rw_aligned_union. Provides a typedef type that
// is the type with the strictest alignment requirement in the
// sequence of provided types.
template <class... _Types>
struct __rw_strictest;

template <class _TypeT, class... _Types>
struct __rw_strictest<_TypeT, _Types...>
{
    typedef typename
    __rw_strictest<_Types...>::type _TypeU;

    typedef typename
    __rw_conditional<   __rw_alignment_of<_TypeT>::value
                      < __rw_alignment_of<_TypeU>::value,
                     _TypeU, _TypeT>::type type;
};

template <class _TypeT>
struct __rw_strictest<_TypeT>
{
    typedef _TypeT type;
};

template <_RWSTD_SIZE_T _Len, class... _Types>
struct __rw_aligned_union;

template <_RWSTD_SIZE_T _Len, class _TypeT, class... _Types>
struct __rw_aligned_union<_Len, _TypeT, _Types...>
{
    typedef typename
    __rw_biggest<_TypeT, _Types...>::type _C_biggest;

    typedef typename
    __rw_strictest<_TypeT, _Types...>::type _C_strictest;

    static const _RWSTD_SIZE_T _C_size_value =
        sizeof (_C_biggest);

    static const _RWSTD_SIZE_T alignment_value =
        __rw_alignment_of<_C_strictest>::value;

    typedef typename
    __rw_aligned_storage<_Len < _C_size_value ? _C_size_value : _Len,
                         alignment_value>::type type;
};

#if 0
#  ifndef _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION

template <_RWSTD_SIZE_T _Len, class... _Types>
const _RWSTD_SIZE_T
__rw_aligned_union<_Len, _Types...>::alignment_value;

template <_RWSTD_SIZE_T _Len, class... _Types>
const _RWSTD_SIZE_T
__rw_aligned_union<_Len, _Types...>::_C_size_value;

#  endif // _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION
#endif // 0

#else // _RWSTD_NO_VARIADIC_TEMPLATES

struct __rw_empty { };

// Helper for __rw_aligned_union. Provides a typedef type that
// is the largest type in the sequence of provided types.
template <class _Type1             , class _Type2 = __rw_empty,
          class _Type3 = __rw_empty, class _Type4 = __rw_empty,
          class _Type5 = __rw_empty, class _Type6 = __rw_empty,
          class _Type7 = __rw_empty, class _Type8 = __rw_empty>
struct __rw_biggest
{
    typedef typename
    __rw_conditional<(sizeof (_Type1) < sizeof (_Type2)),
                     _Type2, _Type1>::type _Type12;

    typedef typename
    __rw_conditional<(sizeof (_Type3) < sizeof (_Type4)),
                     _Type4, _Type3>::type _Type34;

    typedef typename
    __rw_conditional<(sizeof (_Type5) < sizeof (_Type6)),
                     _Type6, _Type5>::type _Type56;

    typedef typename
    __rw_conditional<(sizeof (_Type7) < sizeof (_Type8)),
                     _Type8, _Type7>::type _Type78;

    typedef typename
    __rw_conditional<(sizeof (_Type12) < sizeof (_Type34)),
                     _Type34, _Type12>::type _Type1234;

    typedef typename
    __rw_conditional<(sizeof (_Type56) < sizeof (_Type78)),
                     _Type78, _Type56>::type _Type5678;

    typedef typename
    __rw_conditional<(sizeof (_Type1234) < sizeof (_Type5678)),
                     _Type5678, _Type1234>::type type;
};

// Helper for __rw_aligned_union. Provides a typedef type that
// is the type with the strictest alignment requirement in the
// sequence of provided types.
template <class _Type1             , class _Type2 = __rw_empty,
          class _Type3 = __rw_empty, class _Type4 = __rw_empty,
          class _Type5 = __rw_empty, class _Type6 = __rw_empty,
          class _Type7 = __rw_empty, class _Type8 = __rw_empty>
struct __rw_strictest
{
    typedef typename
    __rw_conditional<   (__rw_alignment_of<_Type1>::value)
                      < (__rw_alignment_of<_Type2>::value),
                     _Type2, _Type1>::type _Type12;

    typedef typename
    __rw_conditional<   (__rw_alignment_of<_Type3>::value)
                      < (__rw_alignment_of<_Type4>::value),
                     _Type4, _Type3>::type _Type34;

    typedef typename
    __rw_conditional<   (__rw_alignment_of<_Type5>::value)
                      < (__rw_alignment_of<_Type6>::value),
                     _Type6, _Type5>::type _Type56;

    typedef typename
    __rw_conditional<   (__rw_alignment_of<_Type7>::value)
                      < (__rw_alignment_of<_Type8>::value),
                     _Type8, _Type7>::type _Type78;

    typedef typename
    __rw_conditional<   (__rw_alignment_of<_Type12>::value)
                      < (__rw_alignment_of<_Type34>::value),
                     _Type34, _Type12>::type _Type1234;

    typedef typename
    __rw_conditional<   (__rw_alignment_of<_Type56>::value)
                      < (__rw_alignment_of<_Type78>::value),
                     _Type78, _Type56>::type _Type5678;

    typedef typename
    __rw_conditional<   (__rw_alignment_of<_Type1234>::value)
                      < (__rw_alignment_of<_Type5678>::value),
                     _Type5678, _Type1234>::type type;
};

template <_RWSTD_SIZE_T _Len,
          class _Type1             , class _Type2 = __rw_empty,
          class _Type3 = __rw_empty, class _Type4 = __rw_empty,
          class _Type5 = __rw_empty, class _Type6 = __rw_empty,
          class _Type7 = __rw_empty, class _Type8 = __rw_empty>
struct __rw_aligned_union
{
    typedef typename
    __rw_biggest<_Type1, _Type2, _Type3, _Type4,
                      _Type5, _Type6, _Type7, _Type8>::type _C_biggest;

    typedef typename
    __rw_strictest<_Type1, _Type2, _Type3, _Type4,
                         _Type5, _Type6, _Type7, _Type8>::type _C_strictest;

    static const _RWSTD_SIZE_T _C_size_value =
        sizeof (_C_biggest);

    static const _RWSTD_SIZE_T alignment_value =
        __rw_alignment_of<_C_strictest>::value;

    typedef typename
    __rw_aligned_storage<_C_size_value < _Len ? _Len : _C_size_value,
                         alignment_value>::type type;
};

#ifndef _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION

template <_RWSTD_SIZE_T _Len,
          class _Type1, class _Type2, class _Type3, class _Type4,
          class _Type5, class _Type6, class _Type7, class _Type8>
const _RWSTD_SIZE_T
__rw_aligned_union<_Len,
                   _Type1, _Type2, _Type3, _Type4,
                   _Type5, _Type6, _Type7, _Type8>::alignment_value;

template <_RWSTD_SIZE_T _Len,
          class _Type1, class _Type2, class _Type3, class _Type4,
          class _Type5, class _Type6, class _Type7, class _Type8>
const _RWSTD_SIZE_T
__rw_aligned_union<_Len,
                   _Type1, _Type2, _Type3, _Type4,
                   _Type5, _Type6, _Type7, _Type8>::_C_size_value;

#endif // _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION

#endif // !_RWSTD_NO_VARIADIC_TEMPLATES



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

