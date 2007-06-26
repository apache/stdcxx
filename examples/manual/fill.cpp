/**************************************************************************
 *
 * fill.cpp - Example program for initializing a range with a given value.
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

#include <algorithm>   // for copy(), fill()
#include <iostream>    // for cout
#include <iterator>    // for ostream_iterator
#include <vector>      // for vector

#include <examples.h>


int main ()
{
    // Typedef for convenience.
    typedef std::vector<int, std::allocator<int> > Vector;

    const Vector::value_type arr[] = { 1, 2, 3, 4 };

    // Set up two vectors.
    Vector v1 (arr, arr + sizeof arr / sizeof *arr);
    Vector v2 (v1.begin (), v1.end ());

    // Set up one empty vector.
    Vector v3;

    // Fill all of v1 with 9.
    std::fill (v1.begin (), v1.end (), 9);

    // Fill first 3 of v2 with 7.
    std::fill_n (v2.begin (), 3, 7);

    // Use insert iterator to fill v3 with 5 11's.
    std::fill_n (std::back_inserter (v3), 5, 11);

    // Copy all three to cout.
    std::ostream_iterator<Vector::value_type,
                          char,
                          std::char_traits<char> > out (std::cout, " ");

    std::copy (v1.begin (), v1.end (), out);
    std::cout << std::endl;

    std::copy (v2.begin (), v2.end (), out);
    std::cout << std::endl;

    std::copy (v3.begin (), v3.end (), out);
    std::cout << std::endl;

    // Fill cout with 3 5's.
    std::fill_n (out, 3, 5);
    std::cout << std::endl;

    return 0;
}
