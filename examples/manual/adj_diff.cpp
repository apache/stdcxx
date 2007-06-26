 /**************************************************************************
 *
 * adj_diff.cpp - Example program for adjacent_difference. 
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

#include <numeric>       // for adjacent_difference
#include <vector>        // for vector
#include <iostream>      // for cout
#include <iterator>      // for ostream_iterator
#include <functional>    // for multiplies

#include <examples.h>


int main ()
{
    // Typedefs for convenience.
    typedef std::vector<long, std::allocator<long> > Vector;

    typedef std::ostream_iterator <Vector::value_type,
                                   char,
                                   std::char_traits<char> > os_iter;

    // Initialize a Vector of ints from an array.
    const Vector::value_type arr [] = {
        1, 1, 2, 3, 5, 8, 13, 21, 34, 55
    };

    Vector v (arr + 0, arr + sizeof arr / sizeof *arr);

    // Two vectors initialized to all zeros for storing results.
    Vector diffs (v.size ()),
           prods (v.size ());

    // Calculate difference(s) using default operator (minus).
    std::adjacent_difference (v.begin (), v.end (), diffs.begin ());

    // Calculate difference (s) using the times operator.
    std::adjacent_difference (v.begin (), v.end (), prods.begin (),
                              std::multiplies<Vector::value_type> ());

    // Create an ostream_iterator object (for convenience).
    os_iter osit (std::cout, " ");

    // Output the results.
    std::cout << "For the vector: " << std::endl << "     ";
    std::copy (v.begin (), v.end (), osit);

    std::cout << std::endl << std::endl;

    std::cout << "The differences between adjacent elements are: "
              << std::endl << "    ";

    std::copy (diffs.begin (), diffs.end (), osit);
    std::cout << std::endl << std::endl;

    std::cout << "The products of adjacent elements are: "
              << std::endl << "     ";

    std::copy (prods.begin (), prods.end (), osit);
    std::cout << std::endl;

    return 0;
}
