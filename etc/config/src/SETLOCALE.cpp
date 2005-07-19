// checking for setlocale() in <locale.h>

#include <locale.h>

#include "config.h"

#ifndef _RWSTD_NO_NAMESPACE

namespace std { }
using namespace std;

#endif   // _RWSTD_NO_NAMESPACE

int main ()
{
  // 7.11.1.1 in C99
    return !(0 != setlocale);
}
