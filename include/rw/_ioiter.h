/***************************************************************************
 *
 * _ioiter.h - Definitions of stream iterator templates
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_IOITER_H_INCLUDED
#define _RWSTD_RW_IOITER_H_INCLUDED

#ifndef _RWSTD_RW_IOSFWD_H_INCLUDED
#  include <rw/_iosfwd.h>
#endif   // _RWSTD_RW_IOSFWD_H_INCLUDED

#ifndef _RWSTD_RW_ITERBASE_H_INCLUDED
#  include <rw/_iterbase.h>
#endif   // _RWSTD_RW_ITERBASE_H_INCLUDED


_RWSTD_NAMESPACE (std) { 


#ifndef _RWSTD_IOSFWD_INCLUDED

template <class _CharT, class _Traits = char_traits<_CharT> >
struct istreambuf_iterator;

template <class _CharT, class _Traits = char_traits<_CharT> >
struct ostreambuf_iterator;

#endif   // _RWSTD_IOSFWD_INCLUDED


// 24.5.3
template <class _CharT, class _Traits>
struct istreambuf_iterator
    : iterator<input_iterator_tag, _CharT, 
               typename _Traits::off_type, _CharT*, _CharT&>
{
    typedef _CharT                                  char_type;
    typedef _Traits                                 traits_type;
    typedef typename traits_type::int_type         int_type;
    typedef basic_streambuf<char_type, traits_type> streambuf_type;
    typedef basic_istream<char_type, traits_type>   istream_type;

private:

    // 24.5.3.1 (exposition only)
    class _C_proxy {
        char_type       _C_keep;
        streambuf_type *_C_sb;

        _C_proxy (char_type __c, streambuf_type *__sb)
            : _C_keep (__c), _C_sb (__sb) { }

    public:
        
        char_type operator* () const {
            return _C_keep;
        }

        friend struct istreambuf_iterator<char_type, traits_type>;
    };

public:

#ifndef _RWSTD_STRICT_ANSI

    // expose proxy in relaxed mode
    typedef _C_proxy proxy;

#endif   // _RWSTD_STRICT_ANSI

    // 24.5.3.2, p1
    istreambuf_iterator (streambuf_type *__sb = 0) _THROWS (())
    : _C_sb (__sb) { }

    // 24.5.3.2, p2
    istreambuf_iterator (istream_type&) _THROWS (());

    // 24.5.3.2, p3
    istreambuf_iterator (const _C_proxy &__proxy) _THROWS (())
    : _C_sb (__proxy._C_sb) { }

    // 24.5.3.3, p1
    char_type operator*() const;

    // 24.5.3.4, p1
    istreambuf_iterator& operator++ ();

    // 24.5.3.4, p3
    _C_proxy operator++ (int);

    // 24.5.3.5, p1 - const follows the resolution of lwg issue 110
    bool equal (const istreambuf_iterator&) const;

private:
    streambuf_type *_C_sb;
};


// defined here as opposed to within the body of the class
// to work around IBM VisualAge C++ bug #396 (PMR 02173)
template <class _CharT, class _Traits>
inline istreambuf_iterator<_CharT, _Traits>::
istreambuf_iterator (istream_type &__strm) _THROWS (())
    : _C_sb (__strm.rdbuf ())
{
    // no-op
} 


template <class _CharT, class _Traits>
inline typename istreambuf_iterator<_CharT, _Traits>::char_type
istreambuf_iterator<_CharT, _Traits>::
operator*() const
{
    _RWSTD_ASSERT (0 != _C_sb);
    _RWSTD_ASSERT (!equal (istreambuf_iterator ()));

    return traits_type::to_char_type (_C_sb->sgetc ());
}


template <class _CharT, class _Traits>
inline istreambuf_iterator<_CharT, _Traits>&
istreambuf_iterator<_CharT, _Traits>::
operator++()
{
    _RWSTD_ASSERT (0 != _C_sb);
    _RWSTD_ASSERT (!equal (istreambuf_iterator ()));

    _C_sb->sbumpc ();

    return *this;
}


template <class _CharT, class _Traits>
inline typename istreambuf_iterator<_CharT, _Traits>::_C_proxy 
istreambuf_iterator<_CharT, _Traits>::
operator++(int)
{
    _RWSTD_ASSERT (0 != _C_sb);
    _RWSTD_ASSERT (!equal (istreambuf_iterator ()));

    return _C_proxy (traits_type::to_char_type (_C_sb->sbumpc ()), _C_sb);
}


template <class _CharT, class _Traits>
inline bool
istreambuf_iterator<_CharT, _Traits>::
equal (const istreambuf_iterator &__rhs) const
{
    // Returns true if and only if both iterators are at end-of-stream,
    // or neither is at end-of-stream, regardless of what streambuf
    // object they use.
    // See also lwg issue 111.

    const int_type __eof = traits_type::eof ();

    const bool __eof_lhs =
        !_C_sb || traits_type::eq_int_type (_C_sb->sgetc (), __eof);

    const bool __eof_rhs =
        !__rhs._C_sb || traits_type::eq_int_type (__rhs._C_sb->sgetc(), __eof);

    return __eof_lhs == __eof_rhs;
}


// 24.5.3.6
template <class _CharT, class _Traits>
inline bool
operator== (const istreambuf_iterator<_CharT, _Traits>& __x,
            const istreambuf_iterator<_CharT, _Traits>& __y)
{
    return __x.equal (__y);
}


// 24.5.3.7
template <class _CharT, class _Traits>
inline bool
operator!= (const istreambuf_iterator<_CharT, _Traits>& __x, 
            const istreambuf_iterator<_CharT, _Traits>& __y)
{
    return !__x.equal (__y);
}


// 24.5.4
template <class _CharT, class _Traits>
struct ostreambuf_iterator 
    : public iterator<output_iterator_tag, void, void, void, void>
{
    typedef _CharT                                  char_type;
    typedef _Traits                                 traits_type;
    typedef basic_streambuf<char_type, traits_type> streambuf_type;
    typedef basic_ostream<char_type, traits_type>   ostream_type;

    // 24.5.4.1, p1
    ostreambuf_iterator (ostream_type&) _THROWS (());

    // 24.5.4.1, p2
    ostreambuf_iterator (streambuf_type *__sb) _THROWS (())
    : _C_sb (__sb) {
        _RWSTD_ASSERT (__sb != 0);
    }

    // 24.5.4.2, p3
    ostreambuf_iterator& operator* () {
        return *this;
    }

    // 24.5.4.2, p4
    ostreambuf_iterator& operator++ () {
        return *this;
    }

    // 24.5.4.2, p4
    ostreambuf_iterator& operator++ (int) {
        return *this;
    }

    // 24.5.4.2, p1
    ostreambuf_iterator& operator= (char_type __c) { 
        if (_C_sb && traits_type::eq_int_type (_C_sb->sputc (__c),
                                               traits_type::eof ()))
            _C_sb = 0;
        return *this;
    }

    // 24.5.4.2, p5
    bool failed () const _THROWS (()) {
        return !_C_sb;
    }

private:
    streambuf_type *_C_sb;
};


// defined here as opposed to within the body of the class
// to work around IBM VisualAge C++ bug #396 (PMR 02173)
template <class _CharT, class _Traits>
inline ostreambuf_iterator<_CharT, _Traits>::
ostreambuf_iterator (ostream_type &__strm) _THROWS (())
    : _C_sb (__strm.rdbuf ())
{
    _RWSTD_ASSERT (__strm.rdbuf () != 0);   // lwg issue 112
}


}   // namespace std


#endif   // _RWSTD_RW_IOITER_H_INCLUDED
