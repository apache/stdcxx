/**************************************************************************
 *
 * pnt2fnct.cpp - Example program of pointer to function.
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/pnt2fnct.cpp#10 $
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

#include <algorithm>    // for copy, transform
#include <functional>   // for ptr_fun
#include <iostream>     // cout, endl
#include <iterator>     // for ostream_iterator
#include <deque>        // for deque
#include <vector>       // for vector

#include <examples.h>


int factorial (int x)
{
    return x > 1 ? x * factorial (x - 1) : x;
}


int main ()
{
    typedef std::deque<int, std::allocator<int> >                     Deque;
    typedef std::vector<int, std::allocator<int> >                    Vector;
    typedef std::ostream_iterator<int, char, std::char_traits<char> > Iter;

    // Initialize a deque with an array of integers.
    const Deque::value_type a[] = { 1, 2, 3, 4, 5, 6, 7 };
    Deque d (a + 0, a + sizeof a / sizeof *a);

    // Create an empty vector to store the factorials.
    Vector v (Vector::size_type (7));

    // Compute factorials of the contents of `d' store results in `v'.
    std::transform (d.begin (), d.end (), v.begin (),
                    std::ptr_fun (factorial));

    // Print the results.
    std::cout << "The following numbers: \n     ";
    std::copy (d.begin (), d.end (), Iter (std::cout," "));

    std::cout << "\n\nHave the factorials: \n     ";
    std::copy (v.begin (), v.end (), Iter (std::cout, " "));
    std::cout << std::endl;

    return 0;
}
