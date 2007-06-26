/**************************************************************************
 *
 * includes.cpp - Example program of basic set operation for sorted 
 *                sequences.
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

#include <algorithm>   // for includes
#include <iostream>    // for cout, endl
#include <iterator>    // for ostream_iterator
#include <set>         // for set

#include <examples.h>


int main ()
{
    // Typedefs for convenience.
    typedef std::set<int, std::less<int>, std::allocator<int> >       Set;
    typedef std::ostream_iterator<int, char, std::char_traits<char> > os_iter;

    int a1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    int a2[] = { 2, 4, 6, 8, 10, 12 };
    int a3[] = { 3, 5, 7, 8 };

    // Initialize three sets.
    Set all   (a1, a1 + sizeof a1 / sizeof *a1);
    Set even  (a2, a2 + sizeof a2 / sizeof *a2);
    Set other (a3, a3 + sizeof a3 / sizeof *a3);

    // Demonstrate includes.
    std::cout << "The set: ";
    std::copy (all.begin (), all.end (), os_iter (std::cout, " "));

    bool answer = std::includes (all.begin (), all.end (), 
                                 other.begin (), other.end ());

    std::cout << std::endl << (answer ? "INCLUDES " : "DOES NOT INCLUDE ");
    std::copy (other.begin (), other.end (), os_iter (std::cout, " "));

    answer = std::includes (all.begin (), all.end (),
                            even.begin (), even.end ());

    std::cout << ", and" << std::endl
              << (answer ? "INCLUDES" : "DOES NOT INCLUDE ");

    std::copy (even.begin (), even.end (), os_iter (std::cout, " "));

    std::cout << std::endl << std::endl;

    return 0;
}
