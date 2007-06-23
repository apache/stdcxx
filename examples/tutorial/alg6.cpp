/**************************************************************************
 *
 * alg6.cpp - STL generic algorithms that produce new sequences.
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

#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iterator>
#include <functional>

#include <examples.h>

typedef std::ostream_iterator<int, char, std::char_traits<char> > ostrm_iter_type;


int square (int n) {
    return n * n;
}


class iotaGen {
public:
    iotaGen (int iv) : current (iv)
        { }
    int operator () () {
        return current++;
    }
private:
    int current;
};


// Illustrate the use of the transform algorithm.
void transform_example () {

    // Generate a std::list of values from 1 to 6.
    std::list<int, std::allocator<int> > aList;

    std::generate_n (std::inserter (aList, aList.begin ()), 6, iotaGen (1));
    std::cout << "Original list: ";
    std::copy (aList.begin (), aList.end (), ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;

    // Transform elements by squaring, copy into std::vector.
    std::vector<int, std::allocator<int> > aVec (6);

    std::transform (aList.begin (), aList.end (), aVec.begin (), square);
    std::cout << "After squaring: ";
    std::copy (aVec.begin (), aVec.end (), ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;

    // Transform std::vector again, in place, yielding 4th powers.
    std::transform (aVec.begin (), aVec.end (), aVec.begin (), square);
    std::cout << "After squaring again: ";
    std::copy (aVec.begin (), aVec.end (),ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;

    // Transform in parallel, yielding cubes.
    std::vector<int, std::allocator<int> > cubes (6);

    std::transform (aVec.begin (), aVec.end (), aList.begin (), cubes.begin (),
                   std::divides<int> ());
    std::cout << "After division: ";
    std::copy (cubes.begin (), cubes.end (), ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;
}


// Illustrate the use of the partial sum algorithm.
void partial_sum_example () {

    // Generate values 1 to 5.
    std::vector<int, std::allocator<int> > aVec (5);
    std::generate (aVec.begin (), aVec.end (), iotaGen (1));

    // Output partial sums.
    std::cout << "Partial sums examples" << std::endl;
    std::cout << "Partial sums : ";
    std::partial_sum (aVec.begin (), aVec.end (),
                 ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;

    // Output partial products.
    std::cout << "Partial products: ";
    std::partial_sum (aVec.begin (), aVec.end (),
                 ostrm_iter_type (std::cout, " "), std::multiplies<int> () );

    std::cout << std::endl;
}


// Illustrate the use of the adjacent difference algorithm.
void adjacent_difference_example () {

    // Generate values 1 to 5.
    std::vector<int, std::allocator<int> > aVec (5);
    std::generate (aVec.begin (), aVec.end (), iotaGen (1));

    // Output partial sums.
    std::cout << "Adjacent Differences examples" << std::endl;
    std::cout << "Adjacent Differences : ";
    std::adjacent_difference
        (aVec.begin (), aVec.end (), ostrm_iter_type (std::cout, " "));
    std::cout << std::endl;

    // Output partial products.
    std::cout << "Adjacent sums: ";
    std::adjacent_difference (aVec.begin (), aVec.end (),
                              ostrm_iter_type (std::cout, " "),
                              std::plus<int> ());

    std::cout << std::endl;
}


int main () {

    std::cout << "STL generic algorithms -- that transform sequences"
              << std::endl;

    transform_example ();
    partial_sum_example ();
    adjacent_difference_example ();

    std::cout << "End generic transform algorithms example" << std::endl;

    return 0;
}







