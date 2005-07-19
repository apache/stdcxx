// checking if long double is a native type

#include "config.h"

int foo (float)
{
    return 1;
}


int foo (double)
{
    return 2;
}


int foo (long double)
{
    return 0;
}

int main ()
{
    long double ld = 0;

    return foo (ld);
}
