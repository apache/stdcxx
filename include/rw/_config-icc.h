/***************************************************************************
 *
 * _config-icc.h - Intel C++ configuration definitions
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

#if !defined (_WIN32) && !defined (_WIN64)

   // enable the <cxxx> form of libc headers
   // we can only use the rw version in include/ansi
#  ifndef _RWSTD_NO_PURE_C_HEADERS
#    define _RWSTD_NO_PURE_C_HEADERS
#  endif   // _RWSTD_NO_PURE_C_HEADERS

#  ifndef _RWSTD_NO_DEPRECATED_C_HEADERS
#    define _RWSTD_NO_DEPRECATED_C_HEADERS
#  endif   // _RWSTD_NO_DEPRECATED_C_HEADERS

#  undef _RWSTD_NO_NEW_HEADER
#  undef _RWSTD_NO_LIBC_IN_STD

#else   // if defined (_WIN{32,64}
#  if defined (_RWSHARED)
     // only when using shared libstd and Intel C++/MSVC
     // working around an Intel C++ 7.1 bug (PR #29178)
#    define _RWSTD_NO_OUTLINED_USE_FACET_SPECIALIZATIONS
#  endif   // _MSC_VER > 1200 && _RWSHARED
#endif
