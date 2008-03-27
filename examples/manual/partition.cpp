/**************************************************************************
 *
 * prtition.cpp - Example program for partition.
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

#include <algorithm>    // for copy
#include <deque>        // for deque
#include <functional>   // for unary_function
#include <iostream>     // for cout, endl
#include <iterator>     // for ostream_iterator

#include <examples.h>

typedef std::modulus<int>               int_modulus;
typedef std::binder2nd<int_modulus>     bnd_modulus;
typedef std::unary_negate<bnd_modulus>  is_even;

int main ()
{
    typedef std::deque<int, std::allocator<int> >                     Deque;
    typedef std::ostream_iterator<int, char, std::char_traits<char> > Iter;

    // Initialize a deque with an array of integers.
    const Deque::value_type a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    Deque d1 (a + 0, a + sizeof a / sizeof *a);
    Deque d2 (d1);

    // Print out the original values.
    std::cout << "Unpartitioned values: \t\t";
    std::copy (d1.begin (), d1.end (), Iter (std::cout, " "));

    // A partition of the deque according to even/oddness.
    std::partition (d2.begin (), d2.end (), is_even ( bnd_modulus(int_modulus (), 2)) );

    // Output result of partition.
    std::cout << "\nPartitioned values: \t\t";
    std::copy (d2.begin (), d2.end (), Iter (std::cout, " "));

    // A stable partition of the deque according to even/oddness.
    std::stable_partition (d1.begin (), d1.end (),  is_even ( bnd_modulus(int_modulus (), 2)) );

    // Output result of partition.
    std::cout << "\nStable partitioned values: \t";
    std::copy (d1.begin (), d1.end (), Iter (std::cout, " "));
    std::cout << std::endl;

    return 0;
}









