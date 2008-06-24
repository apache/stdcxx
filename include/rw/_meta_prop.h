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

#ifndef _RWSTD_RW_META_PROP_H_INCLUDED
#define _RWSTD_RW_META_PROP_H_INCLUDED

#include <rw/_defs.h>
#include <rw/_meta_cat.h>
#include <rw/_meta_arr.h>
#include <rw/_meta_cv.h>
#include <rw/_static_assert.h>

_RWSTD_NAMESPACE (__rw) {

#define _RWSTD_TRAIT_SPEC_0_CV(Trait,Type)                           \
  template <> struct Trait<Type> : __rw_true_type { };               \
  template <> struct Trait<Type const > : __rw_true_type { };        \
  template <> struct Trait<Type volatile> : __rw_true_type { };      \
  template <> struct Trait<Type const volatile> : __rw_true_type { }

/**
 * UnaryTypeTrait indicates that _TypeT is const-qualified.
 */
template <class _TypeT>
struct __rw_is_const : __rw_false_type
{
};

template <class _TypeT>
struct __rw_is_const<const _TypeT> : __rw_true_type
{
};

#define _RWSTD_IS_CONST(T) _RW::__rw_is_const<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT is volatile-qualified.
 */
template <class _TypeT>
struct __rw_is_volatile : __rw_false_type
{
};

template <class _TypeT>
struct __rw_is_volatile<volatile _TypeT> : __rw_true_type
{
};

#define _RWSTD_IS_VOLATILE(T) _RW::__rw_is_volatile<T>::value


template <class _TypeT>
struct __rw_is_standard_layout_impl
{
    typedef typename __rw_remove_all_extents<_TypeT>::type _TypeU;
    enum { _C_value = __rw_is_scalar<_TypeU>::value };
};

template <class _TypeT>
struct __rw_is_pod_impl
{
    typedef typename __rw_remove_all_extents<_TypeT>::type _TypeU;

    enum { _C_value =    __rw_is_scalar<_TypeU>::value
#ifdef _RWSTD_TT_IS_POD
                      || _RWSTD_TT_IS_POD(_TypeU)
#endif
    };
};

template <class _TypeT>
struct __rw_is_empty_impl
{
    enum { _C_value =    __rw_is_class<_TypeT>::value
#ifdef _RWSTD_TT_IS_EMPTY
                      && _RWSTD_TT_IS_EMPTY(_TypeT)
#endif
    };
};

#undef _RWSTD_TT_IS_STDANDARD_LAYOUT
#define _RWSTD_TT_IS_STDANDARD_LAYOUT(T) __rw_is_standard_layout_impl<T>::_C_value

#ifdef _MSC_VER

#  undef _RWSTD_TT_IS_POD
#  define _RWSTD_TT_IS_POD(T) __rw_is_pod_impl<T>::_C_value

#  undef _RWSTD_TT_IS_EMPTY
#  define _RWSTD_TT_IS_EMPTY(T) __rw_is_empty_impl<T>::_C_value

#endif   // _MSC_VER



/**
 * UnaryTypeTrait indicates that _TypeT is a standard layout type.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified)
 */
template <class _TypeT>
struct __rw_is_standard_layout
    : __rw_integral_constant<bool, _RWSTD_TT_IS_STDANDARD_LAYOUT(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_IS_STANDARD_LAYOUT(T) _RW::__rw_is_standard_layout<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT is a pod type.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified)
 */
template <class _TypeT>
struct __rw_is_pod
    : __rw_integral_constant<bool, _RWSTD_TT_IS_POD(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_IS_POD(T) _RW::__rw_is_pod<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT is an empty type.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT>
struct __rw_is_empty
    : __rw_integral_constant<bool, _RWSTD_TT_IS_EMPTY(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_IS_EMPTY(T) _RW::__rw_is_empty<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT is a polymorphic type.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT>
struct __rw_is_polymorphic
    : __rw_integral_constant<bool, _RWSTD_TT_IS_POLYMORPHIC(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_IS_POLYMORPHIC(T) _RW::__rw_is_polymorphic<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT is an abstract type.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT>
struct __rw_is_abstract
    : __rw_integral_constant<bool, _RWSTD_TT_IS_ABSTRACT(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_IS_ABSTRACT(T) _RW::__rw_is_abstract<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT has a trivial default constructor.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT>
struct __rw_has_trivial_ctor
    : __rw_integral_constant<bool, _RWSTD_TT_HAS_TRIVIAL_CTOR(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_HAS_TRIVIAL_CTOR(T) _RW::__rw_has_trivial_ctor<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT has a trivial copy constructor.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT>
struct __rw_has_trivial_copy
    : __rw_integral_constant<bool, _RWSTD_TT_HAS_TRIVIAL_COPY(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_HAS_TRIVIAL_COPY(T) _RW::__rw_has_trivial_copy<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT has a trivial assignment operator.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT>
struct __rw_has_trivial_assign
    : __rw_integral_constant<bool, _RWSTD_TT_HAS_TRIVIAL_ASSIGN(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_HAS_TRIVIAL_ASSIGN(T) _RW::__rw_has_trivial_assign<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT has a trivial destructor.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT>
struct __rw_has_trivial_dtor
    : __rw_integral_constant<bool, _RWSTD_TT_HAS_TRIVIAL_DTOR(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_HAS_TRIVIAL_DTOR(T) _RW::__rw_has_trivial_dtor<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT is a trivial type or
 * an array of trivial type.
 */
template <class _TypeT>
struct __rw_is_trivial_impl
{
    typedef typename __rw_remove_cv<_TypeT>::type _TypeU;
    typedef typename __rw_remove_all_extents<_TypeU>::type _TypeV;

    enum { _C_value =    __rw_is_scalar<_TypeV>::value
                      || __rw_has_trivial_ctor<_TypeV>::value
                      && __rw_has_trivial_copy<_TypeV>::value
                      && __rw_has_trivial_dtor<_TypeV>::value
                      && __rw_has_trivial_assign<_TypeV>::value
                      && (__rw_is_class<_TypeV>::value || __rw_is_union<_TypeV>::value) };
};

/**
 * UnaryTypeTrait indicates that _TypeT is a trivial type or an array of
 * trivial type.
 *
 * Trivial types include scalar types, and trivial class types. A trivial
 * class type has a trivial default ctor, copy ctor, operator= and dtor.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT>
struct __rw_is_trivial
    : __rw_integral_constant<bool, __rw_is_trivial_impl<_TypeT>::_C_value>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_IS_TRIVIAL(T) _RW::__rw_is_trivial<T>::value

/**
 * UnaryTypeTrait to determine if _TypeT has a nothrow default constructor.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT>
struct __rw_has_nothrow_ctor
    : __rw_integral_constant<bool, _RWSTD_TT_HAS_NOTHROW_CTOR(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_HAS_NOTHROW_CTOR(T) _RW::__rw_has_nothrow_ctor<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT has a nothrow copy constructor.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT>
struct __rw_has_nothrow_copy
    : __rw_integral_constant<bool, _RWSTD_TT_HAS_NOTHROW_COPY(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_HAS_NOTHROW_COPY(T) _RW::__rw_has_nothrow_copy<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT has a nothrow assignment operator.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT>
struct __rw_has_nothrow_assign
    : __rw_integral_constant<bool, _RWSTD_TT_HAS_NOTHROW_ASSIGN(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_HAS_NOTHROW_ASSIGN(T) _RW::__rw_has_nothrow_assign<T>::value


/**
 * UnaryTypeTrait indicates that _TypeT has a virtual destructor.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT>
struct __rw_has_virtual_dtor
    : __rw_integral_constant<bool, _RWSTD_TT_HAS_VIRTUAL_DTOR(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_HAS_VIRTUAL_DTOR(T) _RW::__rw_has_virtual_dtor<T>::value


/**
 * UnaryTypeTrait that indicates that _TypeT is an unsigned type.
 */
template <class _TypeT>
struct __rw_is_unsigned : __rw_false_type
{
};

/**
 * UnaryTypeTrait that indicates that _TypeT is a signed type.
 */
template <class _TypeT>
struct __rw_is_signed : __rw_false_type
{
};

#if (_RWSTD_CHAR_MIN < 0)
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed,   char);
#else
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_unsigned, char);
#endif

#ifndef _RWSTD_NO_NATIVE_WCHAR_T
#  if (_RWSTD_WCHAR_MIN < 0)
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed,   wchar_t);
#  else
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_unsigned, wchar_t);
#  endif
#endif   // _RWSTD_NO_NATIVE_WCHAR_T

_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed,     signed char);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_unsigned, unsigned char);

_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed,     signed short);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_unsigned, unsigned short);

_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed,     signed int);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_unsigned, unsigned int);

