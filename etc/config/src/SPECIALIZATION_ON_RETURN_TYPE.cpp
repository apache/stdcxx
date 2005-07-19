// checking for specialization on return type

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


template <class T>
T foo () { return T (); }

template <>
int foo<int>() { return 1; }

template <>
long foo<long>() { return 2; }

int main ()
{
    int result = 0;

    if (0 != foo<char>())
        result = result << 1 + 1;

    if (1 != foo<int>())
        result = result << 1 + 1;

    if (2 != foo<long>())
        result = result << 1 + 1;

    return result;
}
