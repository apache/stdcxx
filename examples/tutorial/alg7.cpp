/**************************************************************************
 *
 * alg7.cpp - Illustrate the use of the sort related generic algorithms.
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <rw/_config.h>

#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
// Disable implicit inclusion to work around 
// a limitation in IBM's VisualAge 5.0.2.0 (see PR#26959) 

#  define _RWSTD_NO_IMPLICIT_INCLUSION 
#endif

#include <vector>
#include <deque>
#include <list>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>

#include <examples.h>

    
// returns a random integer in the range [0, 100)
int randomValue ()
{
    static int digits[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    };

    std::random_shuffle (digits, digits + sizeof digits / sizeof *digits);

    const int rnd = digits [0] * 10 + digits [1];

    return rnd;
}


typedef std::ostream_iterator<int, char, std::char_traits<char> > Iter;

void sortExample ()
{
    std::cout << "Sort algorithms\n";

    Iter intOut (std::cout, " ");

    // Fill both a std::vector and a std::deque with random integers.
    std::vector<int, std::allocator<int> > aVec (15);
    std::deque<int, std::allocator<int> >  aDec (15);
    std::generate (aVec.begin (), aVec.end (), randomValue);
    std::generate (aDec.begin (), aDec.end (), randomValue);

    // Sort the std::vector ascending.
    std::sort (aVec.begin (), aVec.end ());
    std::copy (aVec.begin (), aVec.end (), intOut);
    std::cout << '\n';

    // Sort the std::deque descending.
    std::sort (aDec.begin (), aDec.end (), std::greater<int>() );
    std::copy (aDec.begin (), aDec.end (), intOut);
    std::cout << '\n';

    // Sort the std::vector descending?
    std::sort (aVec.rbegin (), aVec.rend ());
    std::copy (aVec.begin (), aVec.end (), intOut);
    std::cout << '\n';
}

void partial_sort_example ()
{
    std::cout << "Partial sort examples\n";

    Iter intOut (std::cout, " ");

    // Make a std::vector of 15 random integers.
    std::vector<int, std::allocator<int> > aVec (15);
    std::generate (aVec.begin (), aVec.end (), randomValue);
    std::copy (aVec.begin (), aVec.end (), intOut);
    std::cout << '\n';

    // Partial sort the first seven positions.
    std::partial_sort (aVec.begin (), aVec.begin () + 7, aVec.end ());
    std::copy (aVec.begin (), aVec.end (), intOut);
    std::cout << '\n';

    // Make a std::list of random integers.
    std::list<int, std::allocator<int> > aList (15);
    std::generate (aList.begin (), aList.end (), randomValue);
    std::copy (aList.begin (), aList.end (), intOut);
    std::cout << '\n';

    // Sort only the first seven elements.
    std::vector<int, std::allocator<int> > start (7);
    std::partial_sort_copy (aList.begin (), aList.end (),
                            start.begin (), start.end (), std::greater<int>());
    std::copy (start.begin (), start.end (), intOut);
    std::cout << '\n';   
}


// Illustrate the use of the nth_largest function.
void nth_element_example ()
{
    std::cout << "Nth largest example\n";

    // Make a std::vector of random integers.
    std::vector<int, std::allocator<int> > aVec (10);
    std::generate (aVec.begin (), aVec.end (), randomValue);

    Iter intOut (std::cout, " ");

    // Now find the 5th largest.
    std::vector<int, std::allocator<int> >::iterator nth = aVec.begin () + 4;
    std::nth_element (aVec.begin (), nth, aVec.end ());
    std::cout << "fifth largest is " << *nth << " in\n";
    std::copy (aVec.begin (), aVec.end (), intOut);
    std::cout << '\n';
}


// Illustrate the use of the binary search functions.
void binary_search_example ()
{
    std::cout << "Binary search example\n";

    Iter intOut (std::cout, " ");

    // Make an ordered std::vector of 15 random integers.
    std::vector<int, std::allocator<int> > aVec (15);
    std::generate (aVec.begin (), aVec.end (), randomValue);
    std::sort (aVec.begin (), aVec.end ());
    std::copy (aVec.begin (), aVec.end (), intOut),   std::cout << '\n';

    // See if it contains an eleven.
    if (std::binary_search (aVec.begin (), aVec.end (), 11))
        std::cout << "contains an 11\n";
    else
        std::cout << "does not contain an 11\n";

    // Insert an 11 and a 14.
    std::vector<int, std::allocator<int> >::iterator where;
    where = std::lower_bound (aVec.begin (), aVec.end (), 11);
    aVec.insert (where, 11);
    where = std::upper_bound (aVec.begin (), aVec.end (), 14);
    aVec.insert (where, 14);
    std::copy (aVec.begin (), aVec.end (), intOut),   std::cout << '\n';
}


// Illustrate the use of the merge function.
void merge_example ()
{
    std::cout << "Merge algorithm examples\n";

    // Make a std::list and std::vector of 10 random integers.
    std::vector<int, std::allocator<int> > aVec (10);
    std::list<int, std::allocator<int> > aList (10);
    std::generate (aVec.begin (), aVec.end (), randomValue);
    std::sort (aVec.begin (), aVec.end ());
    std::generate_n (aList.begin (), 10, randomValue);
    aList.sort ();

    // Merge into a std::vector.
    std::vector<int, std::allocator<int> > vResult (aVec.size () + aList.size ());
    std::merge (aVec.begin (), aVec.end (), aList.begin (), aList.end (), 
           vResult.begin ());

    // Merge into a std::list.
    std::list<int, std::allocator<int> > lResult;
    std::merge (aVec.begin (), aVec.end (), aList.begin (), aList.end (), 
           std::inserter (lResult, lResult.begin ()));

    // Merge into the output.
    std::merge (aVec.begin (), aVec.end (), aList.begin (), aList.end (),
                Iter (std::cout, " "));

    std::cout << '\n';
}

class iotaGen
{
public:
    iotaGen (int c = 0) : current (c) { }
    int operator () () { return current++; }
  private:
    int current;
};


// Illustrate the use of the generic set functions.
void set_example ()
{
    std::cout << "Set operations:\n";

    // Make a couple of ordered std::lists.
    std::list <int, std::allocator<int> > listOne, listTwo;
    std::generate_n (std::inserter (listOne, listOne.begin ()), 5, iotaGen (1));
    std::generate_n (std::inserter (listTwo, listTwo.begin ()), 5, iotaGen (3));

    Iter intOut (std::cout, " ");

    // union - 1 2 3 4 5 6 7
    std::set_union (listOne.begin (), listOne.end (),
                    listTwo.begin (), listTwo.end (), intOut);
    std::cout << '\n';

    // merge - 1 2 3 3 4 4 5 5 6 7
    std::merge (listOne.begin (), listOne.end (),
                listTwo.begin (), listTwo.end (), intOut);
    std::cout << '\n';

    // intersection 3 4 5
    std::set_intersection (listOne.begin (), listOne.end (),
                           listTwo.begin (), listTwo.end (), intOut);
    std::cout << '\n';

    // difference 1 2
    std::set_difference (listOne.begin (), listOne.end (),
                         listTwo.begin (), listTwo.end (), intOut);
    std::cout << '\n';

    // symmetric difference 1 2 6 7
    std::set_symmetric_difference (listOne.begin (), listOne.end (), 
                                   listTwo.begin (), listTwo.end (),
                                   intOut);
    std::cout << '\n';
        
    if (std::includes (listOne.begin (), listOne.end (),
                       listTwo.begin (), listTwo.end ()))
        std::cout << "set is subset\n";
    else
        std::cout << "set is not subset\n";
}


// Illustrate the use of the heap functions.
void heap_example ()
{
    Iter intOut (std::cout, " ");

    // Make a heap of 15 random integers.
    std::vector<int, std::allocator<int> > aVec (15);
    std::generate (aVec.begin (), aVec.end (), randomValue);
    std::make_heap (aVec.begin (), aVec.end ());
    std::copy (aVec.begin (), aVec.end (), intOut);
    std::cout << "\nLargest value " << aVec.front () << '\n';

    // Remove largest and reheap.
    std::pop_heap (aVec.begin (), aVec.end ());
    aVec.pop_back ();
    std::copy (aVec.begin (), aVec.end (), intOut);
    std::cout << '\n';

    // Add a 97 to the heap.
    aVec.push_back (97);
    std::push_heap (aVec.begin (), aVec.end ());
    std::copy (aVec.begin (), aVec.end (), intOut);
    std::cout << '\n';

    // Finally, make into sorted collection.
    std::sort_heap (aVec.begin (), aVec.end ());
    std::copy (aVec.begin (), aVec.end (), intOut);
    std::cout << '\n';
}

int main ()
{
    std::cout << "Sorting generic algorithms examples\n";

    sortExample ();
    partial_sort_example ();
    nth_element_example ();
    binary_search_example ();
    merge_example ();
    set_example ();
    heap_example ();
    
    std::cout << "End sorting examples\n";

    return 0;
}
