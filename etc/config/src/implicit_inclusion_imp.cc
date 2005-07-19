
// definitions of out-of-line members declared in implicit_inclusion_imp.h

template <class T>
T S<T>::foo ()
{
    return T ();
}

#ifndef _RWSTD_NO_MEMBER_TEMPLATES

template <class T>
template <class U>
U S<T>::bar (U u)
{
    return u;
}

#endif   // _RWSTD_NO_MEMBER_TEMPLATES
