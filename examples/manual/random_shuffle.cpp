/**************************************************************************
 *
 * rndshufl.cpp - Example program of random shuffle algorithm.
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/random_shuffle.cpp#2 $
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

#include <algorithm>   // for random_shuffle
#include <iostream>    // for cout, endl
#include <iterator>    // for ostream_iterator
#include <string>      // for string

#include <examples.h>


int main ()
{
    // Create a string of doubles (unusual? maybe, but why not...)
    typedef std::basic_string<double, std::char_traits<double>,
                              std::allocator<double> > Bizarre;

    // Initialize a Bizarre with an array of values.
    const Bizarre::value_type a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    Bizarre b (a + 0, a + sizeof a / sizeof *a);

    typedef std::ostream_iterator<double, char, std::char_traits<char> >
        Iter;

    // Suppress decimal point in output.
    std::cout.precision (0);

    // Print out elements in original (sorted) order.
    std::cout << "Elements before random_shuffle: \n     ";
    std::copy (b.begin (), b.end (), Iter (std::cout," "));

    // Mix them up with random_shuffle.
    std::random_shuffle (b.begin (), b.end ());

    // Print out the mixed up elements.
    std::cout << "\n\nElements after random_shuffle: \n     ";
    std::copy (b.begin (), b.end (), Iter (std::cout, " "));
    std::cout << std::endl;

    return 0;
}
