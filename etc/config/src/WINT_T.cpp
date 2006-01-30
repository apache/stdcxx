// checking for wint_t in <wchar.h> and <wctype.h>

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>
#endif   // _RWSTD_NO_WCHAR_H

#ifndef _RWSTD_NO_WCTYPE_H
#  include <wctype.h>
#endif   // _RWSTD_NO_WCTYPE_H

#include <stdio.h>

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


int main ()
{
#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_WINT_T\n");

#endif   // _RWSTD_USE_CONFIG

    //////////////////////////////////////////////////////////////////
    // determine the underlying arithmetic type

    wint_t wi = 0;
    const char* const tname = type_name (wi);
    printf ("#define _RWSTD_WINT_T   %s\n", tname);

    // compute the type's minimum and maximum
    const char* const symbol = type_name (++wi);

    printf ("#define _RWSTD_WINT_MIN _RWSTD_%s_MIN\n"
            "#define _RWSTD_WINT_MAX _RWSTD_%s_MAX\n",
            symbol, symbol);

    return 0;
}
