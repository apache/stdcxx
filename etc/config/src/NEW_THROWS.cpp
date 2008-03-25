// checking if operator new() throws

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

#include <stddef.h>

#include "config.h"
#include "nodbg.h"

#ifndef _RWSTD_NO_SETRLIMIT
// test for setrlimit() presence before compiling current file
#endif   // _RWSTD_NO_SETRLIMIT

// proclimits.h must be included only after #if[n]def _RWSTD_NO_SETRLIMIT
#include "proclimits.h"

#if 2 == __GNUG__
#  ifndef _RWSTD_NO_HONOR_STD
#    ifdef _RWSTD_NO_STD_TERMINATE
#      include "terminate.h"
#    endif   // _RWSTD_NO_STD_TERMINATE
#  endif   // _RWSTD_NO_HONOR_STD
#endif   // gcc 2.x

#ifndef _RWSTD_NO_NEW_OFLOW_SAFE
// test if operator new() checks the argument for overflow
#endif   // _RWSTD_NO_NEW_OFLOW_SAFE


// 18.4.1.1
// -3- Required behavior: Return a non-null pointer ... or else throw 
// a bad_alloc exception.

int main ()
{
    nodbg ();

    // decrease resource limit to a minimum to induce a failure
    // without unreasonably stressing the system
    limit_memory (0);

    void *p = (void*)1;

    try {
        const size_t size = (size_t)1 << (sizeof (size_t) * 8 - 1);

        for (int i = 0; i != 256; ++i) {
            // try to allocate a huge amount of memory
            p = ::operator new (size);
            // do not delete
        }
    }
    catch (...) {
        return 0;
    }

    return 1;
}
