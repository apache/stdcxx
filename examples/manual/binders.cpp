/**************************************************************************
 *
 * binders.cpp - Example program for binder1st & binder2nd.
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
 * Copyright 1994-2006 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <algorithm>   // for find_if
#include <functional>  // for equal_to, bind1st, bind2nd
#include <iostream>    // for cout
#include <iterator>    // for ostream_iterator
#include <vector>      // for vector

#include <examples.h>


int main ()
{
    typedef std::vector<int>                  Vector;
    typedef std::equal_to<Vector::value_type> EqualTo;

    const Vector::value_type arr [] = { 1, 2, 3, 4, 5 };

    // Initialize a vector with the array elements.
    const Vector v1 (arr, arr + sizeof arr / sizeof *arr);

    // Value to look for.
    const Vector::value_type x (3);

    // Create an 'equal to 3' unary predicate by binding the value
    // 3 to the EqualTo binary predicate.
    const std::binder1st<EqualTo> equal_to_3 (std::bind1st (EqualTo (), x));

    // Now use this new predicate in a call to find_if.
    const Vector::const_iterator it1 =
        std::find_if (v1.begin (), v1.end (), equal_to_3);

    // Even better, construct the new predicate on the fly.
    const Vector::const_iterator it2 =
        std::find_if (v1.begin (), v1.end (), std::bind1st (EqualTo (), x));

    // And now the same thing using bind2nd.
    // Same result since EqualTo is commutative.
    const Vector::const_iterator it3 =
        std::find_if (v1.begin (), v1.end (), std::bind2nd (EqualTo (), x));

    // Use the same predicate to count the number of elements
    // equal to 3.
    const Vector::size_type n =
        std::count_if (v1.begin (), v1.end (), std::bind2nd (EqualTo (), x));

    // Output results.
    std::ostream_iterator<Vector::value_type> out (std::cout, " ");

    std::cout << "The vector { ";
    std::copy (v1.begin (), v1.end (), out);

    std::cout << "} contains " << n << " element equal to " << x
              <<  " at offset " << it1 - v1.begin () << ".\n";
    
    // Exit with status of 0 on success, 1 on failure.
    const bool success = 1 == n && it1 == it2 && it1 == it2 && *it1 == x;

    return success ? 0 : 1;
}
