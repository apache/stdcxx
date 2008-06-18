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

#    include <rw/_forward.h>
#    include <rw/_pair.h>

#    if !defined _RWSTD_NO_VARIADIC_TEMPLATES


/// @defgroup tuple Tuples [tuple]
/// @{

_RWSTD_NAMESPACE (std) {


// 20.3.1, class template tuple:

/**
 * A fixed-size collection of values with variable, heterogenous types.
 * This class template is used to instantatiate tuple types.  A tuple
 * type specifies zero or more element types for defining tuple values.
 * A tuple value can be constructed from a tuple type that holds one
 * value for each element type in the tuple.
 *
 * By definition, a homogenous tuple is a tuple that has the exact same
 * number and type of elements as another tuple type.  A heterogenous
 * tuple is just the opposite: a tuple type that has either a different
 * number of element types or one or more different element types.
 *
 * @tparam Types A list of zero or more types.  No applicable type
 *               requirements or restrictions.
 */
template < class... Types >
class tuple;

/**
 * @internal
 * The template specialization for empty tuples.  This specialization
 * also serves as the terminating instantiation of a recursive tuple
 * with one or more element types.
 */
_RWSTD_SPECIALIZED_CLASS
class tuple< >
{
    // empty
};

/**
 * @internal
 * The basic template specialization for most tuples.  This class
 * template is used to instantiate all tuple types except for empty
 * tuples and tuples with exactly two element types.
 *
 * @tparam _HeadT First element type (required).
 * @tparam _TailT Zero or more additional element types.
 */
template < class _HeadT, class... _TailT >
class tuple< _HeadT, _TailT... >
    : tuple< _TailT... >
{
    typedef tuple< _TailT... >  _Base;

protected:

    _HeadT        _C_head;

    _Base&        _C_tail ()       { return *this; }
    const _Base&  _C_tail () const { return *this; }

public:

    /**
     * Construct tuple with default values.
     */
    tuple ()
        : _Base (), _C_head () { /* empty */ }

    /**
     * Copy construct tuple from a different tuple value.
     * @param __tuple Another tuple value with same type.
     */
    tuple (const tuple& __tuple)
        : _Base (__tuple._C_tail ())
        , _C_head (__tuple._C_head) { /* empty */ }

    /**
     * Copy assign tuple from a different tuple value.
     * @param __tuple Some other tuple value.
     * @param __tuple Another tuple value with same type.
     * @return This tuple value.
     */
    tuple& operator= (const tuple& __tuple) {
        _Base::operator= (__tuple._C_tail ());
        _C_head = __tuple._C_head;
        return *this;
    }

    /**
     * Copy construct tuple from element values.  This explicit
     * constructor creates a tuple value from element values.
     *
     * @param __head A value corresponding to first tuple element type.
     * @param __tail List of corresponding values of remaining tuple
     *               element types.
     */
    _EXPLICIT
    tuple (const _HeadT& __head, const _TailT&... __tail)
        : _Base (__tail...), _C_head (__head) { /* empty */ }

#      if !defined _RWSTD_NO_RVALUE_REFERENCES

    /**
     * Construct tuple by moving a value from some other tuple value.
     * This move constructor moves the value from the given tuple value
     * into this tuple.
     * @param __tuple Some other homogenous tuple value.
     */
    tuple (tuple&& __tuple)
        : _Base (std::forward<_Base> (__tuple._C_tail ()))
        , _C_head (_RWSTD_MOVE (__tuple._C_head)) { /* empty */ }

    /**
     * Assign tuple by moving a value from some other homogenous tuple.
     * This assignment operator moves the value from the given tuple
     * value into this tuple.
     * @param __tuple Some other homogenous tuple value.
     * @return Lvalue reference to this tuple value.
     */
    tuple& operator= (tuple&& __tuple) {
        _Base::operator= (std::forward<_Base> (__tuple._C_tail ()));
        _C_head = _RWSTD_MOVE (__tuple._C_head);
        return *this;
    }

#      endif   // !defined _RWSTD_NO_RVALUE_REFERENCES

#      if !defined _RWSTD_NO_MEMBER_TEMPLATES

    /**
     * Construct tuple by copying a heterogenous tuple value.  This copy
     * constructor copies the value from a compatible, heterogenous
     * tuple.
     * @tparam _HeadU First element type in tuple.
     * @tparam _TailU Remaining element types in tuple.
     * @param __tuple A compatible, heterogenous tuple value.
     */
    template <class _HeadU, class... _TailU>
    tuple (const tuple<_HeadU, _TailU...>& __tuple)
        : _Base (__tuple._C_tail ())
        , _C_head (__tuple._C_head) { /* empty */ }

    /**
     * Assign tuple by copying a heterogenous tuple value.  This
     * assignment operator copies the value from a compatible,
     * heterogenous tuple.
     * @tparam _HeadU First element type in tuple.
     * @tparam _TailU Remaining element types in tuple.
     * @param __tuple A compatible, heterogenous tuple value.
     * @return Lvalue reference to this tuple value.
     */
    template <class _HeadU, class... _TailU>
    tuple& operator= (const tuple<_HeadU, _TailU...>& __tuple) {
        _Base::operator= (__tuple._C_tail ());
        _C_head = __tuple._C_head;
        return *this;
    }

#      endif   // !defined _RWSTD_NO_MEMBER_TEMPLATES

#      if !defined _RWSTD_NO_RVALUE_REFERENCES \
          && !defined _RWSTD_NO_MEMBER_TEMPLATES

    /**
     * Construct tuple by moving element values.  This explicit move
     * constructor moves values from the corresponding element types.
     * @tparam _HeadU First element type in other tuple.
     * @tparam _TailU Remaining element types in other tuple.
     * @param __head First value in element type list.
     * @param __tail Remaining values in element type list.
     */
    template <class _HeadU, class... _TailU>
    _EXPLICIT tuple (_HeadU&& __head, _TailU&&... __tail)
        : _Base (std::forward<_TailU> (__tail)...)
        , _C_head (_RWSTD_MOVE (__head)) { /* empty */ }

    /**
     * Construct tuple by moving a heterogenous tuple value.  This move
     * constructor moves values from a heterogenous tuple into this
     * tuple value.
     * @tparam _HeadU First element type in other tuple.
     * @tparam _TailU Remaining element types in other tuple.
     * @param __tuple A compatible, heterogenous tuple value.
     */
    template <class _HeadU, class... _TailU>
    tuple (tuple<_HeadU, _TailU...>&& __tuple)
        : _Base (std::forward<_Base> (__tuple._C_tail ()))
        , _C_head (_RWSTD_MOVE (__tuple._C_head)) { /* empty */ }

    /**
     * Assign tuple by moving a value from some other heterogenous tuple.
     * This move assignment operator assigns a value to this tuple by
     * moving values from a compatible, heterogenous tuple.
     * @tparam _HeadU First element type in other tuple.
     * @tparam _TailU Remaining element types in other tuple.
     * @param __tuple A compatible, heterogenous tuple value.
     * @return Lvalue reference to this tuple.
     */
    template <class _HeadU, class... _TailU>
    tuple& operator= (tuple<_HeadU, _TailU...>&& __tuple) {
        _Base::operator= (std::forward<_Base> (__tuple._C_tail ()));
        _C_head = _RWSTD_MOVE (__tuple._C_head);
        return *this;
    }

#      endif   // !defined _RWSTD_NO_RVALUE_REFERENCES
               // && !defined _RWSTD_NO_MEMBER_TEMPLATES
};


#      if !defined _RWSTD_NO_MEMBER_TEMPLATES

/**
 * @internal
 * Template specialization for tuples with exactly two element types.
 * This specialization provides additional constructors and operators for
 * making class template \c std::pair implicitly compatible with tuples.
 * @tparam _TypeT1 First element type.
 * @tparam _TypeT2 Second element type.
 */
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

#        if !defined _RWSTD_NO_RVALUE_REFERENCES

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

#        endif   // !defined _RWSTD_NO_RVALUE_REFERENCES

};

#      endif   // !defined _RWSTD_NO_MEMBER_TEMPLATES


/// @}

}   // namespace std


#    endif   // !defined _RWSTD_NO_VARIADIC_TEMPLATES

#  endif   // !defined _RWSTD_NO_EXT_CXX_0X

#endif   // _RWSTD_TUPLE_INCLUDED

