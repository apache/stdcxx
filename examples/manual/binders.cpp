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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <algorithm>   // for find_if
#include <functional>  // for equal_to, bind1st, bind2nd
#include <iostream>    // for cout, endl
#include <vector>      // for vector

#include <examples.h>


int main ()
{
    typedef std::vector<int, std::allocator<int> > Vector;
    typedef std::equal_to<Vector::value_type>      equal_to;

    const Vector::value_type arr [] = { 1, 2, 3, 4 };

    // Set up a vector.
    Vector v1 (arr + 0, arr + sizeof arr / sizeof *arr);

    // Create an 'equal to 3' unary predicate by binding 3 to
    // the equal_to binary predicate.
    std::binder1st<equal_to> equal_to_3 = std::bind1st (equal_to (), 3);

    // Now use this new predicate in a call to find_if.
    Vector::iterator it1 = std::find_if (v1.begin (), v1.end (),
                                         equal_to_3);

    // Even better, construct the new predicate on the fly.
    Vector::iterator it2 =
        std::find_if (v1.begin (), v1.end (),
                      std::bind1st (equal_to (), 3)); 

    // And now the same thing using bind2nd.
    // Same result since equal_to is commutative.
    Vector::iterator it3 =
        std::find_if (v1.begin (), v1.end (),
                      std::bind2nd (equal_to (), 3)); 

    // Output results.
    std::cout << *it1 << " " << *it2 << " " << *it3 << '\n';

    return 0;
}
