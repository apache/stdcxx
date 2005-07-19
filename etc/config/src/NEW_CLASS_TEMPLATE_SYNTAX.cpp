// checking class template specialization

#include "config.h"

template <class T>
struct S
{
    enum { e = 0 };
};

template <>   // <-- testing
struct S<int>
{
    enum { e = 1 };
};
