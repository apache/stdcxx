// checking for wint_t in <wchar.h> and <wctype.h>

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


#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>
#endif   // _RWSTD_NO_WCHAR_H

#ifndef _RWSTD_NO_WCTYPE_H
#  include <wctype.h>
#endif   // _RWSTD_NO_WCTYPE_H

#include <stdio.h>

// establish a dependency on the test for long long
// and #define the LONG_LONG macro used in "type.h"
#ifndef _RWSTD_NO_LONG_LONG
#  define LONG_LONG long long
#else   // if defined (_RWSTD_NO_LONG_LONG)
#  if defined (_MSC_VER)
#    define LONG_LONG   __int64
#  endif   // _MSC_VER
#endif   // _RWSTD_NO_LONG_LONG

#include "types.h"   // for type_name()


int main ()
{
    //////////////////////////////////////////////////////////////////
    // determine the underlying arithmetic type

    wint_t wi = 0;
    const char* const tname = type_name (wi);
    printf ("#define _RWSTD_WINT_T   %s\n", tname);

    // compute the type's minimum and maximum
    const char* const symbol = type_name (++wi);

    printf ("#define _RWSTD_WINT_MIN _RWSTD_%s_MIN\n"
            "#define _RWSTD_WINT_MAX _RWSTD_%s_MAX\n",
            symbol, symbol);

    return 0;
}
