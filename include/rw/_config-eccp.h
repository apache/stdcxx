/***************************************************************************
 *
 * _config-eccp.h - EDG eccp configuration definitions
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

// identify the EDG eccp standalone front-end in order to help avoid
// confusing it with compilers such as Compaq C++, Intel C++, or SGI
// MIPSpro, that use the front-end and (sometimes) #define __EDG__
#define _RWSTD_EDG_ECCP

#if defined (_RWSTD_REENTRANT) && !defined (_RWSTD_POSIX_THREADS)
#  define _RWSTD_POSIX_THREADS
#endif // (_RWSTD_REENTRANT) && !(_RWSTD_POSIX_THREADS)

#undef _RWSTD_NO_NEW_HEADER
#undef _RWSTD_NO_LIBC_IN_STD

#ifndef _RWSTD_NO_EXPORT
#  undef _RWSTD_NO_IMPLICIT_INCLUSION
#endif

// the vanilla front-end doesn't actually #include the header,
// but it treats it specially instead, which throws off the test
#undef _RWSTD_ANSI_C_STDARG_H 
#define _RWSTD_ANSI_C_STDARG_H <stdarg.h>

#ifdef _RWSTD_REENTRANT
   // std::uncaught_exception() not thread-safe in the front-end demo
#  define _RWSTD_NO_UNCAUGHT_EXCEPTION
#endif   // _RWSTD_REENTRANT

#undef _RWSTD_NO_DEPRECATED_C_HEADERS
#undef _RWSTD_NO_PURE_C_HEADERS

#ifndef __EXCEPTIONS
   // disable exceptions when the macro __EXCEPTIONS
   // is not #defined by the compiler, e.g., when
   // the -no_exceptions option is used
#  ifndef _RWSTD_NO_EXCEPTIONS
#    define _RWSTD_NO_EXCEPTIONS
#  endif   // _RWSTD_NO_EXCEPTIONS
#endif   // __EXCEPTIONS

#if (310 <= __EDG_VERSION__)
#  define _RWSTD_TT_IS_ENUM(T)              __is_enum(T)
#  define _RWSTD_TT_IS_UNION(T)             __is_union(T)
#  define _RWSTD_TT_IS_CLASS(T)             __is_class(T)
#  define _RWSTD_TT_IS_POD(T)               __is_pod(T)

#  define _RWSTD_TT_IS_EMPTY(T)             __is_empty(T)
#  define _RWSTD_TT_IS_POLYMORPHIC(T)       __is_polymorphic(T)

// __is_convertible_to() fails for decay and void-void conversions
//#  define _RWSTD_TT_IS_CONVERTIBLE(T,U)     __is_convertible_to(T,U)
#  define _RWSTD_TT_IS_ABSTRACT(T)          __is_abstract(T)

// __is_base_of() gives false positives for union types
//#  define _RWSTD_TT_IS_BASE_OF(T,U)         __is_base_of(T,U)

#  define _RWSTD_TT_HAS_TRIVIAL_CTOR(T)     __has_trivial_constructor(T)
#  define _RWSTD_TT_HAS_TRIVIAL_COPY(T)     __has_trivial_copy(T)
#  define _RWSTD_TT_HAS_TRIVIAL_ASSIGN(T)   __has_trivial_assign(T)
#  define _RWSTD_TT_HAS_TRIVIAL_DTOR(T)     __has_trivial_destructor(T)

#  define _RWSTD_TT_HAS_NOTHROW_CTOR(T)     __has_nothrow_constructor(T)
#  define _RWSTD_TT_HAS_NOTHROW_COPY(T)     __has_nothrow_copy(T)
#  define _RWSTD_TT_HAS_NOTHROW_ASSIGN(T)   __has_nothrow_assign(T)

#  define _RWSTD_TT_HAS_VIRTUAL_DTOR(T)     __has_virtual_destructor(T)

#  define _RWSTD_TT_ALIGN_OF(T)             __alignof__(T)
#  define _RWSTD_TT_MAX_ALIGNMENT           16

// need to find a way to align data
//#  define _RWSTD_TT_ALIGNED_POD(N)          struct { double _C_align; }
#endif   // __EDG_VERSION__ >= 3.10 

