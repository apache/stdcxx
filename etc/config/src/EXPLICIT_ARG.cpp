// checking explicit function template arguments

#include "config.h"


template<class T>
int foo ()
{
    return 0;
}

int main ()
{
    return foo<int>();
}
