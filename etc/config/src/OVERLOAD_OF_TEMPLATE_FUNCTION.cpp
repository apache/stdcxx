// checking complicated partial ordering

#ifdef _RWSTD_USE_CONFIG
#  include "config.h"
#endif   // _RWSDT_USE_CONFIG


template <class T, class U>
class S { };

template<class T, class U>
S<T, U>& operator<< (S<T, U> &s, T)
{
    return s;
}

template<class T, class U>
S<T, U>& operator<< (S<T, U> &s, char)
{
    return s;
}

template <class U>
S<char, U>& operator<< (S<char, U> &s, char)
{
    return s;
}

template <class U>
S<int, U>& operator<< (S<int, U> &s, int)
{
    return s;
}

void foo ()
{
    S<char, int> sci;
    S<int, int>  sii;

    sci << 'a';
    sii << 1;
}
