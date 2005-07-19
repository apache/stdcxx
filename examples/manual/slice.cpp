/**************************************************************************
 *
 * slice.cpp -- Valarray slice examples
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/slice.cpp#13 $
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

#include <iostream>    // for cout, endl
#include <valarray.h>  // Includes valarray and provides stream inserter.
#include <examples.h>  // Generic header for all examples.


int main ()
{
    typedef std::valarray<int> Valarray;

    // create a non-const valarray of integers
    const Valarray::value_type ia [] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    Valarray vi (ia, sizeof ia / sizeof *ia);

    // print out its contents
    std::cout << "original valarray vi: "
              << vi << "\n\n";

    // print out a slice of the valarrayt object
    std::cout << "vi [slice (1, 3, 3)]: "
              << vi [std::slice (1, 3, 3)] << '\n';

    // create a const valarray object
    const Valarray vj (ia, sizeof ia / sizeof *ia);

    // print out a slice (uses a different operator)
    std::cout << "vi [slice (0, 5, 2)]: "
              << vj [std::slice (0, 5, 2)] << '\n';

    return 0;
}
