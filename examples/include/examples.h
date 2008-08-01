/**************************************************************************
 *
 * examples.h - common definitions to condition example code for broken
 *              compilers; must be #included last, _after_ all lib headers.
 *
 * The goal of this file is to allow completely conforming syntax
 * in examples' code even with non-conforming compilers.
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

#ifndef EXAMPLES_H_INCLUDED
#define EXAMPLES_H_INCLUDED


#ifdef _RWSTD_NO_NAMESPACE
# define std
#endif   // _RWSTD_NO_NAMESPACE


#ifdef _RWSTD_NO_FOR_LOCAL_SCOPE
#  define for if (0) ; else for
#endif   // _RWSTD_NO_FOR_LOCAL_SCOPE

#include <cstddef>  // for ptrdiff_t

#ifdef _RWSTD_NO_LIBC_IN_STD

_RWSTD_NAMESPACE (std) { 

// accomdate compilers that don't have size_t in namespace std
_USING (::size_t);
_USING (::ptrdiff_t);

}   // namespace std

#endif   // _RWSTD_NO_LIBC_IN_STD


#ifdef _RWSTD_NO_CLASS_PARTIAL_SPEC

// supply conforming versions of count, count_if and distance for platforms
// that do not correctly support partial specialization of class templates

_RWSTD_NAMESPACE (std) {     
    
template<class InputIter, class T>
size_t count (InputIter a, InputIter b, const T& x)
{
    size_t n = 0;
    count (a, b, x, n);
    return n;
}

template<class InputIter, class T>
size_t count_if (InputIter a, InputIter b, const T& x)
{
    size_t n = 0;
    count_if (a, b, x, n);
    return n;

}

template<class InputIter>
size_t distance (InputIter a, InputIter b)
{
    size_t n = 0;
    distance (a, b, n);
    return n;
} 

}   // namespace std

#endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC




#ifdef _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE

#if !defined (_MSC_VER) || _MSC_VER > 1300

_RWSTD_NAMESPACE (std) { 

#define has_facet fake_has_facet
#define use_facet fake_use_facet

// fake std::use_facet
template <class Facet>
struct has_facet
{
    has_facet (const std::locale &loc)

#ifndef _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE

        : result (std::has_facet<Facet>(loc)) { }

#else

        : result (std::has_facet (loc, (Facet*)0)) { }

#endif   // _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE

    operator bool () const {
        return result;
    }

    bool result;
};


// fake std::use_facet
template <class Facet>
struct use_facet
{
    use_facet (const std::locale &loc)
        : facet (std::_USE_FACET (Facet, loc)) { }

    operator const Facet& () const {
        return facet;
    }

    const Facet &facet;
};

}   // namespace std

#else   // if defined (_MSC_VER) && _MSC_VER <= 1300
#  define _HAS_FACET(facet, locale)   has_facet (locale, (facet*)0)
#endif   // !defined (_MSC_VER) || _MSC_VER > 1300

#endif   // _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE


#ifdef _RWSTD_NO_EXCEPTIONS
#  define try            if (1)
#  define catch(ex)      else if (0) for (ex ; 0 ; )
#  define throw(ignore)  abort ()
#endif   // _RWSTD_NO_EXCEPTIONS


#ifdef _RWSTD_NO_WCHAR_T

// remove `L' from wide string literals
#define L

// fake wide stream objects
#define wcin          cin
#define wcout         cout
#define wcerr         cerr
#define wclog         clog

// fake wide string and stringstream
#define wstring       string
#define wstringstream stringstream

#define wchar_t       char

#endif   // _RWSTD_NO_WCHAR_T


// undefine macros #defined by some evil C libraries (e.g., MSVC)
#undef max
#undef min


#endif   // EXAMPLES_H_INCLUDED
