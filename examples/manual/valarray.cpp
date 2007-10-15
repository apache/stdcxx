/**************************************************************************
 *
 * valarray.cpp -- valarray example
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
 * Copyright 1994-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <cstddef>      // for size_t
#include <iostream>     // for cout
#include <valarray>     // for valarray

#include <examples.h>


template <class T> 
inline std::ostream&
operator<< (std::ostream &out, const std::valarray<T> &v)
{
    out << '[';

    for (std::size_t i = 0; i < v.size (); ++i) {
        out << v [i];
        if (i < v.size () - 1)
            out << ',';
    }

    return out << ']';
}


int main ()
{
    const int ibuf[]  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };  
    const int ibuf2[] = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };  

    // create 2 valarrays of ints
    std::valarray<int> vi (ibuf, sizeof ibuf / sizeof *ibuf);
    std::valarray<int> vi2 (ibuf2, sizeof ibuf2 / sizeof *ibuf2);

    // print them out
    std::cout << vi << '\n' << vi2 << '\n';

    vi += vi2;
    vi2 *= 2;

    std::valarray<int> vi3 = vi2 % vi;

    // print them out again
    std::cout << vi << '\n' << vi2 << '\n' << vi3 << '\n';

    return 0;
}
