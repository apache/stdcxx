/**************************************************************************
 *
 * merge.cpp - Example program of merging sequences. 
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/merge.cpp#13 $
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

#include <rw/_config.h>

#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
// Disable implicit inclusion to work around 
// a limitation in IBM's VisualAge 5.0.2.0 (see PR#26959) 

#  define _RWSTD_NO_IMPLICIT_INCLUSION 
#endif

#include <algorithm>    // advance, copy, inplace_merge, merge
#include <functional>   // less
#include <iostream>     // cout
#include <iterator>     // back_inserter, ostream_iterator
#include <vector>       // vector

#include <examples.h>


int main ()
{
    typedef std::vector<int, std::allocator<int> > Vector;

    const Vector::value_type d1[] = { 1, 2, 3, 4};
    const Vector::value_type d2[] = { 11, 13, 15, 17, 12, 14, 16, 18};

    // Set up two vectors.
    Vector v1 (d1 + 0, d1 + sizeof d1 / sizeof *d1);
    Vector v2 (d1 + 0, d1 + sizeof d1 / sizeof *d1);

    // Set up four destination vectors.
    Vector v3 (d2 + 0, d2 + sizeof d2 / sizeof *d2);
    Vector v4 (v3);
    Vector v5 (v3);
    Vector v6 (v3);

    // Set up one empty vector.
    Vector v7;

    // Merge v1 with v2.
    std::merge (v1.begin (), v1.end (),
                v2.begin (), v2.end (), v3.begin ());

    // Now use comparator.
    std::merge (v1.begin (), v1.end (),
                v2.begin (), v2.end (), v4.begin (), std::less<int>());

    // In place merge v5.
    Vector::iterator mid = v5.begin ();
    std::advance (mid, 4);   // equivalent to mid += 4 but more generic
    std::inplace_merge (v5.begin (), mid, v5.end ());

    // Now use a comparator on v6.
    mid = v6.begin ();
    std::advance (mid, 4);   // equivalent to mid += 4 but more generic
    std::inplace_merge (v6.begin (), mid, v6.end (), std::less<int>());	  

    // Merge v1 and v2 to empty vector using insert iterator.
    std::merge (v1.begin (), v1.end (),
                v2.begin (), v2.end (), std::back_inserter (v7));

    // Copy all to cout.
    std::ostream_iterator<int, char, std::char_traits<char> >
        out (std::cout," ");

    std::copy (v1.begin (), v1.end (), out);
    std::cout << std::endl;
    std::copy (v2.begin(), v2.end (), out);
    std::cout << std::endl;
    std::copy (v3.begin (), v3.end (), out);
    std::cout << std::endl;
    std::copy (v4.begin (), v4.end (), out);
    std::cout << std::endl;
    std::copy (v5.begin (), v5.end (), out);
    std::cout << std::endl;
    std::copy (v6.begin (),v6.end (), out);
    std::cout << std::endl;
    std::copy (v7.begin (), v7.end (), out);
    std::cout << std::endl;

    // Merge v1 and v2 to cout.
    std::merge (v1.begin (), v1.end (), v2.begin (), v2.end (), out);

    return 0;
}
