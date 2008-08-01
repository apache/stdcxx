// -*- C++ -*-
/***************************************************************************
 *
 * _tuple.h - internal class template and helpers for <tuple> header
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
 *
 * $Id: _tuple.h 677985 2008-07-18 18:05:55Z elemings $
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

#ifndef _RWSTD_RW_TUPLE_H_INCLUDED
#  define _RWSTD_RW_TUPLE_H_INCLUDED

#  include <rw/_defs.h>

#  include <rw/_allocator.h>        // for std::allocator_arg_t
#  include <rw/_forward.h>          // for _RWSTD_FORWARD, _RWSTD_MOVE
#  include <rw/_meta_other.h>       // for _RWSTD_DECAY


_RWSTD_NAMESPACE (std) {

template <class _TypeT>
class reference_wrapper;

}   // namespace std


_RWSTD_NAMESPACE (__rw) {


// internal tuple class template

#  if !defined _RWSTD_NO_VARIADIC_TEMPLATES

template <class... _TypesT>
class __rw_tuple;

_RWSTD_SPECIALIZED_CLASS
class __rw_tuple<>
{
    // empty
};

template <class _HeadT, class... _TailT>
class __rw_tuple<_HeadT, _TailT...>
    : public __rw_tuple<_TailT...>
{
    typedef __rw_tuple<_TailT...>   _Base;

    _HeadT  _C_data;

public:

    __rw_tuple ()
        : _Base (), _C_data () { /* empty */ }

    __rw_tuple (const __rw_tuple& __tuple)
        : _Base (__tuple._C_tail ())
        , _C_data (__tuple._C_data) { /* empty */ }

    __rw_tuple& operator= (const __rw_tuple& __tuple) {
        _Base::operator= (__tuple._C_tail ());
        _C_data = __tuple._C_data;
        return *this;
    }

    explicit
    __rw_tuple (const _HeadT& __head, const _TailT&... __tail)
        : _Base (__tail...), _C_data (__head) { /* empty */ }

    template <class _HeadU, class... _TailU>
    __rw_tuple (const __rw_tuple<_HeadU, _TailU...>& __tuple)
        : _Base (__tuple._C_tail ())
        , _C_data (__tuple._C_head ()) { /* empty */ }

    template <class _HeadU, class... _TailU>
    __rw_tuple& operator= (const __rw_tuple<_HeadU, _TailU...>& __tuple) {
        _Base::operator= (__tuple._C_tail ());
        _C_data = __tuple._C_head ();
        return *this;
    }

#    if !defined _RWSTD_NO_RVALUE_REFERENCES

    __rw_tuple (__rw_tuple&& __tuple)
        : _Base (_STD::move<_Base&&> (__tuple._C_tail ()))
        , _C_data (_RWSTD_FORWARD (_HeadT, __tuple._C_data)) { /* empty */ }

    __rw_tuple& operator= (__rw_tuple&& __tuple) {
        _Base::operator= (_RWSTD_FORWARD (_Base, __tuple._C_tail ()));
        _C_data = _RWSTD_MOVE (__tuple._C_data);
        return *this;
    }

    template <class _HeadU, class... _TailU>
    explicit __rw_tuple (_HeadU&& __head, _TailU&&... __tail)
        : _Base (_RWSTD_FORWARD (_TailU, __tail)...)
        , _C_data (_RWSTD_MOVE (__head)) { /* empty */ }

    template <class _HeadU, class... _TailU>
    __rw_tuple (__rw_tuple<_HeadU, _TailU...>&& __tuple)
        : _Base (_RWSTD_MOVE (__tuple._C_tail ()))
        , _C_data (_RWSTD_MOVE (__tuple._C_head ())) { /* empty */ }

    template <class _HeadU, class... _TailU>
    __rw_tuple& operator= (__rw_tuple<_HeadU, _TailU...>&& __tuple) {
        _Base::operator= (_RWSTD_MOVE (__tuple._C_tail ()));
        _C_data = _RWSTD_MOVE (__tuple._C_head ());
        return *this;
    }

    // allocator-extended constructors:

    template <class _Alloc, class... _TypesU>
    __rw_tuple (_STD::allocator_arg_t, const _Alloc& __alloc,
                const _TypesU&&... __values);

    template <class _Alloc>
    __rw_tuple (_STD::allocator_arg_t, const _Alloc& __alloc,
                __rw_tuple&& __tuple);

    template <class _Alloc, class... _TypesU>
    __rw_tuple (_STD::allocator_arg_t, const _Alloc& __alloc,
                __rw_tuple<_TypesU...>&& __tuple);

#    endif   // !defined _RWSTD_NO_RVALUE_REFERENCES

    template <class _Alloc>
    __rw_tuple (_STD::allocator_arg_t, const _Alloc& __alloc);

    template <class _Alloc>
    __rw_tuple (_STD::allocator_arg_t, const _Alloc& __alloc,
           const _HeadT& __head, const _TailT&... __tail);

    template <class _Alloc>
    __rw_tuple (_STD::allocator_arg_t, const _Alloc& __alloc,
                const __rw_tuple& __tuple);

    template <class _Alloc, class... _TypesU>
    __rw_tuple (_STD::allocator_arg_t, const _Alloc& __alloc,
                const __rw_tuple<_TypesU...>& __tuple);

    // accessors:

    _HeadT&         _C_head ()       { return _C_data; }
    const _HeadT&   _C_head () const { return _C_data; }

    _Base&          _C_tail ()       { return *this; }
    const _Base&    _C_tail () const { return *this; }

};


struct __rw_ignore
{
    template <class _TypeT>
    void operator= (const _TypeT&) const {}
};


template <class _TypeT>
struct __rw_deduce_ref
{
    typedef _TypeT  _C_type;
};

template <class _TypeT>
struct __rw_deduce_ref<_STD::reference_wrapper<_TypeT> >
{
    typedef _TypeT& _C_type;
};

template <class _TypeT>
struct __rw_deduce_ref<const _STD::reference_wrapper<_TypeT> >
{
    typedef _TypeT& _C_type;
};

template <class _TypeT>
struct __rw_make_tuple
{
    typedef typename _RWSTD_DECAY (_TypeT)                 _C_decay;
    typedef typename __rw_deduce_ref<_C_decay>::_C_type    _C_type;
};


template <class...  _TypesT, class... _TypesU>
bool operator== (const __rw_tuple<_TypesT...>& __x,
                 const __rw_tuple<_TypesU...>& __y) {
    return     (__x._C_head () == __y._C_head ())
            && (__x._C_tail () == __y._C_tail ());
}

_RWSTD_SPECIALIZED_FUNCTION
bool operator== (const __rw_tuple<>& /*__x*/,
                 const __rw_tuple<>& /*__y*/) {
    return true;
}

template <class...  _TypesT, class... _TypesU>
bool operator< (const __rw_tuple<_TypesT...>& __x,
                const __rw_tuple<_TypesU...>& __y) {
    return     (__x._C_head () < __y._C_head ())
            || (  !(__y._C_head () < __x._C_head ())
                && (__x._C_tail () < __y._C_tail ()));
}

_RWSTD_SPECIALIZED_FUNCTION
bool operator< (const __rw_tuple<>& /*__x*/,
                const __rw_tuple<>& /*__y*/) {
    return false;
}

#  endif   // !defined _RWSTD_NO_VARIADIC_TEMPLATES


}   // namespace __rw


#endif   // _RWSTD_TUPLE_INCLUDED
