// checking for struct lconv in <locale.h>

#include <locale.h>   // for lconv
#include <stddef.h>   // for offsetof, size_t
#include <stdio.h>    // for printf
#include <string.h>   // for strlen
#include "config.h"

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
        offsetof (lconv, member),      \
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

