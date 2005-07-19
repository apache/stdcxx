template <class T>
struct S
{
    T foo ();

#ifndef _RWSTD_NO_MEMBER_TEMPLATES

    template <class U>
    U bar (U);

#endif   // _RWSTD_NO_MEMBER_TEMPLATES

};

// the file containing the definitions of S<T>::foo() and S<T>::bar(),
// implicit_inclusion_imp.cc,  is expected to be implicitly #included
// by the [pre]linker at the time of instantiation
