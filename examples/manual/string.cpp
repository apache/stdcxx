/**************************************************************************
 *
 * string.cpp - Example program of string.
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
#include <string>     // for string

#include <examples.h>


int main ()
{
    std::string test;

    // Type in a string over five characters long.
    while (test.empty() ||  test.size() < 5) {
        std::cout << "Type a string between 5 and 100 characters long. \n";
        std::cin >> test;
    }

    // Try operator[] access.
    std::cout << "Changing the third character from "
              << test[2] << " to * \n";
    test [2] = '*';

    std::cout << "now its: " << test << "\n\n";

    // Try the insertion member function.
    test.insert (test.size() / 2, "(the middle is here!)");
    std::cout << "Identifying the middle: " << test << "\n\n";

    // Try replacement.
    test.replace (test.find ("middle", 0), 6, "center");
    std::cout << "I didn't like the word 'middle', so instead, I'll say: \n"
              << test << std::endl; 

    return 0;
}
