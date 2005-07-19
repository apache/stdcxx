// checking for conversion in template default arguments

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

template <class T>
struct A { };

template <class T>
void foo (T t = T ('x'))
{
}

template <class T>
A<T> bar (T t = T ('y'))
{
    return A<T>();
}

void baz ()
{
    foo<int>();
    bar<int>();
}
