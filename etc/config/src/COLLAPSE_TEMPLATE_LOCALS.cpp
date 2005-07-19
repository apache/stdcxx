// checking static locals in template code

#include "config.h"

#if 0   // guard invalid preprocessor symbol below

   // establish a dependency on collapse_template_locals.lib.*
#  ifndef _RWSTD_NO_collapse_template_locals.lib

     // link this compiled translation unit with the lib below
     // LDOPTS = ./collapse_template_locals.lib$(LIBSUFFIX)

#  endif   // _RWSTD_NO_collapse_template_locals.lib
#endif   // 0


// also defined in collapse_template_locals.lib.cpp
template <class T>
T foo (T)
{
    // multiple instances of `t' must be collapsed
    static T t;
    return t++;
}

// bar() returns foo (0)
extern int bar ();


int main ()
{
    // compilers that corectly collapse static locals will
    // behave as commented below (others will return 0 from bar())

    int i = foo (0);   // returns 0
    int j = bar ();    // should return 1

    return !(i != j);
}
