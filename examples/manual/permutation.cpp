/**************************************************************************
 *
 * permute.cpp - Example program of permutations.
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

#include <algorithm>  // for next_permutation, prev_permutation
#include <functional> // for less
#include <iostream>   // for cout, endl
#include <iterator>   // for ostream_iterator
#include <numeric>    // for accumulate
#include <vector>     // for vector

#include <examples.h>

int main ()
{
    typedef std::vector<int, std::allocator<int> > IntVec;
    typedef std::vector<char, std::allocator<char> > CharVec;

    // Initialize a vector using an array of integers.
    const IntVec::value_type  a1[] = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 };
    const CharVec::value_type a2[] = "abcdefghji";

    // Create the initial set and copies for permuting.
    IntVec m1      (a1 + 0, a1 + sizeof a1 / sizeof *a1); 
    IntVec prev_m1 (10);
    IntVec next_m1 (10);

    CharVec m2      (a2 + 0, a2 + sizeof a2 / sizeof *a2 - 1);
    CharVec prev_m2 (10);
    CharVec next_m2 (10);

    std::copy (m1.begin (), m1.end (), prev_m1.begin ());
    std::copy (m1.begin (), m1.end (), next_m1.begin ());
    std::copy (m2.begin (), m2.end (), prev_m2.begin ());
    std::copy (m2.begin (), m2.end (), next_m2.begin ());

    // Create permutations.
    typedef std::less<IntVec::value_type>  IntLess;
    typedef std::less<CharVec::value_type> CharLess;

    std::prev_permutation (prev_m1.begin (), prev_m1.end (), IntLess ());
    std::next_permutation (next_m1.begin (), next_m1.end (), IntLess ());
    std::prev_permutation (prev_m2.begin (), prev_m2.end (), CharLess ());
    std::next_permutation (next_m2.begin (), next_m2.end (), CharLess ());

    // Output results.

    typedef std::ostream_iterator<IntVec::value_type, char,
                                  std::char_traits<char> >
        IntOSIter;

    typedef std::ostream_iterator<CharVec::value_type, char,
                                  std::char_traits<char> >
        CharOSIter;

    std::cout << "Example 1: \n     Original values:      ";
    std::copy (m1.begin (), m1.end (), IntOSIter (std::cout, " "));

    std::cout << "\n     Previous permutation: ";
    std::copy (prev_m1.begin (), prev_m1.end (), IntOSIter (std::cout, " "));

    std::cout << "\n     Next Permutation:     ";
    std::copy (next_m1.begin (), next_m1.end (), IntOSIter (std::cout, " "));

    std::cout << "\n\nExample 2: \n     Original values:      ";
    std::copy (m2.begin (), m2.end (), CharOSIter (std::cout, " "));     

    std::cout << "\n     Previous Permutation: ";
    std::copy (prev_m2.begin (), prev_m2.end (), CharOSIter (std::cout, " "));

    std::cout << "\n     Next Permutation:     ";
    std::copy (next_m2.begin (), next_m2.end (), CharOSIter (std::cout, " "));  

    std::cout << std::endl << std::endl;

    return 0;
}
