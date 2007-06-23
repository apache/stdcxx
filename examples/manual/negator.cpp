/**************************************************************************
 *
 * negator.cpp - Example program for reversing the sense of predicate 
 *               function objects by using function adaptors and function
 *               objects.
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

#include <functional>   // for unary_function
#include <ios>          // for boolalpha
#include <iostream>     // for cout, endl

#include <examples.h>


// Create a new predicate from unary_function.
template <class Arg>
struct is_odd : public std::unary_function<Arg, bool>
{
    bool operator() (const Arg &arg1) const {
        return arg1 % 2 != 0;
    }
};


int main ()
{
    std::less<int> less_func;

    // Use not2 on less.
    std::cout << std::boolalpha
              << less_func (1, 4) << '\n'
              << less_func (4, 1) << '\n'
              << std::not2 (std::less<int>())(1, 4) << '\n'
              << std::not2 (std::less<int>())(4, 1) << '\n';

    // Create an instance of our predicate.
    is_odd<int> odd;

    // Use not1 on our user defined predicate.
    std::cout << odd (1) << '\n'
              << odd (4) << '\n'
              << std::not1 (odd)(1) << '\n'
              << std::not1 (odd)(4) << std::endl;

    return 0;
}
