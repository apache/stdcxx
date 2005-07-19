// checking for dependent template

#include "config.h"

#ifdef _RWSTD_NO_TYPENAME

#  define typename

#endif

template <class T>
struct A
{
    template <class U>
    struct B {
        typedef A<U> X;
    };
};

template <class T, class U>
void foo (T, U)
{
    // checking whether the template keyword is allowed
    // where required by 14.2, p4
    typedef typename A<T>::template B<U>::X ABX;
}

void bar ()
{
    foo (0, '\0');
}
