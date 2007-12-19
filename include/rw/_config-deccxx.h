/***************************************************************************
 *
 * _config-deccxx.h - DEC/Compaq/HP C++ configuration definitions
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

// _REENTRANT is defined by -pthread option

#if defined (_RWSTD_REENTRANT)
#  if defined (_PTHREAD_USE_D4) && !defined (_RWSTD_DCE_THREADS)
#    define _RWSTD_DCE_THREADS
#  endif   // _PTHREAD_USE_D4 && !_RWSTD_DCE_THREADS

#  if !defined (_RWSTD_DCE_THREADS) && !defined (_RWSTD_POSIX_THREADS)
#    define _RWSTD_POSIX_THREADS
#  endif   // !(_RWSTD_DCE_THREADS) && !(_RWSTD_POSIX_THREADS)
#endif   // (_RWSTD_REENTRANT)

#if __DECCXX_VER < 60390008
   // version of Compaq C++ prior to C++ do not implement
   // C++ C library headers (the <cxxx> kind)
#  define _RWSTD_NO_NEW_HEADER
#endif   // __DECCXX_VER < 60390008

#ifndef __EXCEPTIONS
   // disable exceptions when the macro __EXCEPTIONS
   // is not #defined by the compiler, e.g., when
   // the -LANG:exceptions=OFF option is used
#  ifndef _RWSTD_NO_EXCEPTIONS
#    define _RWSTD_NO_EXCEPTIONS
#  endif   // _RWSTD_NO_EXCEPTIONS
#endif   // __EXCEPTIONS
