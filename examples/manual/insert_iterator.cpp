/**************************************************************************
 *
 * ins_itr.cpp - Example program of insert iterator. 
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

#include <algorithm>   // for copy
#include <iostream>    // for cout, endl
#include <iterator>    // for ostream_iterator, xxx_inserter
#include <deque>       // for deque

#include <examples.h>


int main ()
{
    // Typedefs for convenience.
    typedef std::deque<int, std::allocator<int> >                     Deque;
    typedef std::ostream_iterator<int, char, std::char_traits<char> > os_iter;

    // Initialize a deque using an array.
    Deque::value_type arr[] = { 3, 4, 7, 8 };
    Deque d (arr, arr + sizeof arr / sizeof *arr);

    // Output the original deque.
    std::cout << "Start with a deque: \n     ";
    std::copy (d.begin (), d.end (), os_iter (std::cout, " "));

    // Insert into the middle.
    std::insert_iterator<Deque> ins (d, d.begin () + 2);
    *ins = 5;
    *ins = 6;

    // Output the new deque.
    std::cout << "\n\nUse an insert_iterator: \n     ";
    std::copy (d.begin (), d.end (), os_iter (std::cout, " "));

    // A deque of four 1s.
    Deque d2 (4, 1);

    // Insert d2 at front of d.
    std::copy (d2.begin (), d2.end (), std::front_inserter (d));

    // Output the new deque.
    std::cout << "\n\nUse a front_inserter: \n     ";
    std::copy (d.begin (), d.end (), os_iter (std::cout, " "));

    // Insert d2 at back of d.
    std::copy (d2.begin (), d2.end (), std::back_inserter (d));

    // Output the new deque.
    std::cout << "\n\nUse a back_inserter: \n     ";
    std::copy (d.begin (), d.end (), os_iter (std::cout, " "));

    std::cout << std::endl;
   
    return 0;
}
