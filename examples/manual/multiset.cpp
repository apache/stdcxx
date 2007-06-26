/**************************************************************************
 *
 * multiset.cpp - Example program for multiset class.
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

#include <algorithm>   // for set_intersection(), set_union()
#include <iostream>    // for cout, endl, ostream
#include <iterator>    // for ostream_iterator
#include <sstream>     // for ostringstream
#include <set>         // for set

#include <examples.h>


typedef std::multiset<int, std::less<int>, std::allocator<int> > IntSet;


std::ostream& operator<< (std::ostream& out, const IntSet& s)
{
    std::ostringstream strm;

    typedef std::ostream_iterator<int, char, std::char_traits<char> >
        OstreamIter;

    std::copy (s.begin (), s.end (), OstreamIter (strm, " "));

    const std::ostream::sentry guard (out);

    if (guard) {
        const std::string data = strm.str ();

        if (   std::streamsize (data.size ())
            != out.rdbuf ()->sputn (data.data (), data.size ()))
            out.setstate (std::ios::badbit);
    }

    return out;
}


int main ()
{
    // Create a multiset of integers.
    IntSet  si;

    for (IntSet::value_type j = 0; j < 2; j++) {
        for (IntSet::value_type i = 0; i < 10; ++i)
            // Insert values with a hint.
            si.insert (si.begin (), i);
    }

    // Print out the multiset.
    std::cout << si << std::endl;

    // Make another multiset and an empty multiset.
    IntSet si2, result;
    for (IntSet::value_type i = 0; i < 10; i++)
        si2.insert (i + 5);

    std::cout << si2 << std::endl;

    // Try a couple of set algorithms.
    std::set_union (si.begin (), si.end (), si2.begin (), si2.end (),
                    std::inserter (result, result.begin ()));

    std::cout << "Union:\n" << result << std::endl;

    result.erase (result.begin (), result.end ());

    std::set_intersection (si.begin (), si.end (), si2.begin (), si2.end (),
                          std::inserter (result, result.begin ()));

    std::cout << "Intersection:\n" << result << std::endl;

    return 0;
}
