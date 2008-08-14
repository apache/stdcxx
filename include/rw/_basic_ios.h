// -*- C++ -*-
/***************************************************************************
 *
 * _basic_ios.h - definition of the class template basic_ios
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

#ifndef _RWSTD_RW_BASIC_IOS_H_INCLUDED
#define _RWSTD_RW_BASIC_IOS_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#ifndef _RWSTD_NO_REDUNDANT_DEFINITIONS
#  include <streambuf>
#endif   // _RWSTD_NO_REDUNDANT_DEFINITIONS

#include <loc/_ctype.h>    // for ctype

#ifndef _RWSTD_RW_IOSBASE_H_INCLUDED
#  include <rw/_iosbase.h>   // for ios_base
#endif   // _RWSTD_RW_IOSBASE_H_INCLUDED

#ifndef _RWSTD_RW_IOSFWD_H_INCLUDED
#  include <rw/_iosfwd.h>
#endif   // _RWSTD_RW_IOSFWD_H_INCLUDED


_RWSTD_NAMESPACE (std) { 

#ifndef _RWSTD_IOSFWD_INCLUDED

_EXPORT
template <class _CharT, class _Traits = char_traits<_CharT> >
class basic_ios;

typedef basic_ios<char> ios;

#  ifndef _RWSTD_NO_WCHAR_T

typedef basic_ios<wchar_t> wios;

#  endif   // _RWSTD_NO_WCHAR_T
#endif   // _RWSTD_IOSFWD_INCLUDED


_EXPORT
template <class _CharT, class _Traits>
class basic_ios: public ios_base
{
public:

    typedef _CharT                          char_type;
    typedef _Traits                         traits_type;
    typedef typename traits_type::int_type int_type;
    typedef typename traits_type::pos_type pos_type;
    typedef typename traits_type::int_type off_type;

protected:

    typedef basic_streambuf<char_type, traits_type> streambuf_type; 
    typedef basic_ostream<char_type, traits_type>   ostream_type;

public:

    // 27.4.4.1, p1 - NOTE: `sb' may point to a yet uninitialized
    //  object; it is unsafe to reference any of its members
    explicit basic_ios (streambuf_type *__sb) {
        init (__sb);
    }

    // 27.4.4.3, p1

#ifndef _RWSTD_NO_EXT_IOS_SAFE_CONVERSION

private:

#  if !defined (__INTEL_COMPILER) || !defined (_WIN32)

    // SunPro 5.7 needs a data pointer and misoptimizes the conversion
    // operator when this is a pointer to a member function (bug #446)
    struct _C_uniq_type { int _C_foo;   /* never used */ };
    typedef int _C_uniq_type::*_C_uniq_ptr;

#  else   // if __INTEL_COMPILER && _WIN32

    // working around Intel C++ 8.1 -Zi bug #570 on Win32
    struct _C_uniq_type { void _C_foo () { };   /* never used */ };
    typedef void (_C_uniq_type::*_C_uniq_ptr)();

#  endif   // __INTEL_COMPILER/_WIN32

public:

    operator _C_uniq_ptr () const {
        return fail () ? _C_uniq_ptr (0) : &_C_uniq_type::_C_foo;
    }

#else   // if defined (_RWSTD_NO_EXT_IOS_SAFE_CONVERSION)

    operator void*() const {
        return fail () ? (void*)0 : (void*)1;
    }

#endif   // _RWSTD_NO_EXT_IOS_SAFE_CONVERSION

    // 27.4.4.3, p2
    bool operator! () const {
        return fail ();
    }

    // 27.4.4.3, p3
    iostate rdstate () const {
        return iostate (_C_state);
    }

    // 27.4.4.3, p4
    void clear (iostate __state = goodbit) {
        _C_set (__state, _C_except, rdbuf ());
    }

    // 27.4.4.3, p6
    void setstate (iostate __st) {
        clear (rdstate () | __st);
    }

    // 27.4.4.3, p7
    bool good () const {
        return goodbit == rdstate ();
    }

    // 27.4.4.3, p8
    bool eof () const {
        return 0 != (rdstate () & eofbit);
    }

    // 27.4.4.3, p9
    bool fail () const {
        return 0 != (rdstate () & (failbit | badbit));
    }

    // 27.4.4.3, p10
    bool bad () const {
        return 0 != (rdstate () & badbit);
    }

    // 27.4.4.3, p11
    iostate exceptions () const {
        return iostate (_C_except);
    }

    // 27.4.4.3, p12
    void exceptions (iostate __state) {
        _C_set (rdstate (), __state, rdbuf ());
    }

    // 27.4.4.2, p1
    ostream_type* tie () const {
        return _C_usr ? _RWSTD_STATIC_CAST (ostream_type*, _C_tie ()) : 0;
    }

    // 27.4.4.2, p2
    ostream_type* tie (ostream_type *__strm) {
        return _RWSTD_STATIC_CAST (ostream_type*, _C_tie (__strm));
    }

    // 27.4.4.2, p4 - NOTE: the pointer returned from rdbuf() in derived
    // classes may be different from the one passed to rdbuf (streambuf_type*)
    streambuf_type* rdbuf () const {
        return _RWSTD_STATIC_CAST (streambuf_type*, _C_rdbuf);
    }

    // 27.4.4.2, p5
    streambuf_type* rdbuf (streambuf_type *__rdbuf) {
        return _RWSTD_STATIC_CAST (streambuf_type*,
                                   _C_set (goodbit, _C_except, __rdbuf));
    }

    // 27.4.4.2, p15
    basic_ios& copyfmt (const basic_ios &__x) {
        return _C_copyfmt (__x, &_C_fill, &__x._C_fill, sizeof _C_fill), *this;
    }

    // 27.4.4.2, p12
    char_type fill () const;

    // 27.4.4.2, p13
    char_type fill (char_type);

    // 27.4.4.2, p8
    locale imbue (const locale&);

    // 27.4.4.2, p10
    char narrow (char_type __c, char __dflt) const {
        return _USE_FACET (ctype<char_type>, getloc ()).narrow (__c, __dflt);
    }

    // 27.4.4.2, p11
    char_type widen (char __c) const {
        return _USE_FACET (ctype<char_type>, getloc ()).widen (__c);
    }

