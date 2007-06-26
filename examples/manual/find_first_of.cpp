/**************************************************************************
 *
 * find_f_o.cpp - Example program for finding a subsequence. 
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

#include <algorithm>    // for find_first_of()
#include <functional>   // for equal_to
#include <iostream>     // for cout, endl
#include <iterator>     // for ostream_iterator
#include <vector>       // for vector

#include <examples.h>

int main ()
{
    // Typedef for convenience.
    typedef std::vector<int, std::allocator<int> > Vector;

    const Vector::value_type a1[] = { 0, 1, 2, 2, 3, 4, 2, 2, 6, 7 };
    const Vector::value_type a2[] = { 6, 4 };

    // Set up two vectors.
    const Vector v1 (a1, a1 + sizeof a1 / sizeof *a1);
    const Vector v2 (a2, a2 + sizeof a2 / sizeof *a2);

    // Try both find_first_of variants.
    Vector::const_iterator it1 =
        std::find_first_of (v1.begin (), v1.end (), v2.begin (), v2.end ()); 

    Vector::const_iterator it2 =
        std::find_first_of (v1.begin (), v1.end (), v2.begin (), v2.end (),
                            std::equal_to<Vector::value_type>());

    // Create an output stream iterator.
    std::ostream_iterator<Vector::value_type,
                          char,
                          std::char_traits<char> > out (std::cout, " " );

    // Output results.
    std::cout << "For the vectors { ";
    std::copy (v1.begin (), v1.end (), out);

    std:: cout << "} and { ";
    std::copy (v2.begin (), v2.end (), out);

    std::cout << "}\nboth versions of find_first_of point to: "
              << *it1 << std::endl; 

    // Return 0 on success, non-0 on failure.
    return !(*it1 == *it2);
}
