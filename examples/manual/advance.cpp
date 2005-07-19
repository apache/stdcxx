/**************************************************************************
 *
 * advance.cpp - Example program for advancing iterators. 
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/advance.cpp#12 $
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

#include <iterator>   // for advance
#include <iostream>   // for cout, endl
#include <list>       // for list

#include <examples.h>


int main ()
{
    // Typedefs for convenience.
    typedef std::list<long, std::allocator<long> > List;
    typedef std::ostream_iterator<List::value_type,
                                  char, std::char_traits<char> > os_iter;

    // Initialize a list using an array.
    const List::value_type arr [] = { 3, 4, 5, 6, 7, 8 };
    List l (arr + 0, arr + sizeof arr / sizeof *arr);

    // Declare a list iterator, s.b. a ForwardIterator.
    List::iterator itr = l.begin ();

    // Output the original list.
    std::cout << "For the list: ";
    std::copy (l.begin (), l.end (), os_iter (std::cout, " "));

    std::cout << std::endl << std::endl
              << "When the iterator is initialized to l.begin ()," 
              << "\nit points to " << *itr << std::endl;

    // operator+ is not available for a ForwardIterator, so use advance.
    std::advance (itr, 4);
    std::cout << "\nAfter advance (itr, 4), the iterator points to "
              << *itr << std::endl;

    return 0;
}
