// -*- C++ -*-
/***************************************************************************
 *
 * _ref_wrap.h - reference wrappers for <functional> header
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
 *
 * $Id: _ref_wrap.h 673534 2008-07-02 22:49:15Z elemings $
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

template <class _TypeT>
class reference_wrapper
{
    _TypeT* _C_ptr;

public:

    typedef _TypeT  type;

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


template <class _TypeT>
inline reference_wrapper<_TypeT>
ref (_TypeT& __x)
{
    return reference_wrapper<_TypeT> (__x);
}

template <class _TypeT>
inline reference_wrapper<_TypeT>
ref (reference_wrapper<_TypeT>& __x)
{
    return reference_wrapper<_TypeT> (__x);
}


template <class _TypeT>
inline reference_wrapper<const _TypeT>
cref (const _TypeT& __x)
{
    return reference_wrapper<const _TypeT> (__x);
}

template <class _TypeT>
inline reference_wrapper<const _TypeT>
cref (reference_wrapper<const _TypeT>& __x)
{
    return reference_wrapper<const _TypeT> (__x);
}


}   // namespace std


#endif   // _RWSTD_RW_REF_WRAP_INCLUDED
