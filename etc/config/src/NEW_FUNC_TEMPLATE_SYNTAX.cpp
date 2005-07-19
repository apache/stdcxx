// checking template specialization syntax

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

template <class T>
int foo (T)
{
    return 0;
}

template<>
int foo (int)
{
    return 1;
}

int bar ()
{
    return !(1 == foo (0));
}
