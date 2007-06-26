/**************************************************************************
 *
 * queue.cpp - Example program of queues.
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
#include <list>       // for list
#include <queue>      // for queue
#include <string>     // for string

#include <examples.h>

int main ()
{
    typedef std::queue<int, std::list<int, std::allocator<int> > > IQueue;

    // Make a queue using a deque container.
    IQueue q;

    // Push a couple of values on and retrieve them.
    q.push (1);
    q.push (2);

    std::cout << q.front () << std::endl;
    std::cout << q.back () << std::endl;

    typedef std::queue<std::string, std::deque<std::string,
                       std::allocator<std::string> > > SQueue;

    // Make a queue of strings using a deque container.
    SQueue qs;

    // Push on a few strings then pop them back off.
    for (std::string::size_type i = 0U; i != 10U; i++) {
        qs.push (std::string (i + 1, 'a'));
        std::cout << qs.front () << std::endl;
    }

    for (std::string::size_type j = 0U; j != 10U; j++) {
        std::cout << qs.front () << std::endl;
        qs.pop ();
    }

    // Return 0 if both containers are empty, a non-zero value otherwise.
    return !(2 == q.size () && qs.empty ());
}
