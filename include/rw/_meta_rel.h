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

#ifndef _RWSTD_RW_META_REL_H_INCLUDED
#define _RWSTD_RW_META_REL_H_INCLUDED

#include <rw/_defs.h>

#ifndef _RWSTD_TT_IS_CONVERTIBLE
#  include <rw/_meta_arr.h>
#  include <rw/_meta_ptr.h>
#  include <rw/_meta_ref.h>
#  include <rw/_meta_cat.h>
#  include <rw/_meta_cv.h>
#endif // _RWSTD_TT_IS_CONVERTIBLE

#ifndef _RWSTD_TT_IS_BASE_OF
#  include <rw/_meta_cv.h>
#  include <rw/_meta_ptr.h>
#endif //_RWSTD_TT_IS_BASE_OF

_RWSTD_NAMESPACE (__rw) {

template <class _TypeT, class _TypeU>
struct __rw_is_same : __rw_false_type
{
};

template <class _TypeT>
struct __rw_is_same<_TypeT, _TypeT> : __rw_true_type
{
};

#define _RWSTD_IS_SAME(T,U) _RW::__rw_is_same<T,U>::value


#ifndef _RWSTD_TT_IS_BASE_OF

//
// Primary template  handles case that either of _Base or _Derived
// is not a class type.
//
template <class _Base, class _Derived,
          bool =   __rw_is_class<_Base>::value
                && __rw_is_class<_Derived>::value>
struct __rw_is_base_of_impl
{
    enum { _C_value = 0 };
};

//
// This specialization is for the case that _Base and
// _Derived are class types, but not the same type.
//
// This little gem was taken from a comp.lang.c++.moderated post
// by Rani Sharoni [see http://tinyurl.com/6pdv3k]
//
template <class _Base, class _Derived>
struct __rw_is_base_of_impl<_Base, _Derived, true>
{
    struct _C_no  { };
    struct _C_yes { _C_no __pad [2]; };

    struct _C_nest
    {
        operator const volatile _Base&    () const;
        operator const volatile _Derived& ();

        // the template is used so that the compiler will prefer the
        // non-template _C_is, in case that the conversion would be
        // ambiguous (as it is in the case where the types are unrelated).
        template <class _TypeT>
        static _C_yes _C_is (const volatile _Derived&, _TypeT);
        static _C_no _C_is  (const volatile _Base&   , int);
    };

    enum { _C_value = 
           _RW::__rw_is_same<const volatile _Base,
                             const volatile _Derived&>::value
        || sizeof (_C_yes) == sizeof (_C_nest::_C_is (_C_nest (), 0))
    };
};

//
// This specialization is for the case that _Base and
// _Derived are the same class type.
//
template <class _TypeT>
struct __rw_is_base_of_impl<_TypeT, _TypeT, true>
{
    enum { _C_value = 1 };
};

#  define _RWSTD_IS_BASE_OF(T,U) \
     _RW::__rw_is_base_of_impl<T,U>::_C_value

#elif defined (_MSC_VER)
#  define _RWSTD_IS_BASE_OF(T,U)     \
    (   _RW::__rw_is_class<T>::value \
     && _RW::__rw_is_class<U>::value \
     && _RWSTD_TT_IS_BASE_OF(T,U))
#else
#  define _RWSTD_IS_BASE_OF(T,U) _RWSTD_TT_IS_BASE_OF(T,U)
#endif // _RWSTD_TT_IS_BASE_OF

template <class _Base, class _Derived>
struct __rw_is_base_of
    : __rw_integral_constant<bool, _RWSTD_IS_BASE_OF(_Base,_Derived)>
{
    //_RWSTD_ASSERT (    _RWSTD_IS_CLASS (_Base)
    //               &&  _RWSTD_IS_CLASS (_Derived)
    //               && !_RWSTD_IS_SAME(_Base, _Derived)
    //               ||
};


#if !defined (_RWSTD_TT_IS_CONVERTIBLE)

template <class _From, class _To>
struct __rw_is_convertible_impl
{
    struct _C_no  { };
    struct _C_yes { _C_no __pad [2]; };

    struct _Dummy
    {
        template <class _Anything>
        _Dummy (_Anything);
    };

    template <class _TypeT>
    static _C_no _C_is (_Dummy, _TypeT);
    static _C_yes _C_is (_To, int);

    static _From _C_make ();

    enum { _C_value = sizeof (_C_yes) == sizeof (_C_is (_C_make (), 0)) };
};

template <class _TypeT, class _TypeU,
          bool = __rw_is_array<_TypeT>::value,
          bool = __rw_is_function<_TypeT>::value>
struct __rw_is_convertible_3
{
    // _TypeT is neither an array nor a function type, so just do a
    // straight conversion test
    enum { _C_value = __rw_is_convertible_impl<_TypeT, _TypeU>::_C_value };
};

template <class _TypeT, class _TypeU>
struct __rw_is_convertible_3<_TypeT, _TypeU, true, false>
{
    // _TypeT is an array type, see if we can convert it to a _TypeU*
    typedef typename __rw_remove_extent<_TypeT>::type _TypeV;
    typedef typename __rw_add_pointer<_TypeV>::type _TypeT_Ptr;

    enum { _C_value =
        __rw_is_convertible_impl<_TypeT_Ptr, _TypeU>::_C_value };
};

template <class _TypeT, class _TypeU>
struct __rw_is_convertible_3<_TypeT, _TypeU, false, true>
{
    // _TypeT is an function type, try to convert to reference or pointer
    typedef typename __rw_add_lvalue_reference<_TypeT>::type _TypeT_Ref;
    typedef typename __rw_add_pointer<_TypeT>::type _TypeT_Ptr;

    enum { _C_value =
           __rw_is_convertible_impl<_TypeT_Ref, _TypeU>::_C_value
        || __rw_is_convertible_impl<_TypeT_Ptr, _TypeU>::_C_value };
};
        
template <class _TypeT, class _TypeU,
          bool =    __rw_is_array<_TypeU>::value
                 || __rw_is_function<_TypeU>::value>
struct __rw_is_convertible_2
{
    // _TypeU is neither an array nor a function type
    enum { _C_value = __rw_is_convertible_3<_TypeT, _TypeU>::_C_value };
};

template <class _TypeT, class _TypeU>
struct __rw_is_convertible_2<_TypeT, _TypeU, true>
{
    // _TypeU is either an array or a function type, and we can't convert to
    // either of those.
    enum { _C_value = 0 };
};
        
template <class _TypeT, class _TypeU, bool = __rw_is_void<_TypeT>::value,
                                      bool = __rw_is_void<_TypeU>::value>
struct __rw_is_convertible_1
{
    // void-to-non-void or non-void to void
    enum { _C_value = 0 };
};

template <class _TypeT, class _TypeU>
struct __rw_is_convertible_1<_TypeT, _TypeU, false, false>
{
    // neither is _TypeT or _TypeU is (possibly cv-qualified) void
    enum { _C_value = __rw_is_convertible_2<_TypeT,_TypeU>::_C_value };
};

template <class _TypeT, class _TypeU>
struct __rw_is_convertible_1<_TypeT, _TypeU, true, true>
{
    // both _TypeT and _TypeU are (possibly cv-qualified) void
    enum { _C_value = 1 };
};

#  define _RWSTD_IS_CONVERTIBLE(T,U) \
     _RW::__rw_is_convertible_1<T,U>::_C_value

#elif defined (_MSC_VER)

template <class _TypeT, class _TypeU>
struct __rw_is_convertible_1
{
    enum { _C_value =
         __rw_is_void<_TypeT>::value && __rw_is_void<_TypeU>::value
      || _RWSTD_TT_IS_CONVERTIBLE(_TypeT, _TypeU) };
};

#  define _RWSTD_IS_CONVERTIBLE(T,U) \
     _RW::__rw_is_convertible_1<T,U>::_C_value

#else
#  define _RWSTD_IS_CONVERTIBLE(T,U) _RWSTD_TT_IS_CONVERTIBLE(T,U)
#endif // _RWSTD_TT_IS_CONVERTIBLE

template <class _TypeT, class _TypeU>
struct __rw_is_convertible
    : __rw_integral_constant<bool, _RWSTD_IS_CONVERTIBLE(_TypeT,_TypeU)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

} // namespace __rw


#endif   // _RWSTD_RW_META_REL_H_INCLUDED

