// checking for mbstate_t in <wchar.h> and <wctype.h>

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

// defined to get the correct definition of mbstate_t, available on HPUX;
// once we get the correct size of the struct, _RWSTD_NO_MBSTATE_T is 
// defined in rw/_config.h and we (and the users of the library) are left 
// with our definition only
#if defined (__HP_aCC)
#  define _INCLUDE__STDC_A1_SOURCE
#endif

#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>
#endif   // _RWSTD_NO_WCHAR_H

#ifndef _RWSTD_NO_WCTYPE_H
#  include <wctype.h>
#endif

#include <stdio.h>

int main ()
{
    printf ("#define _RWSTD_MBSTATE_T_SIZE %u\n", sizeof (mbstate_t));
}
