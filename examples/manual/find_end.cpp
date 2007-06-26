/**************************************************************************
 *
 * find_end.cpp - Example program for finding a subsequence. 
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

#include <algorithm>   // for find_first_of(), find_end()
#include <functional>  // for equal_to
#include <iostream>    // for cout, endl
#include <iterator>    // for ostream_iterator
#include <list>        // for list

#include <examples.h>


int main ()
{
    typedef std::list<int, std::allocator<int> > List;

    const List::value_type a1[] = { 0, 1, 6, 5, 3, 2, 2, 6, 5, 7 };
    const List::value_type a2[] = { 6, 5, 0, 0 };

    // Set up two sequences.
    const List l1 (a1, a1 + sizeof a1 / sizeof *a1);
    const List l2 (a2, a2 + 2);

    // Try both find_first_of variants.
    List::const_iterator it1 =
        std::find_first_of (l1.begin (), l1.end (), l2.begin (), l2.end ());

    List::const_iterator it2 =
        std::find_first_of (l1.begin (), l1.end (), l2.begin (), l2.end (),
                            std::equal_to<List::value_type>());

    // Try both find_end variants.
    List::const_iterator it3 =
        std::find_end (l1.begin (), l1.end (), l2.begin (), l2.end ()); 

    List::const_iterator it4 =
        std::find_end (l1.begin (), l1.end (), l2.begin (), l2.end (),
                       std::equal_to<List::value_type>());

    // Output results of find_first_of.
    // Iterator now points to the first element that matches
    // one of a set of values.

    std::ostream_iterator<List::value_type, 
                          char,
                          std::char_traits<char> > out (std::cout, " " );

    if (it3 == it4 && it1 == it2) {

        std::cout << "For the sequences { ";
        std::copy (l1.begin (), l1.end (), out);

        std::cout << "} and { ";
        std::copy (l2.begin (), l2.end (), out);
             
        std::cout << "} \nboth versions of find_first_of point to: "
                  << *it1 << std::endl;

        // Output results of find_end.
        // Iterator now points to the first element of the last
        // find subsequence.

        std::cout << "both versions of find_end point to: "
                  << *it3 << std::endl;
    }

    return 0;
}