protected:

    // initialization of standard iostream objects may depend on
    // the default ctor not to re-initialize the object
    basic_ios () { /* no-op as per 27.4.4.1, p2 */ }

    // 27.4.4.1, p3
    void init (streambuf_type*);

    // (conditionally) returns a pointer to a mutex in the associated
    // basic_streambuf object
    _RW::__rw_mutex* _C_bufmutex () const;

private:

    basic_ios (const basic_ios&);              //  not defined
    basic_ios& operator= (const basic_ios&);   //  not defined

    int_type _C_fill;      // fill character (space by default)
};


template<class _CharT, class _Traits>
inline typename basic_ios<_CharT, _Traits>::char_type
basic_ios<_CharT, _Traits>::fill () const
{
    // delayed initialization used to allow objects of stream types other
    // than those whose char_type is char or wchar_t to be successfully
    // constructed even if the global locale doesn't contain a ctype facet
    // specialized on the stream type's char_type
    if (traits_type::eq_int_type (traits_type::eof (), _C_fill))
        _RWSTD_CONST_CAST (basic_ios*, this)->fill (widen (' '));

    return traits_type::to_char_type (_C_fill);
}


template<class _CharT, class _Traits>
inline typename basic_ios<_CharT, _Traits>::char_type
basic_ios<_CharT, _Traits>::fill (char_type __ch)
{
    // convert to int type before swapping
    const int_type __c = traits_type::to_int_type (__ch);

    _RWSTD_MT_GUARD (flags () & _RWSTD_IOS_NOLOCK ? 0 : &_C_mutex);

    // save the previous value of the fill character
    traits_type::assign (__ch, traits_type::to_char_type (_C_fill));

    _C_fill = __c;

    return __ch;
}


template<class _CharT, class _Traits>
inline locale
basic_ios<_CharT, _Traits>::imbue (const locale& __loc)
{
    _RWSTD_MT_GUARD (flags () & _RWSTD_IOS_NOLOCK ? 0 : &_C_mutex);

    const locale __tmp = _C_unsafe_imbue (__loc);

    if (rdbuf ())
        rdbuf ()->pubimbue (__loc);

    return __tmp;
}


template<class _CharT, class _Traits>
inline void
basic_ios<_CharT, _Traits>::init (streambuf_type *__sb)
{
    // invalidate the fill character
    // the character will be initialized the first time fill() is called
    // so that objects of stream types other than those specialized on char
    // and wchar_t can be successfully constructed even if the global locale
    // doesn't contain a ctype facet specialized on the stream type's
    // char_type
    _C_fill = traits_type::eof ();

    _C_init (__sb);
}


template<class _CharT, class _Traits>
inline _RW::__rw_mutex*
basic_ios<_CharT, _Traits>::_C_bufmutex () const
{
#if !defined (_RWSTD_REENTRANT) || defined (_RWSTD_NO_EXT_REENTRANT_IO)

    // unconditionally return pointer to buffer's mutex
    // (real or fake if not in an MT environment)
    return rdbuf () ? &rdbuf ()->_C_mutex : 0;

#else

    // return pointer to buffer's mutex unless the object is in
    // a state where buffer locking is disabled)
    return flags () & _RWSTD_IOS_NOLOCKBUF || !rdbuf ()
        ? 0 : &rdbuf ()->_C_mutex;

#endif   // !_RWSTD_REENTRANT || !_RWSTD_NO_EXT_REENTRANT_IO
}


}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE_FIRST (_BASIC_IOS)
#  include <rw/_basic_ios.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (_BASIC_IOS)


#ifdef _RWSTD_MSVC
#  pragma warning (push)
#  pragma warning (disable: 4231)
#endif   // _RWSTD_MSVC


_RWSTD_NAMESPACE (std) { 

#if _RWSTD_INSTANTIATE (_BASIC_IOS, _CHAR)

_RWSTD_INSTANTIATE_2 (class _RWSTD_TI_EXPORT
                      basic_ios<char, char_traits<char> >);

#endif   // _RWSTD_INSTANTIATE (_BASIC_IOS, _CHAR)

#if _RWSTD_INSTANTIATE (_BASIC_IOS, _WCHAR_T)

_RWSTD_INSTANTIATE_2 (class _RWSTD_TI_EXPORT
                      basic_ios<wchar_t, char_traits<wchar_t> >);

#endif   // _RWSTD_INSTANTIATE (_BASIC_IOS, _WCHAR_T)

}   // namespace std


#ifdef _RWSTD_MSVC
#  pragma warning (pop)
#endif   // _RWSTD_MSVC


#if _RWSTD_DEFINE_TEMPLATE_LAST (_BASIC_IOS)
#  include <rw/_basic_ios.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_LAST (_BASIC_IOS)


#endif   // _RWSTD_RW_BASIC_IOS_H_INCLUDED
