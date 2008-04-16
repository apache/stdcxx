/***************************************************************************
 *
 * _traits.h - definition of the char_traits class template
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

#ifndef _RWSTD_RW_TRAITS_H_INCLUDED
#define _RWSTD_RW_TRAITS_H_INCLUDED

#ifndef _RWSTD_RW_IOSFWD_H_INCLUDED
#  include <rw/_iosfwd.h>
#endif   // _RWSTD_RW_IOSFWD_H_INCLUDED

#ifndef _RWSTD_RW_MBSTATE_H_INCLUDED
#  include <rw/_mbstate.h>   // for _RWSTD_MBSTATE_T
#endif   // _RWSTD_RW_MBSTATE_H_INCLUDED


#if defined (_RWSTDDEBUG) || defined (_RWSTD_EDG_ECCP)

// avoid including <cstring> and <cwchar> in debug mode or when using
// the vanilla EDG eccp (i.e., in strict conformance mode) to prevent
// namespace pollutiuon

_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT void* __rw_memcpy (void*, const void*, _RWSTD_SIZE_T);
_RWSTD_EXPORT void* __rw_memmove (void*, const void*, _RWSTD_SIZE_T);
_RWSTD_EXPORT const void* __rw_memchr (const void*, int, _RWSTD_SIZE_T);
_RWSTD_EXPORT void* __rw_memset (void*, int, _RWSTD_SIZE_T);
_RWSTD_EXPORT int __rw_memcmp (const void*, const void*, _RWSTD_SIZE_T);
_RWSTD_EXPORT _RWSTD_SIZE_T __rw_strlen (const char*);

#  ifndef _RWSTD_NO_WCHAR_T

_RWSTD_EXPORT wchar_t* __rw_wmemcpy (wchar_t*, const wchar_t*, _RWSTD_SIZE_T);
_RWSTD_EXPORT wchar_t* __rw_wmemmove (wchar_t*, const wchar_t*, _RWSTD_SIZE_T);
_RWSTD_EXPORT const wchar_t*
__rw_wmemchr (const wchar_t*, wchar_t, _RWSTD_SIZE_T);
_RWSTD_EXPORT wchar_t* __rw_wmemset (wchar_t*, wchar_t, _RWSTD_SIZE_T);
_RWSTD_EXPORT int __rw_wmemcmp (const wchar_t*, const wchar_t*, _RWSTD_SIZE_T);
_RWSTD_EXPORT _RWSTD_SIZE_T __rw_wcslen (const wchar_t*);

#  endif   // _RWSTD_NO_WCHAR_T

}   // namespace __rw

#  define _RWSTD_MEMCPY    _RW::__rw_memcpy
#  define _RWSTD_MEMCMP    _RW::__rw_memcmp
#  define _RWSTD_MEMCHR    _RW::__rw_memchr
#  define _RWSTD_MEMMOVE   _RW::__rw_memmove
#  define _RWSTD_MEMSET    _RW::__rw_memset
#  define _RWSTD_STRLEN    _RW::__rw_strlen

#  define _RWSTD_WMEMCPY   _RW::__rw_wmemcpy
#  define _RWSTD_WMEMCMP   _RW::__rw_wmemcmp
#  define _RWSTD_WMEMCHR   _RW::__rw_wmemchr
#  define _RWSTD_WMEMMOVE  _RW::__rw_wmemmove
#  define _RWSTD_WMEMSET   _RW::__rw_wmemset
#  define _RWSTD_WCSLEN    _RW::__rw_wcslen
#else   // if !defined (_RWSTDDEBUG) && !defined (_RWSTD_EDG_ECCP)

#  if 4 <= __GNUG__ && !defined (__INTEL_COMPILER)
     // use gcc 4.x intrinsic functions
#    define _RWSTD_MEMCPY    __builtin_memcpy
#    define _RWSTD_MEMCMP    __builtin_memcmp
#    define _RWSTD_MEMMOVE   __builtin_memmove
#    define _RWSTD_MEMSET    __builtin_memset
#    define _RWSTD_STRLEN    __builtin_strlen

#    if 4 < __GNUG__ || 3 <= __GNUC_MINOR__
       // __builtin_memchr() is only available in gcc 4.3 and beyond
#      define _RWSTD_MEMCHR    __builtin_memchr
#    else   // gcc < 4.3
#      include _RWSTD_CSTRING   // for memchr()
#      define _RWSTD_MEMCHR    _RWSTD_C::memchr
#    endif   // gcc 4.3
#  else   // gcc < 4.0
#    include _RWSTD_CSTRING   // for memcmp(), ...

#    define _RWSTD_MEMCPY    _RWSTD_C::memcpy
#    define _RWSTD_MEMCMP    _RWSTD_C::memcmp
#    define _RWSTD_MEMMOVE   _RWSTD_C::memmove
#    define _RWSTD_MEMSET    _RWSTD_C::memset
#    define _RWSTD_STRLEN    _RWSTD_C::strlen
#    define _RWSTD_MEMCHR    _RWSTD_C::memchr
#  endif   // gcc 4.0

#  include _RWSTD_CWCHAR    // wmemcmp(), ...

#  ifndef _RWSTD_NO_WMEMCPY
#    define _RWSTD_WMEMCPY   _RWSTD_C::wmemcpy
#  elif !defined (_RWSTD_NO_WCHAR_T)

_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT wchar_t*
__rw_wmemcpy (wchar_t*, const wchar_t*, _RWSTD_SIZE_T);


}   // namespace __rw

#    define _RWSTD_WMEMCPY   _RW::__rw_wmemcpy
#  endif   // _RWSTD_NO_WMEMCPY

#  ifndef _RWSTD_NO_WMEMCMP
#    define _RWSTD_WMEMCMP   wmemcmp
#  elif !defined (_RWSTD_NO_WCHAR_T)

_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT int
__rw_wmemcmp (const wchar_t*, const wchar_t*, _RWSTD_SIZE_T);


}   // namespace __rw

#    define _RWSTD_WMEMCMP   _RW::__rw_wmemcmp
#  endif   // _RWSTD_NO_WMEMCMP

#  ifndef _RWSTD_NO_WMEMMOVE
#    define _RWSTD_WMEMMOVE  _RWSTD_C::wmemmove
#  elif !defined (_RWSTD_NO_WCHAR_T)

_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT wchar_t*
__rw_wmemmove (wchar_t*, const wchar_t*, _RWSTD_SIZE_T);


}   // namespace __rw

#    define _RWSTD_WMEMMOVE   _RW::__rw_wmemmove
#  endif   // _RWSTD_NO_WMEMMOVE

#  ifndef _RWSTD_NO_WMEMSET
#    define _RWSTD_WMEMSET   _RWSTD_C::wmemset
#  elif !defined (_RWSTD_NO_WCHAR_T)

_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT wchar_t*
__rw_wmemset (wchar_t*, wchar_t, _RWSTD_SIZE_T);


}   // namespace __rw

#    define _RWSTD_WMEMSET   _RW::__rw_wmemset
#  endif   // _RWSTD_NO_WMEMSET

#  ifndef _RWSTD_NO_WCSLEN
#    define _RWSTD_WCSLEN    _RWSTD_C::wcslen
#  elif !defined (_RWSTD_NO_WCHAR_T)

_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_wcslen (const wchar_t*);


}   // namespace __rw

#    define _RWSTD_WCSLEN   _RW::__rw_wcslen
#  endif   // _RWSTD_NO_WCSLEN

#  ifndef _RWSTD_NO_WMEMCHR
#    define _RWSTD_WMEMCHR   _RWSTD_C::wmemchr
#  elif !defined (_RWSTD_NO_WCHAR_T)

_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT const wchar_t*
__rw_wmemchr (const wchar_t*, wchar_t, _RWSTD_SIZE_T);


}   // namespace __rw

#    define _RWSTD_WMEMCHR   _RW::__rw_wmemchr
#  endif   // _RWSTD_NO_WMEMCHR

#endif   // !_RWSTDDEBUG && !(vanilla EDG eccp demo)


_RWSTD_NAMESPACE (std) { 


// 27.4.1, p2
// the C++ type ptrdiff_t is the same as the POSIX ssize_t suggested
// to be used here in 27.4.2, Footnote 266, except for Win64 where
// the former is 64 bits while the latter is only 32 bits wide
typedef _RWSTD_PTRDIFF_T streamsize;


// 27.4.3
template <class _StateT>
class fpos
{
public:

    typedef _StateT state_type;
      
#ifndef _RWSTD_NO_POD_ZERO_INIT

    fpos (_RWSTD_STREAMOFF __off = 0,
          const state_type &__state = state_type ())
        : _C_pos (__off),
          _C_state (__state)
        { }

#else   // if defined (_RWSTD_NO_POD_ZERO_INIT)

    fpos (_RWSTD_STREAMOFF __off = 0)
        : _C_pos (__off) {
        _RWSTD_MEMSET (&_C_state, 0, sizeof _C_state);
    }

    fpos (_RWSTD_STREAMOFF __off, const state_type &__state)
        : _C_pos (__off),
          _C_state (__state)
        { }

#endif   // _RWSTD_NO_POD_ZERO_INIT

    operator _RWSTD_STREAMOFF () const {
        return _C_pos;
    }

    // 27.4.3.1, p1
    void state (state_type __state) {
        _C_state = __state;
    }

    // 27.4.3.1, p2
    state_type state () const {
        return _C_state;
    }

    // 27.4.3.2, p1: Table 88
    fpos& operator-= (_RWSTD_STREAMOFF __off) {
        return _C_pos -= __off, *this;
    }

    fpos& operator+= (_RWSTD_STREAMOFF __off) {
        return _C_pos += __off, *this;
    }

    fpos operator- (_RWSTD_STREAMOFF __off) const {
        return fpos (*this) -= __off;
    }

    fpos operator+ (_RWSTD_STREAMOFF __off) const {
        return fpos (*this) += __off;
    }

    // equality and relational operators provided
    // via the conversion operator to streamoff

private:

    _RWSTD_STREAMOFF _C_pos;     // signed displacement
    state_type       _C_state;   // conversion state
};


template <class  _CharT> 
struct char_traits;


#ifndef _RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY

template <class  _CharT> 
struct char_traits
{
    typedef _CharT           char_type;
    typedef int              int_type;
    typedef _RWSTD_STREAMOFF off_type;
    typedef _RWSTD_MBSTATE_T state_type;
    typedef fpos<state_type> pos_type;

    static int_type eof () {
        return -1;
    }

    static void assign (char_type& __c1, const char_type& __c2) {
        __c1 = __c2;
    }

    static bool eq (const char_type& __c1, const char_type& __c2) {
        return __c1 == __c2;
    }

    static bool lt (const char_type& __c1, const char_type& __c2) {
        return __c1 < __c2;
    }

    static int
    compare (const char_type* __s1, const char_type* __s2, _RWSTD_SIZE_T __n) {
        for (_RWSTD_SIZE_T __i = 0; __i != __n; ++__i) {
            if (!eq (__s1[__i], __s2[__i])) {
                return lt (__s1[__i], __s2[__i]) ? -1 : 1;
            }
        }
        return 0;
    }
        
    static _RWSTD_SIZE_T length (const char_type *__s) {
        _RWSTD_SIZE_T __len = 0;
        while (!eq (*__s++, char_type ()))
            ++__len;
        return __len;
    }
 
    static const char_type* 
    find (const char_type* __s, _RWSTD_SIZE_T __n, const char_type& __c) {
        while (__n-- > 0 && !eq (*__s, __c) )
            ++__s;
        return  eq (*__s, __c) ? __s : 0;
    }

    static char_type*
    move (char_type* __s1, const char_type* __s2, _RWSTD_SIZE_T __n) {
        _RWSTD_MEMMOVE (__s1, __s2, __n * sizeof (char_type));
        return __s1;
    }

    static char_type*
    copy (char_type *__dst, const char_type *__src, _RWSTD_SIZE_T __n) {
        _RWSTD_MEMCPY (_RWSTD_STATIC_CAST (void*, __dst),
                       _RWSTD_STATIC_CAST (const void*, __src),
                       __n * sizeof (char_type));
        return __dst;
    }

    static char_type*
    assign (char_type* __s, _RWSTD_SIZE_T __n, char_type __c) {
        char_type* __tmp = __s;
        while (__n-- > 0) 
            assign (*__tmp++, __c);
        return __s;
    }

    static bool eq_int_type (const int_type& __c1, const int_type& __c2) {
        return __c1 == __c2;
    }

    static int_type not_eof (const int_type& __c) {
        return eq_int_type (eof (), __c) ? 0 : __c;
    }

    static char_type to_char_type (const int_type& __c) {
        // cast to prevent warnings for unusual types
        return _RWSTD_STATIC_CAST (char_type, __c);
    }
      
    static int_type to_int_type (const char_type& __c) {
        // cast to prevent warnings for unusual types
        return _RWSTD_STATIC_CAST (int_type, __c);
    }
};

#endif   // _RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY


// 21.1.3.1
_RWSTD_SPECIALIZED_CLASS  
struct char_traits<char> 
{
    typedef char             char_type;
    typedef _RWSTD_INT_T     int_type;
    typedef _RWSTD_STREAMOFF off_type; 
    typedef _RWSTD_MBSTATE_T state_type;
    typedef fpos<state_type> pos_type;

    static int_type eof () {
        return int_type (_RWSTD_EOF);
    }

    static void assign (char_type& __c1, const char_type& __c2){
        __c1 = __c2;
    }

    static bool eq (const char_type& __c1, const char_type& __c2) {
        return __c1 == __c2;
    }

    static bool lt (const char_type& __c1, const char_type& __c2) {
        // lwg issue 467: cast arguments to unsigned char
        // to get the same result as memcmp(&c1, &c2) < 0
        return   _RWSTD_STATIC_CAST (unsigned char, __c1)
               < _RWSTD_STATIC_CAST (unsigned char, __c2);
    }

    static int
    compare (const char_type* __s1, const char_type* __s2, _RWSTD_SIZE_T __n) {
        return _RWSTD_MEMCMP (__s1, __s2, __n);
    }

    static const char_type*
    find (const char_type* __s, _RWSTD_SIZE_T __n, const char_type& __c) {
        // cast to const void* used to get around a gcc 2.95 bug
        // that prevents a static_cast from void* --> const T*
        // (only occurs if memchr() isn't overloaded on const)
        return _RWSTD_STATIC_CAST (
            const char_type*, (const void*)_RWSTD_MEMCHR (
                __s, _RWSTD_STATIC_CAST (unsigned char, __c), __n));
    }

    static _RWSTD_SIZE_T length (const char_type *__s) {
        return _RWSTD_STRLEN (__s);
    }

    static char_type*
    move (char_type* __s1, const char_type* __s2, _RWSTD_SIZE_T __n) {
        _RWSTD_MEMMOVE (__s1, __s2, __n);
        return __s1;
    }

    static char_type*
    copy (char_type *__dst, const char_type *__src, _RWSTD_SIZE_T __n) {
        _RWSTD_MEMCPY (__dst, __src, __n);
        return __dst;
    }

    static char_type*
    assign (char_type* __s, _RWSTD_SIZE_T __n, char_type __c) {
        _RWSTD_MEMSET (__s, __c, __n);
        return __s;
    }

    static bool eq_int_type (const int_type& __c1, const int_type& __c2) {
        return __c1 == __c2;
    }

    static int_type not_eof (const int_type& __c) {
        return eq_int_type (eof (), __c) ? 0 : __c;
    }
                        
    static char_type to_char_type (const int_type& __c) {
        return _RWSTD_STATIC_CAST (char_type, __c);
    }
      
    static int_type to_int_type (const char_type& __c) {
        // make sure (signed char)'\xff' converts to 255 and not -1
        return _RWSTD_STATIC_CAST (unsigned char, __c);
    }
};


#ifndef _RWSTD_NO_WCHAR_T

// 21.1.3.2
_RWSTD_SPECIALIZED_CLASS  
struct char_traits<wchar_t> 
{
    typedef wchar_t          char_type;
    typedef _RWSTD_WINT_T    int_type;
    typedef _RWSTD_STREAMOFF off_type;
    typedef _RWSTD_MBSTATE_T state_type;
    typedef fpos<state_type> pos_type;

    static int_type eof () {
        return int_type (_RWSTD_WEOF);
    }

    static void assign (char_type& __c1, const char_type& __c2) {
        __c1 = __c2;
    }

    static bool eq (const char_type& __c1, const char_type& __c2) {
        return __c1 == __c2;
    }

    static bool lt (const char_type& __c1, const char_type& __c2) {
        return __c1 < __c2;
    }

    static int
    compare (const char_type* __s1, const char_type* __s2, _RWSTD_SIZE_T __n) {
        return _RWSTD_WMEMCMP (__s1, __s2, __n);
    }

    static _RWSTD_SIZE_T length (const char_type *__s) {
        // [harmless] cast necessary on CygWin
        return _RWSTD_WCSLEN (_RWSTD_CONST_CAST (char_type*, __s));
    }
      
    static const char_type*
    find (const char_type* __s, _RWSTD_SIZE_T __n, const char_type& __c) {
        // const cast in case of a const-incorrect wmemchr()
        return _RWSTD_STATIC_CAST (const char_type*,
                   _RWSTD_WMEMCHR (_RWSTD_CONST_CAST (char_type*, __s),
                                   __c, __n));
    }

    static char_type*
    copy (char_type *__dst, const char_type *__src, _RWSTD_SIZE_T __n) {
        _RWSTD_WMEMCPY (__dst, __src, __n);
        return __dst;
    }
      
    static char_type*
    move (char_type* __s1, const char_type* __s2, _RWSTD_SIZE_T __n) {
        _RWSTD_WMEMMOVE (__s1, __s2, __n);
        return __s1;
    }

    static char_type*
    assign (char_type* __s, _RWSTD_SIZE_T __n, char_type __c) {
        _RWSTD_WMEMSET (__s, __c, __n);
        return __s;
    }

    static bool eq_int_type (const int_type& __c1, const int_type& __c2) {
        return __c1 == __c2;
    }

    static int_type not_eof (const int_type& __c) {
        return eq_int_type (eof (), __c) ? 0 : __c;
    }

    static char_type to_char_type (const int_type& __c) {
        return __c;
    }
      
    static int_type to_int_type (const char_type& __c) {
        return __c;
    }
};

#endif   // _RWSTD_NO_WCHAR_T


}   // namespace std


#endif   // _RWSTD_RW_TRAITS_H_INCLUDED
