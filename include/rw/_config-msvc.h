/***************************************************************************
 *
 * _config-msvc.h - Microsoft Visual C++ configuration definitions
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
 *
 * $Id$
 *
 ***************************************************************************
 * 
 * Copyright 2005-2006 The Apache Software Foundation or its licensors,
 * as applicable.
 *
 * Copyright 1994-2006 Rogue Wave Software.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 **************************************************************************/

#if _MSC_VER <= 1310   // MSVC <= 7.1
#endif   // MSVC <= 7.1

#if _MSC_VER <= 1300   // MSVC <= 7.0
#  define _RWSTD_NO_TYPENAME
#  define _RWSTD_EXCEPTION_HANDLER_IN_STD
#  define _RWSTD_NO_STATIC_DEF3
#  define _RWSTD_NO_PTR_VALUE_TEMPLATE_OVERLOAD
#  define _RWSTD_NO_PRAGMA_INSTANTIATE
     // disable explicit instantiation extensions if building
     // optimized client code (all templates still explicitly
     // instantiated in the library); 
     // *** Not currently done for MSVC 7, since an implicit
     //     instantiation bug causes multiply defined symbols
     //     in downstream libraries
#  ifndef _RWSTD_NO_STRING_NPOS_TYPE
       // define basic_string::npos to be of type size_t
       // instead of basic_string::size_type to work around
       // an MSVC 7.0 bug (PR #26549)
#    define _RWSTD_NO_STRING_NPOS_TYPE
#  endif   // _RWSTD_NO_STRING_NPOS_TYPE
#endif   // MSVC <= 7.0

#if _MSC_VER <= 1200   // MSVC <= 6.0
#endif   // MSVC <= 6.0

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

   // static data members of cass templates aren't properly collapsed
   // (compiler emits one copy per each translation unit that contains
   // the definition of the template member, and the linker fails to
   // multiple copies that reside in shared libraries)
#ifndef _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS
#  define _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS
#endif   // _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS

   // operator new and delete is not reliably replaceable across
   // shared library boundaries, which includes the shared library
   // version of the language support library
#define _RWSTD_NO_REPLACEABLE_NEW_DELETE

   // names of (most) extern "C" libc symbols begin with an underscore
#define _RWSTD_LIBC_SYM(name)   _ ## name

#ifdef _DLL
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
#define _RWSTD_LONG_LONG __int64

   // not declared/defined in libc
#if !defined (_RWSTD_NO_STRTOLL_IN_LIBC)
#  define _RWSTD_NO_STRTOLL_IN_LIBC
#endif   // _RWSTD_NO_STRTOLL_IN_LIBC

#if !defined (_RWSTD_NO_STRTOULL_IN_LIBC)
#  define _RWSTD_NO_STRTOULL_IN_LIBC
#endif   // _RWSTD_NO_STRTOULL_IN_LIBC

#if defined (_WIN64)
     // FIXME: handle by forward declaring fuctions in <rw/_mutex.h>
#  define _RWSTD_NO_FWD_DECLARATIONS
#endif   // _WIN64

   // disable "Same type qualifier used more than once"
#pragma warning (disable: 4114)

   // disable "return type for operator->' is not a UDT"
#pragma warning (disable: 4284)

   // disable "decorated name length exceeded"
#pragma warning (disable: 4503)

   // disable "identifier was truncated to 255 characters"
#pragma warning (disable: 4786)

#if defined (WIN32) && !defined(_WIN32)
#  define _WIN32
#endif

#ifdef _MT
#  ifndef _RWSTD_REENTRANT
#    define _RWSTD_REENTRANT 1
#  endif   // _RWSTD_REENTRANT

#  ifndef _RWSTD_NO_TLS
       // thread-local stoprage declaration specifier
#    ifndef _RWSTD_THREAD
#      define _RWSTD_THREAD   __declspec (thread)
#    endif   // _RWSTD_THREAD
#  endif   // _RWSTD_NO_TLS
#else   // if !defined (_MT)
#  ifndef _RWSTD_THREAD
#    define _RWSTD_THREAD   /* empty */
#  endif   // _RWSTD_THREAD
#endif   //_MT

   // clean C++ C headers (the <cxxx> kind) not provided
