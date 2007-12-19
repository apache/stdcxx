/***************************************************************************
 *
 * _config-mipspro.h - SGI MIPSpro configuration definitions
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

#define _RWSTD_SGI_MIPSPRO

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
#endif

#define _RWSTD_NO_PLACEMENT_DELETE
//  The comptest sets compile instantiate when it shouldn't.
//  override it here.
#if defined (_RWSTD_NO_IMPLICIT_INCLUSION)
#  undef _RWSTD_NO_IMPLICIT_INCLUSION
#endif

#ifndef __EXCEPTIONS
   // disable exceptions when the macro __EXCEPTIONS
   // is not #defined by the compiler, e.g., when
   // the -noexceptions option is used
#  ifndef _RWSTD_NO_EXCEPTIONS
#    define _RWSTD_NO_EXCEPTIONS
#  endif   // _RWSTD_NO_EXCEPTIONS
#endif   // __EXCEPTIONS

