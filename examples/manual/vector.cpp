/**************************************************************************
 *
 * vector.cpp - Example program of vector. See Class Reference Section
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
 
#include <iostream>   // for cout, endl
#include <iterator>   // for ostream_iterator
#include <vector>     // for vector

#include <examples.h>


// streams out a vector
template <class T>
std::ostream&
operator<< (std::ostream &out, const std::vector<T, std::allocator<T> > &v)
{
    typedef std::ostream_iterator<T, char, std::char_traits<char> > Iter;

    std::copy (v.begin (), v.end (), Iter (out, " "));

    return out;
}


int main ()
{
    const int a[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    typedef std::vector<int, std::allocator<int> >        IntVector;
    typedef std::vector<double, std::allocator<double> >  DblVector;

    // demonstrate the initialization a vector of one type
    // from a range of of values of another type denoted by
    // either a pair of pointers or iterators
    const IntVector vi (a, a + sizeof a / sizeof *a);
    DblVector vd (vi.begin (), vi.end ());

    // print out the two vectors
    std::cout.setf (std::cout.showpoint);
    std::cout.precision (2);

    std::cout << vi << '\n' << vd << '\n';

    // overwrite the second vector with the same values
    // but of a different type
    const IntVector::size_type mid = vi.size () / 2;

    // demonstrate the assignment to a vector of one type
    // from a range of values of another type denoted by
    // a pair of iterators
    vd.assign (vi.begin (), vi.begin () + mid);

    // demonstrate the insertion into a vector of one type
    // from a range of values of another type denoted by
    // a pair of pointers
    vd.insert (vd.end (), a + mid, a + sizeof a / sizeof *a);

    // print out the second vector
    std::cout << vd << '\n';

    return 0;
}
