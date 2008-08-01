/***************************************************************************
 *
 * _codecvt.h - Definitions of the code conversion facets
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
 * Copyright 1994-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _RWSTD_LOC_CODECVT_H_INCLUDED
#define _RWSTD_LOC_CODECVT_H_INCLUDED


#include <rw/_defs.h>
#include <loc/_facet.h>
#include <rw/_mbstate.h>   // for _RWSTD_MBSTATE_T


_RWSTD_NAMESPACE (__rw) { 

struct __rw_ctype_t;

}   // namespace __rw

_RWSTD_NAMESPACE (std) { 

// 22.2.1.5 - Template class codecvt

struct codecvt_base
{
    enum result { ok, partial, error, noconv };
};


_EXPORT
template <class _InternT, class _ExternT, class _StateT>
class codecvt;


#ifndef _RWSTD_NO_EXT_CODECVT_PRIMARY

_EXPORT
template <class _InternT, class _ExternT, class _StateT>
class codecvt: public _RW::__rw_facet,
               public codecvt_base
{
public:
    typedef _InternT intern_type;
    typedef _ExternT extern_type;
    typedef _StateT  state_type;

protected:

    // extension: where reasonable, the default behavior of the primary
    // template is the same as that of codecvt<char, char, mbstate_t> if
    // sizeof (intern_type) == sizeof (extern_type) to allow iostream
    // instantiations on signed char or unsigned char to have somewhat
    // meaningful behavior

    virtual result
    do_out (state_type&,
            const intern_type*, const intern_type*, const intern_type*&,
            extern_type*, extern_type*, extern_type*&) const {
        return sizeof (intern_type) == sizeof (extern_type) ? noconv : error;
    }

    virtual result
    do_in (state_type&,
           const extern_type*, const extern_type*, const extern_type*&,
           intern_type*, intern_type*, intern_type*&) const {
        return sizeof (intern_type) == sizeof (extern_type) ? noconv : error;
    }

    virtual result
    do_unshift (state_type&, extern_type*, extern_type*, extern_type*&) const {
        return sizeof (intern_type) == sizeof (extern_type) ? noconv : error;
    }

    virtual int do_encoding () const _THROWS (()) {
        return -1;   // unknown, possibly state dependent
    }

    virtual bool do_always_noconv () const _THROWS (()) {
        return sizeof (intern_type) == sizeof (extern_type);
    }

    virtual int do_max_length () const _THROWS (()) {
        return _RWSTD_INT_MAX;
    }

    virtual int
    do_length (state_type&, 
               const extern_type*, const extern_type*, _RWSTD_SIZE_T) const {
        return _RWSTD_INT_MAX;
    }

public:

    explicit codecvt (_RWSTD_SIZE_T __ref = 0): _RW::__rw_facet (__ref) { }

    virtual ~codecvt () _RWSTD_ATTRIBUTE_NOTHROW;

    // 22,2,1,5,1, p1
    result out (state_type& __state,
                const intern_type* __from, const intern_type* __from_end,
                const intern_type*& __from_next,
                extern_type* __to, extern_type* __to_limit,
                extern_type*& __to_next) const {
        return do_out(__state,__from,__from_end,__from_next,
                      __to,__to_limit,__to_next);
    }

    // 22,2,1,5,1, p2
    result unshift (state_type& __state, extern_type* __to,
                    extern_type* __to_limit, extern_type*& __to_next) const {
        return do_unshift (__state,__to,__to_limit,__to_next);
    }

    // 22,2,1,5,1, p3
    result in (state_type& __state, const extern_type* __from,
               const extern_type* __from_end, const extern_type*& __from_next,
               intern_type* __to, intern_type* __to_limit,
               intern_type*& __to_next) const {
        return do_in (__state,__from,__from_end,__from_next,
                      __to,__to_limit,__to_next);
    }

    // 22,2,1,5,1, p5
    bool always_noconv () const _THROWS (()) {
        return do_always_noconv();
    }

    // 22,2,1,5,1, p4
    int encoding () const _THROWS (()) {
        return do_encoding();
    }

    // 22,2,1,5,1, p6, lwg issue 75
    int length (state_type& __state, const extern_type* __from,
                const extern_type* __end, _RWSTD_SIZE_T __maxlen) const {
        return do_length (__state, __from, __end, __maxlen);
    }

    // 22,2,1,5,1, p7
    int max_length() const _THROWS (())  {
        return do_max_length();
    }

    static _RW::__rw_facet_id id;

};

#endif   // _RWSTD_NO_EXT_CODECVT_PRIMARY


// 22.2.1.5, p3 - performs no conversion

_RWSTD_SPECIALIZED_CLASS
class _RWSTD_EXPORT codecvt<char, char, _RWSTD_MBSTATE_T>
    : public _RW::__rw_facet,
      public codecvt_base
{
public:

    typedef char             extern_type;   
    typedef char             intern_type;
    typedef _RWSTD_MBSTATE_T state_type;

    explicit codecvt (_RWSTD_SIZE_T = 0);

    virtual ~codecvt () _RWSTD_ATTRIBUTE_NOTHROW;

    result out (state_type &__state,
                const intern_type *__from, const intern_type* __from_end,
                const intern_type *&__from_next,
                extern_type* __to, extern_type* __to_limit,
                extern_type *&__to_next) const {
        return do_out (__state, __from, __from_end, __from_next,
                       __to, __to_limit, __to_next);
    }

    result unshift (state_type &__state,
                    extern_type *__to, extern_type *__to_limit,
                    extern_type*& __to_next) const {
        return do_unshift (__state, __to, __to_limit, __to_next);
    }

    result in (state_type &__state,
               const extern_type *__from, const extern_type *__from_end,
               const extern_type *&__from_next,
               intern_type *__to, intern_type *__to_limit,
               intern_type *&__to_next) const {
        return do_in (__state, __from, __from_end, __from_next,
                      __to, __to_limit, __to_next);
    }

    int encoding () const _THROWS (()) {
        return do_encoding ();
    }

    bool always_noconv () const _THROWS (());

    // signature follows lwg issue 75
    int length (state_type &__state, const extern_type *__from,
                const extern_type *__end, _RWSTD_SIZE_T __maxlen) const {
        return do_length (__state, __from, __end, __maxlen);
    }

    int max_length () const _THROWS (()) {
        return do_max_length ();
    }

    static _RW::__rw_facet_id id;

protected:

    virtual result
    do_out (state_type&,
            const intern_type*, const intern_type*, const intern_type*&,
            extern_type*, extern_type*, extern_type*&) const;

    virtual result
    do_in (state_type&,
           const extern_type*, const extern_type*, const extern_type*&,
           intern_type*, intern_type*, intern_type*&) const;

    // 22.2.1.5.2, p5 - stores no characters
    virtual result
    do_unshift (state_type&,
                extern_type*, extern_type*, extern_type*&) const;

    virtual int do_encoding () const _THROWS (());

    virtual bool do_always_noconv () const _THROWS (());

    // signature follows lwg issue 75
    virtual int
    do_length (state_type&,
               const extern_type*, const extern_type*, _RWSTD_SIZE_T) const;

    virtual int do_max_length () const _THROWS (());

private:

    int _C_always_noconv;   // cached value
};


inline bool
codecvt<char, char, _RWSTD_MBSTATE_T>::
always_noconv () const _THROWS (())
{
    // optimize away repeated calls to the virtual function
    if (_C_always_noconv < 0) {

        // typedef works around an HP aCC x.38 bug (PR #25832)
        typedef codecvt<char, char, _RWSTD_MBSTATE_T> _CodeCvt;

        _CodeCvt *__self = _RWSTD_CONST_CAST (_CodeCvt*, this);

        __self->_C_always_noconv = do_always_noconv ();
    }

    return 1 == _C_always_noconv;
}


#ifndef _RWSTD_NO_WCHAR_T

// 22.2.1.5, p3 - converts between wide and narrow characters
// of the native character set (i.e., widens and narrows)

_RWSTD_SPECIALIZED_CLASS
class _RWSTD_EXPORT codecvt<wchar_t, char, _RWSTD_MBSTATE_T>
    : public _RW::__rw_facet,
      public codecvt_base
{
public:

    typedef wchar_t          intern_type;
    typedef char             extern_type;
    typedef _RWSTD_MBSTATE_T state_type;

protected:

    virtual result
    do_out (state_type&, const intern_type*, const intern_type*,
            const intern_type*&,
            extern_type*, extern_type*, extern_type*&) const;

    virtual result
    do_in (state_type&,
           const extern_type*, const extern_type*, const extern_type*&,
           intern_type*, intern_type*, intern_type*&) const;

    // 22.2.1.5.2, p5 - stores no characters
    virtual result
    do_unshift (state_type&, extern_type*, extern_type*, extern_type*&) const;

    virtual bool do_always_noconv () const _THROWS (());

    virtual int do_encoding () const _THROWS (());

    // signature follows lwg issue 75
    virtual int
    do_length (state_type&,
               const extern_type*, const extern_type*, _RWSTD_SIZE_T) const;

    virtual int do_max_length () const _THROWS (());

public:

    explicit codecvt (_RWSTD_SIZE_T = 0);

    virtual ~codecvt () _RWSTD_ATTRIBUTE_NOTHROW;

    result out (state_type& __state,
                const intern_type *__from, const intern_type *__from_end,
                const intern_type *&__from_next,
                extern_type *__to, extern_type* __to_limit,
                extern_type *& __to_next) const {
        return do_out (__state, __from, __from_end, __from_next, __to,
                       __to_limit, __to_next);
    }

    result unshift (state_type &__state,
                    extern_type *__to, extern_type *__to_limit,
                    extern_type *&__to_next) const {
        return do_unshift (__state, __to, __to_limit, __to_next);
    }

    result in (state_type& __state,
               const extern_type *__from, const extern_type *__from_end,
               const extern_type *&__from_next,
               intern_type *__to, intern_type *__to_limit,
               intern_type *&__to_next) const {
        return do_in (__state, __from, __from_end, __from_next,
                      __to, __to_limit, __to_next);
    }

    int encoding () const _THROWS (()) {
        return do_encoding();
    }

    bool always_noconv () const _THROWS (()) {
        return do_always_noconv ();
    }

    // signature follows lwg issue 75
    int length (state_type &__state,
                const extern_type *__from, const extern_type *__end,
                _RWSTD_SIZE_T __maxlen) const {
        return do_length (__state, __from, __end, __maxlen);
    }

    int max_length () const _THROWS (()) {
        return do_max_length ();
    }

    static _RW::__rw_facet_id id;
};


#endif // _RWSTD_NO_WCHAR_T


// 22.2.1.6
template <class _InternT, class _ExternT, class _StateT>
class codecvt_byname: public codecvt<_InternT, _ExternT, _StateT>
{
    char _C_namebuf [32];

public:

    typedef _InternT intern_type;
    typedef _ExternT extern_type;
    typedef _StateT  state_type; 

    explicit codecvt_byname (const char *__name, _RWSTD_SIZE_T __ref = 0)
        : codecvt <_InternT, _ExternT, _StateT> (__ref) {
        this->_C_set_name (__name, _C_namebuf, sizeof _C_namebuf);
    }

    virtual ~codecvt_byname () _RWSTD_ATTRIBUTE_NOTHROW;
};


_RWSTD_SPECIALIZED_CLASS
class _RWSTD_EXPORT codecvt_byname<char, char, _RWSTD_MBSTATE_T>
    : public codecvt<char, char, _RWSTD_MBSTATE_T>
{
    char _C_namebuf [32];

public:

    explicit codecvt_byname (const char*, _RWSTD_SIZE_T = 0);

    virtual ~codecvt_byname () _RWSTD_ATTRIBUTE_NOTHROW;
};



#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
class _RWSTD_EXPORT codecvt_byname<wchar_t, char, _RWSTD_MBSTATE_T>
    : public codecvt<wchar_t, char, _RWSTD_MBSTATE_T>
{
    char _C_namebuf [32];

    unsigned _C_flags;

public:

    explicit codecvt_byname (const char*, _RWSTD_SIZE_T = 0);

    virtual ~codecvt_byname () _RWSTD_ATTRIBUTE_NOTHROW;

protected:

    virtual codecvt_base::result
    do_out (state_type&,
            const intern_type*, const intern_type*, const intern_type*&,
            extern_type*, extern_type*, extern_type*&) const;

    virtual codecvt_base::result
    do_in (state_type&,
           const extern_type*, const extern_type*, const extern_type*&,
           intern_type*, intern_type*, intern_type*&) const;


    virtual codecvt_base::result
    do_unshift (state_type&,
                extern_type*, extern_type*, extern_type*&) const;

    virtual int do_encoding () const _THROWS (()); 

    // signature follows lwg issue 75
    virtual int
    do_length (state_type&, const extern_type*, const extern_type*,
               _RWSTD_SIZE_T) const;

    virtual int do_max_length () const _THROWS (());
};


#endif   // _RWSTD_NO_WCHAR_T


}   // namespace std


#if _RWSTD_DEFINE_TEMPLATE (_CODECVT)
#  include <loc/_codecvt.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE (_CODECVT)


#endif   // _RWSTD_LOC_CODECVT_H_INCLUDED