_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed,     signed long);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_unsigned, unsigned long);

#ifndef _RWSTD_NO_LONG_LONG
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed,     signed long long);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_unsigned, unsigned long long);
#endif   // _RWSTD_NO_LONG_LONG

_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed,     float);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed,     double);

#ifndef _RWSTD_NO_LONG_DOUBLE
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed,     long double);
#endif   // _RWSTD_NO_LONG_DOUBLE

#define _RWSTD_IS_SIGNED(T)   _RW::__rw_is_signed<T>::value
#define _RWSTD_IS_UNSIGNED(T) _RW::__rw_is_unsigned<T>::value


/**
 * UnaryTypeTrait that gets the alignment of _TypeT.
 */
template <class _TypeT>
struct __rw_alignment_of
    : __rw_integral_constant<_RWSTD_SIZE_T, _RWSTD_TT_ALIGN_OF(_TypeT)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

#define _RWSTD_ALIGNMENT_OF(T) _RW::__rw_alignment_of<T>::value

/**
 * UnaryTypeTrait gives the number of dimensions of the type _TypeT, if
 * _TypeT is an array, otherwise 0. The primary template is for non-array
 * types.
 */
template <class _TypeT>
struct __rw_rank
    : __rw_integral_constant<_RWSTD_SIZE_T, 0>
{
};

