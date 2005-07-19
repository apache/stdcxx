
#include "config.h"

int foo (int i)
{
    if (i)
        throw i;

    return foo (i + 1);
}
