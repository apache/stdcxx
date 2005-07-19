/**************************************************************************
 *
 * max.cpp - Example program for finding maximum of a pair of values.
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/max.cpp#11 $
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

#include <algorithm>    // for max, min
#include <functional>   // for greater
#include <iostream>     // for cout

#include <examples.h>


int main ()
{
    double d1 = 10.0, d2 = 20.0;

    // Find minimum.
    double val1 = std::min (d1, d2);

    // The greater comparator returns the greater of the two values.
    double val2 = std::min (d1, d2, std::greater<double>());

    // Find minimum.
    double val3 = std::max (d1, d2);

    // The less comparator returns the smaller of the  two values.

    // Note that, like every comparison in the library, max is
    // defined in terms of the < operator, so using less here
    // is the same as using the max algorithm with a default
    // comparator.
    double val4 = std::max(d1, d2, std::less<double>());

    std::cout << val1 << " " << val2 << " "
              << val3 << " " << val4 << std::endl;

    return 0;
}
