/**************************************************************************
 *
 * sort.cpp - Example program of sort algorithm. See Class Reference Section
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

#include <algorithm>    // for sort
#include <iostream>     // for cout
#include <utility>      // for pair
#include <vector>       // for vector

#include <examples.h>


typedef std::pair<int, char> Pair;

struct PairCompare
{
    bool operator() (const Pair &x, const Pair &y) const {
        return x.first < y.first;
    }
};

inline std::ostream& operator<< (std::ostream &s, const Pair &x)
{
    return s << '<' << x.first << ';' << x.second << '>';
}

int main ()
{
    const Pair array[] = {
        Pair (-4, ' '), Pair (16, ' '), Pair (17, ' '), Pair (-3, 's'),
        Pair (14, ' '), Pair (-6, ' '), Pair (-1, ' '), Pair (-3, 't'),
        Pair (23, ' '), Pair (-3, 'a'), Pair (-2, ' '), Pair (-7, ' '),
        Pair (-3, 'b'), Pair (-8, ' '), Pair (11, ' '), Pair (-3, 'l'),
        Pair (15, ' '), Pair (-5, ' '), Pair (-3, 'e'), Pair (15, ' ')
    };

    typedef std::vector<Pair, std::allocator<Pair> > Vector;

    const Vector::size_type array_size = sizeof array / sizeof *array;

    // Create two modifiable sequences.
    Vector v1 (array, array + array_size);
    Vector v2 (v1);

    // Sort vector #1.
    std::sort (v1.begin (), v1.end (), PairCompare ());

    // Stable sort vector #2.
    std::stable_sort (v2.begin (), v2.end (), PairCompare ());

    // Display the original and the sorted sequences.
    std::cout << "original   std::sort  std::stable_sort\n";

    for (Vector::size_type i = 0; i != array_size; ++i)
        std::cout << array [i] << "     "
                  << v1 [i] << "     "
                  << v2 [i] << '\n';

    return 0;
}
