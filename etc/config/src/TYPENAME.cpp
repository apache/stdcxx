// checking the typename keyword

#if defined _RWSTD_USE_CONFIG
#  include "config.h"
#endif   // _RWSTD_USE_CONFGI


struct A
{
    typedef int I;
};


template <class T>
struct B
{
    typedef typename T::I I;
    typedef const T*      J;

    I foo (I) const;
    J bar (J) const;
};


template <class T>
typename B<T>::I B<T>::foo (typename B<T>::I i) const
{
    return i;
}


template <class T>
typename B<T>::J B<T>::bar (J j) const
{
    return j;
}


int foobar ()
{
    B<A> b;

    // instantiate B and its members
    return b.foo (0) + !b.bar (0);
}
