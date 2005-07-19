// checking for C library in namespace std

#include "config.h"

#ifndef _RWSTD_NO_NEW_HEADER

#include <cctype>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#else   // _RWSTD_NO_NEW_HEADER

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

#endif   // _RWSTD_NO_NEW_HEADER


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
