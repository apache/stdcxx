/**************************************************************************
 *
 * list.cpp - Example program of list class.
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/list.cpp#13 $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#include <algorithm>   // for find
#include <list>        // for list
#include <string>      // for string
#include <iostream>    // for cout, endl

#include <examples.h>


typedef std::list<std::string, std::allocator<std::string> > StringList;


// stream out a list of strings
std::ostream& operator<< (std::ostream &out, const StringList &l)
{
    const std::ostream::sentry guard (out);

    for (StringList::const_iterator i = l.begin (); i != l.end (); ++i) {

        const std::streamsize n =
            out.rdbuf ()->sputn ((*i).data (), (*i).size ());

        if (   std::streamsize ((*i).size ()) != n
            ||    std::ostream::traits_type::to_int_type (' ')
               != out.rdbuf ()->sputc (' ')) {
            out.setstate (std::ios::badbit);
            break;
        }
    }

    return out;
}


int main ()
{
    // Create a list of critters.
    StringList critters;

    // Insert several critters.
    critters.insert (critters.begin (), "antelope");
    critters.insert (critters.begin (), "bear");
    critters.insert (critters.begin (), "cat");

    // Print out the list.
    std::cout << critters << std::endl;

    // Change cat to cougar.
    *std::find (critters.begin (),critters.end (), "cat") = "cougar";
    std::cout << critters << std::endl;

    // Put a zebra at the beginning, an ocelot ahead of antelope,
    // and a rat at the end.
    critters.push_front ("zebra");
    critters.insert (std::find (critters.begin (), critters.end (),
                                "antelope"), "ocelot");
    critters.push_back ("rat");
    std::cout << critters << std::endl;

    // Sort the list (Use list's sort function since the 
    // generic algorithm requires a random access iterator 
    // and list only provides bidirectional)
    critters.sort ();
    std::cout << critters << std::endl;

    // Now let's erase half of the critters.
    StringList::size_type half = critters.size () / 2;
    for (StringList::size_type i = 0; i != half; ++i)
        critters.erase (critters.begin ());

    std::cout << critters << std::endl;
  
    return 0;
}
