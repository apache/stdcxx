/**************************************************************************
 *
 * reverse.cpp - Example program reverse algorithm.
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/reverse.cpp#10 $
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

#include <algorithm>   // for reverse, reverse_copy
#include <vector>      // for vector
#include <iostream>    // for cout, endl
#include <iterator>    // for ostream_iterator

#include <examples.h>


int main ()
{
    typedef std::vector<int, std::allocator<int> >                    Vector;
    typedef std::ostream_iterator<int, char, std::char_traits<char> > Iter;

    // Initialize a vector with an array of integers.
    const Vector::value_type a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    Vector v (a + 0, a + sizeof a / sizeof *a);

    // Print out elements in original (sorted) order.
    std::cout << "Elements before reverse: \n     ";
    std::copy (v.begin (), v.end (), Iter (std::cout, " "));

    // Reverse the ordering.
    std::reverse (v.begin (), v.end ());

    // Print out the reversed elements.
    std::cout << "\n\nElements after reverse: \n     ";
    std::copy (v.begin (), v.end (), Iter (std::cout, " "));

    std::cout << "\n\nA reverse_copy to cout: \n     ";
    std::reverse_copy (v.begin (), v.end (), Iter (std::cout, " "));
    std::cout << std::endl;

    return 0;
}
