/**************************************************************************
 *
 * heap_ops.cpp - Example program for heap operations. 
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

#include <algorithm>    // for copy, make_heap, pop_heap, push_heap
#include <functional>   // for less
#include <iostream>     // for cout
#include <iterator>     // for ostream_iterator
#include <vector>       // for vector

#include <examples.h>


template <class charT, class Traits, class T, class Allocator>
void print_vector (std::basic_ostream<charT, Traits> &strm,
                   const std::vector<T, Allocator>   &v)
{
    std::copy (v.begin (), v.end (),
               std::ostream_iterator<T, charT, Traits> (strm, " "));

    strm << std::endl;
}


int main ()
{
    typedef std::vector<int, std::allocator<int> > Vector;

    const Vector::value_type d1[] = { 1, 2, 3, 4 };
    const Vector::value_type d2[] = { 1, 3, 2, 4 };

    // Set up two vectors.
    Vector v1 (d1 + 0, d1 + sizeof d1 / sizeof *d1);
    Vector v2 (d2 + 0, d2 + sizeof d2 / sizeof *d2);

    // Make heaps.
    std::make_heap (v1.begin (), v1.end ());
    std::make_heap (v2.begin (), v2.end (), std::less<int>());

    // v1 = (4, x, y, z)  and  v2 = (4, x, y, z)

    // Note that x, y and z represent the remaining values in the
    // container (other than 4).  The definition of the heap and heap
    // operations  does not require any particular ordering
    // of these values.

    // Copy both vectors to cout.
    print_vector (std::cout, v1);
    print_vector (std::cout, v2);

    // Now let's pop.
    std::pop_heap (v1.begin (), v1.end ());
    std::pop_heap (v2.begin (), v2.end (), std::less<int>());

    print_vector (std::cout, v1);
    print_vector (std::cout, v2);

    // And push.
    std::push_heap (v1.begin (), v1.end ());
    std::push_heap (v2.begin (), v2.end (), std::less<int>());

    print_vector (std::cout, v1);
    print_vector (std::cout, v2);

    // Now sort those heaps.
    std::sort_heap (v1.begin (), v1.end ());
    std::sort_heap (v2.begin (), v2.end (), std::less<int>());

    print_vector (std::cout, v1);
    print_vector (std::cout, v2);

    return 0;
}
