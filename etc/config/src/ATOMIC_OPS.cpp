// checking for atomic operations

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

#if defined (_WIN32) && !defined (_WIN64)

#include <stdio.h>
#include <windows.h>


extern "C" {

typedef int  __stdcall pfiipv_t (int*);
typedef int  __stdcall pfiip_t (volatile int*);
typedef long __stdcall pfllp_t (long*);
typedef long __stdcall pfllpv_t (volatile long*);

}   // extern "C"


const char* foo (pfiip_t) { return "int"; }
const char* foo (pfiipv_t) { return "volatile int"; }

const char* foo (pfllp_t) { return "long"; }
const char* foo (pfllpv_t) { return "volatile long"; }


int main ()
{
    // determine the argument type of InterlockedIncrement()
    // (the type changes from long* to volatile long* depending
    // on the version/patch of MSVC)

    printf ("#define _RWSTD_INTERLOCKED_T %s\n", foo (InterlockedIncrement));

    return 0;
}

#else   // not 32-bit Windows

#include <stdio.h>

int main ()
{
   return 0;
}

#endif   // Windows
