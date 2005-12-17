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

// give the for-init-variable a local scope
#ifdef _RWSTD_NO_FOR_LOCAL_SCOPE
#  define for   if (0); else for
#endif   // _RWSTD_NO_FOR_LOCAL_SCOPE


// _RWSTD_PRI{d,i,o,u,x}MAX: macros corresponding to those described
// in 7.8.1 of C99; each of them expands to a character string literal
// containing a conversion specifier, possibly modified by a length 
// modifier, suitable for use within the format argument of a formatted
// input/output function when converting the corresponding integer type
#if _RWSTD_LONG_SIZE < _RWSTD_LLONG_SIZE
   // using LLONG_SIZE instead of ULLONG_MAX in the preprocessor
   // conditional above to work around a gcc 3.2 bug (PR #28595)
#  define _RWSTD_PRIdMAX   _RWSTD_LLONG_PRINTF_PREFIX "d"
#  define _RWSTD_PRIiMAX   _RWSTD_LLONG_PRINTF_PREFIX "i"
#  define _RWSTD_PRIoMAX   _RWSTD_LLONG_PRINTF_PREFIX "o"
#  define _RWSTD_PRIuMAX   _RWSTD_LLONG_PRINTF_PREFIX "u"
#  define _RWSTD_PRIxMAX   _RWSTD_LLONG_PRINTF_PREFIX "x"
#elif _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE
#  define _RWSTD_PRIdMAX   "ld"
#  define _RWSTD_PRIiMAX   "li"
#  define _RWSTD_PRIoMAX   "lo"
#  define _RWSTD_PRIuMAX   "lu"
#  define _RWSTD_PRIxMAX   "lx"
#else
#  define _RWSTD_PRIdMAX   "d"
#  define _RWSTD_PRIiMAX   "i"
#  define _RWSTD_PRIoMAX   "o"
#  define _RWSTD_PRIuMAX   "u"
#  define _RWSTD_PRIxMAX   "x"
#endif


// _RWSTD_PRIz: expands to a conversion specifier corresponding
// to "%z" (i.e., C99 size_t specifier)
#if _RWSTD_SIZE_MAX == _RWSTD_UINT_MAX
   // sizeof (size_t) == sizeof (unsigned int)
#  define _RWSTD_PRIz   ""
#elif _RWSTD_SIZE_MAX == _RWSTD_ULONG_MAX
   // sizeof (size_t) == sizeof (unsigned long)
#  define _RWSTD_PRIz   "l"
#elif _RWSTD_SIZE_MAX == _RWSTD_ULLONG_MAX
   // sizeof (size_t) == sizeof (unsigned long long)
#  define _RWSTD_PRIz   _RWSTD_LLONG_PRINTF_PREFIX ""
#else
  // assume sizeof (size_t) == sizeof (unsigned)
#  define _RWSTD_PRIz   ""
#endif


// test assertion
#ifndef _RWSTD_NO_PRETTY_FUNCTION
   // gcc, HP aCC, and Intel C++ all support __PRETTY_FUNCTION__
#  define RW_ASSERT(expr)                                                \
   ((expr) ? (void)0 : _RW::__rw_assert_fail (#expr, __FILE__, __LINE__, \
                                              __PRETTY_FUNCTION__))
#elif !defined (_RWSTD_NO_FUNC)
   // C99 specifies the __func__ special identifier
#  define RW_ASSERT(expr)                                                \
   ((expr) ? (void)0 : _RW::__rw_assert_fail (#expr, __FILE__, __LINE__, \
                                              __func__))
#elif defined (__FUNCSIG__)
   // MSVC macro
#  define RW_ASSERT(expr)                                                \
   ((expr) ? (void)0 : _RW::__rw_assert_fail (#expr, __FILE__, __LINE__, \
                                              __FUNCSIG__))
#else   // if _RWSTD_NO_PRETTY_FUNCTION && _RWSTD_NO_FUNC
#  define RW_ASSERT(expr)   \
   ((expr) ? (void)0 : _RW::__rw_assert_fail (#expr, __FILE__, __LINE__, 0))
#endif   // _RWSTD_NO_PRETTY_FUNCTION, _RWSTD_NO_FUNC


#endif   // RW_TESTDEFS_H_INCLUDED
