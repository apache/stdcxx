/************************************************************************
 *
 * testdefs.h - common testsuite definitions
 *
 * $Id$
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

#ifndef RW_TESTDEFS_H_INCLUDED
#define RW_TESTDEFS_H_INCLUDED


#include <rw/_defs.h>


#if    (defined (_WIN32) || defined (_WIN64)) \
    && (defined (RWDLL) || defined (_RWSHARED))
#  ifndef _RWSTD_TEST_SRC
     // using a shared lib, import names
#    define _TEST_EXPORT   __declspec (dllimport)
#  else
     // building a shared (test) lib, export names
#    define _TEST_EXPORT   __declspec (dllexport)
#  endif   // _RWSTD_LIB_SRC
#else
#    define _TEST_EXPORT
#endif   // archive/shared library


#if defined (_RWSTD_NO_TYPENAME) && !defined (typename)
#  define typename /* ignore */
#endif   // _RWSTD_NO_TYPENAME && !typename


#ifndef _RWSTD_NO_FOR_INIT_SCOPE
#  if defined (__GNUG__) && __GNUG__ < 3
#    define _RWSTD_NO_FOR_INIT_SCOPE
#  endif   // gcc < 3.0
#  if defined (_MSC_VER) && _MSC_VER < 1300
#    define _RWSTD_NO_FOR_INIT_SCOPE
#  endif   // MSVC < 7.0
#endif   // _RWSTD_NO_FOR_INIT_SCOPE

#ifdef _RWSTD_NO_FOR_INIT_SCOPE
#  define for   if (0) ; else for
#endif   // _RWSTD_NO_FOR_INIT_SCOPE


#endif   // RW_TESTDEFS_H_INCLUDED
