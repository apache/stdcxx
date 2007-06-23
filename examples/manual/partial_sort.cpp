/**************************************************************************
 *
 * partsort.cpp - Example program of partial sort.
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

#include <algorithm>   // for partial_sort
#include <iostream>    // for cout, endl
#include <iterator>    // for ostream_iterator
#include <vector>      // for vector

#include <examples.h>


int main()
{
    typedef std::vector<int, std::allocator<int> >                    Vector;
    typedef std::ostream_iterator<int, char, std::char_traits<char> > Iter;

    const Vector::value_type a[] = {
        17, 3,  5,  -4, 1, 12, -10, -1, 14, 7,
        -6, 8, 15, -11, 2, -2,  18,  4, -3, 0
    };

    Vector v1 (a + 0, a + sizeof a / sizeof *a);

    // Output original vector.
    std::cout << "For the vector: ";
    std::copy (v1.begin (), v1.end (), Iter (std::cout, " "));

    // Partial sort the first seven elements.
    std::partial_sort (v1.begin (), v1.begin () + 7, v1.end ());

    // Output result.
    std::cout << "\n\nA partial_sort of seven elements gives: \n     ";
    std::copy (v1.begin (), v1.end (), Iter (std::cout, " "));
    std::cout << std::endl;

    // A vector of ten elements.
    Vector v2 (Vector::size_type (10), 0);

    // Sort the last ten elements in v1 into v2.
    std::partial_sort_copy (v1.begin () + 10, v1.end (),
                            v2.begin (), v2.end ());

    // Output result.
    std::cout
        << "\nA partial_sort_copy of the last ten elements gives: \n     ";
    std::copy (v2.begin (), v2.end (), Iter (std::cout, " "));

    std::cout << std::endl;

    return 0;
}
