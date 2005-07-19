// checking for wctrans_t in <wchar.h> and <wctype.h>

#include "config.h"

#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>
#endif   // _RWSTD_NO_WCHAR_H

#ifndef _RWSTD_NO_WCTYPE_H
#  include <wctype.h>
#endif


#include <stdio.h>
#include <string.h>

#undef strcpy
#undef strcat


const char* foo (char)               { return "char"; }
const char* foo (short)              { return "short"; }
const char* foo (unsigned short)     { return "unsigned short"; }
const char* foo (int)                { return "int"; }
const char* foo (unsigned int)       { return "unsigned int"; }
const char* foo (long)               { return "long"; }
const char* foo (unsigned long)      { return "unsigned long"; }


#ifndef _RWSTD_NO_LONG_LONG

const char* foo (long long)          { return "long long"; }
const char* foo (unsigned long long) { return "unsigned long long"; }

#endif   // _RWSTD_NO_LONG_LONG


#ifndef _RWSTD_NO_WINT_T

extern "C" typedef _RWSTD_WINT_T wctrans_fun_t (_RWSTD_WINT_T);

const char* foo (wctrans_fun_t*)
{
    return "__rw_wctrans_t\n"
        "typedef _RWSTD_WINT_T (*__rw_wctrans_t)(_RWSTD_WINT_T);\n";
}

#endif   // _RWSTD_NO_WINT_T

template<class T>
const char* foo (const T*)
{
    static char name [256];

    const char *nm = foo (T ());

    strcpy (name, nm);
    strcat (name, "*");

    return name;
}


const char* foo (...)
{
    return 0;
}

int main ()
{
    if (foo (wctrans_t ()))
        printf ("#define _RWSTD_WCTRANS_T %s\n", foo (wctrans_t ()));
}
