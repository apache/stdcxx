// checking for fpos_t, ptrdiff_t, and size_t

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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>   // for RAND_MAX
#include <time.h>     // for CLOCKS_PER_SEC

#ifndef _RWSTD_NO_WCTYPE_H
#  include <wctype.h>   // for WEOF
#endif   // _RWSTD_NO_WCTYPE_H

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


void get_type_names (int dummy, ...)
{
    clock_t   clk  = 0;           // arithmetic type
    fpos_t    pos  = fpos_t ();   // object type
    ptrdiff_t diff = 0;           // signed integral type
    size_t    size = 0;           // unsigned integral type
    time_t    tim  = 0;           // arithmetic type

    printf ("#define _RWSTD_CLOCK_T        %s\n", type_name (clk));
    printf ("#define _RWSTD_PTRDIFF_T      %s\n", type_name (diff));
    printf ("#define _RWSTD_SIZE_T         %s\n", type_name (size));

    // compute the maximum and minimum for size_t and ptrdiff_t
    ++size;
    ++diff;
    printf ("#define _RWSTD_SIZE_MAX       _RWSTD_%s_MAX\n",
            type_name (size));
    printf ("#define _RWSTD_PTRDIFF_MIN    _RWSTD_%s_MIN\n"
            "#define _RWSTD_PTRDIFF_MAX    _RWSTD_%s_MAX\n",
            type_name (diff), type_name (diff));


    printf ("#define _RWSTD_TIME_T         %s\n", type_name (tim));

#if defined (CLOCKS_PER_SEC)
    printf ("#define _RWSTD_CLOCKS_PER_SEC %d\n", CLOCKS_PER_SEC);
#else
    printf ("#define _RWSTD_NO_CLOCKS_PER_SEC\n");
#endif   // CLOCKS_PER_SEC

#if defined (RAND_MAX)
    printf ("#define _RWSTD_RAND_MAX       %d\n", RAND_MAX);
#else
    printf ("#define _RWSTD_NO_RAND_MAX\n");
#endif   // RAND_MAX

#if defined (EOF)
    printf ("#define _RWSTD_EOF            %d\n", EOF);
#else
    // define _RWSTD_EOF to the usual value even if WEOF is not #defined
    // to avoid having to #ifdef around wchar_t code that needs the macro
    printf ("#define _RWSTD_EOF %d\n", -1);
    // also #define _RWSTD_NO_EOF to make it possible to determine
    // whether EOF is or isn't defined in libc headers
    printf ("#define _RWSTD_NO_EOF\n");
#endif   // EOF

#if defined (WEOF)
    printf ("#define _RWSTD_WEOF           %d\n", WEOF);
#else
    // define _RWSTD_WEOF to the usual value even if WEOF is not #defined
    // to avoid having to #ifdef around wchar_t code that needs the macro
    printf ("#define _RWSTD_WEOF %d   /* best guess */\n", -1);
    // also #define _RWSTD_NO_WEOF to make it possible to determine
    // whether WEOF is or isn't defined in libc headers
    printf ("#define _RWSTD_NO_WEOF\n");
#endif   // WEOF

#if defined (L_tmpnam)
    printf ("#define _RWSTD_L_TMPNAM       %d\n", L_tmpnam);
#else
    printf ("#define _RWSTD_NO_L_TMPNAM\n");
#endif   // L_tmpnam

#if defined (_IOFBF)
    printf ("#define _RWSTD_IOFBF          %d\n", _IOFBF);
#else
    printf ("#define _RWSTD_NO_IOFBF\n");
#endif   // _IOFBF

#if defined (_IOLBF)
    printf ("#define _RWSTD_IOLBF          %d\n", _IOLBF);
#else
    printf ("#define _RWSTD_NO_IOLBF\n");
#endif   // _IOLBF

#if defined (_IONBF)
    printf ("#define _RWSTD_IONBF          %d\n", _IONBF);
#else
    printf ("#define _RWSTD_NO_IONBF\n");
#endif   // _IONBF

#if defined (BUFSIZ)
    printf ("#define _RWSTD_BUFSIZ         %d\n", BUFSIZ);
#else
    printf ("#define _RWSTD_NO_BUFSIZ\n");
#endif   // BUFSIZ

#if defined (FOPEN_MAX)
    printf ("#define _RWSTD_FOPEN_MAX      %d\n", FOPEN_MAX);
#else
    printf ("#define _RWSTD_NO_FOPEN_MAX\n");
#endif   // FOPEN_MAX

#if defined (FILENAME_MAX)
    printf ("#define _RWSTD_FILENAME_MAX   %d\n", FILENAME_MAX);
#else
    printf ("#define _RWSTD_NO_FILENAME_MAX\n");
#endif   // FILENAME_MAX

#if defined (TMP_MAX)
    printf ("#define _RWSTD_TMP_MAX        %d\n", TMP_MAX);
#else
    printf ("#define _RWSTD_NO_TMP_MAX\n");
#endif   // TMP_MAX

    if (type_name (pos))
        printf ("#define _RWSTD_FPOS_T         %s\n", type_name (pos));
    else {
        printf ("#define _RWSTD_NO_NATIVE_FPOS_T"
                "/* may be an aggregate */\n");
        printf ("#define _RWSTD_FPOS_T_SIZE    %u\n", sizeof pos);
    }
}


int main ()
{
    get_type_names (0);

    return 0;
}
