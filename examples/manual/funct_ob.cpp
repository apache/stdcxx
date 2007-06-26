/**************************************************************************
 *
 * funct_ob.cpp - Example program for function objects.
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

#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <vector>


// Create a new function object from unary_function.
template <class Arg, class Result>
struct factorial: public std::unary_function<Arg, Result>
{
    Result operator() (const Arg &arg) {
        Result a = 1;
        for (Arg i = 2; i <= arg; i++)
            a *= i;
        return a;
    }
};


int main ()
{
    // Typedefs for convenience.
    typedef std::deque<int, std::allocator<int> >    Deque;
    typedef std::vector<long, std::allocator<long> > Vector;

    // Initialize a deque with an array of integers.
    Deque::value_type arr[] = { 1, 2, 3, 4, 5, 6, 7 };
    Deque d (arr, arr + sizeof arr / sizeof *arr);

    // Create an empty vector to store the factorials.
    Vector v;

    // Transform the numbers in the deque to their factorials
    // and store in the vector.
    std::transform (d.begin (), d.end (), 
                    std::back_inserter (v),
                    factorial<Deque::value_type, Vector::value_type>());

    // Create an iterator to output deque elements.
    std::ostream_iterator<Deque::value_type, 
                          char, 
                          std::char_traits<char> > outd (std::cout, " ");

    // Print the results.
    std::cout << "The following numbers: \n     ";
    std::copy (d.begin (), d.end (), outd);

    // Create an iterator to output vector elements.
    std::ostream_iterator<Vector::value_type, 
                          char, 
                          std::char_traits<char> > outv (std::cout, " ");

    std::cout << std::endl;
    std::cout << "\nHave the factorials: \n     ";
    std::copy (v.begin (), v.end (), outv);
    std::cout << std::endl;

    return 0;
}
