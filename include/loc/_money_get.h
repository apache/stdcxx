/***************************************************************************
 *
 * _money_get.h - definition of the std::money_get class templates
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
 *
 * $Id: //stdlib/dev/include/loc/_money_get.h#26 $
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

#ifndef _RWSTD_LOC_MONEY_GET_H_INCLUDED
#define _RWSTD_LOC_MONEY_GET_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#include <string>
#include <rw/_ioiter.h>
#include <rw/_iosfwd.h>
#include <loc/_facet.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (_V3_LOCALE) { 


_EXPORT
template <class _CharT,
    class _InputIter _RWSTD_COMPLEX_DEFAULT (_RWSTD_ISTREAMBUF_ITER (_CharT)) >
struct money_get: _RW::__rw_facet
{
    typedef _CharT     char_type;
    typedef _InputIter iter_type;
    typedef
    basic_string<char_type, char_traits<char_type>, allocator<char_type> >
    string_type;

    _EXPLICIT money_get (_RWSTD_SIZE_T __ref = 0)
        : _RW::__rw_facet (__ref) { }

    iter_type get (iter_type __begin, iter_type __end, bool __intl,
                   ios_base &__flags, _RWSTD_IOSTATE &__err,
                   long double &__val) const {
        return do_get (__begin, __end, __intl, __flags, __err, __val);
    }

    iter_type get (iter_type __begin, iter_type __end, bool __intl,
                   ios_base &__flags, _RWSTD_IOSTATE &__err,
                   string_type &__str) const {
        return do_get (__begin, __end, __intl, __flags, __err, __str);
    }

    static _RW::__rw_facet_id id;

protected:

#ifndef _RWSTD_NO_LONG_DOUBLE

    virtual iter_type
    do_get (iter_type __it, iter_type __end, bool __intl, ios_base &__fl,
            _RWSTD_IOSTATE &__err, long double &__val) const {
        return _C_get (__it, __end, __intl, __fl, __err, &__val, 0);
    }

#else   // if defined (_RWSTD_NO_LONG_DOUBLE)

    virtual iter_type
    do_get (iter_type __it, iter_type __end, bool __intl, ios_base &__fl,
            _RWSTD_IOSTATE &__err, double &__val) const {
        return _C_get (__it, __end, __intl, __fl, __err, &__val, 0);
    }

#endif   // _RWSTD_NO_LONG_DOUBLE

    virtual iter_type
    do_get (iter_type __it, iter_type __end, bool __intl, ios_base &__fl,
            _RWSTD_IOSTATE &__err, string_type &__str) const {
        return _C_get (__it, __end, __intl, __fl, __err, 0, &__str);
    }

private:

    iter_type
    _C_get (iter_type, iter_type, bool, ios_base&,
            _RWSTD_IOSTATE&, void*, string_type*) const;
};


#ifndef _RWSTD_NO_SPECIALIZED_FACET_ID

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id money_get<char, _RWSTD_ISTREAMBUF_ITER (char) >::id;

#  ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
_RW::__rw_facet_id money_get<wchar_t, _RWSTD_ISTREAMBUF_ITER (wchar_t) >::id;

#  endif   // _RWSTD_NO_WCHAR_T
#endif   // _RWSTD_NO_SPECIALIZED_FACET_ID

}   // namespace _V3_LOCALE


#if _RWSTD_DEFINE_TEMPLATE_FIRST (_MONEY_GET)
#  include <loc/_money_get.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (_MONEY_GET)


_RWSTD_NAMESPACE (_V3_LOCALE) { 

#if _RWSTD_INSTANTIATE (_MONEY_GET, _CHAR)

_RWSTD_INSTANTIATE_2 (struct _RWSTD_EXPORT
                      money_get<char, _RWSTD_ISTREAMBUF_ITER (char) >);

#endif   // _RWSTD_INSTANTIATE (_MONEY_GET, _CHAR)

#if _RWSTD_INSTANTIATE (_MONEY_GET, _WCHAR_T)

_RWSTD_INSTANTIATE_2 (struct _RWSTD_EXPORT
                      money_get<wchar_t, _RWSTD_ISTREAMBUF_ITER (wchar_t) >);

#endif   // _RWSTD_INSTANTIATE (_MONEY_GET, _WCHAR_T)

}   // namespace _V3_LOCALE


#if _RWSTD_DEFINE_TEMPLATE_LAST (_MONEY_GET)
#  include <loc/_money_get.cc>
#endif   // _RWSTD_DEFINE_TEMPLATE_LAST (_MONEY_GET)


#endif   // _RWSTD_LOC_MONEY_GET_H_INCLUDED
