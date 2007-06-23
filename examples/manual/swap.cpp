/**************************************************************************
 *
 * swap.cpp - Example program of swap algorithm.
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

#include <algorithm>   // for copy, iter_swap
#include <iostream>    // for cout, endl
#include <iterator>    // for ostream_iterator
#include <vector>      // for vector

int main ()
{
    typedef std::ostream_iterator<int, char, std::char_traits<char> > Iter;

    const int a[] = { 6, 7, 8, 9, 10, 1, 2, 3, 4, 5 };

    // Set up a vector.
    std::vector<int, std::allocator<int> > v (a, a + sizeof a / sizeof *a);

    // Output original vector.
    std::cout << "For the vector: ";
    std::copy (v.begin (), v.end (), Iter (std::cout, " "));

    // Swap the first five elements with the last five elements.
    std::swap_ranges (v.begin (), v.begin () + 5, v.begin () + 5);

    // Output result.
    std::cout << "\n\nSwaping the first five elements "
        "with the last five gives: \n     ";

    std::copy (v.begin (), v.end (), Iter (std::cout," "));

    // Now an example of iter_swap -- swap first and last elements.
    std::iter_swap (v.begin(), v.end () - 1);

    // Output result.
    std::cout << "\n\nSwaping the first and last elements gives: \n     ";
    std::copy (v.begin (), v.end (), Iter (std::cout, " "));

    std::cout << std::endl;

    return 0;
}
