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

#  if !defined _RWSTD_NO_EXT_CXX_0X

#    include <rw/_defs.h>
#    include <rw/_forward.h>
#    include <rw/_pair.h>

#    if !defined _RWSTD_NO_VARIADIC_TEMPLATES


_RWSTD_NAMESPACE (std) {


// 20.3.1, class template tuple:

/**
 * @defgroup tuple Tuples [tuple]
 *
 * A fixed-size collection of values with variable, heterogenous types.
 * This class template is used to instantatiate tuple types.  A tuple
 * type specifies zero or more element types for defining tuple values.
 * A tuple value can be constructed from a tuple type that holds one
 * value for each element type in the tuple.
 *
 * @param Types A list of zero or more types.  No applicable type
 *              requirements or restrictions.
 */
template < class... Types >
class tuple;

/**
 * @internal
 * The template specialization for empty tuples.
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
 * @param _HeadT First element type (required).
 * @param _TailT Template parameter pack for remaining element types.
 */
template < class _HeadT, class... _TailT >
class tuple< _HeadT, _TailT... >
    : tuple< _TailT... >
{
    typedef tuple< _TailT... >  _Base;

protected:

    _HeadT  _C_head;

public:

    /**
     * Construct tuple with default values.
     */
    tuple ()
        : _C_head (), _Base () { /* empty */ }

    /**
     * Copy construct tuple from a different tuple value.
     * @param __tuple Another tuple value with same type.
     */
    tuple (const tuple& __tuple)
        : _C_head (__tuple._C_head), _Base (__tuple) { /* empty */ }

    /**
     * Copy assign tuple from a different tuple value.
     * @param __tuple Another tuple value with same type.
     */
    tuple& operator= (const tuple& __tuple) {
        _C_head = __tuple._C_head;
        _Base::operator= (__tuple);
        return *this;
    }

    _EXPLICIT
    tuple (const _HeadT& __head, const _TailT&... __tail)
        : _C_head (__head), _Base (__tail...) { /* empty */ }

#      if !defined _RWSTD_NO_RVALUE_REFERENCES

    tuple (tuple&& __tuple)
        : _C_head (std::move (__tuple._C_head))
        , _Base (std::forward<_Base> (__tuple)) { /* empty */ }

    tuple& operator= (tuple&& __tuple) {
        _C_head = std::move (__tuple._C_head);
        _Base::operator= (__tuple);
        return *this;
    }

#      endif   // !defined _RWSTD_NO_RVALUE_REFERENCES

#      if !defined _RWSTD_NO_MEMBER_TEMPLATES

    template <class _HeadU, class... _TailU>
    tuple (const tuple<_HeadU, _TailU...>& __tuple);

    template <class _HeadU, class... _TailU>
    tuple& operator= (const tuple<_HeadU, _TailU...>& __tuple);

#      endif   // !defined _RWSTD_NO_MEMBER_TEMPLATES

#      if !defined _RWSTD_NO_RVALUE_REFERENCES \
          && !defined _RWSTD_NO_MEMBER_TEMPLATES

    template <class _HeadU, class... _TailU>
    _EXPLICIT tuple (_HeadU&& __head, _TailU&&... __tail);

    template <class _HeadU, class... _TailU>
    tuple (tuple<_HeadU, _TailU...>&& __tuple);

    template <class _HeadU, class... _TailU>
    tuple& operator= (tuple<_HeadU, _TailU...>&& __tuple);

#      endif   // !defined _RWSTD_NO_RVALUE_REFERENCES
               // && !defined _RWSTD_NO_MEMBER_TEMPLATES
};


#      if !defined _RWSTD_NO_MEMBER_TEMPLATES

template <class _TypeT1, class _TypeT2>
class tuple<_TypeT1, _TypeT2>
    : pair<_TypeT1, _TypeT2>
{
    typedef pair< _TypeT1, _TypeT2 > _Base;

public:

    tuple (): _Base () { /* empty */ }
    tuple (const tuple& __tuple): _Base (__tuple) { /* empty */ }
    tuple& operator= (const tuple& __tuple);

    _EXPLICIT tuple (const _TypeT1& __t1, const _TypeT2& __t2);

    template <class _TypeU1, class _TypeU2>
    tuple (const pair<_TypeU1, _TypeU2>& __pair);

    template <class _TypeU1, class _TypeU2>
    tuple& operator= (const pair<_TypeU1, _TypeU2>& __pair);

#        if !defined _RWSTD_NO_RVALUE_REFERENCES

    template <class _TypeU1, class _TypeU2>
    tuple (pair<_TypeU1, _TypeU2>&& __pair);

    template <class _TypeU1, class _TypeU2>
    tuple& operator= (pair<_TypeU1, _TypeU2>&& __pair);

#        endif   // !defined _RWSTD_NO_RVALUE_REFERENCES

};

#      endif   // !defined _RWSTD_NO_MEMBER_TEMPLATES


}   // namespace std


#    endif   // !defined _RWSTD_NO_VARIADIC_TEMPLATES

#  endif   // !defined _RWSTD_NO_EXT_CXX_0X

#endif   // _RWSTD_TUPLE_INCLUDED

