/***************************************************************************
 *
 * _config-msvcrt.h - Microsoft Visual C++ run-time library
 *                    configuration definitions
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

#if _MSC_VER <= 1300   // MSVC <= 7.0
#  define _RWSTD_EXCEPTION_HANDLER_IN_STD
#  define _RWSTD_NO_STATIC_DEF3
#endif   // MSVC <= 7.0

   // enable the <cxxx> form of libc headers
   // we can only use the rw version in include/ansi
#ifndef _RWSTD_NO_PURE_C_HEADERS
#  define _RWSTD_NO_PURE_C_HEADERS
#endif   // _RWSTD_NO_PURE_C_HEADERS

#if defined _RWSTD_NO_NEW_HEADER
#  undef _RWSTD_NO_NEW_HEADER
#endif   // _RWSTD_NO_NEW_HEADER

   // our <cxxx> libc headers put the libc functions in namespace std
#ifdef _RWSTD_NO_LIBC_IN_STD
#  undef _RWSTD_NO_LIBC_IN_STD
#endif   // _RWSTD_NO_LIBC_IN_STD

#ifndef _RWSTD_NO_DEPRECATED_C_HEADERS
#  define _RWSTD_NO_DEPRECATED_C_HEADERS
#endif   // _RWSTD_NO_DEPRECATED_C_HEADERS

#ifndef _RWSTD_NO_WCSTOK
// MSVC CRT has incorrect prototype of the wcstok()
#  define _RWSTD_NO_WCSTOK
#endif   // _RWSTD_NO_WCSTOK

#ifndef _RWSTD_NO_WCSTOK_IN_LIBC
#  define _RWSTD_NO_WCSTOK_IN_LIBC
#endif   // _RWSTD_NO_WCSTOK_IN_LIBC

   // operator new and delete is not reliably replaceable across
   // shared library boundaries, which includes the shared library
   // version of the language support library
#define _RWSTD_NO_REPLACEABLE_NEW_DELETE

   // names of (most) extern "C" libc symbols begin with an underscore
#define _RWSTD_LIBC_SYM(name)   _ ## name

#ifdef _DLL
   // the _DLL macro is defined by the compiler when building a shared
   // library, i.e., in response to either of the /MD and /MDd options
   // _RWSTD_DLLIMPORT is used when declaring libc functions that need
   // to be "imported" into the shared library
#  define _RWSTD_DLLIMPORT   __declspec (dllimport)
#endif   // _DLL

#ifndef _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION
     // both MSVC 7.x and Intel C++/Windows allow "inline" initializers
     // for static const integral data members but out-of-line definitions
     // cause multiply defined symbol errors (see PR #26562 and #30260)
     // disable their definitions in source files (the members are still
     // declared)
#  define _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION
#endif   // _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION

   // enable iostream and locale support for long long integers
#if _MSC_VER <= 1300 || defined (_RWSTD_NO_LONG_LONG)
#  define _RWSTD_LONG_LONG __int64
#endif   // _MSC_VER <= 1300 || _RWSTD_NO_LONG_LONG

#if defined (WIN32) && !defined(_WIN32)
#  define _WIN32
#endif

#if defined (_MT) && !defined (_RWSTD_NO_REENTRANT)
#  ifndef _RWSTD_REENTRANT
#    define _RWSTD_REENTRANT 1
#  endif   // _RWSTD_REENTRANT

#  ifndef _RWSTD_NO_TLS
       // thread-local stoprage declaration specifier
#    ifndef _RWSTD_THREAD
#      define _RWSTD_THREAD   __declspec (thread)
#    endif   // _RWSTD_THREAD
#  endif   // _RWSTD_NO_TLS
#else   // if !defined (_MT) || defined (_RWSTD_NO_REENTRANT)
#  ifdef _RWSTD_REENTRANT
#    undef _RWSTD_REENTRANT
#  endif   // _RWSTD_REENTRANT
#  ifndef _RWSTD_THREAD
#    define _RWSTD_THREAD   /* empty */
#  endif   // _RWSTD_THREAD
#endif   // _MT && !_RWSTD_NO_REENTRANT

#if defined (_CPPRTTI)
     // override in case library was configured with -GR-
     // (i.e., with RTTI disabled)
#  undef _RWSTD_NO_DYNAMIC_CAST
#elif !defined (_RWSTD_NO_DYNAMIC_CAST)
     // override in case library was configured with -GR
     // (i.e., with RTTI enabled)
#  define _RWSTD_NO_DYNAMIC_CAST
#endif   // _CPPRTTI

#ifndef _NATIVE_WCHAR_T_DEFINED
   // define wchar_t if it is not a keyword recognized by the compiler
   // (use the /Zc:wchar_t compiler option to enable wchar_t as a keyword)
typedef unsigned short wchar_t;

#endif   // _NATIVE_WCHAR_T_DEFINED

#if _RWSTD_VER_MAJOR < 5 && defined (_DLL)
   // defined for binary compatibility with stdcxx 4.1.x
   // (for all versions of MSVC and Intel C++)
#  define _RWSTD_NO_BAD_CAST_ASSIGNMENT
#  define _RWSTD_NO_BAD_CAST_COPY_CTOR
#  define _RWSTD_NO_BAD_CAST_DTOR
#  define _RWSTD_NO_BAD_TYPEID_ASSIGNMENT
#  define _RWSTD_NO_BAD_TYPEID_COPY_CTOR
#  define _RWSTD_NO_BAD_TYPEID_DTOR
#  define _RWSTD_NO_EXCEPTION_ASSIGNMENT
#  define _RWSTD_NO_EXCEPTION_COPY_CTOR
#  define _RWSTD_NO_EXCEPTION_DEFAULT_CTOR
#  define _RWSTD_NO_EXCEPTION_DTOR
#  define _RWSTD_NO_EXCEPTION_WHAT
#endif   // _RWSTD_VER_MAJOR < 5 && _DLL
