// checking for fpos_t in <stdio.h>

#include <stdio.h>

#include "config.h"

#ifndef _RWSTD_NO_NAMESPACE

namespace std { }
using namespace std;

#endif   // _RWSTD_NO_NAMESPACE

int main ()
{
    fpos_t pos;
    return !(0 != &pos);
}
