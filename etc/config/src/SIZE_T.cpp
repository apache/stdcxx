// checking for fpos_t, ptrdiff_t, and size_t

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>
#endif   // _RWSTD_NO_WCHAR_H


// determine the underlying arithmetic type of a typedef
#define DEFINE_TYPE_HELPER(T)                       \
    const char* get_type_name (T) { return # T; }   \
    typedef void unused_type


DEFINE_TYPE_HELPER (char);
DEFINE_TYPE_HELPER (short);
DEFINE_TYPE_HELPER (unsigned short);
DEFINE_TYPE_HELPER (int);
DEFINE_TYPE_HELPER (unsigned int);
DEFINE_TYPE_HELPER (long);
DEFINE_TYPE_HELPER (unsigned long);

#ifndef _RWSTD_NO_LONG_LONG

DEFINE_TYPE_HELPER (long long);
DEFINE_TYPE_HELPER (unsigned long long);

#elif defined (_MSC_VER)

DEFINE_TYPE_HELPER (__int64);
DEFINE_TYPE_HELPER (unsigned __int64);

#endif   // _RWSTD_NO_LONG_LONG, _MSC_VER

const char* get_type_name (...) { return 0; }


void get_type_names (int dummy, ...)
{
    clock_t   clk  = 0;           // arithmetic type
    fpos_t    pos  = fpos_t ();   // object type
    ptrdiff_t diff = 0;           // signed integral type
    size_t    size = 0;           // unsigned integral type
    time_t    tim  = 0;           // arithmetic type

#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_SIZE_T\n");

#endif   // _RWSTD_USE_CONFIG

    printf ("#define _RWSTD_CLOCK_T %s\n", get_type_name (clk));
    printf ("#define _RWSTD_PTRDIFF_T %s\n", get_type_name (diff));
    printf ("#define _RWSTD_SIZE_T %s\n", get_type_name (size));
    printf ("#define _RWSTD_TIME_T %s\n", get_type_name (tim));

#if defined (CLOCKS_PER_SEC)
    printf ("#define _RWSTD_CLOCKS_PER_SEC %d\n", CLOCKS_PER_SEC);
#else
    printf ("#define _RWSTD_NO_CLOCKS_PER_SEC\n");
#endif   // CLOCKS_PER_SEC

#if defined (RAND_MAX)
    printf ("#define _RWSTD_RAND_MAX %d\n", RAND_MAX);
#else
    printf ("#define _RWSTD_NO_RAND_MAX\n");
#endif   // RAND_MAX

#if defined (EOF)
    printf ("#define _RWSTD_EOF %d\n", EOF);
#else
    // define _RWSTD_EOF to the usual value even if WEOF is not #defined
    // to avoid having to #ifdef around wchar_t code that needs the macro
    printf ("#define _RWSTD_EOF %d\n", -1);
    // also #define _RWSTD_NO_EOF to make it possible to determine
    // whether EOF is or isn't defined in libc headers
    printf ("#define _RWSTD_NO_EOF\n");
#endif   // EOF

#if defined (WEOF)
    printf ("#define _RWSTD_WEOF %d\n", WEOF);
#else
    // define _RWSTD_WEOF to the usual value even if WEOF is not #defined
    // to avoid having to #ifdef around wchar_t code that needs the macro
    printf ("#define _RWSTD_WEOF %d   /* best guess */\n", -1);
    // also #define _RWSTD_NO_WEOF to make it possible to determine
    // whether WEOF is or isn't defined in libc headers
    printf ("#define _RWSTD_NO_WEOF\n");
#endif   // WEOF

#if defined (L_tmpnam)
    printf ("#define _RWSTD_L_TMPNAM %d\n", L_tmpnam);
#else
    printf ("#define _RWSTD_NO_L_TMPNAM\n");
#endif   // L_tmpnam

#if defined (_IOFBF)
    printf ("#define _RWSTD_IOFBF %d\n", _IOFBF);
#else
    printf ("#define _RWSTD_NO_IOFBF\n");
#endif   // _IOFBF

#if defined (_IOLBF)
    printf ("#define _RWSTD_IOLBF %d\n", _IOLBF);
#else
    printf ("#define _RWSTD_NO_IOLBF\n");
#endif   // _IOLBF

#if defined (_IONBF)
    printf ("#define _RWSTD_IONBF %d\n", _IONBF);
#else
    printf ("#define _RWSTD_NO_IONBF\n");
#endif   // _IONBF

#if defined (BUFSIZ)
    printf ("#define _RWSTD_BUFSIZ %d\n", BUFSIZ);
#else
    printf ("#define _RWSTD_NO_BUFSIZ\n");
#endif   // BUFSIZ

#if defined (FOPEN_MAX)
    printf ("#define _RWSTD_FOPEN_MAX %d\n", FOPEN_MAX);
#else
    printf ("#define _RWSTD_NO_FOPEN_MAX\n");
#endif   // FOPEN_MAX

#if defined (FILENAME_MAX)
    printf ("#define _RWSTD_FILENAME_MAX %d\n", FILENAME_MAX);
#else
    printf ("#define _RWSTD_NO_FILENAME_MAX\n");
#endif   // FILENAME_MAX

#if defined (TMP_MAX)
    printf ("#define _RWSTD_TMP_MAX %d\n", TMP_MAX);
#else
    printf ("#define _RWSTD_NO_TMP_MAX\n");
#endif   // TMP_MAX

    if (get_type_name (pos))
        printf ("#define _RWSTD_FPOS_T %s\n", get_type_name (pos));
    else {
        printf ("#define _RWSTD_NO_NATIVE_FPOS_T"
                "/* may be an aggregate */\n");
        printf ("#define _RWSTD_FPOS_T_SIZE %u\n", sizeof pos);
    }
}


int main ()
{
    get_type_names (0);

    return 0;
}