#ifndef _RWSTD_NO_PURE_C_HEADERS
#  define _RWSTD_NO_PURE_C_HEADERS
#endif   // _RWSTD_NO_PURE_C_HEADERS

#ifndef _RWSTD_NO_EXPORT
     // not implemented
#  define _RWSTD_NO_EXPORT
#endif   // _RWSTD_NO_EXPORT

#define _RWSTD_MUNMAP_ARG1_T void*
#define _RWSTD_NO_NL_LANGINFO

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

   // functions are defined in MSVC's msvcprt.lib and msvcprtd.lib
   // which include the compiler's implementation of the C++ Standard
   // library and with which we (obviously) cannot use
#ifndef _RWSTD_NO_WCTOB
#  define _RWSTD_NO_WCTOB
#endif   // _RWSTD_NO_WCTOB

#ifndef _RWSTD_NO_WCTOB_IN_LIBC
#  define _RWSTD_NO_WCTOB_IN_LIBC
#endif   // _RWSTD_NO_WCTOB_IN_LIBC

#ifndef _RWSTD_NO_MBRTOWC
#  define _RWSTD_NO_MBRTOWC
#endif   // _RWSTD_NO_MBRTOWC

#ifndef _RWSTD_NO_MBRTOWC_IN_LIBC
#  define _RWSTD_NO_MBRTOWC_IN_LIBC
#endif   // _RWSTD_NO_MBRTOWC_IN_LIBC

#ifndef _RWSTD_NO_WCRTOMB
#  define _RWSTD_NO_WCRTOMB
#endif   // _RWSTD_NO_WCRTOMB

#ifndef _RWSTD_NO_WCRTOMB_IN_LIBC
#  define _RWSTD_NO_WCRTOMB_IN_LIBC
#endif   // _RWSTD_NO_WCRTOMB_IN_LIBC

#ifndef _RWSTD_NO_MBSTOWCS
#  define _RWSTD_NO_MBSTOWCS
#endif   // _RWSTD_NO_MBSTOWCS

#ifndef _RWSTD_NO_MBSTOWCS_IN_LIBC
#  define _RWSTD_NO_MBSTOWCS_IN_LIBC
#endif   // _RWSTD_NO_MBSTOWCS_IN_LIBC

#ifndef _RWSTD_NO_MBRLEN
#  define _RWSTD_NO_MBRLEN
#endif   // _RWSTD_NO_MBRLEN

#ifndef _RWSTD_NO_MBRLEN_IN_LIBC
#  define _RWSTD_NO_MBRLEN_IN_LIBC
#endif   // _RWSTD_NO_MBRLEN_IN_LIBC

#ifndef _RWSTD_NO_WCSRTOMBS
#  define _RWSTD_NO_WCSRTOMBS
#endif   // _RWSTD_NO_WCSRTOMBS

#ifndef _RWSTD_NO_WCSRTOMBS_IN_LIBC
#  define _RWSTD_NO_WCSRTOMBS_IN_LIBC
#endif   // _RWSTD_NO_WCSRTOMBS_IN_LIBC

#ifndef _RWSTD_NO_BTOWC
#  define _RWSTD_NO_BTOWC
#endif   // _RWSTD_NO_BTOWC

#ifndef _RWSTD_NO_BTOWC_IN_LIBC
#  define _RWSTD_NO_BTOWC_IN_LIBC
#endif   // _RWSTD_NO_BTOWC_IN_LIBC

#ifndef _RWSTD_NO_MBSRTOWCS
#  define _RWSTD_NO_MBSRTOWCS
#endif   // _RWSTD_NO_MBSRTOWCS

#ifndef _RWSTD_NO_MBSRTOWCS_IN_LIBC
#  define _RWSTD_NO_MBSRTOWCS_IN_LIBC
#endif   // _RWSTD_NO_MBSRTOWCS_IN_LIBC
