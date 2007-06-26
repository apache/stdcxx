// -*- C++ -*-
/***************************************************************************
 *
 * cwctype - C++ Standard library interface to the ANSI C header wctype.h
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

#ifndef _RWSTD_CWCTYPE_H_INCLUDED
#define _RWSTD_CWCTYPE_H_INCLUDED


#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


#ifndef WEOF
#  define WEOF   _RWSTD_WEOF
#endif   // WEOF

extern "C" {


#ifdef _RWSTD_WCTRANS_T
typedef _RWSTD_WCTRANS_T wctrans_t;
#else
typedef int wctrans_t;
#endif   // _RWSTD_WCTRANS_T

#ifdef _RWSTD_WCTYPE_T
typedef _RWSTD_WCTYPE_T wctype_t;
#else
typedef int wctype_t;
#endif   // _RWSTD_WCTYPE_T

#ifdef _RWSTD_WINT_T
typedef _RWSTD_WINT_T wint_t;
#else
typedef long wint_t;
#endif   // _RWSTD_WINT_T


int iswalnum (wint_t);
int iswalpha (wint_t);
int iswcntrl (wint_t);
int iswdigit (wint_t);
int iswgraph (wint_t);
int iswlower (wint_t);
int iswprint (wint_t);
int iswpunct (wint_t);
int iswspace (wint_t);
int iswupper (wint_t);
int iswxdigit (wint_t);
wctype_t wctype (const char*);
int iswctype (wint_t, wctype_t);
wint_t towlower (wint_t);
wint_t towupper (wint_t);
wctrans_t wctrans (const char*);
wint_t towctrans (wint_t, wctrans_t);


}   // extern "C"


}   // namespace std


#endif   // _RWSTD_CWCTYPE_H_INCLUDED
