/**************************************************************************
 *
 * binary_search.cpp - Example program of binary search. 
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

#include <algorithm>    // for binary_search(), copy(), sort()
#include <deque>        // for deque
#include <functional>   // for less
#include <iostream>     // for cout
#include <iterator>     // for ostream_iterator, char_traits

#include <examples.h>


int main ()
{
    // Typedefs for convenience.
    typedef short                                      Value;
    typedef std::deque<Value, std::allocator<Value> >  Deque;
    typedef std::char_traits<char>                     Traits;
    typedef std::ostream_iterator<Value, char, Traits> Iterator;

    const Value arr[] = { 0, 1, 2, 2, 3, 4, 2, 2, 6, 7 };

    // Populate and sort the container.
    Deque d (arr + 0, arr + sizeof arr / sizeof *arr);
    std::sort (d.begin (), d.end ());

    // Arbitrary values to search for.
    const Value val_1 = 3;
    const Value val_2 = 11;

    // Try binary_search variants.
    const bool found_1 = std::binary_search (d.begin (), d.end (), val_1);
    const bool found_2 = std::binary_search (d.begin (), d.end (), val_2,
                                             std::less<Value>());

    // Output the sorted sequence.
    std::cout << "Container contents: ";
    std::copy (d.begin (), d.end (), Iterator (std::cout, " "));

    // Output the results of the algorithms.
    std::cout << "\nThe number " << val_1 << " was "
              << (found_1 ? "" : "NOT ") <<  "found";

    std::cout << "\nThe number " << val_2 << " was "
              << (found_2 ? "" : "NOT ") << "found\n";

    return 0;
}
