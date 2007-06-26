/***************************************************************************
 *
 * stdlib.h
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

#include <rw/_defs.h>

#ifndef _RWSTD_NO_DEPRECATED_C_HEADERS

#  ifndef _RWSTD_STDLIB_H_INCLUDED
#    define _RWSTD_STDLIB_H_INCLUDED

#    include <cstdlib>

#    ifndef _RWSTD_NO_NAMESPACE

using std::div_t;
using std::ldiv_t;
using std::size_t;

#if !defined (__GNUG__) || __GNUG__ > 2

// prevent collisions with gcc's intrinsic functions
using std::abort;
using std::abs;
using std::exit;
using std::labs;

#endif   // gcc > 2

using std::atexit;
using std::atof;
using std::atoi;
using std::atol;
using std::bsearch;
using std::calloc;
using std::div;
using std::free;
using std::getenv;
using std::ldiv;
using std::malloc;
using std::mblen;
using std::mbstowcs;


#ifndef _RWSTD_NO_MBTOWC
using std::mbtowc;
#elif !defined (_RWSTD_NO_MBTOWC_IN_LIBC)

#  ifndef _RWSTD_NO_WCHAR_T

extern "C" int mbtowc (wchar_t*, const char*, size_t);

#    undef _RWSTD_NO_MBTOWC

#  endif   // _RWSTD_NO_WCHAR_T
#endif   // _RWSTD_NO_MBTOWC


using std::qsort;
using std::rand;
using std::realloc;
using std::srand;
using std::strtod;
using std::strtol;
using std::strtoul;
using std::system;
using std::wcstombs;
using std::wctomb;

#    endif   // _RWSTD_NO_NAMESPACE

#  endif   // _RWSTD_STDLIB_H_INCLUDED

#else
#  include _RWSTD_ANSI_C_STDLIB_H

#  if    defined (__HP_aCC) \
      && defined (_MATH_INCLUDED) && !defined (__MATH_INCLUDED)

// hacking around an HP aCC quirk when using system headers
// in /usr/include without the compiler's wrappers (i.e.,
// when -I/usr/include is on the command line)

extern "C" {

inline int abs (int __x)
{
    return __x < 0 ? -__x : __x;
}

}   // extern "C"

#  endif   // HP aCC && /usr/include/math.h included

#  undef ldiv

#endif   // _RWSTD_NO_DEPRECATED_C_HEADERS
