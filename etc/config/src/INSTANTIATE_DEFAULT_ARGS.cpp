// checking if default args are instantiated

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


template <class T>
struct A
{
    void foo (const T& = T ()) { }
};

struct B
{
private:
    B () { }
};

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION
// 14.7.2, p9 - an explicit instantiation does not constitute
//              a use of a default argument
template class A<B>;

#else   // if defined (_RWSTD_NO_EXPLICIT_INSTANTIATION)

void foo ()
{
    &A<int>::foo;
}

#endif   // _RWSTD_NO_EXPLICIT_INSTANTIATION
