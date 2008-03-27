/**************************************************************************
 *
 * generate.cpp - Example program for initializing a container with values 
 *                produced by a value-generator class.
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

#include <algorithm>    // for generate, generate_n
#include <iostream>     // for cout, endl
#include <iterator>     // for ostream_iterator
#include <vector>       // for vector

#include <examples.h>


// Value generator simply doubles the current value and returns it.
template <class T>
class generator
{
    T val_;
public:
    generator (const T &val) : val_ (val) { }
    T operator() () {
        return val_ += val_;
    }
};


int main ()
{
    // Typedef for convenience.
    typedef std::vector<short, std::allocator<short> > Vector;

    Vector::value_type arr[4] = { 1, 2, 3, 4 };

    // Set up two vectors.
    Vector v1 (arr, arr + sizeof arr / sizeof *arr);
    Vector v2 = v1;

    // Set up one empty vector.
    Vector v3;

    // Create a generator function object.
    generator<Vector::value_type> gen (1);

    // Generate values for all of v1.
    std::generate (v1.begin (), v1.end (), gen);

    // Generate values for first 3 of v2.
    std::generate_n (v2.begin (), 3, gen);

    // Use back_insert_iterator to generate 5 values for v3.
    std::generate_n (std::back_inserter (v3), 5, gen);

    // Copy all three to cout.
    std::ostream_iterator<Vector::value_type,
                          char,
                          std::char_traits<char> > out (std::cout, " ");

    std::copy (v1.begin (), v1.end (), out);
    std::cout << std::endl;

    std::copy (v2.begin (), v2.end (), out);
    std::cout << std::endl;

    std::copy (v3.begin (), v3.end (), out);
    std::cout << std::endl;

    // Generate 3 values into cout.
    std::generate_n (out, 3, gen);
    std::cout << std::endl;

    return 0;
}
