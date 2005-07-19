/**************************************************************************
 *
 * b_search.cpp - Example program of binary search. 
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/b_search.cpp#12 $
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

#include <algorithm>    // for binary_search(), copy(), sort()
#include <deque>        // for deque
#include <functional>   // for less
#include <iostream>     // for cout, endl
#include <iterator>     // for ostream_iterator

#include <examples.h>


int main ()
{
    // Typedefs for convenience.
    typedef std::deque<short, std::allocator<short> > Deque;

    typedef std::ostream_iterator<Deque::value_type,
                                  char,
                                  std::char_traits<char> > os_iter;

    const Deque::value_type arr[] = { 0, 1, 2, 2, 3, 4, 2, 2, 6, 7 };

    // Populate and sort the container.
    Deque d (arr + 0, arr + sizeof arr / sizeof *arr);
    std::sort (d.begin (), d.end ());

    // Try binary_search variants.
    bool b1 = std::binary_search (d.begin (), d.end (), 3);
    bool b2 = std::binary_search (d.begin (), d.end (), 11,
                                  std::less<int>());

    // Output results.
    std::cout << "Container contents: ";
    std::copy (d.begin (), d.end (), os_iter (std::cout, " "));

    std::cout << "\nThe number 3 was " 
              << ("NOT found" + b1 * 4);

    std::cout << "\nThe number 11 was "
              << ("NOT found" + b2 * 4) << std::endl;

    return 0;
}
