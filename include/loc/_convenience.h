/***************************************************************************
 *
 * _convenience.h - definitions of locale convenience interfaces
 *                  as described in [lib.locale.convenience]
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

#ifndef _RWSTD_LOC_CONVENIENCE_H_INCLUDED
#define _RWSTD_LOC_CONVENIENCE_H_INCLUDED


#include <loc/_ctype.h>
#include <loc/_locale.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


// convenience interfaces: is*(char)
// names parenthesized to avoid clashing with masking macros


template <class _CharT>
inline bool (isspace)(_CharT __c, const locale &__loc)
{
    return _RWSTD_USE_FACET (ctype<_CharT>, __loc).is (ctype_base::space, __c);
}


template <class _CharT>
inline bool (isprint)(_CharT __c, const locale &__loc)
{
    return _RWSTD_USE_FACET (ctype<_CharT>, __loc).is (ctype_base::print, __c);
}

template <class _CharT>
inline bool (iscntrl)(_CharT __c, const locale &__loc)
{
    return _RWSTD_USE_FACET (ctype<_CharT>, __loc).is(ctype_base::cntrl, __c);
}


template <class _CharT>
inline bool (isupper)(_CharT __c, const locale &__loc)
{
    return _RWSTD_USE_FACET (ctype<_CharT>, __loc).is (ctype_base::upper, __c);
}


template <class _CharT>
inline bool (islower)(_CharT __c, const locale &__loc)
{
    return _RWSTD_USE_FACET (ctype<_CharT>, __loc).is (ctype_base::lower, __c);
}


template <class _CharT>
inline bool (isalpha)(_CharT __c, const locale &__loc)
{
    return _RWSTD_USE_FACET (ctype<_CharT>, __loc).is (ctype_base::alpha, __c);
}


template <class _CharT>
inline bool (isdigit)(_CharT __c, const locale &__loc)
{
    return _RWSTD_USE_FACET (ctype<_CharT>, __loc).is (ctype_base::digit, __c);
}


template <class _CharT>
inline bool (ispunct)(_CharT __c, const locale &__loc)
{
    return _RWSTD_USE_FACET (ctype<_CharT>, __loc).is(ctype_base::punct, __c);
}


template <class _CharT>
inline bool (isxdigit)(_CharT __c, const locale &__loc)
{
    return _RWSTD_USE_FACET (ctype<_CharT>, __loc).is (ctype_base::xdigit,
                                                       __c);
}


template <class _CharT>
inline bool (isalnum)(_CharT __c, const locale &__loc)
{
    return _RWSTD_USE_FACET (ctype<_CharT>, __loc).is (ctype_base::alnum, __c);
}


template <class _CharT>
inline bool (isgraph)(_CharT __c, const locale &__loc)
{
    return _RWSTD_USE_FACET (ctype<_CharT>, __loc).is (ctype_base::graph, __c);
}


template <class _CharT>
inline _CharT (toupper)(_CharT __c, const locale &__loc)
{
    return (_RWSTD_USE_FACET (ctype<_CharT>, __loc).toupper) (__c);
}


template <class _CharT>
inline _CharT (tolower)(_CharT __c, const locale &__loc)
{
    return (_RWSTD_USE_FACET (ctype<_CharT>, __loc).tolower) (__c);
}

}   // namespace std


#endif   // _RWSTD_LOC_CONVENIENCE_H_INCLUDED
