// -*- C++ -*-
/***************************************************************************
 *
 * cmath - C++ Standard library interface to the ANSI C header math.h
 *
 * $Id: //stdlib/dev/include/ansi/_cmath.h#8 $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#ifndef _RWSTD_CMATH_H_INCLUDED
#define _RWSTD_CMATH_H_INCLUDED

#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


#ifndef HUGE_VAL
#  define HUGE_VAL _RWSTD_DBL_MAX
#endif   // HUGE_VAL 


extern "C" {

double acos (double);
double asin (double);
double atan (double);
double atan2(double, double);
double cos (double);
double sin (double);
double tan (double);
double cosh (double);
double sinh (double);
double tanh (double);
double exp (double);
double frexp (double, int *);
double ldexp (double, int);
double log (double);
double log10 (double);
double modf (double, double*);
double pow (double, double);
double sqrt (double);
double ceil (double);
double fabs (double);
double floor (double);
double fmod (double, double);


}

}   // namespace std


#endif   // _RWSTD_CMATH_H_INCLUDED
