/**************************************************************************
 *
 * mismatch.cpp - Example program of comparing elements from two sequences
 *                and returning the first two elements that don't 
 *                match each other.
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

#include <algorithm>    // mismatch
#include <functional>   // less_equal
#include <iostream>     // cout, endl
#include <utility>      // pair
#include <vector>       // vector

#include <examples.h>


int main ()
{
    typedef std::vector<int, std::allocator<int> > Vector;
    typedef Vector::const_iterator                 Iter;

    const Vector::value_type d1[] = { 1, 2, 3, 4 };
    const Vector::value_type d2[] = { 1, 3, 2, 4 };

    // Set up two vectors.
    const Vector vi1 (d1 + 0, d1 + sizeof d1 / sizeof *d1);
    const Vector vi2 (d2 + 0, d2 + sizeof d2 / sizeof *d2);

    // p1 will contain two iterators that point to the first pair of
    // elements that are different between the two vectors.
    std::pair<Iter, Iter> p1 =
        std::mismatch (vi1.begin (), vi1.end (), vi2.begin ());

    // Find the first two elements such that an element in the
    // first vector is greater than the element in the second vector.
    std::pair<Iter, Iter> p2 =
        std::mismatch (vi1.begin (), vi1.end (),
                       vi2.begin (), std::less_equal<int>());

    // Output results.
    std::cout << *p1.first << ", " << *p1.second << std::endl;
    std::cout << *p2.first << ", " << *p2.second << std::endl;

    return 0;
}
