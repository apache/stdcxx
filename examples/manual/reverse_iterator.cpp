/**************************************************************************
 *
 * reverse_iterator.cpp - Example program of reverse iterator.
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

#include <iostream>   // for cout, endl
#include <vector>     // for vector

#include <examples.h>


int main ()
{
    typedef std::vector<int, std::allocator<int> > Vector;

    // Initialize a vector using an array.
    const Vector::value_type arr[] = { 3, 4, 7, 8 };
    const Vector v (arr + 0, arr + sizeof arr / sizeof *arr);

    // Output the original vector.
    std::cout << "Traversing vector with iterator: \n     ";
    for (Vector::const_iterator i = v.begin (); i != v.end (); ++i)
        std::cout << *i << " ";

    // Output the vector backwards.
    std::cout << "\n\nSame vector, same loop, reverse_itertor: \n     ";
    for (Vector::const_reverse_iterator j (v.end ());
         j != Vector::const_reverse_iterator (v.begin ()); ++j)
        std::cout << *j << " ";

    std::cout << std::endl;

    return 0;
}
