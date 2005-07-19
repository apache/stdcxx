// checking for friend templates of templates

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


template <class T>
struct A { enum { i }; };

template <class T>
struct B
{
private:

    template <class U, class V>
    friend int foo (A<U>, B<V>);

    int bar (int i) const { return i; }
};

template <class T, class U>
int foo (A<T> a, B<U> b)
{
    return b.bar (a.i);
}

#ifndef _RWSTD_NO_EXPLICIT_FUNC_INSTANTIATION

// explicitly instantiate to detect bugs such as SunPro 5.3's PR #30227
template int foo (A<int>, B<double>);

#endif   // _RWSTD_NO_EXPLICIT_FUNC_INSTANTIATION

int baz ()
{
    A<int> a;
    B<double> b;

    return foo (a, b);
}
