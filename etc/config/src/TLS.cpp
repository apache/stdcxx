// checking for thread-local storage

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
 * Copyright 1999-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <stdio.h>

#if defined (_MSC_VER)
#  define _RWSTD_THREAD __declspec (thread)
#else
#  define _RWSTD_THREAD __thread
#endif

#define STR(x) #x
#define PRINT_MACRO(name) printf ("#define " #name " " STR (name) "\n")

_RWSTD_THREAD int tls_i;

static _RWSTD_THREAD char tls_static_array [256];
extern _RWSTD_THREAD char tls_extern_array [256];

_RWSTD_THREAD char tls_extern_array [256];

int main ()
{
    int result = tls_i + tls_static_array [0] + tls_extern_array [0];

    if (0 == result)
        PRINT_MACRO (_RWSTD_THREAD);

    return result;
}
