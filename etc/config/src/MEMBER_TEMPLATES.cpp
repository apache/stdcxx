// checking for member templates

// both tests must successfully compile
#include "INLINE_MEMBER_TEMPLATES.cpp"

struct C
{
    template <class T>
    struct CA;   // fully defined below

    template <class T>
    void foo (T);   // fully defined below
};


// verify that member templates may be defined outside
// the body of the class of which they are a member
template <class T>
struct C::CA { };

template <class T>
void C::foo (T) { }


template <class T>
struct D
{
    template <class U>
    struct DA;   // fully defined below

    template <class U>
    void foo (U);   // fully defined below
};


// verify that member templates may be defined outside
// the body of the template of which they are a member
template <class T>
template <class U>
struct D<T>::DA { };

template <class T>
template <class U>
void D<T>::foo (U) { }


// check that member templates are usable
void bar (C c, C::CA<int> cca, D<int> d, D<char>::DA<int> dda)
{
    // all used types are complete
    c.foo (cca);

    // all used types are complete
    d.foo (dda);
}
