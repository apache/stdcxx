// -*- C++ -*-
/***************************************************************************
 *
 * ctime - C++ Standard library interface to the ANSI C header time.h
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

#ifndef _RWSTD_CTIME_H_INCLUDED
#define _RWSTD_CTIME_H_INCLUDED


#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


extern "C" {


#ifndef CLOCKS_PER_SEC
#  define CLOCKS_PER_SEC   _RWSTD_CLOCKS_PER_SEC
#endif

#ifndef NULL
#  define NULL 0
#endif


#ifdef _RWSTD_CLOCK_T
typedef _RWSTD_CLOCK_T clock_t;
#else
typedef long clock_t;
#endif   // _RWSTD_CLOCK_T

#ifdef _RWSTD_TIME_T
typedef _RWSTD_TIME_T time_t;
#else
typedef long time_t;
#endif   // _RWSTD_TIME_T

#ifdef _RWSTD_SIZE_T
typedef _RWSTD_SIZE_T size_t;
#else
typedef unsigned long size_t;
#endif   // _RWSTD_SIZE_T


struct tm _RWSTD_STRUCT_TM;


clock_t clock ();
double difftime (time_t, time_t);
time_t mktime (struct tm*);
time_t time (time_t*);
char* asctime (const struct tm*);
char* ctime (const time_t*);
struct tm* gmtime (const time_t*);
struct tm* localtime (const time_t*);
size_t strftime (char*, size_t, const char*, const struct tm*);


}   // extern "C"


}   // namespace std


#endif   // _RWSTD_CTIME_H_INCLUDED
