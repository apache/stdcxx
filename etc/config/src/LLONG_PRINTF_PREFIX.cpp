// checking for long long printf format prefix

#include "config.h"

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
