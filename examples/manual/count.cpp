/**************************************************************************
 *
 * count.cpp - Example program for counting number of elements in container. 
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

#include <algorithm>    // for count, count_if
#include <cassert>      // for assert
#include <functional>   // bind2nd, less
#include <iostream>     // for cout
#include <iterator>     // for ostream_iterator
#include <vector>       // for vector
 
#include <examples.h>


// streams out a vector
template <class T>
std::ostream&
operator<< (std::ostream &out,
            const std::vector<T, std::allocator<T> > &vec)
{
    typedef std::ostream_iterator<T, char, std::char_traits<char> > Iter;

    out << "{ ";

    std::copy (vec.begin (), vec.end (), Iter (out, " "));

    out << "}";

    return out;
}


int main ()
{
    // Typedef for convenience.
    typedef std::vector<short, std::allocator<short> > Vector;

    const Vector::value_type a [] = { 1, 2, 3, 4, 5, 5, 7, 8, 9, 10 };

    // construct a vector from the array
    const Vector vec (a + 0, a + 10);

    Vector::size_type n_fives = 0;
    Vector::size_type n_sixes = 0;
    Vector::size_type n_lt_8  = 0;

#ifndef _RWSTD_NO_EXT_VOID_COUNT

    // using the extended overloads of function template count()
    // and count_if() provided by this implmentation, count the
    // number of fives, sixes, and values less than 8 in the
    // sequence

    std::count (vec.begin (), vec.end (), 5, n_fives);
    std::count (vec.begin (), vec.end (), 6, n_sixes);

    std::count_if (vec.begin (), vec.end (),
                   std::bind2nd (std::less<Vector::value_type>(), 8),
                   n_lt_8);

    assert (2 == n_fives);
    assert (0 == n_sixes);
    assert (7 == n_lt_8);

#endif   // _RWSTD_NO_EXT_VOID_COUNT

    // do the same using the standard function templates count()
    // and count_if()

    n_fives = std::count (vec.begin (), vec.end (), 5);
    n_sixes = std::count (vec.begin (), vec.end (), 6);
    n_lt_8  = std::count_if (vec.begin (), vec.end (),
                             std::bind2nd (std::less<Vector::value_type>(), 8));

    std::cout << "sequence: " << vec;
    std::cout << "\nnumber of fives: " << n_fives
              << "\nnumber of sixes: " << n_sixes
              << "\nnumber of values less than 8: " << n_lt_8 << '\n';

    return 0;
}
