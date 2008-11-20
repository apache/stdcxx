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

#include "_config-msvcrt.h"

#if _MSC_VER <= 1300   // MSVC <= 7.0
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

// disable "Same type qualifier used more than once"
#pragma warning (disable: 4114)

// disable "return type for operator->' is not a UDT"
#pragma warning (disable: 4284)

// disable "nonstandard extension used :"
// "'extern' before template explicit instantiation"
#pragma warning (disable: 4231)

// RWDLL - defined for all Rogue Wave(R) products built as shared libs
// _RWSHARED - defined for libstd built/used as a shared lib
#if defined (RWDLL) || defined (_RWSHARED)

   // disable "class needs to have dll-interface to be used by cliens"
#  pragma warning (disable: 4251)

   // disable "non dll-interface class used as base for dll-interface class"
#  pragma warning (disable: 4275)

#endif   // RWDLL || _RWSHARED

// disable "decorated name length exceeded"
#pragma warning (disable: 4503)

// convert warning "'identifier' used on polymorphic type 'type' with /GR-;"
// "unpredictable behavior may result" to error
#pragma warning (error: 4541)

// disable "'identifier' : no suitable definition"
// "provided for explicit template instantiation request"
#pragma warning (disable: 4661)

// disable "identifier was truncated to 255 characters"
#pragma warning (disable: 4786)

// disable "'function': was declared deprecated"
#pragma warning (disable: 4996)

// type trait helpers provided by msvc
#if defined (_MSC_FULL_VER) && (_MSC_FULL_VER >= 140050215)
#  define _RWSTD_TT_HAS_NOTHROW_ASSIGN(T)  __has_nothrow_assign(T) 
#  define _RWSTD_TT_HAS_NOTHROW_CTOR(T)    __has_nothrow_constructor(T) 
#  define _RWSTD_TT_HAS_NOTHROW_COPY(T)    __has_nothrow_copy(T) 

#  define _RWSTD_TT_HAS_TRIVIAL_ASSIGN(T)  __has_trivial_assign(T) 
#  define _RWSTD_TT_HAS_TRIVIAL_CTOR(T)    __has_trivial_constructor(T) 
#  define _RWSTD_TT_HAS_TRIVIAL_COPY(T)    __has_trivial_copy(T) 
#  define _RWSTD_TT_HAS_TRIVIAL_DTOR(T)    __has_trivial_destructor(T) 

#  define _RWSTD_TT_HAS_VIRTUAL_DTOR(T)    __has_virtual_destructor(T) 

#  define _RWSTD_TT_IS_ABSTRACT(T)         __is_abstract(T) 
#  define _RWSTD_TT_IS_BASE_OF(T,U)        __is_base_of(T,U)
#  define _RWSTD_TT_IS_CLASS(T)            __is_class(T) 
#  define _RWSTD_TT_IS_POD(T)              __is_pod(T)
#  define _RWSTD_TT_IS_CONVERTIBLE(T,U)    __is_convertible_to(T,U) 
#  define _RWSTD_TT_IS_EMPTY(T)            __is_empty(T) 
#  define _RWSTD_TT_IS_ENUM(T)             __is_enum(T) 
#  define _RWSTD_TT_IS_POLYMORPHIC(T)      __is_polymorphic(T) 
#  define _RWSTD_TT_IS_UNION(T)            __is_union(T) 

#  define _RWSTD_TT_ALIGN_OF(T)            __alignof(T)
#  define _RWSTD_TT_ALIGNED_POD(N)         \
      struct {__declspec(align(N)) unsigned char _C_align; }

#  define _RWSTD_TT_MAX_ALIGNMENT          8192
#endif // _MSC_FULL_VER >= 140050215

