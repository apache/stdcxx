// checking for overloads of abs()

// determine whether the abs() function (including
// any overloads) is defined in the usual libc headers

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#ifndef _RWSTD_NO_LONG_LONG
#  if defined _MSC_VER
#    define LONG_LONG   __int64
#  else
#    define LONG_LONG   long long
#  endif
#endif   // _RWSTD_NO_LONG_LONG


int template_used;

template <class T>
T abs (T t)
{
    ++template_used;

    return t;
}

template <class T>
void check_abs (T t, const char *tname)
{
    template_used = 0;

    abs (t);

    if (0 == template_used)
        printf ("%s", "// ");

    printf ("#define _RWSTD_NO_ABS_%s\n", tname);
}

int main ()
{
    check_abs ((bool)0, "BOOL");
    check_abs ((char)0, "CHAR");
    check_abs ((signed char)0, "SCHAR");
    check_abs ((unsigned char)0, "UCHAR");
    check_abs ((short)0, "SHRT");
    check_abs ((unsigned short)0, "USHRT");
    check_abs ((int)0, "INT");
    check_abs ((unsigned)0, "UINT");
    check_abs ((long)0, "LONG");
    check_abs ((unsigned long)0, "ULONG");

#if defined (LONG_LONG)
    check_abs ((LONG_LONG)0, "LLONG");
    check_abs ((unsigned LONG_LONG)0, "ULLONG");
#endif   // LONG_LONG

    return 0;
}
