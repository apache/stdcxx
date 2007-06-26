/**************************************************************************
 *
 * set_union.cpp - Example program of set union algorithm.
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
#include <set>
#include <iostream>
#include <iterator>

int main ()
{
    typedef std::set<int, std::less<int>, std::allocator<int> > Set;

    // Initialize sets.
    const int a1[] = { 2, 4, 6, 8, 10, 12 };
    const int a2[] = { 3, 5, 7, 8 };

    const Set s1 (a1, a1 + sizeof a1 / sizeof *a1);
    const Set s2 (a2, a2 + sizeof a2 / sizeof *a2);

    Set result;

    // Create an insert_iterator for result.
    std::insert_iterator<Set> res_ins (result, result.begin ());

    // Compute union of two sets.
    std::set_union (s2.begin (), s2.end (), s1.begin (), s1.end (), res_ins);

    // Display result on standard output.
    typedef std::ostream_iterator<int, char, std::char_traits<char> > OSIter;

    std::cout << "The result of:\n{ ";

    std::copy (s2.begin (),s2.end (), OSIter (std::cout," "));

    std::cout << "} union { ";

    std::copy (s1.begin (),s1.end (), OSIter (std::cout," "));

    std::cout << "} =\n{ ";

    std::copy (result.begin (),result.end (), OSIter (std::cout," "));

    std::cout << "}\n\n";

    return 0;
}
