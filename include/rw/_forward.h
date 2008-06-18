// -*- C++ -*-
/***************************************************************************
 *
 * _forward - forward/move helpers for <utility> header
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
 * Copyright 2008 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_FORWARD_INCLUDED
#  define _RWSTD_RW_FORWARD_INCLUDED

#  if !defined _RWSTD_NO_EXT_CXX_0X

#    include <rw/_defs.h>
#    include <rw/_meta_ref.h>   // for __rw_remove_reference


_RWSTD_NAMESPACE (std) {


// [20.2.2]  forward/move helpers

/**
 * An identity wrapper.  Similar to the identity property, the identity
 * wrapper is a class template that simply reflects the type of its
 * template parameter.  This class template is used when a template
 * parameter type must be explicitly specified in order to apply the
 * correct move/forwarding semantics, usually in the \c std::forward()
 * function.
 *
 * @tparam _Type Any type.  No restrictions or requirements.
 * @see std::forward
 */
template <class _Type>
struct identity
{
    /** Identifies template parameter type. */
    typedef _Type type;

    /**
     * Conversion operator.  This operator converts the parameter value
     * to the wrapped identity type.
     *
     * @param __x An value convertible to identity type.
     * @return Same value as the function argument with identity type.
     */
    const _Type& operator() (const _Type& __x) const {
        return __x;
    }
};


#    if !defined _RWSTD_NO_RVALUE_REFERENCES

/**
 * Forwards appropriate rvalue or lvalue reference type.  This function
 * is used to ensure that the appropriate reference type is used in move
 * semantics.
 *
 * @tparam _Type An lvalue or rvalue reference type.
 * @param __x An lvalue reference or rvalue reference.
 * @return An lvalue if __x is an lvalue reference; otherwise, an rvalue.
 */
_EXPORT
template <class _Type>
_Type&&
forward (_TYPENAME identity<_Type>::type&& __x)
{
    return __x;
}

/**
 * Move a value to an rvalue reference.  This function is used to
 * explicitly bind constructors and other functions with rvalue
 * references that employ move semantics.
 *
 * @tparam _Type Any type.  No requirements or restrictions.
 * @param __x An lvalue or rvalue.
 * @return Same value as parameter with rvalue reference type.
 */
_EXPORT
template <class _Type>
_TYPENAME _RW::__rw_remove_reference<_Type>::type&&
move (_Type&& __x)
{
    return __x;
}

/**
 * @internal
 * Internal wrapper macro to utilize move semantics if available.
 * @param __x An lvalue or rvalue.
 */
#      define _RWSTD_MOVE(__x)   std::move (__x)
#    else   // no rvalue references
#      define _RWSTD_MOVE(__x)   (__x)

#    endif   // !defined _RWSTD_NO_RVALUE_REFERENCES


}   // namespace std


#  endif   // !defined _RWSTD_NO_EXT_CXX_0X

#endif   // _RWSTD_RW_FORWARD_INCLUDED
