// checking for struct lconv in <locale.h>

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

#include <locale.h>   // for lconv
#include <stddef.h>   // for offsetof, size_t
#include <stdio.h>    // for printf
#include <string.h>   // for strlen

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
// which #defines this (and other) names to undeclared symbols
#undef strlen

int main ()
{
    struct {
        size_t      offset;
        size_t      size;
        const char *type;
        const char *name;
        const char *comment;
    } lconv_info[] = {

#define ENTRY(type, member, comment) { \
        OFF (lconv, member),           \
        sizeof ((lconv*)0)->member,    \
        type,                          \
        # member,                      \
        "/* " comment " */"            \
    }

        // C90 and POSIX.1 members
        ENTRY ("char*", decimal_point, ""),
        ENTRY ("char*", thousands_sep, ""),
        ENTRY ("char*", grouping, ""),
        ENTRY ("char*", int_curr_symbol, ""),
        ENTRY ("char*", currency_symbol, ""),
        ENTRY ("char*", mon_decimal_point, ""),
        ENTRY ("char*", mon_thousands_sep, ""),
        ENTRY ("char*", mon_grouping, ""),
        ENTRY ("char*", positive_sign, ""),
        ENTRY ("char*", negative_sign, ""),
        ENTRY ("char", int_frac_digits, ""),
        ENTRY ("char", frac_digits, ""),
        ENTRY ("char", p_cs_precedes, ""),
        ENTRY ("char", p_sep_by_space, ""),
        ENTRY ("char", n_cs_precedes, ""),
        ENTRY ("char", n_sep_by_space, ""),
        ENTRY ("char", p_sign_posn, ""),
        ENTRY ("char", n_sign_posn, "")

#ifndef _RWSTD_NO_LCONV_INT_FMAT

        // C99 and POSIX.2 members
        , ENTRY ("char", int_p_cs_precedes, ""),
        ENTRY ("char", int_n_cs_precedes, ""),
        ENTRY ("char", int_p_sep_by_space, ""),
        ENTRY ("char", int_n_sep_by_space, ""),
        ENTRY ("char", int_p_sign_posn, ""),
        ENTRY ("char", int_n_sign_posn, "")

#endif   // _RWSTD_NO_LCONV_INT_FMAT

    };

    const size_t N = sizeof lconv_info / sizeof *lconv_info;

    char def [4096] = "";

    size_t curoff = 0;   // current offset
    size_t size   = 0;   // cumulative size

    for (size_t i = 0; i != N; ++i) {

        for (size_t j = 0; j != N; ++j) {

            if (curoff == lconv_info [j].offset) {

                sprintf (def + strlen (def),
                         "    %s %s;   \\\n",
                         lconv_info [j].type,
                         lconv_info [j].name);

                // invalidate offset
                lconv_info [j].offset = size_t (-1);
                curoff += lconv_info [j].size;
                size   += lconv_info [j].size;
                
                break;
            }
        }
    }

    printf ("#define _RWSTD_LCONV {   "
            "/* sizeof (lconv) == %u */ \\\n%s",
            sizeof (lconv), def);


    if (size != sizeof (lconv)) {
        printf ("    char _C_fill [%u];   /* fill to size */ \\\n",
                sizeof (lconv) - size);
    }

    printf ("  }\n");

    return 0;
}

