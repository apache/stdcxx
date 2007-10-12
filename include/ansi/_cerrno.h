// -*- C++ -*-
/***************************************************************************
 *
 * _cerrno.h - C++ Standard library interface to the ANSI C header errno.h
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
 * Copyright 2005-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _RWSTD_CERRNO_H_INCLUDED
#define _RWSTD_CERRNO_H_INCLUDED


#include <rw/_defs.h>


#ifdef __linux__

_RWSTD_NAMESPACE (std) { 

extern "C" {

extern int* __errno_location ();


 // 17.4.1.2, p5 - errno may be a macro
 // lwg issue 310 - errno must be a macro
#define errno (*_STD::__errno_location ())


#define EDOM     33
#define ERANGE   34

// lwg issue 288
#define EILSEQ   84

}   // extern "C"

}   // namespace std

#elif defined (__sun__) || defined (__sun)

_RWSTD_NAMESPACE (std) { 

extern "C" {

extern int errno;

extern int* ___errno ();

#ifdef _RWSTD_REENTRANT
   // 17.4.1.2, p5 - errno may be a macro
   // lwg issue 310 - errno must be a macro
#  define errno (*_STD::___errno ())
#else   // if !defined (_RWSTD_REENTRANT)
#  define errno _STD::errno
#endif   // _RWSTD_REENTRANT


#define EDOM     33
#define ERANGE   34

// lwg issue 288
#define EILSEQ   84

}   // extern "C"

}   // namespace std

#else   // if !__linux__ && !__sun__
#  include _RWSTD_ANSI_C_ERRNO_H
#endif   // __linux__


#endif   // _RWSTD_CERRNO_H_INCLUDED
