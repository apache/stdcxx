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

#ifndef _RWSTD_META_REL_H_INCLUDED
#define _RWSTD_META_REL_H_INCLUDED

#include <rw/_defs.h>

//#include <rw/_meta_ptr.h>
//#include <rw/_meta_ref.h>
//#include <rw/_meta_cat.h>
//#include <rw/_meta_cv.h>

_RWSTD_NAMESPACE (__rw) {

/**
 * UnaryTypeTrait indicates that the given types _TypeT and _TypeU are
 * the same type, including cv-qualifiers.
 *
 * The primary template is for the case that _TypeT and _TypeU are indeed
 * different types.
 */
template <class _TypeT, class _TypeU>
struct __rw_is_same : __rw_false_type
{
};

/**
 * UnaryTypeTrait indicates that the given types _TypeT and _TypeU are
 * the same type, including cv-qualifiers.
 *
 * This specialization is for the case that _TypeT and _TypeU are exactly
 * the same types.
 */
template <class _TypeT>
struct __rw_is_same<_TypeT, _TypeT> : __rw_true_type
{
};

//#define _RWSTD_IS_SAME(T,U)                                           \
//    __rw_is_same<T,U>::value

/**
 * BinaryTypeTrait indicates that _TypeT is a base class of _TypeU
 * or _TypeT and _TypeU are not unions and name the same class type,
 * without regard to cv-qualifiers.
 *
 * If _TypeT and _TypeU are class types and are different types, without
 * regard to cv-qualifiers, then _TypeT shall be a complete type.
 */
template <class _TypeT, class _TypeU>
struct __rw_is_base_of
    : __rw_integral_constant<bool, _RWSTD_TT_IS_BASE_OF(_TypeT,_TypeU)>
{
    //_RWSTD_ASSERT (    _RWSTD_IS_CLASS (_TypeT)
    //               &&  _RWSTD_IS_CLASS (_TypeU)
    //               && !_RWSTD_IS_SAME(_TypeT, _TypeU)
    //               ||
};

//#define _RWSTD_IS_BASE_OF(T,U)                                        \
//    __rw_is_base_of<T,U>::value


/**
 * Primitive type trait tells us if we can create a _TypeU from a _TypeT.
 */
template <class _TypeT, class _TypeU>
struct __rw_is_convertible_impl
{
    struct _C_no  { };
    struct _C_yes { _C_no __pad [2]; };

    static _C_yes _C_is (int, _TypeU);
    static _C_no  _C_is (int, ...);

    static _TypeT _C_make ();

    enum { _C_value = sizeof (_C_yes) == sizeof (_C_is (0, _C_make ())) };
};






//template <class _TypeT, class _TypeU, bool = __rw_is_array<_TypeT>::value,
//                                      bool = __rw_is_function<_TypeT>::value>
//struct __rw_is_convertible_3
//{
//    // _TypeT is neither an array nor a function type, so just do a
//    // straight conversion test
//    enum { _C_value = __rw_is_convertible_impl<_TypeT, _TypeU>::_C_value };
//};
//
//template <class _TypeT, class _TypeU>
//struct __rw_is_convertible_3<_TypeT,_TypeU,true,false>
//{
//    // _TypeT is an array type, see if we can convert it to a _TypeU*
//    typedef _TYPENAME __rw_remove_extent<_TypeT>::type _TypeV;
//    typedef _TYPENAME __rw_add_pointer<_TypeV>::type _TypeT_Ptr;
//
//    enum { _C_value = __rw_is_convertible_impl<_TypeT_Ptr, _TypeU>::_C_value };
//};
//
//template <class _TypeT, class _TypeU>
//struct __rw_is_convertible_3<_TypeT,_TypeU,false,true>
//{
//    // _TypeT is an function type, try to convert to reference or pointer
//    typedef _TYPENAME __rw_add_lvalue_reference<_TypeT>::type _TypeT_Ref;
//    typedef _TYPENAME __rw_add_pointer<_TypeT>::type _TypeT_Ptr;
//
//    enum { _C_value =    __rw_is_convertible_impl<_TypeT_Ref, _TypeU>::_C_value
//                      || __rw_is_convertible_impl<_TypeT_Ptr, _TypeU>::_C_value };
//};
//        
//template <class _TypeT, class _TypeU, bool =   __rw_is_array<_TypeU>::value
//                                             ||__rw_is_function<_TypeU>::value>
//struct __rw_is_convertible_2
//{
//    // _TypeU is neither an array nor a function type
//    enum { _C_value = __rw_is_convertible_3<_TypeT, _TypeU>::_C_value };
//};
//
//template <class _TypeT, class _TypeU>
//struct __rw_is_convertible_2<_TypeT, _TypeU, true>
//{
//    // _TypeU is either an array or a function type, and we can't convert to
//    // either of those.
//    enum { _C_value = 0 };
//};
//        
//template <class _TypeT, class _TypeU, bool = __rw_is_void<_TypeT>::value,
//                                      bool = __rw_is_void<_TypeU>::value>
//struct __rw_is_convertible_1
//{
//    // void-to-non-void or non-void to void
//    enum { _C_value = 0 };
//};
//
//template <class _TypeT, class _TypeU>
//struct __rw_is_convertible_1<_TypeT, _TypeU, false, false>
//{
//    // neither is _TypeT or _TypeU is (possibly cv-qualified) void
//    enum { _C_value = __rw_is_convertible_2<_TypeT,_TypeU>::_C_value };
//};
//
//template <class _TypeT, class _TypeU>
//struct __rw_is_convertible_1<_TypeT, _TypeU, true, true>
//{
//    // both _TypeT and _TypeU are (possibly cv-qualified) void
//    enum { _C_value = 1 };
//};

/**
 * BinaryTypeTrait indicates that _TypeT is convertible to _TypeU
 * using only implicit conversions.
 *
 * _TypeT shall be a complete type, an array of unknown bound, or void
 * (possibly cv-qualified).
 */
template <class _TypeT, class _TypeU>
struct __rw_is_convertible
    : __rw_integral_constant<bool, _RWSTD_TT_IS_CONVERTIBLE(_TypeT,_TypeU)>
{
    //_RWSTD_COMPILE_ASSERT (   _RWSTD_IS_COMPLETE (_TypeT)
    //                       || _RWSTD_IS_ARRAY (_TypeT)
    //                       || _RWSTD_IS_VOID (_TypeT));
};

//#define _RWSTD_IS_CONVERTIBLE(T,U)                                    \
//    __rw_is_convertible<T,U>::value

} // namespace __rw


#endif   // _RWSTD_META_REL_H_INCLUDED
