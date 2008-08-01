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
#include <rw/_meta_comp.h>
#include <rw/_meta_arr.h>
#include <rw/_meta_cv.h>

_RWSTD_NAMESPACE (__rw) {

#define _RWSTD_TRAIT_SPEC_0_CV(Trait,Type)                           \
  template <> struct Trait<Type> : __rw_true_type { };               \
  template <> struct Trait<Type const > : __rw_true_type { };        \
  template <> struct Trait<Type volatile> : __rw_true_type { };      \
  template <> struct Trait<Type const volatile> : __rw_true_type { }

template <class _TypeT>
struct __rw_is_const : __rw_false_type
{
};

template <class _TypeT>
struct __rw_is_const<const _TypeT> : __rw_true_type
{
};

template <class _TypeT>
struct __rw_is_volatile : __rw_false_type
{
};

template <class _TypeT>
struct __rw_is_volatile<volatile _TypeT> : __rw_true_type
{
};

#if    defined (__SUNPRO_CC) && (__SUNPRO_CC <= 0x590) \
    || defined (_MSC_VER) && (_MSC_VER <= 1400)

// additional specializations needed for these compilers

template <class _TypeT>
struct __rw_is_const<const _TypeT []> : __rw_true_type
{
};

template <class _TypeT, _RWSTD_SIZE_T _Size>
struct __rw_is_const<const _TypeT [_Size]> : __rw_true_type
{
};

template <class _TypeT>
struct __rw_is_volatile<volatile _TypeT []> : __rw_true_type
{
};

template <class _TypeT, _RWSTD_SIZE_T _Size>
struct __rw_is_volatile<volatile _TypeT [_Size]> : __rw_true_type
{
};

#endif

#define _RWSTD_IS_CONST(T) _RW::__rw_is_const<T>::value
#define _RWSTD_IS_VOLATILE(T) _RW::__rw_is_volatile<T>::value


#if !defined(_RWSTD_TT_IS_POD)

template <class _TypeT>
struct __rw_is_pod_impl
{
    typedef typename __rw_remove_all_extents<_TypeT>::type _TypeU;
    typedef typename __rw_remove_cv<_TypeU>::type _NoCV_TypeU;

    enum { _C_value = __rw_is_scalar<_NoCV_TypeU>::value };
};

#  define _RWSTD_IS_POD(T) _RW::__rw_is_pod_impl<T>::_C_value

#elif defined (__EDG_VERSION__) || defined (_MSC_VER)

template <class _TypeT>
struct __rw_is_pod_impl
{
    typedef typename __rw_remove_all_extents<_TypeT>::type _TypeU;
    typedef typename __rw_remove_cv<_TypeU>::type _NoCV_TypeU;

    // __is_pod works for pod class types only, add scalars
    enum { _C_value =    __rw_is_scalar<_NoCV_TypeU>::value
                      || _RWSTD_TT_IS_POD(_NoCV_TypeU) };
};

#  define _RWSTD_IS_POD(T) _RW::__rw_is_pod_impl<T>::_C_value

#else
#  define _RWSTD_IS_POD(T) _RWSTD_TT_IS_POD(T)
#endif // _RWSTD_TT_IS_POD

template <class _TypeT>
struct __rw_is_pod
    : __rw_integral_constant<bool, _RWSTD_IS_POD(_TypeT)>
{
};


#if !defined (_RWSTD_TT_IS_STANDARD_LAYOUT)
#  define _RWSTD_IS_STANDARD_LAYOUT(T) _RWSTD_IS_POD(T)
#else
#  define _RWSTD_IS_STANDARD_LAYOUT(T) _RWSTD_TT_IS_STANDARD_LAYOUT(T)
#endif // _RWSTD_TT_IS_STANDARD_LAYOUT

template <class _TypeT>
struct __rw_is_standard_layout
    : __rw_integral_constant<bool, _RWSTD_IS_STANDARD_LAYOUT(_TypeT)>
{
};



#if defined (_RWSTD_TT_IS_EMPTY)

#  if defined (__EDG_VERSION__) || defined (_MSC_VER)
#    define _RWSTD_IS_EMPTY(T) \
        (_RWSTD_TT_IS_EMPTY(T) && !_RW::__rw_is_union<T>::value)
#  else
#    define _RWSTD_IS_EMPTY(T) _RWSTD_TT_IS_EMPTY(T)
#  endif

#elif defined (_RWSTD_TT_IS_CLASS) || defined (_RWSTD_TT_IS_UNION)

//
// The primary template is for the case that _TypeT is a class type.
// It checks that the size of a derived class is larger than the size
// of a non-derived class. If the sizes are the same, then _TypeT is
// assumed to be an empty class.
//
template <class _TypeT, bool = __rw_is_class<_TypeT>::value>
struct __rw_is_empty_impl
{
    struct _C_empty_1 : _TypeT { long _C_pad [32]; };
    struct _C_empty_2          { long _C_pad [32]; };

    enum { _C_value = sizeof (_C_empty_1) == sizeof (_C_empty_2) }; 
};

//
// This specialization is for the case that _TypeT is a union or other
// non-class type.
//
template <class _TypeT>
struct __rw_is_empty_impl<_TypeT, false>
{
    enum { _C_value = 0 };
};

#  define _RWSTD_IS_EMPTY(T) _RW::__rw_is_empty_impl<T>::_C_value

#else
   // we have no reliable way to tell if the type is empty,
   // so we assume that it is not.
#  define _RWSTD_IS_EMPTY(T) 0
#endif // !_RWSTD_TT_IS_EMPTY

template <class _TypeT>
struct __rw_is_empty
    : __rw_integral_constant<bool, _RWSTD_IS_EMPTY(_TypeT)>
{
};


#if defined (_RWSTD_TT_IS_POLYMORPHIC)
#  define _RWSTD_IS_POLYMORPHIC(T) _RWSTD_TT_IS_POLYMORPHIC(T)
#elif defined (_RWSTD_TT_IS_CLASS) || defined (_RWSTD_TT_IS_UNION)

//
// The primary template is for the case that _TypeT is class type.
// It checks the size of a derived class against the size of a
// derived class that provides a virtual method. If the size of
// the objects are the same then we assume that _TypeT already
// has a vtbl pointer.
//
template <class _TypeT, bool = __rw_is_class<_TypeT>::value>
struct __rw_is_polymorphic_impl
{
    template <class _TypeU>
    struct _C_type_1 : _TypeU
    {
        // only polymorphic if _TypeT is
    };

    template <class _TypeU>
    struct _C_type_2 : _TypeU
    {
        virtual void _C_method ();
    };

    enum { _C_value =    sizeof (_C_type_1<_TypeT>)
                      == sizeof (_C_type_2<_TypeT>) };
};

//
// This specialization is for the case that _TypeT is not a class type.
//
template <class _TypeT>
struct __rw_is_polymorphic_impl<_TypeT, false>
{
    enum { _C_value = 0 };
};

#  define _RWSTD_IS_POLYMORPHIC(T) \
       _RW::__rw_is_polymorphic_impl<T>::_C_value

#else
#  define _RWSTD_IS_POLYMORPHIC(T) _RW::__rw_is_class_or_union<T>::value
#endif // _RWSTD_TT_IS_POLYMORPHIC

template <class _TypeT>
struct __rw_is_polymorphic
    : __rw_integral_constant<bool, _RWSTD_IS_POLYMORPHIC(_TypeT)>
{
};



#if defined (_RWSTD_TT_IS_ABSTRACT)
#  define _RWSTD_IS_ABSTRACT(T) _RWSTD_TT_IS_ABSTRACT(T)
#elif defined (_RWSTD_TT_IS_CLASS) || defined (_RWSTD_TT_IS_UNION)

//
// The primary template is for the case that _TypeT is class type.
// It checks that _TypeT is an abstract type by exploiting the
// resolution to CWG#337 [http://tinyurl.com/6yltlk]
//
template <class _TypeT, bool = __rw_is_class<_TypeT>::value>
struct __rw_is_abstract_impl
{
    struct _C_no  { };
    struct _C_yes { _C_no __pad[2]; };
    
    template <class U>
    static _C_yes _C_is (int, ...);

    template <class U>
    static _C_no  _C_is (int, U(*)[1]);

    enum { _C_value =    sizeof (_C_yes)
                      == sizeof (_C_is<_TypeT>(0, 0)) };
};

//
// This specialization is for the case that _TypeT is not a class type.
//
template <class T>
struct __rw_is_abstract_impl<T,true>
{
    enum { _C_value = 0 };
};

#  define _RWSTD_IS_ABSTRACT(T) _RW::__rw_is_abstract_impl<T>::_C_value

#else
#  define _RWSTD_IS_ABSTRACT(T) _RW::__rw_is_class_or_union<T>::value
#endif // !_RWSTD_TT_IS_ABSTRACT

template <class _TypeT>
struct __rw_is_abstract
    : __rw_integral_constant<bool, _RWSTD_IS_ABSTRACT(_TypeT)>
{
};



#if !defined (_RWSTD_TT_HAS_TRIVIAL_CTOR)
#  define _RWSTD_HAS_TRIVIAL_CTOR(T) _RW::__rw_is_pod<T>::value
#elif defined (__EDG_VERSION__) || defined (_MSC_VER)

template <class _TypeT>
struct __rw_has_trivial_ctor_impl
{
    typedef typename __rw_remove_all_extents<_TypeT>::type _TypeU;

    enum { _C_value =    __rw_is_scalar<_TypeU>::value
                      || _RWSTD_TT_HAS_TRIVIAL_CTOR (_TypeU) };
};

#  define _RWSTD_HAS_TRIVIAL_CTOR(T) \
     _RW::__rw_has_trivial_ctor_impl<T>::_C_value

#else
#  define _RWSTD_HAS_TRIVIAL_CTOR(T) _RWSTD_TT_HAS_TRIVIAL_CTOR(T)
#endif // _RWSTD_TT_HAS_TRIVIAL_CTOR

template <class _TypeT>
struct __rw_has_trivial_ctor
    : __rw_integral_constant<bool, _RWSTD_HAS_TRIVIAL_CTOR(_TypeT)>
{
};



#if !defined (_RWSTD_TT_HAS_TRIVIAL_COPY)
#  define _RWSTD_HAS_TRIVIAL_COPY(T)         \
    (  !_RW::__rw_is_array<T>::value         \
     && (   _RW::__rw_is_reference<T>::value \
         || _RW::__rw_is_pod<T>::value))
#elif defined (__EDG_VERSION__) || defined (_MSC_VER)
#  define _RWSTD_HAS_TRIVIAL_COPY(T)      \
     (   _RW::__rw_is_reference<T>::value \
      || _RW::__rw_is_scalar<T>::value    \
      || _RWSTD_TT_HAS_TRIVIAL_COPY(T))
#elif defined (__GNUG__)
#  define _RWSTD_HAS_TRIVIAL_COPY(T) \
    (!_RW::__rw_is_array<T>::value && _RWSTD_TT_HAS_TRIVIAL_COPY(T)) 
#else
#  define _RWSTD_HAS_TRIVIAL_COPY(T) _RWSTD_TT_HAS_TRIVIAL_COPY(T)
#endif // !_RWSTD_TT_HAS_TRIVIAL_COPY

template <class _TypeT>
struct __rw_has_trivial_copy
    : __rw_integral_constant<bool, _RWSTD_HAS_TRIVIAL_COPY(_TypeT)>
{
};


#if !defined (_RWSTD_TT_HAS_TRIVIAL_ASSIGN)
#  define _RWSTD_HAS_TRIVIAL_ASSIGN(T)    \
    (    _RW::__rw_is_pod<T>::value       \
     && !_RW::__rw_is_const<T>::value     \
     && !_RW::__rw_is_reference<T>::value \
     && !_RW::__rw_is_array<T>::value)
#elif defined (__EDG_VERSION__)
#  define _RWSTD_HAS_TRIVIAL_ASSIGN(T) \
        (!_RW::__rw_is_array<T>::value \
      && !_RW::__rw_is_const<T>::value \
      && !_RW::__rw_is_reference<T>::value \
      && (_RW::__rw_is_scalar<T>::value || _RWSTD_TT_HAS_TRIVIAL_ASSIGN(T)))
#elif defined (__GNUG__)
#  define _RWSTD_HAS_TRIVIAL_ASSIGN(T) \
    (!_RW::__rw_is_array<T>::value && _RWSTD_TT_HAS_TRIVIAL_ASSIGN(T))
#elif defined (_MSC_VER)
#  define _RWSTD_HAS_TRIVIAL_ASSIGN(T) \
        (!_RW::__rw_is_const<T>::value \
      && (_RW::__rw_is_scalar<T>::value || _RWSTD_TT_HAS_TRIVIAL_ASSIGN(T)))
#else
#  define _RWSTD_HAS_TRIVIAL_ASSIGN(T) _RWSTD_TT_HAS_TRIVIAL_ASSIGN(T)
#endif // _RWSTD_TT_HAS_TRIVIAL_ASSIGN

template <class _TypeT>
struct __rw_has_trivial_assign
    : __rw_integral_constant<bool, _RWSTD_HAS_TRIVIAL_ASSIGN(_TypeT)>
{
};



#if !defined (_RWSTD_TT_HAS_TRIVIAL_DTOR)
#  define _RWSTD_HAS_TRIVIAL_DTOR(T) \
    (_RW::__rw_is_pod<T>::value || _RW::__rw_is_reference<T>::value)
#elif defined (__GNUC__)
#  define _RWSTD_HAS_TRIVIAL_DTOR(T)         \
    (  _RW::__rw_is_reference<_TypeT>::value \
     || _RWSTD_TT_HAS_TRIVIAL_DTOR (_TypeT))
#elif defined (__EDG_VERSION__) || defined (_MSC_VER)

template <class _TypeT>
struct __rw_has_trivial_dtor_impl
{
    typedef typename __rw_remove_all_extents<_TypeT>::type _TypeU;

    enum { _C_value =    __rw_is_scalar<_TypeU>::value
                      || __rw_is_reference<_TypeU>::value
                      || _RWSTD_TT_HAS_TRIVIAL_DTOR (_TypeU) };
};

#  define _RWSTD_HAS_TRIVIAL_DTOR(T) \
    _RW::__rw_has_trivial_dtor_impl<T>::_C_value

#else
#  define _RWSTD_HAS_TRIVIAL_DTOR(T) _RWSTD_TT_HAS_TRIVIAL_DTOR(T)
#endif // !_RWSTD_TT_HAS_TRIVIAL_DTOR

template <class _TypeT>
struct __rw_has_trivial_dtor
    : __rw_integral_constant<bool, _RWSTD_HAS_TRIVIAL_DTOR(_TypeT)>
{
};



template <class _TypeT>
struct __rw_is_trivial_impl
{
    typedef typename __rw_remove_all_extents<_TypeT>::type _TypeU;
    typedef typename __rw_remove_cv<_TypeU>::type _NoCV_TypeU;

    enum { _C_value =    __rw_is_scalar<_NoCV_TypeU>::value
                      || __rw_has_trivial_ctor<_NoCV_TypeU>::value
                      && __rw_has_trivial_copy<_NoCV_TypeU>::value
                      && __rw_has_trivial_dtor<_NoCV_TypeU>::value
                      && __rw_has_trivial_assign<_NoCV_TypeU>::value
                      && (   __rw_is_class<_NoCV_TypeU>::value
                          || __rw_is_union<_NoCV_TypeU>::value) };
};

template <class _TypeT>
struct __rw_is_trivial
    : __rw_integral_constant<bool, __rw_is_trivial_impl<_TypeT>::_C_value>
{
};

#define _RWSTD_IS_TRIVIAL(T) _RW::__rw_is_trivial<T>::value



#if !defined (_RWSTD_TT_HAS_NOTHROW_CTOR)
#  define _RWSTD_HAS_NOTHROW_CTOR(T) _RW::__rw_has_trivial_ctor<T>::value
#elif defined (__EDG_VERSION__) || defined (_MSC_VER)

template <class _TypeT>
struct __rw_has_nothrow_ctor_impl
{
    typedef typename __rw_remove_all_extents<_TypeT>::type _TypeU;

    enum { _C_value =    __rw_has_trivial_ctor<_TypeT>::value
                      || _RWSTD_TT_HAS_NOTHROW_CTOR (_TypeU) };
};

#  define _RWSTD_HAS_NOTHROW_CTOR(T) \
     _RW::__rw_has_nothrow_ctor_impl<T>::_C_value

#else
#  define _RWSTD_HAS_NOTHROW_CTOR(T) _RWSTD_TT_HAS_NOTHROW_CTOR(T)
#endif // _RWSTD_TT_HAS_NOTHROW_CTOR

template <class _TypeT>
struct __rw_has_nothrow_ctor
    : __rw_integral_constant<bool, _RWSTD_HAS_NOTHROW_CTOR(_TypeT)>
{
};


#if !defined (_RWSTD_TT_HAS_NOTHROW_COPY)
#  define _RWSTD_HAS_NOTHROW_COPY(T) _RW::__rw_has_trivial_copy<T>::value
#elif defined (__EDG_VERSION__) || defined (_MSC_VER)
#  define _RWSTD_HAS_NOTHROW_COPY(T) \
    (_RW::__rw_has_trivial_copy<T>::value || _RWSTD_TT_HAS_NOTHROW_COPY(T))
#elif defined (__GNUG__)
#  define _RWSTD_HAS_NOTHROW_COPY(T) \
    (!_RW::__rw_is_array<T>::value && _RWSTD_TT_HAS_NOTHROW_COPY(T))
#else
#  define _RWSTD_HAS_NOTHROW_COPY(T) _RWSTD_TT_HAS_NOTHROW_COPY(T)
#endif // !_RWSTD_TT_HAS_NOTHROW_COPY

template <class _TypeT>
struct __rw_has_nothrow_copy
    : __rw_integral_constant<bool, _RWSTD_HAS_NOTHROW_COPY(_TypeT)>
{
};



#if !defined (_RWSTD_TT_HAS_NOTHROW_ASSIGN)
#  define _RWSTD_HAS_NOTHROW_ASSIGN(T) \
    _RW::__rw_has_trivial_assign<T>::value
#elif defined (__EDG_VERSION__)
#  define _RWSTD_HAS_NOTHROW_ASSIGN(T) \
      (!_RW::__rw_is_const<T>::value \
    && !_RW::__rw_is_reference<T>::value \
    && (   _RW::__rw_has_trivial_assign<T>::value \
        || _RWSTD_TT_HAS_NOTHROW_ASSIGN(T)))
#elif defined (__GNUG__)
#  define _RWSTD_HAS_NOTHROW_ASSIGN(T) \
      (  !_RW::__rw_is_array<T>::value \
       && _RWSTD_TT_HAS_NOTHROW_ASSIGN(T))
#elif defined (_MSC_VER)
#  define _RWSTD_HAS_NOTHROW_ASSIGN(T) \
      (!_RW::__rw_is_const<T>::value \
    && (   _RW::__rw_has_trivial_assign<T>::value \
        || _RWSTD_TT_HAS_NOTHROW_ASSIGN(T)))
#else
#  define _RWSTD_HAS_NOTHROW_ASSIGN(T) _RWSTD_TT_HAS_NOTHROW_ASSIGN(T)
#endif // !_RWSTD_TT_HAS_NOTHROW_ASSIGN

template <class _TypeT>
struct __rw_has_nothrow_assign
    : __rw_integral_constant<bool, _RWSTD_HAS_NOTHROW_ASSIGN(_TypeT)>
{
};


#if !defined (_RWSTD_TT_HAS_VIRTUAL_DTOR)
#  define _RWSTD_HAS_VIRTUAL_DTOR(T) _RW::__rw_is_polymorphic<T>::value
#else
#  define _RWSTD_HAS_VIRTUAL_DTOR(T) _RWSTD_TT_HAS_VIRTUAL_DTOR(T)
#endif // _RWSTD_TT_HAS_VIRTUAL_DTOR

template <class _TypeT>
struct __rw_has_virtual_dtor
    : __rw_integral_constant<bool, _RWSTD_HAS_VIRTUAL_DTOR(_TypeT)>
{
};


template <class _TypeT>
struct __rw_is_unsigned : __rw_false_type
{
};

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

#if 0
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_unsigned, char16_t);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_unsigned, char32_t);
#endif

