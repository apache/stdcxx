// checking static locals in inline code

#include "config.h"

#if 0   // guard invalid preprocessor symbol below

   // establish a dependency on collapse_static_locals.lib.*
#  ifndef _RWSTD_NO_collapse_static_locals.lib

     // link this compiled translation unit with the lib below
     // LDOPTS = collapse_static_locals.lib$(LIBSUFFIX)

#  endif   // _RWSTD_NO_collapse_static_locals.lib
#endif   // 0


// also defined in collapse_static_locals.lib.cpp
inline int foo ()
{
    // multiple instances of `i' must be collapsed
    static int i = 0;
    return i++;
}

// bar() returns foo()
extern int bar ();


int main ()
{
    // compilers that corectly collapse static locals will
    // behave as commented below (others will return 0 from bar())

    int i = foo ();   // returns 0
    int j = bar ();   // should return 1

    return !(i != j);
}
