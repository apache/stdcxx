/**************************************************************************
 *
 * inr_prod.cpp - Example program computes the inner product A X B of two
 *                ranges A and B.
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

#include <algorithm>     // for copy()
#include <functional>    // for plus, minus
#include <iostream>      // for cout, endl
#include <iterator>      // for ostream_iterator
#include <list>          // for list
#include <numeric>       // for inner_product()
#include <vector>        // for vector

#include <examples.h>


int main ()
{
    typedef std::list<int, std::allocator<int> >                      List;
    typedef std::vector<int, std::allocator<int> >                    Vector;
    typedef std::ostream_iterator<int, char, std::char_traits<char> > os_iter;

    // Initialize a list and a vector using arrays of ints.
    int a1[] = { 6, -3, -2 };
    int a2[] = { -2, -3, -2 };

    List   l (a1, a1 + sizeof a1 / sizeof *a1);
    Vector v (a2, a2 + sizeof a2 / sizeof *a2);

    // Calculate the inner product of the two sets of values.
    List::value_type prod = std::inner_product (l.begin (), l.end (),
                                                v.begin (), 0);

    // Calculate a wacky inner product using the same values.
    List::value_type wacky =
        std::inner_product (l.begin (), l.end (), v.begin (), 0, 
                            std::plus<List::value_type>(),
                            std::minus<List::value_type>());
    // Print the output.
    std::cout << "For the sets of numbers: { ";
    std::copy (v.begin (), v.end (), os_iter (std::cout, " "));

    std::cout << "} and { ";
    std::copy (l.begin (), l.end (), os_iter (std::cout, " "));

    std::cout << "}\nThe inner product is: " << prod
              << "\nThe wacky result is:  " << wacky
              << std::endl;

    return 0;
}
