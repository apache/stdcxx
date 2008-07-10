/***************************************************************************
 *
 * _streamiter.h - Definitions of stream iterator templates
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_STREAMITER_H_INCLUDED
#define _RWSTD_RW_STREAMITER_H_INCLUDED

#ifndef _RWSTD_RW_ITERBASE_H_INCLUDED
#  include <rw/_iterbase.h>
#endif   // _RWSTD_RW_ITERBASE_H_INCLUDED

#include <iosfwd>


_RWSTD_NAMESPACE (std) { 


template <class _TypeT, class _CharT, class _Traits, class _Distance>
class istream_iterator;

template <class _TypeT, class _CharT, class _Traits, class _Distance>
bool operator== (const istream_iterator<_TypeT, _CharT, _Traits, _Distance>&,
                 const istream_iterator<_TypeT, _CharT, _Traits, _Distance>&);

// 24.5.1
#if !defined (_MSC_VER) || _MSC_VER > 1300

template <class _TypeT, class _CharT = char,
          class _Traits = char_traits<_CharT>,
          class _Distance = _RWSTD_PTRDIFF_T>
#else

// prevent MSVC 6.0 ICE
template <class _TypeT, class _CharT, class _Traits, class _Distance>

#endif

class istream_iterator
    : public iterator<input_iterator_tag, _TypeT, _Distance,
                      const _TypeT*, const _TypeT&>
{
    friend bool _RWSTD_SPECIALIZED_FRIEND (operator==)
        (const istream_iterator&, const istream_iterator&);

    typedef iterator<input_iterator_tag, _TypeT, _Distance,
                     const _TypeT*, const _TypeT&> _C_base;

public: 

    typedef _CharT                                char_type;
    typedef _Traits                               traits_type;
    typedef basic_istream<char_type, traits_type> istream_type;

    // for convenience
    typedef typename _C_base::value_type         value_type;

    // 24.5.1.1, p1
    istream_iterator (): _C_strm (0) { }

    // 24.5.1.1, p2
    istream_iterator (istream_type& __s)
        : _C_strm (&__s) {
        ++*this;
    }

    // 24.5.1.2, p1
    const value_type& operator* () const {
        return _C_val;
    }

    // 24.5.1.2, p2
    _RWSTD_OPERATOR_ARROW (const value_type* operator->() const);

    // 24.5.1.2, p3
    istream_iterator& operator++ ();

    // 24.5.1.2, p5
    istream_iterator operator++ (int) {
        istream_iterator __tmp = *this;
        return ++*this, __tmp;
    }
      
protected:

    istream_type *_C_strm;   // associated stream
    value_type    _C_val;    // last extracted value
};


template <class _TypeT, class _CharT, class _Traits, class _Distance>
inline istream_iterator<_TypeT, _CharT, _Traits, _Distance>&
istream_iterator<_TypeT, _CharT, _Traits, _Distance>::
operator++ ()
{
    // incrementing an end-of-stream iterator has undefined behavior

    // see also LWG issue 788
    if (_C_strm && (*_C_strm >> _C_val).fail ())
        _C_strm = 0;

    return *this;
}


// 24.5.1.2, p6
template <class _TypeT, class _CharT, class _Traits, class _Distance>
inline bool
operator== (const istream_iterator<_TypeT, _CharT, _Traits, _Distance>& __x,
            const istream_iterator<_TypeT, _CharT, _Traits, _Distance>& __y)
{
    return __x._C_strm == __y._C_strm;
}


template <class _TypeT, class _CharT, class _Traits, class _Distance>
inline bool
operator!= (const istream_iterator<_TypeT, _CharT, _Traits, _Distance>& __x,
            const istream_iterator<_TypeT, _CharT, _Traits, _Distance>& __y)
{
    return !(__x == __y);
}


// 24.5.2
template <class _TypeT, class _CharT = char,
          class _Traits = char_traits<_CharT> >
struct ostream_iterator: iterator<output_iterator_tag, void, void, void, void>
{
    typedef _CharT                                char_type;
    typedef _Traits                               traits_type;
    typedef basic_ostream<char_type, traits_type> ostream_type;

    // 24.5.2.1, p1, p2
    ostream_iterator (ostream_type& __strm, const char_type* __s = 0) 
      : _C_strm (&__strm), _C_str (__s) { }

    // 24.5.2.2, p1
    ostream_iterator& operator= (const _TypeT &__val) { 
        *_C_strm << __val;
        if (_C_str)
            *_C_strm << _C_str;
        return *this;
    }

    // 24.5.2.2, p2
    ostream_iterator& operator* () {
        return *this;
    }

    // 24.5.2.2, p3
    ostream_iterator& operator++ () {
        return *this;
    }
 
    ostream_iterator& operator++ (int) {
        return *this;
    }

private:

    ostream_type    *_C_strm;   // associated stream
    const char_type *_C_str;    // string to separate values with
};


}   // namespace std


#endif   // _RWSTD_RW_STREAMITER_H_INCLUDED
