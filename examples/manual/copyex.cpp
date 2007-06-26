/**************************************************************************
 *
 * copyex.cpp - Example program for copy.
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

#include <algorithm>   // for copy
#include <iostream>    // for cout, endl
#include <iterator>    // for  ostream_iterator
#include <vector>      // for vector

#include <examples.h>


int main ()
{
    // Typedef for convenience.
    typedef std::vector<short, std::allocator<short> > Vector;

    const Vector::value_type d1[] = { 1, 2, 3, 4 };
    const Vector::value_type d2[] = { 5, 6, 7, 8 };

    // Set up three vectors.
    Vector v1 (d1 + 0, d1 + 4), 
           v2 (d2 + 0, d2 + 4), 
           v3 (d2 + 0, d2 + 4);

    // Set up one empty vector.
    Vector v4;

    // Copy v1 to v2.
    std::copy (v1.begin (), v1.end (), v2.begin ());

    // Copy backwards v1 to v3.
    std::copy_backward (v1.begin (), v1.end (), v3.end ());

    // Use insert iterator to copy into empty vector.
    std::copy (v1.begin (), v1.end (), std::back_inserter (v4));

    // Copy all four vectors to cout.
    std:: ostream_iterator<Vector::value_type,
                           char,
                           std::char_traits<char> >
        out (std::cout, " ");

    std::copy (v1.begin (), v1.end (), out);
    std::cout << std::endl;

    std::copy (v2.begin (), v2.end (), out);
    std::cout << std::endl;

    std::copy (v3.begin (), v3.end (), out);
    std::cout << std::endl;

    std::copy (v4.begin (), v4.end (), out);
    std::cout << std::endl;
   
    return 0;
}
