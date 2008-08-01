/***************************************************************************
 *
 * _ctype.h
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
 * Copyright 2001-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _RWSTD_LOC_CTYPE_H_INCLUDED
#define _RWSTD_LOC_CTYPE_H_INCLUDED


#include <loc/_facet.h>
#include <rw/_bitmask.h>
#include <rw/_defs.h>

_RWSTD_NAMESPACE (__rw) { 

struct __rw_ctype_t;

enum __rw_ctype_mask {
    __rw_none   = 0,
    __rw_space  = 0x0001,
    __rw_print  = 0x0002,
    __rw_cntrl  = 0x0004,
    __rw_upper  = 0x0008,
    __rw_lower  = 0x0010,
    __rw_alpha  = 0x0020,
    __rw_digit  = 0x0040,
    __rw_punct  = 0x0080,
    __rw_xdigit = 0x0100,
    __rw_alnum  = __rw_alpha | __rw_digit,
    __rw_graph  = 0x0200,
    __rw_all    = 0x03ff
};

_RWSTD_DEFINE_BITMASK_OPERATORS (__rw_ctype_mask);

}   // namespace __rw


_RWSTD_NAMESPACE (std) { 

struct _RWSTD_CLASS_EXPORT locale;

// 22.2.1
struct _RWSTD_EXPORT ctype_base
{
    // 22.2.1, p1 - mask is a bitmask type (see 17.3.3.1.2)
    typedef _RWSTD_BITMASK_ENUM (_RW::__rw_ctype_mask) mask;

    _RWSTD_STATIC_CONST (mask, space  = _RW::__rw_space);
    _RWSTD_STATIC_CONST (mask, print  = _RW::__rw_print);
    _RWSTD_STATIC_CONST (mask, cntrl  = _RW::__rw_cntrl);
    _RWSTD_STATIC_CONST (mask, upper  = _RW::__rw_upper);
    _RWSTD_STATIC_CONST (mask, lower  = _RW::__rw_lower);
    _RWSTD_STATIC_CONST (mask, alpha  = _RW::__rw_alpha);
    _RWSTD_STATIC_CONST (mask, digit  = _RW::__rw_digit);
    _RWSTD_STATIC_CONST (mask, punct  = _RW::__rw_punct);
    _RWSTD_STATIC_CONST (mask, xdigit = _RW::__rw_xdigit);
    _RWSTD_STATIC_CONST (mask, alnum  = _RW::__rw_alnum);
    _RWSTD_STATIC_CONST (mask, graph  = _RW::__rw_graph);
};


// 22.2.1.1
_EXPORT
template <class _CharT>
class ctype;


#ifndef _RWSTD_NO_EXT_CTYPE_PRIMARY

_EXPORT
template <class _CharT>
class ctype: public _RW::__rw_facet, public ctype_base
{
public:

    typedef _CharT char_type;

    explicit ctype (_RWSTD_SIZE_T __ref = 0)
        : _RW::__rw_facet (__ref) { }


    // 22.2.1.1.1, p1
    bool is (mask __m, char_type __c) const {
        return do_is ( __m, __c);
    }

    const char_type*
    is (const char_type* __lo, const char_type *__hi, mask *__vec) const {
        return do_is (__lo, __hi, __vec);
    }

    // 22.2.1.1.1, p2
    const char_type*
    scan_is (mask __m, const char_type *__lo, const char_type *__hi) const {
        return do_scan_is ( __m, __lo, __hi);
    }

    // 22.2.1.1.1, p3
    const char_type*
    scan_not (mask __m, const char_type *__lo, const char_type *__hi) const {
        return do_scan_not ( __m, __lo, __hi);
    }

    // 22.2.1.1.1, p4
    char_type (toupper)(char_type __c) const {
        return do_toupper (__c);
    }

    const char_type* (toupper)(char_type *__lo, const char_type *__hi) const {
        return do_toupper (__lo, __hi);
    }

    // 22.2.1.1.1, p5
    char_type (tolower)(char_type __c) const {
        return do_tolower (__c);
    }

    const char_type* (tolower)(char_type *__lo, const char_type *__hi) const {
        return do_tolower (__lo, __hi);
    }

    // 22.2.1.1.1, p6
    char_type widen (char __c) const {
        return do_widen (__c);
    }

    const char*
    widen (const char *__lo, const char *__hi, char_type *__to) const {
        return do_widen (__lo, __hi, __to);
    }

    // 22.2.1.1.1, p7
    char narrow (char_type __c, char __dfault) const {
        return do_narrow (__c, __dfault);
    }

    const char_type*
    narrow (const char_type *__lo, const char_type *__hi,
            char __dfault, char *__to) const {
        return do_narrow (__lo, __hi, __dfault, __to);
    }

    static _RW::__rw_facet_id id;

protected:

    // 22.2.1.1.2, p1
    virtual bool do_is (mask, char_type) const {
        return false;
    }

    virtual const char_type*
    do_is (const char_type*, const char_type*, mask*) const;

    // 22.2.1.1.2, p3
    virtual const char_type*
    do_scan_is (mask, const char_type*, const char_type*) const;

    // 22.2.1.1.2, p5
    virtual const char_type*
    do_scan_not (mask, const char_type*, const char_type*) const;

    // 22.2.1.1.2, p7
    virtual char_type do_toupper (char_type __c) const {
        return __c;
    }

    virtual const char_type*
    do_toupper (char_type*, const char_type*) const;

    // 22.2.1.1.2, p9
    virtual char_type do_tolower (char_type __c) const {
        return __c;
    }

    virtual const char_type*
    do_tolower (char_type*, const char_type*) const;

    // 22.2.1.1.2, p11
    virtual char_type do_widen (char) const {
        return char_type ();
    }

    virtual const char*
    do_widen (const char*, const char*, char_type*) const;

    // 22.2.1.1.2, p13
    virtual char do_narrow (char_type, char __dfault) const {
        return __dfault;
    }

    virtual const char_type*
    do_narrow (const char_type*, const char_type*, char, char*) const;
};

#endif   // _RWSTD_NO_EXT_CTYPE_PRIMARY


_RWSTD_SPECIALIZED_CLASS
class _RWSTD_EXPORT ctype<char>: public _RW::__rw_facet, public ctype_base
{
    typedef unsigned char _UChar;

public:

    typedef char char_type;

    bool is (mask __m, char_type __c) const {
#if _RWSTD_UCHAR_MAX <= 255
        return (_C_mask_tab [_UChar (__c)] & __m) != 0;
#else
        return __c < table_size && (_C_mask_tab [_UChar (__c)] & __m) != 0;
#endif
    }

    _RWSTD_STATIC_CONST (_RWSTD_SIZE_T, table_size = 256);

    explicit ctype (const mask* = 0, bool = false, _RWSTD_SIZE_T = 0);

    const char_type* is (const char_type*, const char_type*, mask*) const;

    const char_type* scan_is (mask, const char_type*, const char_type*) const;

    const char_type* scan_not (mask, const char_type*, const char_type*) const;

    char_type (toupper)(char_type __c) const {
        return do_toupper (__c);
    }

    char_type (tolower)(char_type __c) const {
        return do_tolower (__c);
    }

    // 22.2.1.1.1, p6
    char_type widen (char) const;

    char narrow (char_type, char) const;

    const char_type* (toupper)(char_type *__lo, const char_type *__hi) const {
        return do_toupper (__lo, __hi);
    }

    const char_type* (tolower)(char_type *__lo, const char_type *__hi) const {
        return do_tolower (__lo, __hi);
    }

    const char_type*
    widen (const char *__lo, const char *__hi, char_type *__to) const {
        return do_widen (__lo, __hi, __to);
    }

    const char_type*
    narrow (const char_type *__lo, const char_type *__hi,
            char_type __dfault, char_type *__to) const {
        return do_narrow (__lo, __hi, __dfault, __to);
    }

    static _RW::__rw_facet_id id;

protected:

    const mask* table () const _THROWS (()) {
        return _C_mask_tab;
    }

    static const mask* classic_table () _THROWS (());

    virtual ~ctype ();

    virtual const char_type*
    do_toupper (char_type*, const char_type*) const;

    virtual const char_type*
    do_tolower (char_type*, const char_type*) const; 

    virtual char_type do_toupper (char_type) const;

    virtual char_type do_tolower (char_type) const;

    virtual char_type do_widen (char) const;

    virtual const char_type*
    do_widen (const char_type*, const char_type*, char_type*) const;

    virtual char_type do_narrow (char_type, char_type) const;

    virtual const char_type*
    do_narrow (const char_type*, const char_type*, char_type, char_type*)const;

protected:

    const mask    *_C_mask_tab;         // mask for each character
    unsigned char *_C_upper_tab;        // uppercase version of each character
    unsigned char *_C_lower_tab;        // lowercase version of each character
    char           _C_narrow_tab [256]; // cached results of do_narrow()
    char           _C_wide_tab [256];   // cached results of do_widen()
    int            _C_delete_it;        // delete tables above?
};


inline ctype<char>::char_type
ctype<char>::narrow (char_type __c, char __dfault) const
{
    const _RWSTD_SIZE_T __inx = _RWSTD_STATIC_CAST (unsigned char, __c);

    // optimize away all but the first call to the virtual do_widen()

    if (

#if _RWSTD_CHAR_BIT > 8

        __inx < sizeof _C_narrow_tab / sizeof *_C_narrow_tab &&

#endif   // _RWSTD_CHAR_BIT > 8

        _C_narrow_tab [__inx])
        return _C_narrow_tab [__inx];

    // template argument provided to work around an HP aCC bug (PR #27087)
    ctype<char>* const __self = _RWSTD_CONST_CAST (ctype<char>*, this);

    __c = do_narrow (__c, __dfault);

    if (__c != __dfault)
        __self->_C_narrow_tab [__inx] = __c;

    return __c;
}


inline ctype<char>::char_type
ctype<char>::widen (char __ch) const
{
    const _RWSTD_SIZE_T __inx = _RWSTD_STATIC_CAST (unsigned char, __ch);

    // optimize away all but the first call to the virtual do_widen()

#if _RWSTD_CHAR_BIT <= 8
    const bool __fits = true;
#else
    const bool __fits = __inx < sizeof _C_wide_tab / sizeof *_C_wide_tab;
#endif

    if (__fits) {
        if (_C_wide_tab [__inx])
            return _C_wide_tab [__inx];

        // template argument provided to work around an HP aCC bug (PR #27087)
        ctype<char>* const __self = _RWSTD_CONST_CAST (ctype<char>*, this);

        return __self->_C_wide_tab [__inx] = do_widen (__ch);
    }
    else
        return do_widen (__ch);
}


// 22.2.1.2
template <class _CharT>
class ctype_byname: public ctype<_CharT>
{
    char _C_namebuf [32];

public:

    explicit ctype_byname (const char *__name, _RWSTD_SIZE_T __ref = 0)
        : ctype<_CharT>(__ref) {
        this->_C_set_name (__name, _C_namebuf, sizeof _C_namebuf);
    }
};


_RWSTD_SPECIALIZED_CLASS
class _RWSTD_EXPORT ctype_byname<char>: public ctype<char>
{
    char _C_namebuf [32];

public:

    explicit ctype_byname (const char*, _RWSTD_SIZE_T = 0);
};


#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
class _RWSTD_EXPORT ctype<wchar_t>: public _RW::__rw_facet, public ctype_base
{
    typedef unsigned char _UChar;

    enum { _C_tab_size = 256 };

public:

    typedef wchar_t char_type;

    explicit ctype (_RWSTD_SIZE_T = 0);

    // 22.2.1.1.1, p1
    bool is (mask __m, char_type __c) const {
        return do_is ( __m, __c);
    }

    const char_type*
    is (const char_type *__lo, const char_type *__hi, mask *__vec) const {
        return do_is (__lo, __hi, __vec);
    }

    // 22.2.1.1.1, p2
    const char_type*
    scan_is (mask __m, const char_type *__lo, const char_type *__hi) const {
        return do_scan_is ( __m, __lo, __hi);
    }

    // 22.2.1.1.1, p3
    const char_type*
    scan_not (mask __m, const char_type *__lo, const char_type *__hi) const {
        return do_scan_not ( __m, __lo, __hi);
    }

    // 22.2.1.1.1, p4
    char_type (toupper)(char_type __c) const {
        return do_toupper (__c);
    }

    const char_type* (toupper)(char_type *__lo, const char_type *__hi) const {
        return do_toupper (__lo, __hi);
    }

    // 22.2.1.1.1, p5
    char_type (tolower)(char_type __c) const {
        return do_tolower (__c);
    }

    const char_type* (tolower)(char_type *__lo, const char_type *__hi) const {
        return do_tolower (__lo, __hi);
    }

    // 22.2.1.1.1, p6
    char_type widen (char) const;

    const char*
    widen (const char* __lo, const char *__hi, char_type *__to) const {
        return do_widen (__lo, __hi, __to);
    }

    // 22.2.1.1.1, p7
    char narrow (char_type, char) const;

    const char_type*
    narrow (const char_type *__lo, const char_type *__hi,
            char __dfault, char *__to) const {
        return do_narrow (__lo, __hi, __dfault, __to);
    }

    static _RW::__rw_facet_id id;

protected:

    virtual ~ctype ();

    // 22.2.1.1.2, p1
    virtual bool do_is (mask, char_type) const;

    virtual const char_type*
    do_is (const char_type*, const char_type*, mask*) const;

    // 22.2.1.1.2, p7
    virtual char_type do_toupper (char_type) const;

    // 22.2.1.1.2, p9
    virtual char_type do_tolower (char_type) const;

    // 22.2.1.1.2, p11
    virtual char_type do_widen (char) const;

    virtual const char*
    do_widen (const char*, const char*, char_type*) const;

    // 22.2.1.1.2, p13
    virtual char
    do_narrow (char_type, char) const;

    virtual const char_type*
    do_toupper (char_type*, const char_type*) const;

    virtual const char_type*
    do_tolower (char_type*, const char_type*) const;

    // 22.2.1.1.2, p3
    virtual const char_type*
    do_scan_is (mask, const char_type*, const char_type*) const;

    // 22.2.1.1.2, p5
    virtual const char_type*
    do_scan_not (mask, const char_type*, const char_type*) const;

    virtual const char_type*
    do_narrow (const char_type*, const char_type*, char, char*) const;

private:

    friend class ctype_byname<wchar_t>;

    const mask      *_C_mask_tab;         // mask for each character
    unsigned char   *_C_upper_tab;        // uppercase version of each character
    unsigned char   *_C_lower_tab;        // lowercase version of each character
    char             _C_narrow_tab [256]; // cached results of do_narrow()
    char_type        _C_wide_tab [256];   // cached results of do_widen()
    int              _C_delete_it;        // delete tables above?
};


inline char
ctype<wchar_t>::narrow (char_type __c, char __dfault) const
{
    // carefully convert wchar_t to the unsigned form of its
    // underlying integer type, avoiding sign extension and
    // similar issues
    const _RWSTD_UWCHAR_INT_T __inx =
        _RWSTD_STATIC_CAST (_RWSTD_UWCHAR_INT_T, __c);

    // optimize away all but the first call to the virtual do_widen()
    if (   __inx < sizeof _C_narrow_tab / sizeof *_C_narrow_tab
        && _C_narrow_tab [__inx])
        return _C_narrow_tab [__inx];

    // template argument provided to work around an HP aCC bug (PR #27087)
    ctype<wchar_t>* const __self = _RWSTD_CONST_CAST (ctype<wchar_t>*, this);

    const char __ch = do_narrow (__c, __dfault);

    if (__inx < sizeof _C_narrow_tab / sizeof *_C_narrow_tab && __ch != __dfault)
        __self->_C_narrow_tab [__inx] = __ch;

    return __ch;
}


inline ctype<wchar_t>::char_type
ctype<wchar_t>::widen (char __ch) const
{
    const _RWSTD_SIZE_T __inx = _RWSTD_STATIC_CAST (unsigned char, __ch);

    // optimize away all but the first call to the virtual do_widen()

#if _RWSTD_CHAR_BIT <= 8
    const bool __fits = true;
#else
    const bool __fits = __inx < sizeof _C_wide_tab / sizeof *_C_wide_tab;
#endif

    if (__fits) {
        if (_C_wide_tab [__inx])
            return _C_wide_tab [__inx];

        // template argument provided to work around an HP aCC bug (PR #27087)
        ctype<wchar_t>* const __self = _RWSTD_CONST_CAST (ctype<wchar_t>*, this);

        return __self->_C_wide_tab [__inx] = do_widen (__ch);
    }
    else
        return do_widen (__ch);
}



_RWSTD_SPECIALIZED_CLASS
class _RWSTD_EXPORT ctype_byname<wchar_t>: public ctype<wchar_t>
{
public:

    explicit ctype_byname (const char*, _RWSTD_SIZE_T = 0);

protected:

    virtual ~ctype_byname ();

    virtual bool
    do_is (mask, char_type) const;

    virtual const char_type*
    do_is (const char_type*, const char_type*, mask*) const;

    virtual char_type
    do_toupper (char_type) const;

    virtual const char_type*
    do_toupper (char_type*, const char_type*) const;

    virtual char_type
    do_tolower (char_type) const;

    virtual const char_type*
    do_tolower (char_type*, const char_type*) const;

    virtual const char_type*
    do_scan_is (mask, const char_type*, const char_type*) const;

    virtual const char_type*
    do_scan_not (mask, const char_type*, const char_type*) const;
    
    virtual char
    do_narrow(char_type, char) const;

    virtual const char_type*
    do_narrow (const char_type*, const char_type*, char, char*) const;

    virtual char_type do_widen (char) const;
 
    virtual const char*
    do_widen (const char*, const char*, char_type*) const;

private:

    char           _C_namebuf [32];
    const void    *_C_cvtimpl;     // codecvt database mapped by ctype facet
    _RWSTD_SIZE_T  _C_cvtsize;     // size of the above mapping
};

#endif   // _RWSTD_NO_WCHAR_T


}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE (CTYPE)
#  include <loc/_ctype.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE (CTYPE)


#endif   // _RWSTD_LOC_CTYPE_H_INCLUDED
