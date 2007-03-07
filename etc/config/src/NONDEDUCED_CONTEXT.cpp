// checking nondeduced context

#include "config.h"

#ifdef _RWSTD_NO_TYPENAME
#  define typename
#endif


// T::I is not deducible directly, but can be
// deduced if the T in const T* can (14.8.2.4, p3)
template <class T>
void foo (typename T::I, const T*);

template <class T>
struct A
{
    typedef T I;

    void bar () { foo (I (), this); }
};

// S<T>::I is not deducible directly, but can be deduced
// if the T in the second argument can (14.8.2.4, p3)
template <class T>
void foo (typename A<T>::I, const A<T>*);

template <class T>
void operator+ (typename A<T>::I, const A<T>&);

void foo ()
{
    A<int> a;

    1 + a;

    a.bar ();
}


#ifndef _RWSTD_NO_INLINE_MEMBER_TEMPLATES

template <class T>
struct B
{
    // U::I is not deducible directly, byt can be deduced if U can
    template <class U>
    void baz (const U&, typename U::I) { }
};


void bar ()
{
    A<int> a;
    B<int> b;

    b.baz (a, ' ');
}

#endif   // _RWSTD_NO_INLINE_MEMBER_TEMPLATES
