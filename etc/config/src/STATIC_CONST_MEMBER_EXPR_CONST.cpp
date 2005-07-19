// checking for type-dependent constant expressions 

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#ifdef _RWSTD_NO_TYPENAME
#  define typename /* empty */
#endif   // _RWSTD_NO_TYPENAME

struct A { typedef int I; };

template <class T>
struct B
{
    typedef int           I;
    typedef typename T::I J;

    static const I i = 0;
    static const J j = 1;
};

template <class T>
const typename B<T>::I B<T>::i;

template <class T>
const typename B<T>::J B<T>::j;

template <class T>
int foo (T)
{
    enum { i = B<T>::i };
    enum { j = B<T>::j };

    return i + j;
}

int bar ()
{
    return foo (A ());
}
