// checking if floating point math traps

/***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 1999-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

/* IEEE 754 (IEC 559 supeseded by IEC 60559) specifies that
 * the following operations return NaN unless they trap:
 *
 *     1.   sqrt (n); n < 0
 *     2.   rem (x, 0.0), rem (inf, x)
 *     3.   0 * inf
 *     4.   0.0 / 0.0, inf / inf
 *     5.   inf - inf (when both inf have the same sign)
 *
 * Traps are defined as kind of an exception, typically
 * causing the delivery of SIGFPE (e.g., with Compaq C++
 * on Tru64 UNIX, except when using the -ieee flag which
 * disables traps as required by C).
 */

#include "nodbg.h"

double dbl_get_zero ();
double dbl_get_one ();

int main (int argc, char*[])
{
    nodbg ();

    double dbl_inf;
    double dbl_nan;

    double dbl_zero = dbl_get_zero ();
    double dbl_one  = dbl_get_one ();

    // compute infinity (division by zero may trap)
    dbl_inf = dbl_one / dbl_zero;

    // computation involving infinities that might trap
    dbl_nan = dbl_zero * dbl_inf;    // #3
    dbl_nan = dbl_zero / dbl_zero;   // #4a
    dbl_nan = dbl_inf / dbl_inf;     // #4b
    dbl_nan = dbl_inf - dbl_inf;     // #5

    if (argc > 2)
        return dbl_nan;

    if (argc > 1)
        return dbl_inf;

    // NEGATIVE test: successful exit status indicates a failure
    return 0;
}

// foil optimizers
double dbl_get_zero ()
{
    static double value = 0.0;

    return value;
}


double dbl_get_one ()
{
    static double value = 1.0;

    return value;
}
