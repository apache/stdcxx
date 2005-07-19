// checking if wchar_t is a native type

#include "config.h"

int foo (wchar_t)
{
    return 0;
}

int foo (char);
int foo (signed char);
int foo (unsigned char);
int foo (short);
int foo (unsigned short);
int foo (int);
int foo (unsigned int);
int foo (long);
int foo (unsigned long);

int main ()
{
    wchar_t c;
    return foo (c);
}
