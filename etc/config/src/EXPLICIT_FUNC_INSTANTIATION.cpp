// checking explicit function instantiation

#include "config.h"

template <class T>
int foo (T)
{
    return 0;
}

template int foo (int);
template int foo (char);
