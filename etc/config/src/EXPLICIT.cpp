// checking explicit keyword

#include "config.h"

struct S
{
    S (double) { }

    explicit S (int) { }
};
