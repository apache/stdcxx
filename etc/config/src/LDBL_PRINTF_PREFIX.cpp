// checking for long double printf format prefix

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

#include "config.h"
#include "float_defs.h"   // for LDBL_XXX constants


// avoid having to #include <string.h>
int compare (const char *s1, const char *s2)
{
    for ( ; *s1 && *s1 == *s2; ++s1, ++s2);
    return *s1 - *s2;
}


// checks to see if sprintf/sscanf works with the given format
int chkfmt (const char *fmt)
{
    char outbuf [4096];
    outbuf [0] = '\0';

    long double vals [] = {
        // avoid using edge values to prevent rounding errors
        LDBL_MIN * 10, DBL_MIN * 10, FLT_MIN * 10, -2, -1, -0.1, -0.001,
        0.001, 0.1, 1, FLT_MAX / 10, DBL_MAX / 10, LDBL_MAX / 10
    };

    for (unsigned i = 0; i != sizeof vals / sizeof *vals; ++i) {
        long double ld = 0.0;
        char inbuf [sizeof outbuf] = "";

        // format into a char buffer
        if (0 > sprintf (outbuf, fmt, vals [i]))
            return -1;

        // parse buffer
        if (0 > sscanf (outbuf, fmt, &ld))
            return -1;

        // format parsed number into another buffer
        if (0 > sprintf (inbuf, fmt, ld))
            return -1;

        // compare buffers, expect identical contents
        if (compare (inbuf, outbuf))
            return -1;
    }
    return 0;
}


// avoid having to #include <string.h>
char* append (char *dst, const char *src)
{
    for (; *dst; ++dst);
    for (; (*dst = *src); ++dst, ++src);
    return dst;
}

int main ()
{
    const char* pfx[] = {
        "L",   // C89, C99
        "l",   // on systems where sizeof (long double) == sizeof (double)
        ""     // on systems where sizeof (long double) == sizeof (float)
    };

    for (unsigned i = 0; i != sizeof pfx / sizeof *pfx; ++i) {
        char fmt[8] = "%";
        append (append (fmt, pfx [i]), "e");

        if (0 == chkfmt (fmt)) {
            printf ("#define _RWSTD_LDBL_PRINTF_PREFIX \"%s\"\n", pfx [i]);
            return 0;
        }
    }

    return 1;   // fail
}
