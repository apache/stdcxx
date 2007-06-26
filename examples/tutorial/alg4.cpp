/**************************************************************************
 *
 * alg4.cpp - Example programs for STL generic algorithms removal 
 *            algorithms.
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

#include <list>
#include <set>
#include <algorithm>
#include <iostream>
#include <iterator>

#include <examples.h>

typedef std::ostream_iterator<int, char, std::char_traits<char> >
ostrm_iter_type;


bool isEven (int n) {
    return 0 == (n % 2);
}


// Illustrate the use of the remove algorithm.
void remove_example ()
{
    std::cout << "Remove Algorithm examples" << std::endl;

    // Create a list of numbers.
    int data[] = { 1, 2, 4, 3, 1, 4, 2 };
    std::list<int, std::allocator<int> > aList;

    std::copy (data, data+7, std::inserter (aList, aList.begin ()));
    std::cout << "Original list: ";
    std::copy (aList.begin (), aList.end (),
               ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;

    // Remove 2's, copy into a new list.
    std::list<int, std::allocator<int> > newList;

    std::remove_copy (aList.begin (), aList.end (),
                      std::back_inserter (newList), 2);
    std::cout << "After removing 2's: ";
    std::copy (newList.begin (), newList.end (),
               ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;

    // Remove 2's in place.
    std::list<int, std::allocator<int> >::iterator 
        where = std::remove (aList.begin (), aList.end (), 2);

    std::cout << "List after removal, before erase: ";
    std::copy (aList.begin (), aList.end (),
               ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;

    aList.erase (where, aList.end ());
    std::cout << "List after erase: ";
    std::copy (aList.begin (), aList.end (),
               ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;

    // Remove all even values.
    where = std::remove_if (aList.begin (), aList.end (), isEven);
    aList.erase (where, aList.end ());

    std::cout << "List after removing even values: ";
    std::copy (aList.begin (), aList.end (),
               ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;
}


// Illustrate use of the unqiue algorithm.
void unique_example ()
{
    // First make a list of values.
    int data[] = { 1, 3, 3, 2, 2, 4 };
    std::list<int, std::allocator<int> > aList;

    std::copy (data, data+6, std::inserter (aList, aList.begin ()));
    std::cout << "Origianal List: ";
    std::copy (aList.begin (), aList.end (),
               ostrm_iter_type (std::cout, " "));


    std::cout << std::endl;

    // Copy unique elements into a set.
    std::set<int, std::less<int>, std::allocator<int>  > aSet;

    std::unique_copy (aList.begin (), aList.end (),
                      std::inserter (aSet, aSet.begin ()));

    std::cout << "Set after unique_copy: ";
    std::copy (aSet.begin (), aSet.end (), ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;

    // Copy unique elements in place.
    std::list<int, std::allocator<int> >::iterator 
        where = std::unique (aList.begin (), aList.end ());

    std::cout << "List after calling unique: ";
    std::copy (aList.begin (), aList.end (),
               ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;

    // Remove trailing values.
    aList.erase (where, aList.end ());

    std::cout << "List after erase: ";
    std::copy (aList.begin (), aList.end (),
               ostrm_iter_type (std::cout, " "));

    std::cout << std::endl;

}

int main ()
{
    std::cout << "STL generic algorithms -- Removal Algorithms"
              << std::endl;

    remove_example ();
    unique_example ();
    
    std::cout << "End of removal algorithms sample program"
              << std::endl;

    return 0;
}