/**
 * UnaryTypeTrait gives the number of dimensions of the type _TypeT, if
 * _TypeT is an array, otherwise 0. The primary template is for bounded
 * -array types.
 */
template <class _TypeT, _RWSTD_SIZE_T _Size>
struct __rw_rank<_TypeT [_Size]>
    : __rw_integral_constant<_RWSTD_SIZE_T, 1 + __rw_rank<_TypeT>::value>
{
};

/**
 * UnaryTypeTrait gives the number of dimensions of the type _TypeT, if
 * _TypeT is an array, otherwise 0. The primary template is for unbounded
 * -array types.
 */
template <class _TypeT>
struct __rw_rank<_TypeT []>
    : __rw_integral_constant<_RWSTD_SIZE_T, 1 + __rw_rank<_TypeT>::value>
{
};

#define _RWSTD_RANK(T) _RW::__rw_rank<T>::value


/**
 * UnaryTypeTrait gives the size of the _Depth dimension of _TypeT.
 */
template <class _TypeT, _RWSTD_SIZE_T _Depth>
struct __rw_extent
    : __rw_integral_constant<_RWSTD_SIZE_T, 0>
{
};

template <class _TypeT, _RWSTD_SIZE_T _Size, _RWSTD_SIZE_T _Depth>
struct __rw_extent<_TypeT [_Size], _Depth>
    : __rw_integral_constant<_RWSTD_SIZE_T, __rw_extent<_TypeT, _Depth - 1>::value>
{
};

template <class _TypeT, _RWSTD_SIZE_T _Depth>
struct __rw_extent<_TypeT [], _Depth>
    : __rw_integral_constant<_RWSTD_SIZE_T, __rw_extent<_TypeT, _Depth - 1>::value>
{
};

template <class _TypeT, _RWSTD_SIZE_T _Size>
struct __rw_extent<_TypeT [_Size], 0>
    : __rw_integral_constant<_RWSTD_SIZE_T, _Size>
{
};

template <class _TypeT>
struct __rw_extent<_TypeT [], 0>
    : __rw_integral_constant<_RWSTD_SIZE_T, 0>
{
};

#define _RWSTD_EXTENT_1(T)   _RW::__rw_extent<T>::value
#define _RWSTD_EXTENT_2(T,D) _RW::__rw_extent<T,D>::value

} // namespace __rw


#endif   // _RWSTD_RW_META_PROP_H_INCLUDED

