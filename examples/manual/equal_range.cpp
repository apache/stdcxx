/**************************************************************************
 *
 * eqlrange.cpp - Example program for finding the valid range for insertion 
 *                of a value in a container.
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

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

#include <examples.h>


int main ()
{
    // Typedef for convenience.
    typedef std::vector<int, std::allocator<int> > Vector;

    Vector::value_type arr[] = { 0, 1, 2, 2, 3, 4, 2, 2, 2, 6, 7 };

    // Set up a vector.
    Vector v1 (arr, arr + sizeof arr / sizeof *arr);

    // Try equal_range variants.
    std::pair<Vector::iterator, Vector::iterator> p1 =
        std::equal_range (v1.begin (), v1.end (), 3);

    std::pair<Vector::iterator, Vector::iterator> p2 =
        std::equal_range (v1.begin (), v1.end (), 2,
                     std::less<Vector::value_type> ()); 

    // Output results.
    std::cout << "\nThe equal range for 3 is: "
              << "(" << *p1.first << " , " 
              << *p1.second << ")" << std::endl;

    std::cout << "\nThe equal range for 2 is: "
              << "(" << *p2.first << " , " 
              << *p2.second << ")" << std::endl; 

    return 0;
}
