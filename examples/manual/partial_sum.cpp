/**************************************************************************
 *
 * partsum.cpp - Example program for partial sum.
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

#include <algorithm>  // for copy
#include <functional> // for multiplies
#include <iostream>   // for cout, endl
#include <iterator>   // for ostream_iterator
#include <numeric>    // for partial_sum
#include <vector>     // for vector

#include <examples.h>


int main ()
{
    typedef std::vector<int, std::allocator<int> >                    Vector;
    typedef std::ostream_iterator<int, char, std::char_traits<char> > Iter;

    // Initialize a vector using an array of integers.
    const Vector::value_type a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    Vector v (a + 0, a + sizeof a / sizeof *a);

    // Create an empty vectors to store results.
    Vector sums  (Vector::size_type (10));
    Vector prods (Vector::size_type (10));

    // Compute partial_sums and partial_products.
    std::partial_sum (v.begin (), v.end (), sums.begin ());
    std::partial_sum (v.begin (), v.end (), prods.begin (),
                      std::multiplies<Vector::value_type>());

    // Output the results.
    std::cout << "For the series: \n     ";
    std::copy (v.begin (), v.end (), Iter (std::cout, " "));

    std::cout << "\n\nThe partial sums: \n     ";
    std::copy (sums.begin (), sums.end (), Iter (std::cout, " "));
    std::cout << " should each equal (N*N + N)/2\n\n";

    std::cout << "The partial products: \n     ";
    std::copy (prods.begin (), prods.end (), Iter (std::cout, " "));
    std::cout << " should each equal N!" << std::endl;

    return 0;
}
