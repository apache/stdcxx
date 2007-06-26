/**************************************************************************
 *
 * p_queue.cpp - Example program of a priority queue.
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
 
#include <deque>      // for deque
#include <iostream>   // for cout, endl
#include <queue>      // for priority_queue
#include <string>     // for string
#include <vector>     // for vector

#include <examples.h>


int main ()
{
    // Make a priority queue of int  using a vector container.
    std::priority_queue<int,
                        std::vector<int, std::allocator<int> >,
                        std::less<int> > pq;
    // Push a couple of values.
    pq.push (1);
    pq.push (2);

    // Pop a couple of values and examine the ends.
    std::cout << pq.top () << std::endl;
    pq.pop ();
    std::cout << pq.top () << std::endl;
    pq.pop ();

    // Make a priority queue of strings.
    std::priority_queue<std::string,
                        std::deque<std::string, std::allocator<std::string> >,
                        std::less<std::string> > pqs;

    // Push on a few strings then pop them back off.
    for (int i = 0; i < 10; i++) {
        pqs.push (std::string (i + 1, 'a'));
        std::cout << pqs.top () << std::endl;
    }

    for (int j = 0; j < 10; j++) {
        std::cout << pqs.top () << std::endl;
        pqs.pop ();
    }

    // Make a priority queue of strings using greater.
    std::priority_queue<std::string,
                        std::deque<std::string,
                                   std::allocator<std::string> >, 
                        std::greater<std::string> > pgqs;

    // Push on a few strings then pop them back off.
    for (int k = 0; k < 10; k++)  {
        pgqs.push (std::string (k + 1, 'a'));
        std::cout << pgqs.top () << std::endl;
    }

    for (int m = 0; m < 10; m++) {
        std::cout << pgqs.top () << std::endl;
        pgqs.pop ();
    }

    return 0;
}
