/**************************************************************************
 *
 * replace.cpp - Example program of replace algorithm 
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

#include <rw/_config.h>

#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
// Disable implicit inclusion to work around 
// a limitation in IBM's VisualAge 5.0.2.0 (see PR#26959) 

#  define _RWSTD_NO_IMPLICIT_INCLUSION 
#endif

#include <algorithm>    // for replace, replace_if, replace_copy, ...
#include <vector>       // for vector
#include <iterator>     // for ostream_iterator
#include <functional>   // for not1, unary_function
#include <iostream>     // for cout, endl

#include <examples.h>
#include <replace.h>


int main ()
{
    typedef std::vector<short, std::allocator<short> >                  Vector;
    typedef std::ostream_iterator<short, char, std::char_traits<char> > Iter;

    // Populate a vector with arbitrary values.
    Vector v;
    for (Vector::value_type n = 11111; n != 11211; ++n)
        v.push_back (n);

    // Print out original vector.
    std::cout << "Original sequence:\n    ";
    std::copy (v.begin (), v.end (), Iter (std::cout, " "));
    std::cout << std::endl << std::endl;

    // Replace one number with another.
    std::replace (v.begin (), v.end (), 11199, 11211);

    // Print out the new vector.
    std::cout << "Sequence after replace:\n    ";
    std::copy (v.begin (), v.end (), Iter (std::cout, " "));
    std::cout << std::endl << std::endl;

    // Replace all numbers that aren't primes with zeros.
    std::replace_if (v.begin (), v.end (), std::not1 (is_prime ()), 0);

    // Print out the remaining vector.
    std::cout << "After replace_if:\n    ";
    std::copy (v.begin (), v.end (), Iter (std::cout, " "));
    std::cout << std::endl << std::endl;

    // Replace zeros with ones.
    std::cout << "Sequence replace_copy-ed to cout:\n    ";
    std::replace_copy (v.begin (), v.end (), Iter (std::cout, " "), 0, 1);
    std::cout << std::endl << std::endl;

    // A simple example of replace_copy_if.
    std::cout << "Sequence replace_copy_if-ed to cout:\n    ";
    std::replace_copy_if (v.begin (), v.end (), Iter (std::cout, ""),
                          is_prime (), 1);
    std::cout << std::endl;

    return 0;
}
