/***************************************************************************
 *
 * _config-acc.h - HP aC++ configuration definitions
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

// _REENTRANT is defined by the -mt compiler option

// accommodate aCC 1.xx (HP-UX 10.xx), aCC 3.xx (HP-UX 11/PA-RISC)
// and aCC 5.xx (HP-UX 11/IA64)
#define _RWSTD_HP_aCC_MAJOR   (__HP_aCC / 10000)
#define _RWSTD_HP_aCC_MINOR   (__HP_aCC % 10000)

// new C++ libc headers supported
#undef _RWSTD_NO_NEW_HEADER

#ifndef _RWSTD_NO_PURE_C_HEADERS
   // disable for now
#  define _RWSTD_NO_PURE_C_HEADERS
#endif   // _RWSTD_NO_PURE_C_HEADERS

// deprecated C++ libc headers don't introduce names into namespace std
#ifndef _RWSTD_NO_DEPRECATED_LIBC_IN_STD
#  define _RWSTD_NO_DEPRECATED_LIBC_IN_STD
#endif

// deprecated C++ libc headers aren't used
#define _RWSTD_NO_DEPRECATED_C_HEADERS

// prevents Error (future) 553: Expected type arguments to follow
//                              class template name.
#define _RWSTD_REDUNDANT_TMPL_PARAMS

#if (_RWSTD_HP_aCC_MINOR < 3300) && !defined (_RWSTD_NO_MBSTATE_T)
   // mbstate_t definition is guarded in _mbstate_t.h and is available
   // only if _INCLUDE__STDC_A1_SOURCE is defined (i.e., with -AA)
#  define _RWSTD_NO_MBSTATE_T
#endif

#if defined (_HP_NAMESPACE_STD) || defined (_NAMESPACE_STD)
#  undef _RWSTD_NO_RUNTIME_IN_STD
#  undef _RWSTD_NO_STD_BAD_TYPEID
#  undef _RWSTD_NO_STD_TYPE_INFO
#endif

#ifndef _RWSTD_NO_BAD_ALLOC_WHAT
   // avoid the following ld errors:
   // Unsatisfied symbol "Class tables [Vtable] dependent on
   // key function: "bad_alloc::what() const"" in file [*.o]
   // Unsatisfied symbol "typeid<bad_alloc>" in file [*.o]
#  define _RWSTD_NO_BAD_ALLOC_WHAT
#endif   // _RWSTD_NO_BAD_ALLOC_WHAT

#if __HP_aCC < 60000
#  ifndef _RWSTD_NO_EXTERN_TEMPLATE
#    ifndef _RWSTD_NO_STRING_NPOS_TYPE
       // define the type of basic_string::npos to be size_t
       // instead of basic_string::size_type to work around
       // an HP aCC extern template bug #333 (only the HP
       // front end has this bug; HP aCC 6.0 which uses the
       // EDG front end works fine
#      define _RWSTD_NO_STRING_NPOS_TYPE
#    endif   // _RWSTD_NO_STRING_NPOS_TYPE
#  endif   // _RWSTD_NO_EXTERN_TEMPLATE
#endif   // aCC < 6.0
