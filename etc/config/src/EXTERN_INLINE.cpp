// checking for truly extern inline

#include "config.h"

#if 0   // guard invalid preprocessor symbol below

   // establish a dependency on extern_inline.lib.*
#  ifndef _RWSTD_NO_extern_inline.lib

     // link this compiled translation unit with the lib below
     // LDOPTS = ./extern_inline.lib$(LIBSUFFIX)

#  endif   // _RWSTD_NO_extern_inline.lib
#endif   // 0


extern int foo (int);

int main ()
{
    return !(1 == foo (0));
}
