// checking deprecated C headers and namespace std

#include <ctype.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"

int main ()
{
    (void)&std::isalpha;     // <ctype>
    (void)&std::setlocale;   // <clocale>
    std::pow (0.0, 0.0);     // <cmath>
    (void)&std::fopen;       // <cstdio>
    (void)&std::malloc;      // <cstdlib>
    (void)&std::strcpy;      // <cstring>
    (void)&std::clock;       // <ctime>
    return std::size_t (0);  // <cstddef>, <cstring>, ...
}
