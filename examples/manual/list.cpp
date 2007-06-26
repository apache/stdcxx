/**************************************************************************
 *
 * list.cpp - Example program of list class.
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

#include <algorithm>   // for find
#include <list>        // for list
#include <string>      // for string
#include <iostream>    // for cout

#include <examples.h>


typedef std::list<std::string, std::allocator<std::string> > StringList;


// stream out a list of strings
std::ostream& operator<< (std::ostream &out, const StringList &l)
{
    // create a sentry object to guard the stream
    const std::ostream::sentry guard (out);

    if (guard) {

        // the guard succeeded in preparing the stream for output

        for (StringList::const_iterator i = l.begin (); i != l.end (); ++i) {

            // insert the string into the stream object's buffer
            const std::streamsize n =
                out.rdbuf ()->sputn ((*i).data (), (*i).size ());

            // insert the space character into the buffer
            if (   std::streamsize ((*i).size ()) != n
                ||    std::ostream::traits_type::to_int_type (' ')
                   != out.rdbuf ()->sputc (' ')) {

                // set badbit if either operation failed
                out.setstate (std::ios::badbit);
                break;
            }
        }
    }

    return out;
}


int main ()
{
    // Create a list of critters.
    StringList critters;

    // Insert a few critters.
    critters.insert (critters.begin (), "antelope");
    critters.insert (critters.begin (), "bear");
    critters.insert (critters.begin (), "cat");

    // Print out the list.
    std::cout << critters << '\n';

    // Change cat to cougar.
    *std::find (critters.begin (),critters.end (), "cat") = "cougar";
    std::cout << critters << '\n';

    // Put a zebra at the beginning, an ocelot ahead of antelope,
    // and a rat at the end.
    critters.push_front ("zebra");
    critters.insert (std::find (critters.begin (), critters.end (),
                                "antelope"), "ocelot");
    critters.push_back ("rat");
    std::cout << critters << '\n';

    // Sort the list (Use list's sort function since the 
    // generic algorithm requires a random access iterator 
    // and list only provides bidirectional)
    critters.sort ();
    std::cout << critters << '\n';

    // Now let's erase half of the critters.
    StringList::size_type half = critters.size () / 2;
    for (StringList::size_type i = 0; i != half; ++i)
        critters.erase (critters.begin ());

    std::cout << critters << '\n';
  
    return 0;
}
