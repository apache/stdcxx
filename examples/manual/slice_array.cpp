/**************************************************************************
 *
 * slice_array.cpp -- Valarray slice array examples
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <iostream>    // for cout, endl
#include <valarray.h>  // Includes valarray and provides stream inserter.
#include <examples.h>  // Generic header for all examples.

typedef std::valarray<int> valarray_t;

int main(void) {

    valarray_t::value_type ibuf[9] = {0,1,2,3,4,5,6,7,8};

    // Create a valarray initialized to the sequence of integers above.
    valarray_t vi (ibuf,9);

    std::cout << "original valarray vi\n\n" << vi << "\n\n";

    // Display a slice of the valarray.
    std::cout << "vi[slice(0,3,3)]\n\n"  << vi[std::slice(0,3,3)] << "\n\n";

    // Unfortunately, slice_arrays were not designed to be easily combined.
    // An explicit conversion to valarray is required, leaving us with
    // this awkward casting syntax.
    vi[std::slice (0,3,3)] = static_cast<valarray_t>(vi[std::slice (1,3,3)]) +
                             static_cast<valarray_t>(vi[std::slice (2,3,3)]);

    // The operation above uses slices ([0-2],3,3) to treat our valarray
    // as a 3-dimensional array:
    //
    //  0 1 2                 3 = 1 + 2
    //  3 4 5                 9 = 4 + 5
    //  6 7 8                15 = 7 + 8
    //  | | |                 |   |   |
    //  | | --slice(2,3,3)    |   |   -slice_array for arithmetic operand.
    //  | ----slice(1,3,3)    |   -----slice_array for arithmetic operand.
    //  ------slice(0,3,3)    ----slice_array assignment refers to valarray.

    std::cout << "vi[slice(0,3,3)] = vi[slice(1,3,3)] + vi[slice(2,3,3)]\n\n"
              << vi << std::endl;

    return 0;
}
