/**************************************************************************
 *
 * distance.cpp - Example program for distance between two iterators.
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

#include <cassert>
#include <iterator>
#include <iostream>
#include <vector>

#include <examples.h>


int main ()
{
    // Typedefs for convenience.
    typedef std::vector<int, std::allocator<int> > Vector;

    typedef std::ostream_iterator<Vector::value_type,
                                  char,
                                  std::char_traits<char> > os_iter;

    // Initialize a vector using an array.
    const Vector::value_type arr [] = { 3, 4, 5, 6, 7, 8 };
    const Vector v (arr + 0, arr + sizeof arr/ sizeof *arr);

    // Declare a vector iterator, s.b. a ForwardIterator.
    Vector::const_iterator it = v.begin () + 3;

    // Output the original vector.
    std::cout << "For the vector: ";
    std::copy (v.begin (), v.end (), os_iter (std::cout, " "));
    std::cout << '\n';

    std::cout << "\nWhen the iterator is initialized to point to "
              << *it << '\n';

    // Compute the distance of it from the first element.
    Vector::difference_type dist = 0;

#ifndef _RWSTD_NO_EXT_VOID_DISTANCE

    // using the extended overload of function template distance()
    // provided by this implmentation, compute the distance between
    // the two iterators
    std::distance (v.begin (), it, dist);

    assert (3 == dist);

#endif   // _RWSTD_NO_EXT_VOID_DISTANCE

    // do the same using the standard function template distance()
    dist = std::distance (v.begin (), it);

    std::cout << "The distance between the beginning and "
              << *it << " is " << dist << '\n';

    return 0;
}
