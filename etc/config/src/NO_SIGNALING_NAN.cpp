// checking for signaling NaN

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#include <stdio.h>

#include <float.h>

#if !defined (_RWSTD_NO_FENV_H)
   // [try to] get floating-point exception support
#  include <fenv.h>
#else
#  include <ieeefp.h>
#endif   // _RWSTD_NO_FENV_H


#ifndef _RWSTD_NO_INFINITY

#  if defined _RWSTD_DBL_SNAN_BITS

union {
    char bits[sizeof (double)];
    double val;
} snan = { _RWSTD_DBL_SNAN_BITS };

// assignment or initialization should not trap
static double d = snan.val;

#  else   // if !defined (_RWSTD_DBL_SNAN_BITS)

double foo ()
{
    const int endian = 1;

    // compute infinity (division may trap)
    double x = 0.0;
    x = 1.0 / x;

    // turn infinity into a signaling NaN by setting the LSB
    ((char*)&x)[(sizeof x - 1) * (!!*(char*)&endian)] |= 1;

    return x;
}

static double d = foo ();

#  endif   // _RWSTD_DBL_SNAN_BITS


#endif   // _RWSTD_NO_INFINITY


// prevent optimizers from eliminating expressions such as (d == d)
volatile double *pd = &d;


int main ()
{
    // floating point operations on SNAN's might trap
    // if that happens the test will be considered successful

#if    defined (FE_DIVBYZERO) || defined (FE_INEXACT) || defined (FE_INVALID) \
    || defined (FE_OVERFLOW)  || defined (FE_UNDERFLOW)

#  pragma STDC FENV_ACCESS ON

    const int all = 0

#  ifdef FE_ALL_EXCEPT
        | FE_ALL_EXCEPT |
#  else   // if !defined (FE_ALL_EXCEPT)

#    ifdef FE_DIVBYZERO
        | FE_DIVBYZERO |
#    endif   // FE_DIVBYZERO

#    ifdef FE_INEXACT
        | FE_INEXACT |
#    endif   // FE_INEXACT

#    ifdef FE_INVALID
        | FE_INVALID |
#    endif   // FE_INVALID

#    ifdef FE_OVERFLOW
        | FE_OVERFLOW |
#    endif   // FE_OVERFLOW

#    ifdef FE_UNDERFLOW
        | FE_UNDERFLOW |
#    endif   // FE_UNDERFLOW

#  endif   // FE_ALL_EXCEPT

        0;   // defintion of all


    // assume a C99 conforming environment
    feclearexcept (all);

    d *= d;

    if (fetestexcept (all))
        return *pd != d;   // have a SNAN

    d /= d;
    
    if (fetestexcept (all))
        return *pd != d;   // have a SNAN

#elif defined (sun) || defined (__sun) || defined (__sun__)

    fpsetsticky (0);

    // any math operation should trap
    d *= d;

    if (fpgetsticky ())
        return *pd != d;   // have a SNAN

    d /= d;
    
    if (fpgetsticky ())
        return *pd != d;   // have a SNAN

#endif   // sun

    // NEGATIVE test: successful exit status indicates a failure
    return 0;
}
