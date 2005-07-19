
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

#if defined (INSTANTIATE)

template struct S<int>;

#else   // if !defined (INSTANTIATE)

extern template struct S<int>;

#endif   // INSTANTIATE
