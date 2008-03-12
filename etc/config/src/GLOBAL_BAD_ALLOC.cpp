// checking for class ::bad_alloc

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
#include "nodbg.h"

#if !defined (_RWSTD_NO_NEW_THROWS) && !defined (_RWSTD_NO_NEW_OFLOW_SAFE)
#  include <stddef.h>   // for size_t
#endif   // !_RWSTD_NO_NEW_THROWS && !_RWSTD_NO_NEW_OFLOW_SAFE

#ifndef _RWSTD_NO_SETRLIMIT
// test for setrlimit() presence before compiling current file
#endif

// proclimits.h must be included only after #if[n]def _RWSTD_NO_SETRLIMIT
#include "proclimits.h"

#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE
#    include "terminate.h"
#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


struct bad_alloc { };


int main ()
{
    // prevent MSVC's libc from popping up a dialog box
    // with an out-of-memory message
    nodbg ();

    try {

#ifndef _RWSTD_NO_NEW_THROWS
#  ifndef _RWSTD_NO_NEW_OFLOW_SAFE

        void *p = ::operator new (size_t (-1));

        if (!p)
            return 1;

#  else   // if defined (_RWSTD_NO_NEW_OFLOW_SAFE)

        limit_memory (0);

        for (unsigned long n = 1UL << (sizeof (long) * 8 - 1);
             0 != n; n |= (n >> 1)) {

            // try to allocate a huge amount of memory
            void *p = ::operator new (n);

            if (!p)
                return 1;
        }

#  endif   // _RWSTD_NO_NEW_OFLOW_SAFE
#else   // if defined (_RWSTD_NO_NEW_THROWS)

        return 4;

#endif

    }
    catch (bad_alloc&) {
        return 0;
    }
    catch (...) {
        return 2;
    }

    return 3;
}
