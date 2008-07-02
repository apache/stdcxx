// -*- C++ -*-
/***************************************************************************
 *
 * _ref_wrap.h - reference wrappers for <functional> header
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

#ifndef _RWSTD_RW_REF_WRAP_INCLUDED
#  define _RWSTD_RW_REF_WRAP_INCLUDED

#  include <rw/_defs.h>


_RWSTD_NAMESPACE (std) {


// 20.5.5, class template reference_wrapper:

template <class _Type>
class reference_wrapper
{
    _Type* _C_ptr;

public:

    typedef _Type type;

    reference_wrapper (type& __x)
        : _C_ptr (&__x) { /* empty */ }

    reference_wrapper (const reference_wrapper& __x)
        : _C_ptr (__x._C_ptr) { /* empty */ }

    reference_wrapper& operator= (const reference_wrapper& __x) {
        _RWSTD_ASSERT (0 != __x._C_ptr);
        _C_ptr = __x._C_ptr;
        return *this;
    }

    operator type& () const {
        _RWSTD_ASSERT (0 != _C_ptr);
        return *_C_ptr;
    }

    type& get() const {
        _RWSTD_ASSERT (0 != _C_ptr);
        return *_C_ptr;
    }
};


template <class _Type>
inline reference_wrapper<_Type>
ref (_Type& __x)
{
    return reference_wrapper<_Type> (__x);
}

template <class _Type>
inline reference_wrapper<_Type>
ref (reference_wrapper<_Type>& __x)
{
    return reference_wrapper<_Type> (__x);
}


template <class _Type>
inline reference_wrapper<const _Type>
cref (const _Type& __x)
{
    return reference_wrapper<const _Type> (__x);
}

template <class _Type>
inline reference_wrapper<const _Type>
cref (reference_wrapper<const _Type>& __x)
{
    return reference_wrapper<const _Type> (__x);
}


}   // namespace std


#endif   // _RWSTD_RW_REF_WRAP_INCLUDED
