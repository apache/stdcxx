// -*- C++ -*-
/***************************************************************************
 *
 * _clocale.h - C++ Standard library interface to the ANSI C header locale.h
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

#ifndef _RWSTD_CLOCALE_H_INCLUDED
#define _RWSTD_CLOCALE_H_INCLUDED


#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


#ifndef LC_ALL
#  define LC_ALL   _RWSTD_LC_ALL
#endif   // LC_ALL

#ifndef LC_COLLATE
#  define LC_COLLATE   _RWSTD_LC_COLLATE
#endif   // LC_COLLATE

#ifndef LC_CTYPE
#  define LC_CTYPE   _RWSTD_LC_CTYPE
#endif   // LC_CTYPE

#ifndef LC_MONETARY
#  define LC_MONETARY   _RWSTD_LC_MONETARY
#endif   // LC_MONETARY

#ifndef LC_NUMERIC
#  define LC_NUMERIC   _RWSTD_LC_NUMERIC
#endif   // LC_NUMERIC

#ifndef LC_TIME
#  define LC_TIME   _RWSTD_LC_TIME
#endif   // LC_TIME

#ifndef NULL
#  define NULL   0
#endif   // NULL


extern "C" {

struct lconv _RWSTD_LCONV;

char* setlocale (int, const char*);
struct lconv* localeconv ();

}    // extern "C"


}   // namespace std


#endif   // _RWSTD_CLOCALE_H_INCLUDED
