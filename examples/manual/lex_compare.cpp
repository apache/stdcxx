/**************************************************************************
 *
 * lex_comp.cpp - Example program compares to ranges lexicographically.
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

#include <algorithm>    // for lexicographical_compare
#include <functional>   // for less
#include <ios>          // for boolalpha
#include <iostream>     // for cout, endl
#include <vector>       // for vector

#include <examples.h>


int main()
{
    typedef std::vector<int, std::allocator<int> > Vector;

    const Vector::value_type d1[] = { 1, 3, 5, 32, 64 };
    const Vector::value_type d2[] = { 1, 3, 2, 43, 56 };

    // Create vectors.
    const Vector v1 (d1 + 0, d1 + sizeof d1 / sizeof *d1);
    const Vector v2 (d2 + 0, d2 + sizeof d1 / sizeof *d2);

    // Is v1 less than v2 (I think not).
    bool b1 = std::lexicographical_compare (v1.begin (), v1.end (),
                                            v2.begin (), v2.end ());
    // Is v2 less than v1 (yup, sure is).
    bool b2 = std::lexicographical_compare (v2.begin (), v2.end (),
                                            v1.begin (), v1.end (),
                                            std::less<int>());
    std::cout << std::boolalpha << b1
              << " " << b2 << std::endl;

    return 0;
}
