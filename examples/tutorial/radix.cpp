/**************************************************************************
 *
 * radix.cpp - Radix sort employing various STL classes.
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

#include <deque>
#include <list>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iterator>

#include <examples.h>

typedef std::deque<int, std::allocator<int> >                     IntDeque;
typedef std::vector<IntDeque, std::allocator<IntDeque> >          DeqVector;
typedef std::list<int, std::allocator<int> >                      IntList;
typedef std::ostream_iterator<int, char, std::char_traits<char> > OstreamIter;


class copyIntoBuckets {
public:
    copyIntoBuckets (int d, DeqVector &b, bool &f)
      : divisor (d), buckets (b), flag (f)
        {}

    void operator () (unsigned int v) {
        const int inx = (v / divisor) % 10;
        // flag set to true if any bucket other than the 0-th one is used
        if (inx)
            flag = true; 
        buckets [inx].push_back (v);
    }

    int        divisor;
    DeqVector &buckets;
    bool      &flag;
};


IntList::iterator
copyList (IntList::iterator c, IntDeque &lst) {
    
    return std::copy (lst.begin (), lst.end (), c);
}


void radixSort (IntList & values) {
    bool flag   = true;
    int divisor = 1;
    
    while (flag)
    {
        DeqVector buckets (10);
        flag = false;

        std::for_each (values.begin (), values.end (), 
            copyIntoBuckets (divisor, buckets, flag));
        std::accumulate (buckets.begin (), buckets.end (), values.begin (), copyList);
        divisor *= 10;
        std::copy (values.begin (), values.end (), OstreamIter (std::cout, " "));
        std::cout << std::endl;
    }
}


int main () {
    
    std::cout << "Radix sort program"  << std::endl;

    const IntList::value_type data[] = { 624, 852, 426, 987, 269,
                                       146, 415, 301, 730, 78, 593 };

    IntList values (data, data + sizeof data / sizeof *data);

    radixSort (values);
    std::copy (values.begin (), values.end (), OstreamIter (std::cout, " "));

    std::cout << std::endl << "End radix sort program" << std::endl;

    return 0;   
}
