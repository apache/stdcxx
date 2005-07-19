// checking for extern function template extension

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

// establish a dependency on extern_function_template.{cpp,o}
#ifndef _RWSTD_NO_extern_function_template

// link the object file produced by compiling this file
// with the object file below
// LDOPTS = ./extern_function_template.o

#endif   // _RWSTD_NO_extern_function_template


template <class T>
struct S
{
    T t;
};


template <class T>
S<T> foobar (S<T> s)
{
    // foobar<int>() explicitly instantiated in extern_function_template.cpp
    // is defined to set s.t to 1
    s.t = -1;

    return s;
}

extern template S<int> foobar (S<int>);

int main ()
{
    // S<int>::bar () is defined to return 1 in extern_function_template.o

    S<int> s;

    int res = 0;

    // verify that the call compiles and links
    res += foobar (s).t;

    // verify that the call resolves to the definition emitted
    // by the explicit instantiation in extern_function_template.o
    // and not the one here
    return !(1 == res);
}
