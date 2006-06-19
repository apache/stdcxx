/**************************************************************************
 *
 * io_iter.cpp - Example program of iterator.
 *
 * $Id$
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

#include <algorithm>  // for copy
#include <iostream>   // for cin, cout, endl
#include <iterator>   // for stream_iterators, inserter
#include <vector>     // for vector
#include <numeric>    // for accumulate

#include <examples.h>

int main ()
{
    // Typedefs for convenience.
    typedef std::vector<int, std::allocator<int> > Vector;

    typedef std::istream_iterator<Vector::value_type,
                                  char, std::char_traits<char>,
                                  std::ptrdiff_t> is_iter;

    typedef std::ostream_iterator<Vector::value_type,
                                  char, std::char_traits<char> > os_iter;

    Vector v;

    Vector::value_type sum = 0;

    // Collect values from cin until end of file
    // Note use of default constructor to get ending iterator
    std::cout << "Enter a sequence of integers (eof to quit): " ;
    std::copy (is_iter (std::cin), is_iter (), std::inserter (v, v.begin ()));

    // Stream the whole vector and the sum to cout.
    std::copy (v.begin (), v.end () -1, os_iter (std::cout, " + "));

    if (v.size ())
        std::cout << v.back () << " = " <<  
            std::accumulate (v.begin (), v.end (), sum) << std::endl;

    return 0;
}
