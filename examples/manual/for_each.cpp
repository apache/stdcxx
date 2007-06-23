/**************************************************************************
 *
 * for_each.cpp - Example program for applying a function to each element 
 *                in a range.
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

#include <algorithm>    // for for_each
#include <functional>   // for less, unary_function
#include <iostream>     // for cout, endl
#include <set>          // for set

#include <examples.h>


// Function object that outputs its argument times x.
template <class Arg>
class out_times_x:  public std::unary_function<Arg, void>
{
    Arg multiplier;
public:
    out_times_x (const Arg &x) : multiplier (x) { }

    void operator() (const Arg &x) const {
        std::cout << x * multiplier << " " << std::endl;
    }
};


int main ()
{
    // Typedef for convenience.
    typedef std::set<int, std::less<int>, std::allocator<int> > sequence;
    
    sequence::value_type arr [] = { 1, 2, 3, 4, 5 };

    // Populate a sequence from arr.
    sequence s (arr, arr + sizeof arr / sizeof *arr);

    // Construct a function object.
    out_times_x<sequence::value_type> f2 (2);
  
    // Apply function object's operator() to each element.
    std::for_each (s.begin (), s.end (), f2);
  
    return 0;
}
