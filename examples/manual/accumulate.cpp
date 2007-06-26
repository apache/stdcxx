/**************************************************************************
 *
 * accum.cpp - Example program for accumulate.
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

#include <numeric>    // for accumulate
#include <vector>     // for vector
#include <functional> // for multiplies
#include <iostream>   // for cout

#include <examples.h>

int main ()
{
    // Typedef for convenience.
    typedef std::vector<int, std::allocator<int> > Vector;

    // Initialize a vector using an array of integers.
    const Vector::value_type arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    Vector v1 (arr, arr + sizeof arr / sizeof *arr);

    // Accumulate sums and products.
    Vector::value_type sum =
        std::accumulate (v1.begin (), v1.end (), 0);

    Vector::value_type prod =
        std::accumulate (v1.begin (), v1.end (), 1,
                         std::multiplies<Vector::value_type>());

    // Output the results.
    std::cout << "For the series: ";
    for (Vector::iterator i = v1.begin (); i != v1.end (); ++i)
        std::cout << *i << " ";

    std::cout << "where N = " << v1.size ()
              << "\nThe sum = (N * N + N) / 2 = " << sum
              << "\nThe product = N! = " << prod << std::endl;

    return 0;
}
