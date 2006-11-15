// checking static template members

#include "config.h"

#if 0   // guard invalid preprocessor symbol below

   // establish a dependency on collapse_template_statics.lib.*
#  ifndef _RWSTD_NO_collapse_template_statics.lib

     // link this compiled translation unit with the lib below
     // LDOPTS = collapse_template_statics.lib$(LIBSUFFIX)

#  endif   // _RWSTD_NO_collapse_template_statics.lib
#endif   // 0


// also defined in collapse_template_statics.lib.cpp
template <class T>
struct S
{
    // multiple instances of `t' must be collapsed
    static T t;
};

template <class T>
T S<T>::t;


// bar() returns S<int>::t++
extern int bar ();


int main ()
{
    // compilers that corectly collapse static template members
    // behave as commented below (others will return 0 from bar())

    int i = S<int>::t++;   // yields 0
    int j = bar ();        // should return 1

    return !(i != j);
}
