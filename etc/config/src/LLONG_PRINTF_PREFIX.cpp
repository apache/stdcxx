// checking for long long printf format prefix

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


#if !defined (_MSC_VER)
#  define long_long   long long
#else
#  define long_long   __int64
#endif   // _MSC_VER

// computes the largest positive long long that's a power of 2
long_long get_big_num ()
{
    long_long ll = 1;
    while (ll * 2 > ll)
        ll *= 2;
    return ll;
}


// checks to see if sprintf/sscanf works with the given format
int chkfmt (const char *fmt)
{
    char buf [80] = "";

    long_long vals [] = {
        0,   // placeholder
        -2, -1, 0, 1,
        0    // placeholder
    };

    vals [0]                              = -get_big_num ();
    vals [sizeof vals / sizeof *vals - 1] =  get_big_num ();

    for (unsigned i = 0; i != sizeof vals / sizeof *vals; ++i) {
        long_long ll = 0;

        if (0 > sprintf (buf, fmt, vals [i]))
            return -1;
        if (0 > sscanf (buf, fmt, &ll))
            return -1;
        if (ll != vals [i])
            return -1;
    }
    return 0;
}


// avoid having to #include <string>
char* append (char *dst, const char *src)
{
    for (; *dst; ++dst);
    for (; (*dst = *src); ++dst, ++src);
    return dst;
}

int main ()
{
    const char* pfx[] = {
        "ll",   // 7.19.6.2, p11 of C99, HP-UX 11, SunOS 5.7, ...
        "L",    // Linux
        "LL",
        "q",    // BSD 4.4
        "l",
        "I64"   // Win32/64
    };

    for (unsigned i = 0; i != sizeof pfx / sizeof *pfx; ++i) {
        char fmt[8] = "%";
        append (append (fmt, pfx [i]), "d");

        if (0 == chkfmt (fmt)) {
            printf ("#define _RWSTD_LLONG_PRINTF_PREFIX \"%s\"\n", pfx [i]);
            return 0;
        }
    }

    return 1;
}
