// checking for quiet NaN

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

#include <stdlib.h>

#include "config.h"


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
