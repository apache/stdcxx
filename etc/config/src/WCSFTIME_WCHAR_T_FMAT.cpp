// checking for wcsftime() in <wchar.h>

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#include <stddef.h>   // for size_t
#include <stdio.h>    // for printf()
#include <time.h>     // for struct tm

#ifndef _RWSTD_NO_WCSFTIME
#  ifndef _RWSTD_NO_WCHAR_H
#    include <wchar.h>    // for wcsftime()
#  else   // if defined (_RWSTD_NO_WCHAR_H)
#    include <stdlib.h>   // for wcsftime()
#  endif   // _RWSTD_NO_WCHAR_H


#ifndef _RWSTD_NO_NAMESPACE

namespace std { }
using namespace std;

#endif   // _RWSTD_NO_NAMESPACE


extern "C" {

typedef size_t (f_w)(wchar_t*, size_t, const wchar_t*, const struct tm*);
typedef size_t (f_c)(wchar_t*, size_t, const char*, const struct tm*);

}


const char* foo (f_w*) { return "const wchar_t*"; }

const char* foo (f_c*) { return "const char*"; }


const char* bar ()
{
    // 7.16.6 of C95
    // 7.24.5.1 of C99
    return foo (&wcsftime);
}


int main ()
{

#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_WCSFTIME_WCHAR_T_FMAT\n");

#endif   // _RWSTD_USE_CONFIG
    
    const char *argtype = bar ();
    if (*(argtype+6) == 'c') {
       printf ("#define _RWSTD_NO_WCSFTIME_WCHAR_T_FMAT\n");
    }

    printf ("#define _RWSTD_WCSFTIME_ARG3_T %s\n", argtype);

    return 0;
}

#else   // if defined (_RWSTD_NO_WCSFTIME)

extern "C" size_t wcsftime (wchar_t*, size_t, const wchar_t*, const struct tm*);

struct tm t;

int main ()
{
#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_WCSFTIME_WCHAR_T_FMAT\n");

#endif   // _RWSTD_USE_CONFIG

    wchar_t buf [40] = { 0 };

    // format using a wchar_t format string; if the function takes
    // a whar_t* argument, it will format as expected, otherwise
    // it will format at most one character (unless sizeof (char)
    // == sizeof (wchar_t))
    size_t n = wcsftime (buf, sizeof buf, L"abc", &t);

    const char *argtype;

    if (   3    == n
        && wchar_t ('a')  == buf [0]
        && wchar_t ('b')  == buf [1]
        && wchar_t ('c')  == buf [2]
        && wchar_t ('\0') == buf [3])
        argtype = "const wchar_t*";
    else {
        argtype = "const char*";

        printf ("#define _RWSTD_NO_WCSFTIME_WCHAR_T_FMAT\n");

    }


    printf ("#define _RWSTD_WCSFTIME_ARG3_T %s\n", argtype);

    return 0;
}

#endif   // _RWSTD_NO_WCSFTIME
