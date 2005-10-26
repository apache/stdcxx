// checking for overloads of div()

// determine whether the div() function (including
// any overloads) is defined in the usual libc headers

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#ifndef _RWSTD_NO_LONG_LONG
#  if defined _MSC_VER
#    define LONG_LONG   __int64
#  else
#    define LONG_LONG   long long
#  endif
#endif   // _RWSTD_NO_LONG_LONG


int template_used;

template <class T>
ldiv_t div (T, T)
{
    ++template_used;

    return ldiv_t ();
}

template <class T>
void check_div (T t, const char *tname)
{
    template_used = 0;

    div (t, t);

    if (0 == template_used)
        printf ("%s", "// ");

    printf ("#define _RWSTD_NO_DIV_%s\n", tname);
}

int main ()
{
#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_DIV_OVERLOADS\n");

#endif   // _RWSTD_USE_CONFIG

    check_div ((int)1, "INT");
    check_div ((long)1, "LONG");

#if defined (LONG_LONG)
    check_div ((LONG_LONG)1, "LLONG");
#endif   // LONG_LONG

    return 0;
}
