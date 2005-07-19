/***************************************************************************
 *
 * _traits.h - Declarations for char_traits 
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
 *
 * $Id: //stdlib/dev/include/rw/_traits.h#48 $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#ifndef _RWSTD_TRAITS_H_INCLUDED
#define _RWSTD_TRAITS_H_INCLUDED

#include <rw/_iosfwd.h>
#include <rw/_defs.h>

#include _RWSTD_CSTRING   // for memcmp(), memcpy(), ...
#include _RWSTD_CWCHAR    // for mbstate_t, wmemcmp(), ...

#ifdef _RWSTD_NO_MBSTATE_T
#  include <rw/_mbstate.h>
#endif   // _RWSTD_NO_MBSTATE_T


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
        memset (&_C_state, 0, sizeof _C_state);
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
    typedef _CharT                   char_type;
    typedef int                      int_type;
    typedef _RWSTD_STREAMOFF         off_type;
    typedef mbstate_t                state_type;
    typedef fpos<state_type>         pos_type;

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

    static int compare (const char_type* __s1, const char_type* __s2,
                        _RWSTD_SIZE_T __n) {
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
    find (const char_type* __s, _RWSTD_SIZE_T __n, const char_type& __a) {
        while (__n-- > 0 && !eq (*__s, __a) )
            ++__s;
        return  eq (*__s, __a) ? __s : 0;
    }

    static char_type* move (char_type* __s1, const char_type* __s2,
                            _RWSTD_SIZE_T __n) {
        if (__s1 < __s2)
            copy (__s1, __s2, __n);
        else if (__s1 > __s2) {
            __s1 += __n;
            __s2 += __n;
            while (__n--)
                assign (*--__s1, *--__s2);
        }
        return __s1;
    }

    static char_type*
    copy (char_type *__dst, const char_type *__src, _RWSTD_SIZE_T __n) {
        memcpy (_RWSTD_STATIC_CAST (void*, __dst),
                _RWSTD_STATIC_CAST (const void*, __src),
                __n * sizeof (char_type));
        return __dst;
    }

    static char_type*
    assign (char_type* __s, _RWSTD_SIZE_T __n, char_type __a) {
        char_type* __tmp = __s;
        while (__n-- > 0) 
            assign (*__tmp++, __a);
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
    typedef char                      char_type;
    typedef _RWSTD_INT_T              int_type;
    typedef _RWSTD_STREAMOFF          off_type; 
    typedef mbstate_t                 state_type;
    typedef fpos<state_type>          pos_type;

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
        return __c1 < __c2;
    }

    static int compare (const char_type* __s1, const char_type* __s2,
                        _RWSTD_SIZE_T __n) {
        return memcmp (__s1, __s2, __n);
    }

    static const char_type*
    find (const char_type* __s, _RWSTD_SIZE_T __n, const char_type& __a) {
        // cast to const void* used to get around a gcc 2.95 bug
        // that prevents a static_cast from void* --> const T*
        // (only occurs if memchr() isn't overloaded on const)
        return _RWSTD_STATIC_CAST (const char_type*,
                                   (const void*)memchr (__s, __a, __n));
    }

    static _RWSTD_SIZE_T length (const char_type *__s) {
        _RWSTD_ASSERT (0 != __s);
        return strlen (__s);
    }

    static char_type*
    move (char_type* __s1, const char_type* __s2, _RWSTD_SIZE_T __n) {

#ifndef _RWSTD_NO_MEMMOVE
        memmove (__s1, __s2, __n);
#else   // if defined (_RWSTD_NO_MEMMOVE)
        if (__s1 < __s2) {
            while (__n--)
                assign (*__s1++, *__s2++);
        }
        else {
            __s1 += __n;
            __s2 += __n;
            while (__n--)
                assign (*--__s1, *--__s2);
        }
#endif   // _RWSTD_NO_MEMMOVE

        return __s1;
    }

    static char_type*
    copy (char_type *__dst, const char_type *__src, _RWSTD_SIZE_T __n) {
        _RWSTD_ASSERT (!__n || __dst && __src);
        memcpy (__dst, __src, __n); 
        return __dst;
    }

    static char_type*
    assign (char_type* __s, _RWSTD_SIZE_T __n, char_type __a) {
        memset (__s, __a, __n);
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
    typedef wchar_t                   char_type;
    typedef _RWSTD_WINT_T             int_type;
    typedef _RWSTD_STREAMOFF          off_type;
    typedef mbstate_t                 state_type;
    typedef fpos<state_type>          pos_type;

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

    static int compare (const char_type* __s1, const char_type* __s2,
                        _RWSTD_SIZE_T __n) {
#ifndef _RWSTD_NO_WMEMCMP
        return wmemcmp (__s1, __s2, __n);
#else
        for (_RWSTD_SIZE_T __i = 0; __i != __n; ++__i) {
            if (!eq (__s1[__i], __s2[__i])) {
                return lt (__s1[__i], __s2[__i]) ? -1 : 1;
            }
        }
        return 0;
#endif   // _RWSTD_NO_WMEMCMP
    }

    static _RWSTD_SIZE_T length (const char_type *__s) {

        _RWSTD_ASSERT (0 != __s);

#ifndef _RWSTD_NO_WCSLEN
        // [harmless] cast necessary on CygWin
        return wcslen (_RWSTD_CONST_CAST (char_type*, __s));
#else
        _RWSTD_SIZE_T __len = 0;
        while (!eq (*__s++, char_type ()))
            ++__len;
        return __len;
#endif   // _RWSTD_NO_WCSLEN
    }
      
    static const char_type*
    find (const char_type* __s, _RWSTD_SIZE_T __n, const char_type& __a) {

#ifndef _RWSTD_NO_WMEMCHR
        // const cast in case of a const-incorrect wmemchr()
        return _RWSTD_STATIC_CAST (const char_type*,
                   wmemchr (_RWSTD_CONST_CAST (char_type*, __s), __a, __n));
#else
        while (__n-- > 0 && !eq (*__s, __a))
            ++__s;
        return  eq (*__s, __a) ? __s : 0;
#endif   // _RWSTD_NO_WMEMCHR
    }

    static char_type*
    copy (char_type *__dst, const char_type *__src, _RWSTD_SIZE_T __n) {
        memcpy (__dst, __src, __n * sizeof (char_type));
        return __dst;
    }
      
    static char_type*
    move (char_type* __s1, const char_type* __s2, _RWSTD_SIZE_T __n) {

#ifndef _RWSTD_NO_WMEMMOVE
        wmemmove (__s1, __s2, __n);
#else
        if (__s1 < __s2) {
            while (__n--)
                assign (*__s1++, *__s2++);
        }
        else {
            __s1 += __n;
            __s2 += __n;
            while (__n--)
                assign (*--__s1, *--__s2);
        }
#endif   // _RWSTD_NO_WMEMMOVE

        return __s1;
    }

    static char_type*
    assign (char_type* __s, _RWSTD_SIZE_T __n, char_type __a) {
#ifndef _RWSTD_NO_WMEMSET
        wmemset (__s, __a, __n);
#else
        char_type* __tmp = __s;
        while (__n-- > 0) 
            assign (*__tmp++, __a);
#endif   // _RWSTD_NO_WMEMSET
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


#endif   // _RWSTD_TRAITS_H_INCLUDED
