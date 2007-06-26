/**************************************************************************
 *
 * nthelem.cpp - Example program for rearranging a collection based on nth
 *               element.
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

#include <algorithm>   // for swap
#include <vector>      // for vector
#include <iostream>    // for cout, endl

#include <examples.h>


template <class RandomAccessIterator>
void quik_sort (RandomAccessIterator beg, RandomAccessIterator end)
{
    const long dist = long (end - beg);

    // Stop condition for recursion.
    if (dist > 2) {
        // Use nth_element to do all the work for quik_sort.
        std::nth_element (beg, beg + (dist / 2), end);

        // Recursive calls to each remaining unsorted portion.
        quik_sort (beg, beg + (dist / 2 - 1));
        quik_sort (beg + (dist / 2 + 1), end);
    }
    else if (2 == dist && *(end - 1) < *beg)
        std::swap (beg, end);
}


int main ()
{
    typedef std::vector<int, std::allocator<int> > Vector;

    // Initialize a vector using an array of integers.
    const Vector::value_type arr[] = { 37, 12, 2, -5, 14, 1, 0, -1, 14, 32 };

    Vector v (arr, arr + sizeof arr / sizeof *arr);

    // Print the initial vector.
    std::cout << "The unsorted values are: \n     ";

    for (Vector::iterator i = v.begin (); i != v.end (); ++i)
        std::cout << *i << ", ";

    std::cout << "\n\n";

    // Use the new sort algorithm.
    quik_sort (v.begin (), v.end ());

    // Output the sorted vector.
    std::cout << "The sorted values are: \n     ";
    for (Vector::iterator j = v.begin (); j != v.end (); ++j)
        std::cout << *j << ", ";

    std::cout << "\n\n";

    return 0;
}
