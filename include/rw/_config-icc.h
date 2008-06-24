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

#ifndef _WIN32

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

#  ifndef __EXCEPTIONS
     // disable exceptions when the macro __EXCEPTIONS
     // is not #deined by the compiler, e.g., when
     // the -fno-exceptions option is used
#    ifndef _RWSTD_NO_EXCEPTIONS
#      define _RWSTD_NO_EXCEPTIONS
#    endif   // _RWSTD_NO_EXCEPTIONS
#  endif

#else   // ifdef _WIN32

#  include "_config-msvcrt.h"

#  if defined (_RWSHARED)
     // only when using shared libstd and Intel C++/MSVC
     // working around an Intel C++ 7.1 bug (PR #29178)
#    define _RWSTD_NO_OUTLINED_USE_FACET_SPECIALIZATIONS
#  endif   // _RWSHARED

// disable "function was declared "deprecated"
#  pragma warning (disable: 1786)

// disable "dllexport/dllimport conflict with ... ; dllexport assumed"
#  pragma warning (disable: 1740)

#endif   // _WIN32