_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed, float);
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed, double);

#ifndef _RWSTD_NO_LONG_DOUBLE
_RWSTD_TRAIT_SPEC_0_CV(__rw_is_signed, long double);
#endif   // _RWSTD_NO_LONG_DOUBLE

#define _RWSTD_IS_SIGNED(T)   _RW::__rw_is_signed<T>::value
#define _RWSTD_IS_UNSIGNED(T) _RW::__rw_is_unsigned<T>::value



#ifndef _RWSTD_NO_ALIGN_TRAITS

#  if defined (_RWSTD_TT_ALIGN_OF)
#    define _RWSTD_ALIGN_OF(T) _RWSTD_TT_ALIGN_OF(T)
#  else

template <class _TypeT>
struct __rw_alignment_of_impl
{
    template <class _TypeU>
    struct _C_nest
    {
        unsigned char _C_char;
        _TypeU _C_type;
    };

    enum { _C_value =
       sizeof (_C_nest<_TypeT>) - sizeof (_TypeT) < sizeof (_TypeT)
     ? sizeof (_C_nest<_TypeT>) - sizeof (_TypeT) < sizeof (_TypeT)
     : sizeof (_TypeT) };
};

template <>
struct __rw_alignment_of_impl<void>
{
    enum { _C_value = 0 };
};

