/**************************************************************************
 *
 * trnsform.cpp - Example program of transform algorithm. 
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

#include <algorithm>    // for transform
#include <functional>   // for multiplies
#include <deque>        // for deque
#include <iostream>     // for cout, endl
#include <iomanip>      // for setw


int main ()
{
    typedef std::deque<int, std::allocator<int> > Deque;

    // Initialize a deque with an array of integers.
    const Deque::value_type a [] = { 99, 264, 126, 330, 132 };
    const Deque::value_type b [] = { 280, 105, 220, 84, 210 };

    Deque d1 (a, a + sizeof a / sizeof *a);
    Deque d2 (b, b + sizeof b / sizeof *b);

    // Print the original values.
    std::cout << "The following pairs of numbers: \n     ";
    Deque::iterator i1;
    for (i1 = d1.begin(); i1 != d1.end(); ++i1)
        std::cout << std::setw (6) << *i1 << " ";

    std::cout << "\n     ";
    for (i1 = d2.begin(); i1 != d2.end(); ++i1)
        std::cout << std::setw (6) << *i1 << " ";

    // transform the numbers in one sequence to their factorials
    // and store the results in another sequence
    std::transform (d1.begin (), d1.end (), d2.begin (), d1.begin (),
                    std::multiplies<int>());

    // Display the results.
    std::cout << "\n\nHave the products: \n     ";
    for (i1 = d1.begin (); i1 != d1.end (); ++i1)
        std::cout << std::setw (6) << *i1 << " ";

    std::cout << std::endl;

    return 0;
}
