// checking for wctrans_t in <wchar.h> and <wctype.h>

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
#endif


#include <stdio.h>
#include <string.h>

#undef strcpy
#undef strcat


const char* foo (char)               { return "char"; }
const char* foo (short)              { return "short"; }
const char* foo (unsigned short)     { return "unsigned short"; }
const char* foo (int)                { return "int"; }
const char* foo (unsigned int)       { return "unsigned int"; }
const char* foo (long)               { return "long"; }
const char* foo (unsigned long)      { return "unsigned long"; }


#ifndef _RWSTD_NO_LONG_LONG

const char* foo (long long)          { return "long long"; }
const char* foo (unsigned long long) { return "unsigned long long"; }

#endif   // _RWSTD_NO_LONG_LONG


#ifndef _RWSTD_NO_WINT_T

extern "C" typedef _RWSTD_WINT_T wctrans_fun_t (_RWSTD_WINT_T);

const char* foo (wctrans_fun_t*)
{
    return "__rw_wctrans_t\n"
        "typedef _RWSTD_WINT_T (*__rw_wctrans_t)(_RWSTD_WINT_T);\n";
}

#endif   // _RWSTD_NO_WINT_T

template<class T>
const char* foo (const T*)
{
    static char name [256];

    const char *nm = foo (T ());

    strcpy (name, nm);
    strcat (name, "*");

    return name;
}


const char* foo (...)
{
    return 0;
}

int main ()
{
    if (foo (wctrans_t ()))
        printf ("#define _RWSTD_WCTRANS_T %s\n", foo (wctrans_t ()));
}
