/**************************************************************************
 *
 * ul_bound.cpp - Example program of upper bound algorithm.
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

#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>

int main ()
{
    typedef std::vector<int, std::allocator<int> > Vector;
    typedef Vector::const_iterator                 Iterator;

    const int a[] = { 0, 1, 2, 2, 3, 4, 5, 5, 5, 6, 7 };

    // Set up a vector.
    const Vector v (a, a + sizeof a / sizeof *a);

    // Try lower_bound variants.
    Iterator it1 = std::lower_bound (v.begin (),v.end (), 3);
    Iterator it2 = std::lower_bound (v.begin (),v.end (), 2,
                                     std::less<int>());

    // Try upper_bound variants.
    Iterator it3 = std::upper_bound (v.begin (), v.end (), 3);
    Iterator it4 = std::upper_bound (v.begin (), v.end (), 2,
                                     std::less<int>());

    std::cout << "\n\nThe upper and lower bounds of 3: ( "
              << *it1 << " , " << *it3
              << " ]\n\n\nThe upper and lower bounds of 2: ( "
              << *it2 << " , " << *it4 << " ]\n";

    return 0;
}
