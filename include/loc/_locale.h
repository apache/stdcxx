/***************************************************************************
 *
 * _locale.h - definition of the std::locale class
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
 * Copyright 1994-2005 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_LOC_LOCALE_INCLUDED
#define _RWSTD_LOC_LOCALE_INCLUDED


#include <string>

#include <loc/_facet.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) { 

enum __rw_locale_category {
    __rw_cat_none     = 0x0100,
    __rw_cat_collate  = 0x0200,
    __rw_cat_ctype    = 0x0400,
    __rw_cat_monetary = 0x0800,
    __rw_cat_numeric  = 0x1000,
    __rw_cat_time     = 0x2000,
    __rw_cat_messages = 0x4000,
    __rw_cat_all      = 0x7e00
};


class _RWSTD_EXPORT __rw_locale;

inline const __rw_facet*
__rw_get_std_facet (const _STD::locale&, __rw_facet::_C_facet_type,
                    __rw_facet::_C_ctor_t*);

inline const __rw_facet*
__rw_get_facet_by_id (const _STD::locale&,
                      const __rw_facet_id&,
                      const __rw_facet*);

// called from has_facet and use_facet to strips top level
// const-qualifier from the type of Facet
template <class _Facet>
inline const __rw_facet*
__rw_get_facet (const _STD::locale&, const _Facet*);

}   // namespace __rw


_RWSTD_NAMESPACE (std) {


// 22.1.2, p1
template <class _Facet>
inline const _Facet&
use_facet (const locale &__loc _RWSTD_DUMMY_ARG (const _Facet* = 0))
{
    // Facet could be a const SomeFacet; strip the const qualifier
    // and, for non-standard facets, use dynamic_cast to make sure
    // that the dynamic type of `pfacet' is the same as Facet
    const _RW::__rw_facet* const __pfacet =
        _RW::__rw_get_facet (__loc, (_Facet*)0);

    _RWSTD_REQUIRES (__pfacet,
                     (_RWSTD_ERROR_BAD_CAST,
                      _RWSTD_FUNC ("use_facet (const locale&)")));

    return *_RWSTD_STATIC_CAST (const _Facet*, __pfacet);
}


// 22.1.2, p5
template <class _Facet>
inline bool
has_facet (const locale &__loc _RWSTD_DUMMY_ARG (const _Facet* = 0))
{
    // Facet could be a const SomeFacet; strip the const qualifier 
    const _RW::__rw_facet* const __pfacet =
        _RW::__rw_get_facet (__loc, (_Facet*)0);

    return 0 != __pfacet;
}


// forward declare standard facets before referencing them below
_EXPORT template <class _InternT, class _ExternT, class _StateT>
class codecvt;

_EXPORT template <class _CharT>
class collate;

_EXPORT template <class _CharT>
class ctype;

_EXPORT template <class _CharT, class _Iter>
struct money_get;

_EXPORT template <class _CharT, class _Iter>
struct money_put;

_EXPORT template <class _CharT, bool _Intl>
struct moneypunct;

_EXPORT template <class _CharT, class _Iter>
struct num_get;

_EXPORT template <class _CharT, class _Iter>
struct num_put;

_EXPORT template <class _CharT>
struct numpunct;

_EXPORT template <class _CharT, class _Iter>
class time_get;

_EXPORT template <class _CharT, class _Iter>
struct time_put;

_EXPORT template <class _CharT>
class messages;

 
// 22.1.1
struct _RWSTD_CLASS_EXPORT locale
{
    // 22.1.1.1.1, p1
    typedef int category;

    // 22.1.1.1.2
    typedef _RW::__rw_facet facet;

    // 22.1.1.1.3
    typedef _RW::__rw_facet_id id;

    // values (including none) must not conflict with any LC_XXX constants
    _RWSTD_STATIC_CONST (category, none     = _RW::__rw_cat_none);
    _RWSTD_STATIC_CONST (category, collate  = _RW::__rw_cat_collate);
    _RWSTD_STATIC_CONST (category, ctype    = _RW::__rw_cat_ctype);
    _RWSTD_STATIC_CONST (category, monetary = _RW::__rw_cat_monetary);
    _RWSTD_STATIC_CONST (category, numeric  = _RW::__rw_cat_numeric);
    _RWSTD_STATIC_CONST (category, time     = _RW::__rw_cat_time);
    _RWSTD_STATIC_CONST (category, messages = _RW::__rw_cat_messages);
    _RWSTD_STATIC_CONST (category, all      = _RW::__rw_cat_all);

    // 22.1.1.2, p1
    _RWSTD_MEMBER_EXPORT locale () _THROWS (());

    // 22.1.1.2, p3
    _RWSTD_MEMBER_EXPORT locale (const locale&) _THROWS (());

    // 22.1.1.2, p6
    _RWSTD_MEMBER_EXPORT explicit locale (const char*);

    // 22.1.1.2, p9
    _RWSTD_MEMBER_EXPORT locale (const locale&, const char*, category);

    // 22.1.1.2, p12
    template <class _Facet>
    locale (const locale&, _Facet*);

#ifndef _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE

    // 22.1.1.3, p1
    template <class _Facet>
    locale combine (const locale&) const;

#endif   // _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE

    // 22.1.1.4, p3
    template <class _CharT, class _Traits, class _Allocator>
    bool operator() (const basic_string<_CharT, _Traits, _Allocator>&,
                     const basic_string<_CharT, _Traits, _Allocator>&) const;

    // 22.1.1.2, p14
    _RWSTD_MEMBER_EXPORT locale (const locale&, const locale&, category);

    // 22.1.1.2, p16
    _RWSTD_MEMBER_EXPORT ~locale() _THROWS (());

    // 22.1.1.2, p4
    _RWSTD_MEMBER_EXPORT const locale& operator= (const locale&) _THROWS (());

    // 22.1.1.3, p5
    _RWSTD_MEMBER_EXPORT string name () const;

    // 22.1.1.4, p1
    _RWSTD_MEMBER_EXPORT bool operator== (const locale&) const;

    // 22.1.1.4, p2
    _RWSTD_MEMBER_EXPORT bool operator!= (const locale &__rhs) const {
        return !(*this == __rhs);
    }

    // 22.1.1.5, p1
    _RWSTD_MEMBER_EXPORT static locale global (const locale&);

    // 22.1.1.5, p4
    _RWSTD_MEMBER_EXPORT static const locale& classic ();

private:

    _RWSTD_MEMBER_EXPORT explicit locale (_RW::__rw_locale &__rhs) 
        _THROWS (())
        : _C_body (&__rhs) { }

    _RWSTD_MEMBER_EXPORT locale (_RW::__rw_locale&, const facet*);

    _RW::__rw_locale *_C_body;     // reference-counted body

#if    defined (__SUNPRO_CC) && __SUNPRO_CC <= 0x540

   // working around a SunPro member access control bug (see PR #25910)
public:

#endif   // SunPro <= 5.4

    // (try to) retrieve a facet from a locale given an id
    // may initialize the id if it isn't initialized yet
    _RWSTD_MEMBER_EXPORT const facet*
    _C_get_facet (const id&) const;

    _RWSTD_MEMBER_EXPORT const facet*
    _C_get_std_facet (facet::_C_facet_type, facet::_C_ctor_t*) const;

    friend class  _RW::__rw_locale;
    friend struct _RW::__rw_access;

    // function arguments below are qualified to work around
    // a SunPro 5.5 and prior bug (PR #28626)
    friend const facet*
    _RW::__rw_get_std_facet (const _STD::locale&,
                             _RW::__rw_facet::_C_facet_type,
                             _RW::__rw_facet::_C_ctor_t*);

    friend const _RW::__rw_facet*
    _RW::__rw_get_facet_by_id (const _STD::locale&,
                               const _RW::__rw_facet_id&,
                               const _RW::__rw_facet*);
};

}   // namespace std


_RWSTD_NAMESPACE (__rw) {

inline const __rw_facet*
__rw_get_facet_by_id (const _STD::locale &__loc,
                      const __rw_facet_id      &__id,
                      const __rw_facet*)
{
    // initialize Facet::id and retrieve facet if it's installed
    return __loc._C_get_facet (__id);
}


template <class _Facet>
inline const __rw_facet*
__rw_get_facet (const _STD::locale &__loc, const _Facet *__dummy)
{
    // resolves either to one of the overloads of __rw_get_facet_by_id
    // defined for the set of standard facets or the overload for the base
    // locale::facet* if _Facet is not derived from any of the former
    const __rw_facet* const __pfacet =
        __rw_get_facet_by_id (__loc, _Facet::id, __dummy);

    return _RWSTD_DYNAMIC_CAST (const _Facet*, __pfacet);
}


inline const __rw_facet*
__rw_get_std_facet (const _STD::locale  &__loc,
                    __rw_facet::_C_facet_type  __type,
                    __rw_facet::_C_ctor_t     *__ctor)
{
    // expose an implementation detail of locale body for efficiency
    struct _Facets {
        __rw_facet *__facets [__rw_facet::_C_last_type / 2];
    };

    const _Facets* const __pfacets =
        _RWSTD_REINTERPRET_CAST (const _Facets*, __loc._C_body);

    _RWSTD_ASSERT (0 != __pfacets);

    const __rw_facet *__facet = __pfacets->__facets [__type >> 1];

    if (!__facet)
        __facet = __loc._C_get_std_facet (__type, __ctor);

#if !defined (__HP_aCC) || _RWSTD_HP_aCC_MINOR > 3800

    // working around an HP aCC ICE (PR #28838)
    _RWSTD_ASSERT (0 != __facet);

#endif   // HP aCC

    return __facet;
}


// functions' type must be __rw_facet::_C_ctor_t
// i.e., as if they were declared as follows:
//     __rw_facet::_C_ctor_t __rw_ct_ctype;
// the declaration above causes a Compaq C++ 6.5 ICE (PR #30188)
_RWSTD_EXPORT __rw_facet* __rw_ct_ctype (_RWSTD_SIZE_T, const char*);
_RWSTD_EXPORT __rw_facet* __rw_ct_numpunct (_RWSTD_SIZE_T, const char*);
_RWSTD_EXPORT __rw_facet* __rw_ct_num_get (_RWSTD_SIZE_T, const char*);
_RWSTD_EXPORT __rw_facet* __rw_ct_num_put (_RWSTD_SIZE_T, const char*);


#define _RWSTD_OVERLOAD_GET_FACET(F)                 \
    _RWSTD_EXPORT const __rw_facet*                  \
    __rw_get_facet_by_id (const _STD::locale&,       \
                          const __rw_facet_id&, const F*)

#define _Facet(name, T, U)   _STD::name<T, U >

#define _InIter(T)    _STD::istreambuf_iterator< T, _STD::char_traits<T > >
#define _OutIter(T)   _STD::ostreambuf_iterator< T, _STD::char_traits<T > >

// convenience typedefs to allow them to be used as macro arguments
typedef _STD::codecvt<char, char, _RWSTD_MBSTATE_T>    __rw_codecvt_c;
typedef _Facet (moneypunct, char, false)               __rw_mpunct_c_0;
typedef _Facet (moneypunct, char, true)                __rw_mpunct_c_1;
typedef _Facet (money_get, char, _InIter (char))       __rw_mget_c;
typedef _Facet (money_put, char, _OutIter (char))      __rw_mput_c;
typedef _Facet (num_get, char, _InIter (char))         __rw_nget_c;
typedef _Facet (num_put, char, _OutIter (char))        __rw_nput_c;
typedef _Facet (time_get, char, _InIter (char))        __rw_tget_c;
typedef _Facet (time_put, char, _OutIter (char))       __rw_tput_c;


// inline definitions of heavily used use_facet specializations
// for iostream efficiency

_RWSTD_SPECIALIZED_FUNCTION
inline const __rw_facet*
__rw_get_facet (const _STD::locale &__loc, const _STD::ctype<char>*)
{
    return __rw_get_std_facet (__loc, __rw_facet::_C_ctype,
                               __rw_ct_ctype);
}


_RWSTD_SPECIALIZED_FUNCTION
inline const __rw_facet*
__rw_get_facet (const _STD::locale &__loc, const _STD::numpunct<char>*)
{
    return __rw_get_std_facet (__loc, __rw_facet::_C_numpunct,
                               __rw_ct_numpunct);
}


_RWSTD_SPECIALIZED_FUNCTION
inline const __rw_facet*
__rw_get_facet (const _STD::locale &__loc, const __rw_nget_c*)
{
    return __rw_get_std_facet (__loc, __rw_facet::_C_num_get,
                               __rw_ct_num_get);
}


_RWSTD_SPECIALIZED_FUNCTION
inline const __rw_facet*
__rw_get_facet (const _STD::locale &__loc, const __rw_nput_c*)
{
    return __rw_get_std_facet (__loc, __rw_facet::_C_num_put,
                               __rw_ct_num_put);
}


// declarations of __rw_get_facet_by_id overloads
_RWSTD_OVERLOAD_GET_FACET (_STD::ctype<char>);
_RWSTD_OVERLOAD_GET_FACET (__rw_codecvt_c);
_RWSTD_OVERLOAD_GET_FACET (_STD::collate<char>);
_RWSTD_OVERLOAD_GET_FACET (__rw_mget_c);
_RWSTD_OVERLOAD_GET_FACET (__rw_mput_c);
_RWSTD_OVERLOAD_GET_FACET (__rw_mpunct_c_0);
_RWSTD_OVERLOAD_GET_FACET (__rw_mpunct_c_1);
_RWSTD_OVERLOAD_GET_FACET (__rw_nget_c);
_RWSTD_OVERLOAD_GET_FACET (__rw_nput_c);
_RWSTD_OVERLOAD_GET_FACET (_STD::numpunct<char>);
_RWSTD_OVERLOAD_GET_FACET (__rw_tget_c);
_RWSTD_OVERLOAD_GET_FACET (__rw_tput_c);
_RWSTD_OVERLOAD_GET_FACET (_STD::messages<char>);

#ifndef _RWSTD_NO_WCHAR_T

typedef _STD::codecvt<wchar_t, char, _RWSTD_MBSTATE_T>    __rw_codecvt_w;
typedef _Facet (moneypunct, wchar_t, false)               __rw_mpunct_w_0;
typedef _Facet (moneypunct, wchar_t, true)                __rw_mpunct_w_1;
typedef _Facet (money_get, wchar_t, _InIter (wchar_t))    __rw_mget_w;
typedef _Facet (money_put, wchar_t, _OutIter (wchar_t))   __rw_mput_w;
typedef _Facet (num_get, wchar_t, _InIter (wchar_t))      __rw_nget_w;
typedef _Facet (num_put, wchar_t, _OutIter (wchar_t))     __rw_nput_w;
typedef _Facet (time_get, wchar_t, _InIter (wchar_t))     __rw_tget_w;
typedef _Facet (time_put, wchar_t, _OutIter (wchar_t))    __rw_tput_w;

_RWSTD_OVERLOAD_GET_FACET (_STD::ctype<wchar_t>);
_RWSTD_OVERLOAD_GET_FACET (__rw_codecvt_w);
_RWSTD_OVERLOAD_GET_FACET (_STD::collate<wchar_t>);
_RWSTD_OVERLOAD_GET_FACET (__rw_mget_w);
_RWSTD_OVERLOAD_GET_FACET (__rw_mput_w);
_RWSTD_OVERLOAD_GET_FACET (__rw_mpunct_w_0);
_RWSTD_OVERLOAD_GET_FACET (__rw_mpunct_w_1);
_RWSTD_OVERLOAD_GET_FACET (__rw_nget_w);
_RWSTD_OVERLOAD_GET_FACET (__rw_nput_w);
_RWSTD_OVERLOAD_GET_FACET (_STD::numpunct<wchar_t>);
_RWSTD_OVERLOAD_GET_FACET (__rw_tget_w);
_RWSTD_OVERLOAD_GET_FACET (__rw_tput_w);
_RWSTD_OVERLOAD_GET_FACET (_STD::messages<wchar_t>);

#endif   // _RWSTD_NO_WCHAR_T

// clean up
#undef _RWSTD_OVERLOAD_GET_FACET
#undef _Facet
#undef _InIter
#undef _OutIter


}   // namespace __rw


_RWSTD_NAMESPACE (std) {


template <class _Facet>
inline locale::locale (const locale &__rhs, _Facet *__facet)
    : _C_body (0)
{
    // initialize facet's member id to point to Facet::id
    if (__facet && !__facet->_C_pid) {
        // unusual cast (to size_t** rather than size_t*&)
        // done to work around an HP aCC 3.3x bug
        *_RWSTD_CONST_CAST (_RWSTD_SIZE_T**, &__facet->_C_pid) =
            &_Facet::id._C_id;
    }

    // uninitialized Facet::id will be initialized by has_facet<>()

    // prevent passing `facet' the ctor if it is installed in `rhs'
    // to potentially avoid creating a whole new locale body
    if (_RW::__rw_get_facet (__rhs, (_Facet*)0) == __facet)
        __facet = 0;

    *this = locale (*__rhs._C_body, __facet);
}


#ifndef _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE

template <class _Facet>
inline locale locale::combine (const locale &__rhs) const
{
    // unitialized Facet::id will be initialized by has_facet<>()

    _RWSTD_REQUIRES (has_facet<_Facet>(__rhs),
                     (_RWSTD_ERROR_FACET_NOT_FOUND,
                      _RWSTD_FUNC ("locale::combine (const locale&)"),
                      _Facet::id._C_id, __rhs.name ().c_str ()));

    return locale (*this, &use_facet<_Facet>(__rhs));
}

#endif   // _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE


template <class _CharT, class _Traits, class _Allocator>
inline bool
locale::operator() (const basic_string<_CharT, _Traits, _Allocator> &__x,
                    const basic_string<_CharT, _Traits, _Allocator> &__y) const
{
    // qualify collate to distinguish it from locale::collate
    return use_facet<_STD::collate<_CharT> >(*this)
        .compare (__x.data (), __x.data () + __x.length (),
                  __y.data (), __y.data () + __y.length ()) < 0;
}


}   // namespace std


#endif   // _RWSTD_LOC_LOCALE_INCLUDED
