
// avoid including <config.h> here to avoid inadvertently
// trying to introduce dependencies on other config tests
// that the config infrastructure won't know about (since
// it only searches .cpp files for them)
// #include <config.h>

#if __GNUG__ >= 3
   // disable gcc 3.x (and beyond) error: ISO C++ forbids the use
   // of `extern' on explicit instantiations
#  pragma GCC system_header
#endif   // gcc >= 3


template <class T>
struct S
{
    T t;

    T foo () const;

    T bar () const;

    T baz () const {
        return bar ();
    }

#if defined (NO_INLINE_MEMBER_TEMPLATE)

    int inline_member_template (int) const {
        return 0;
    }

#else   // if !defined (NO_INLINE_MEMBER_TEMPLATE)

    template <class U>
    U inline_member_template (U) const {
        return U ();
    }

#endif   // NO_INLINE_MEMBER_TEMPLATE

#if defined (NO_MEMBER_TEMPLATE)

    int member_template (int) const {
        return 0;
    }

#else   // if !defined (NO_MEMBER_TEMPLATE)

    template <class U>
    U member_template (U) const;

#endif   // NO_MEMBER_TEMPLATE

};

template <class T>
inline T S<T>::bar () const
{
    return foo ();
}

template <class T>
T S<T>::foo () const
{
#if defined (INSTANTIATE)
    return 1;
#else   // if !defined (INSTANTIATE)
    return 0;
#endif   // INSTANTIATE
}

#if !defined (NO_MEMBER_TEMPLATE)

template <class T>
template <class U>
U S<T>::member_template (U) const
{
    return U ();
}

#endif   // NO_MEMBER_TEMPLATE


#if defined (INSTANTIATE)

template struct S<int>;

#else   // if !defined (INSTANTIATE)

extern template struct S<int>;

#endif   // INSTANTIATE
