/**************************************************************************
 *
 * set_diff.cpp - Example program of set differences algorithm. 
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
    // Initialize some sets.
    const int a1 [] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    const int a2 [] = { 2, 4, 6, 8, 10, 12 };

    std::set<int, std::less<int>, std::allocator<int>  >
        all (a1+0, a1+10),
        even (a2+0, a2+6),
        odd;

    // Create an insert_iterator for odd.
    std::insert_iterator<std::set <int, std::less<int>, std::allocator<int> > >
        odd_ins (odd, odd.begin ());

    // Demonstrate set_difference.
    typedef std::ostream_iterator<int, char, std::char_traits<char> > Iter;

    std::cout << "The result of:\n{";

    std::copy (all.begin (), all.end (), Iter (std::cout, " "));
    std::cout << "} - {";

    std::copy (even.begin (), even.end (), Iter (std::cout, " "));
    std::cout << "} =\n{";

    std::set_difference (all.begin (), all.end (),
                         even.begin (), even.end (),
                         odd_ins);

    std::copy (odd.begin (),odd.end (), Iter (std::cout," "));
    std::cout << "}\n\n";

    return 0;
}
