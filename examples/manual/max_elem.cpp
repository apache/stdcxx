/**************************************************************************
 *
 * max_elem.cpp - Example program for finding maximum value in a range.
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

#include <algorithm>    // for max_element
#include <functional>   // for less
#include <iostream>     // for cout, endl
#include <vector>       // for vector


int main ()
{
    typedef std::vector<int, std::allocator<int> > Vector;
    typedef Vector::iterator                       Iterator;

    const Vector::value_type d1[] = { 1, 3, 5, 32, 64 };
    // Set up vector.
    Vector v1(d1 + 0, d1 + sizeof d1 / sizeof *d1);

    // Find the largest element in the vector.
    Iterator it1 = std::max_element (v1.begin (), v1.end ());

    // Find the largest element in the range from
    // the beginning of the vector to the 2nd to last.
    Iterator it2 = std::max_element (v1.begin (), v1.end () - 1,
                                     std::less<int>());

    // Find the smallest element.
    Iterator it3 = std::min_element (v1.begin (), v1.end ());

    // Find the smallest value in the range from
    // the beginning of the vector plus 1 to the end.
    Iterator it4 = std::min_element (v1.begin () + 1, v1.end (),
                                     std::less<int>());

    std::cout << *it1 << " " << *it2 << " "
              << *it3 << " " << *it4 << std::endl;

    return 0;
}
