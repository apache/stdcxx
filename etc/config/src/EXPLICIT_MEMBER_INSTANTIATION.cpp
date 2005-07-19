// checking for explicit instantiation of members

#ifdef _RWSTD_USE_CONFIG
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


template <class T>
T foo (T t) { return t; }

template <class T>
struct S
{
    T bar () const {
        // explicit instantiation of S<T> (and, consequently,
        // S<T>::bar()) must "implicitly" instantiate foo<T>(T)
        return foo (T ());
    }

    static void baz (T t) {
        foo (t);
    }
};

template int S<int>::bar () const;
template double S<double>::bar () const;

template void S<char>::baz (char);
template void S<unsigned>::baz (unsigned);
