// checking for mbstate_t in <wchar.h> and <wctype.h>

/***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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
 * Copyright 1999-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include "config.h"

#if    defined (__EDG__)                \
    && !defined (__DECCXX)              \
    && !defined (__HP_aCC)              \
    && !defined (__INTEL_COMPILER)      \
    && !defined (_SGI_COMPILER_VERSION)
   // disable error #450-D: the type "long long" is nonstandard
   // when using the vanilla EDG eccp in strict mode (i.e., w/o
   // long long support)
#  pragma diag_suppress 450
#endif   // EDG eccp on Linux


// defined to get the correct definition of mbstate_t, available on HPUX;
// once we get the correct size of the struct, _RWSTD_NO_MBSTATE_T is 
// defined in rw/_config.h and we (and the users of the library) are left 
// with our definition only
#if defined (__HP_aCC)
#  define _INCLUDE__STDC_A1_SOURCE
#endif

#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>
#endif   // _RWSTD_NO_WCHAR_H

#ifndef _RWSTD_NO_WCTYPE_H
#  include <wctype.h>
#endif

#include <stdio.h>

int main ()
{
    printf ("#define _RWSTD_MBSTATE_T_SIZE %u\n", sizeof (mbstate_t));
}
