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

#include "config.h"

#include <stdio.h>

#ifndef _RWSTD_NO_SETRLIMIT
// test for setrlimit() presence before compiling current file
#endif   // _RWSTD_NO_SETRLIMIT

// proclimits.h must be included only after #if[n]def _RWSTD_NO_SETRLIMIT
#include "proclimits.h"

#if 2 == __GNUG__
#  ifndef _RWSTD_NO_HONOR_STD
#    ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    static int *ip;

loop:

    while ((ip [0] = ip [1])) {  // force a SIGSEGV
        ++ip;
        terminate ();            // recurse infinitely
    }

    goto loop;
}

}   // namespace std

#    endif   // _RWSTD_NO_STD_TERMINATE
#  endif   // _RWSTD_NO_HONOR_STD
#endif   // gcc 2.x


#if (defined (_WIN32) || defined (_WIN64)) && !defined (__CYGWIN__)

#  include <crtdbg.h>  // for _CrtSetReportMode()

void nodbg ()
{
    // prevent the Microsoft C library from popping up GUI windows
    // on various failures and direct error messages to the console
    // instead
    _CrtSetReportMode (_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode (_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode (_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
}
#else

void nodbg () { }

#endif   // _WIN32 || _WIN64 && !__CYGWIN__


// 18.4.1.1
// -3- Required behavior: Return a non-null pointer ... or else throw 
// a bad_alloc exception.

int main ()
{
    nodbg ();

    void *p = 0;

    try {
        p = ::operator new (size_t (-1));
    }
    catch (...) {
        // exit successfully if new threw

        // this may give a false positive if operaor new()
        // fails to throw for arguments other than -1 as
        // happens to be the case with gcc 2.x
        printf ("// #define _RWSTD_NO_NEW_OFLOW_SAFE\n");
        return 0;
    }

    // otherwise note that operator new() may have not checked
    // its argument for overflow and try to coerce it to throw
    // by passing it a smaller argument
    printf ("#define _RWSTD_NO_NEW_OFLOW_SAFE\n");

    // decrease resource limit to a minimum to induce a failure
    // without unreasonably stressing the system
    limit_memory (0);

    p = (void*)1;

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
