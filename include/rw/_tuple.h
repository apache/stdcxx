// -*- C++ -*-
/***************************************************************************
 *
 * _tuple.h - tuple class template and specializations
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
 * Copyright 2008 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_TUPLE_H_INCLUDED
#  define _RWSTD_RW_TUPLE_H_INCLUDED

#  include <rw/_defs.h>

#  if !defined _RWSTD_NO_EXT_CXX_0X

#    include <rw/_allocator.h>      // for std::allocator_arg_t
#    include <rw/_forward.h>        // for std::forward, _RWSTD_MOVE
#    include <rw/_pair.h>           // for std::pair


#    if !defined _RWSTD_NO_VARIADIC_TEMPLATES

_RWSTD_NAMESPACE (std) {


// 20.3.1, class template tuple:

template < class... Types >
class tuple;

_RWSTD_SPECIALIZED_CLASS
class tuple< >
{
    // empty
};

template < class _HeadT, class... _TailT >
class tuple< _HeadT, _TailT... >
    : public tuple< _TailT... >
{
    typedef tuple< _TailT... >  _Base;

//protected:

    _HeadT        _C_head;

    _Base&        _C_tail ()       { return *this; }
    const _Base&  _C_tail () const { return *this; }

public:

    _HeadT&       __get ()       { return _C_head; }
    const _HeadT& __get () const { return _C_head; }

    tuple ()
        : _Base (), _C_head () { /* empty */ }

    tuple (const tuple& __tuple)
        : _Base (__tuple._C_tail ())
        , _C_head (__tuple._C_head) { /* empty */ }

    tuple& operator= (const tuple& __tuple) {
        _Base::operator= (__tuple._C_tail ());
        _C_head = __tuple._C_head;
        return *this;
    }

    _EXPLICIT
    tuple (const _HeadT& __head, const _TailT&... __tail)
        : _Base (__tail...), _C_head (__head) { /* empty */ }

#      if !defined _RWSTD_NO_RVALUE_REFERENCES

    tuple (tuple&& __tuple)
        : _Base (std::forward<_Base> (__tuple._C_tail ()))
        , _C_head (_RWSTD_MOVE (__tuple._C_head)) { /* empty */ }

    tuple& operator= (tuple&& __tuple) {
        _Base::operator= (std::forward<_Base> (__tuple._C_tail ()));
        _C_head = _RWSTD_MOVE (__tuple._C_head);
        return *this;
    }

#      endif   // !defined _RWSTD_NO_RVALUE_REFERENCES

    template <class _HeadU, class... _TailU>
    tuple (const tuple<_HeadU, _TailU...>& __tuple)
        : _Base (__tuple._C_tail ())
        , _C_head (__tuple._C_head) { /* empty */ }

    template <class _HeadU, class... _TailU>
    tuple& operator= (const tuple<_HeadU, _TailU...>& __tuple) {
        _Base::operator= (__tuple._C_tail ());
        _C_head = __tuple._C_head;
        return *this;
    }

#      if !defined _RWSTD_NO_RVALUE_REFERENCES

    template <class _HeadU, class... _TailU>
    _EXPLICIT tuple (_HeadU&& __head, _TailU&&... __tail)
        : _Base (std::forward<_TailU> (__tail)...)
        , _C_head (_RWSTD_MOVE (__head)) { /* empty */ }

    template <class _HeadU, class... _TailU>
    tuple (tuple<_HeadU, _TailU...>&& __tuple)
        : _Base (std::forward<_Base> (__tuple._C_tail ()))
        , _C_head (_RWSTD_MOVE (__tuple._C_head)) { /* empty */ }

    template <class _HeadU, class... _TailU>
    tuple& operator= (tuple<_HeadU, _TailU...>&& __tuple) {
        _Base::operator= (std::forward<_Base> (__tuple._C_tail ()));
        _C_head = _RWSTD_MOVE (__tuple._C_head);
        return *this;
    }

    // allocator-extended constructors:

    template <class _Alloc, class... _TypesU>
    tuple (allocator_arg_t, const _Alloc& __alloc,
           const _TypesU&&... __values);

    template <class _Alloc>
    tuple (allocator_arg_t, const _Alloc& __alloc,
           tuple&& __tuple);

    template <class _Alloc, class... _TypesU>
    tuple (allocator_arg_t, const _Alloc& __alloc,
           tuple<_TypesU...>&& __tuple);

#      endif   // !defined _RWSTD_NO_RVALUE_REFERENCES

    template <class _Alloc>
    tuple (allocator_arg_t, const _Alloc& __alloc);

    template <class _Alloc>
    tuple (allocator_arg_t, const _Alloc& __alloc,
           const _HeadT& __head, const _TailT&... __tail);

    template <class _Alloc>
    tuple (allocator_arg_t, const _Alloc& __alloc,
           const tuple& __tuple);

    template <class _Alloc, class... _TypesU>
    tuple (allocator_arg_t, const _Alloc& __alloc,
           const tuple<_TypesU...>& __tuple);

};


template < class _TypeT1, class _TypeT2 >
class tuple< _TypeT1, _TypeT2 >
{
    _TypeT1 _C_first;
    _TypeT2 _C_second;

public:

    tuple (): _C_first (), _C_second () { /* empty */ }

    tuple (const tuple& __tuple)
        : _C_first (__tuple._C_first)
        , _C_second (__tuple._C_second) { /* empty */ }

    tuple& operator= (const tuple& __tuple) {
        _C_first = __tuple._C_first;
        _C_second = __tuple._C_second;
        return *this;
    }

    _EXPLICIT tuple (const _TypeT1& __t1, const _TypeT2& __t2)
        : _C_first (__t1), _C_second (__t2) { /* empty */ }

    template <class _TypeU1, class _TypeU2>
    tuple (const pair<_TypeU1, _TypeU2>& __pair)
        : _C_first (__pair.first), _C_second (__pair.second) { /* empty */ }

    template <class _TypeU1, class _TypeU2>
    tuple& operator= (const pair<_TypeU1, _TypeU2>& __pair) {
        _C_first = __pair.first;
        _C_second = __pair.second;
        return *this;
    }

#      if !defined _RWSTD_NO_RVALUE_REFERENCES

    template <class _TypeU1, class _TypeU2>
    tuple (pair<_TypeU1, _TypeU2>&& __pair)
        : _C_first (_RWSTD_MOVE (__pair.first))
        , _C_second (_RWSTD_MOVE (__pair.second)) { /* empty */ }

    template <class _TypeU1, class _TypeU2>
    tuple& operator= (pair<_TypeU1, _TypeU2>&& __pair) {
        _C_first = _RWSTD_MOVE (__pair.first);
        _C_second = _RWSTD_MOVE (__pair.second);
        return *this;
    }

    // allocator-extended constructors:

    template <class _Alloc, class _TypeU1, class _TypeU2>
    tuple (allocator_arg_t, const _Alloc& __alloc,
           pair<_TypeU1, _TypeU2>&& __pair);

#      endif   // !defined _RWSTD_NO_RVALUE_REFERENCES

    template <class _Alloc, class _TypeU1, class _TypeU2>
    tuple (allocator_arg_t, const _Alloc& __alloc,
           const pair<_TypeU1, _TypeU2>& __pair);

public: // internal

    _TypeT1&        __get ()       { return _C_first; }
    const _TypeT1&  __get () const { return _C_first; }

};


}   // namespace std

#    endif   // !defined _RWSTD_NO_VARIADIC_TEMPLATES

#  endif   // !defined _RWSTD_NO_EXT_CXX_0X

#endif   // _RWSTD_TUPLE_INCLUDED

