// checking for quiet NaN

#include <stdlib.h>

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


int main ()
{
#ifdef _RWSTD_NO_DBL_TRAPS

    // if floating point arithmetic doesn't trap, compute
    // the result of (0.0 / 0.0) and exit successfully if
    // the value of the result is a QNAN

    char *endptr = 0;

    // NaN is never equal to another NaN

    // compute NAN by 0.0 / 0.0 using strtod to foil optimizers
    double nan_1 = strtod ("000", &endptr) / strtod ("0.0", &endptr);
    double nan_2 = strtod ("0.0", &endptr) / strtod ("000", &endptr);

    return !(nan_1 != nan_2);

#else   // if defined (_RWSTD_NO_DBL_TRAPS)

    // if floating point arithmetic traps assume QNAN exists
    return 0;

#endif   // _RWSTD_NO_DBL_TRAPS

}
