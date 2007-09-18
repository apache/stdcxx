// checking for struct tm in <time.h>

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

#include <stddef.h>   // for offsetof, size_t
#include <stdio.h>    // for printf
#include <string.h>   // for strlen
#include <time.h>     // for struct tm

#include "config.h"

#ifndef _RWSTD_NO_OFFSETOF
#  define OFF(T, m)   offsetof (T, m)
#else
   // provide own equivalent when offsetof() macro doesn't work
   // (e.g., when using the EDG eccp front end with an incompatible
   // version of gcc)
#  define OFF(T, m)   (((const char*)&((T*)0)->m) - ((const char*)(T*)0))
#endif   // _RWSTD_NO_OFFSETOF

// prevent IBM xlC 5.0 errors caused by using /usr/include/string.h
// which #defines these (and other) names to undeclared symbols
#undef strlen

int main ()
{
    typedef struct tm TimeT;

    struct {
        size_t      offset;
        const char *name;
        const char *comment;
    } tm_info[] = {

#define ENTRY(member, comment)   \
        { OFF (struct tm, member), # member ";", "   /* " comment " */" }

        ENTRY (tm_sec,   "seconds after the minute [O..61] "),
        ENTRY (tm_min,   "minutes after the hour   [0..59] "),
        ENTRY (tm_hour,  "hours since midnight     [0..23] "),
        ENTRY (tm_mday,  "day of the month         [1..31] "),
        ENTRY (tm_mon,   "months since January     [0..11] "),
        ENTRY (tm_year,  "years since 1900                 "),
        ENTRY (tm_wday,  "days since Sunday        [0..6]  "),
        ENTRY (tm_yday,  "days since January 1     [0..365]"),
        ENTRY (tm_isdst, "Daylight Saving Time             "),
    };

    const size_t N = sizeof tm_info / sizeof *tm_info;

    char def [1024] = "";

    size_t curoff = 0;   // current offset

    // assume contiguous standard members
    // optionally followed by non-standard ones
    for (size_t i = 0; i != N; ++i) {
        for (size_t j = 0; j != N; ++j) {
            if (curoff == tm_info [j].offset) {
                sprintf (def + strlen (def),
                         "    int %-10s        %s \\\n",
                         tm_info [j].name, tm_info [j].comment);
                tm_info [j].offset = size_t (-1);
                curoff += sizeof (int);
                break;
            }
        }
        if (!curoff) {
            curoff = sizeof (struct tm) - N * sizeof (int);
        }
    }

    printf ("#define _RWSTD_STRUCT_TM {   "
            "/* sizeof (struct tm) == %u          */ \\\n%s",
            sizeof (struct tm), def);


    if (N * sizeof (int) != sizeof (struct tm)) {
        printf ("    char _C_fill [%u];        "
                "/* fill to size                      */ \\\n",
                sizeof (struct tm) - N * sizeof (int));
    }

    printf ("  }\n");

    return 0;
}
