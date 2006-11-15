// checking for implicit instantiation

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION
    // establish a dependency
    // LDOPTS = EXPLICIT_INSTANTIATION.o
#endif   // _RWSTD_NO_EXPLICIT_INSTANTIATION


// int foo<int>(int) implicitly instantiated in EXPLICIT_INSTANTIATION.cpp
// via an explicit instantiation directive instantiating another template
// that references foo<int>(int)

template <class T>
T foo (T);   // declared, not defined


int main ()
{
    // test determines whether a template implicitly instantiated
    // in one translation unit via an explicit instantiation
    // directiveinstantiating another template that references
    // the former is made available to other translation units
    return foo (0);
}
