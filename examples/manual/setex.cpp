/**************************************************************************
 *
 * setex.cpp - Example program of sets.
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

#include <algorithm>    // for set_intersection, set_union
#include <functional>   // for less
#include <set>          // for set
#include <iostream>     // for cout, endl, ostream_iterator
#include <iterator>     // for ostream_iterator
#include <sstream>      // for ostringstream

#include <examples.h>


typedef std::set<double, std::less<double>, std::allocator<double> > Set;


std::ostream& operator<< (std::ostream& out, const Set &s)
{
    std::ostringstream strm;

    typedef std::ostream_iterator<double, char, std::char_traits<char> > Iter;

    std::copy (s.begin (), s.end (), Iter (std::cout, " "));

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
    // Create a set and insert 10 element into it.
    Set sd;

    for (Set::value_type i = 0; i < 10; ++i)
        sd.insert (i);

    // Print out the set.
    std::cout << sd << "\n\n";

    // Erase half of the elements in the set.
    Set::iterator sdi = sd.begin();
    std::advance (sdi, sd.size () / 2);
    sd.erase (sd.begin(), sdi);

    // Print it out again.
    std::cout << sd << "\n\n";

    // Make another set and an empty result set.
    Set sd2, sdResult;

    for (Set::value_type j = 1; j < 9; ++j)
        sd2.insert (j + 5);

    std::cout << sd2 << std::endl;

    // Try a couple of set algorithms.
    std::set_union (sd.begin (),sd.end (),sd2.begin (), sd2.end (),
                    std::inserter (sdResult, sdResult.begin ()));

    std::cout << "Union:\n" << sdResult << '\n';

    sdResult.erase (sdResult.begin (),sdResult.end ());

    std::set_intersection (sd.begin (), sd.end (), sd2.begin (), sd2.end (),
                     std::inserter (sdResult, sdResult.begin ()));

    std::cout << "Intersection:\n" << sdResult << '\n';
  
    return 0;
}
