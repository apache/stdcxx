// checking for explicit instantiation of ctors

#ifdef _RWSTD_USE_CONFIG
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


template <class T>
struct S
{
    S () { }

    S (const S&) { }
};

template S<int>::S ();
template S<double>::S ();

template S<char>::S (const S<char>&);
template S<unsigned>::S (const S<unsigned>&);
