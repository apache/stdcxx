// checking for explicit instantiation

#if defined (_RWSTD_USE_CONFIG)
#include "config.h"
#endif   // _RWSTD_USE_CONFIG

// IMPLICIT_INSTANTIATION.cpp depends on foo<T>(T)
template <class T>
T foo (T t) { return t; }

template <class T>
struct S
{
    T bar () {
        // explicit instantiation of S<T> (and, consequently,
        // S<T>::bar()) must "implicitly" instantiate foo<T>(T)
        return foo (T ());
    }
};

// the symbols expected to be produced by the explicit instantiation
// directives are:
//     S<int>::bar(), foo<int>(int)
//     S<char>::bar(), foo<char>(char)

// IMPLICIT_INSTANTIATION.cpp uses foo<int>(int)
template class S<int>;
template class S<char>;