template <>
struct __rw_alignment_of_impl<const void>
{
    enum { _C_value = 0 };
};

template <>
struct __rw_alignment_of_impl<volatile void>
{
    enum { _C_value = 0 };
};

template <>
struct __rw_alignment_of_impl<const volatile void>
{
    enum { _C_value = 0 };
};

#    define _RWSTD_ALIGN_OF(T) _RW::__rw_alignment_of_impl<T>::_C_value
#  endif // !_RWSTD_TT_ALIGN_OF

template <class _TypeT>
struct __rw_alignment_of
    : __rw_integral_constant<_RWSTD_SIZE_T, _RWSTD_ALIGN_OF(_TypeT)>
{
};

#endif // !_RWSTD_NO_ALIGN_TRAITS

template <class _TypeT>
struct __rw_rank
    : __rw_integral_constant<_RWSTD_SIZE_T, 0>
{
};

template <class _TypeT, _RWSTD_SIZE_T _Size>
struct __rw_rank<_TypeT [_Size]>
    : __rw_integral_constant<_RWSTD_SIZE_T, 1 + __rw_rank<_TypeT>::value>
{
};

#if !defined (__IBMCPP__) || (900 < __IBMCPP__)

template <class _TypeT>
struct __rw_rank<_TypeT []>
    : __rw_integral_constant<_RWSTD_SIZE_T, 1 + __rw_rank<_TypeT>::value>
{
};

