/**************************************************************************
 *
 * find.cpp - Example program for finding an occurence of value in a 
 *            sequence.
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

#include <vector>      // for vector
#include <algorithm>   // for adjacent_find, find
#include <functional>  // for bind1st, equal_to
#include <iostream>    // for cout, endl

#include <examples.h>


int main ()
{
    // Typedef for convenience.
    typedef std::vector<int, std::allocator<int> > Vector;

    const Vector::value_type arr[] = { 0, 1, 2, 2, 3, 4, 2, 2, 6, 7 };

    // Set up a vector.
    const Vector v1 (arr, arr + sizeof arr / sizeof *arr);

    // Try find.
    Vector::const_iterator it = std::find (v1.begin (), v1.end (), 3);

    std::cout << *it << ' ';

    // Try find_if.
    it = std::find_if (v1.begin (), v1.end (),
                       std::bind1st (std::equal_to<Vector::value_type>(), 3));

    std::cout << *it << ' ';

    // Try both adjacent_find variants.
    it = std::adjacent_find (v1.begin (), v1.end ());

    std::cout << *it << ' ';

    it = std::adjacent_find (v1.begin (), v1.end (),
                             std::equal_to<Vector::value_type>());

    std::cout << *it << std::endl;

    return 0;
}