#endif

#define _RWSTD_RANK(T) _RW::__rw_rank<T>::value



template <class _TypeT, _RWSTD_SIZE_T _Depth>
struct __rw_extent
    : __rw_integral_constant<_RWSTD_SIZE_T, 0>
{
};

template <class _TypeT, _RWSTD_SIZE_T _Size, _RWSTD_SIZE_T _Depth>
struct __rw_extent<_TypeT [_Size], _Depth>
    : __rw_integral_constant<_RWSTD_SIZE_T,
                             __rw_extent<_TypeT, _Depth - 1>::value>
{
};

#if !defined (__IBMCPP__) || (900 < __IBMCPP__)

template <class _TypeT, _RWSTD_SIZE_T _Depth>
struct __rw_extent<_TypeT [], _Depth>
    : __rw_integral_constant<_RWSTD_SIZE_T,
                             __rw_extent<_TypeT, _Depth - 1>::value>
{
};

#endif

template <class _TypeT, _RWSTD_SIZE_T _Size>
struct __rw_extent<_TypeT [_Size], 0>
    : __rw_integral_constant<_RWSTD_SIZE_T, _Size>
{
};

#if !defined (__IBMCPP__) || (900 < __IBMCPP__)

template <class _TypeT>
struct __rw_extent<_TypeT [], 0>
    : __rw_integral_constant<_RWSTD_SIZE_T, 0>
{
};

#endif

#define _RWSTD_EXTENT(T,D) _RW::__rw_extent<T,D>::value

} // namespace __rw


#endif   // _RWSTD_RW_META_PROP_H_INCLUDED

