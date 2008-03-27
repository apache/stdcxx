/**************************************************************************
 *
 * remove.cpp - Example program of remove algorithm.
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

#include <rw/_config.h>

#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
// Disable implicit inclusion to work around 
// a limitation in IBM's VisualAge 5.0.2.0 (see PR#26959) 

#  define _RWSTD_NO_IMPLICIT_INCLUSION 
#endif

#include <algorithm>    // for copy, remove, remove_copy, remove_if
#include <functional>   // for unary_function
#include <iostream>     // for cout, endl
#include <iterator>     // for ostream_iterator
#include <vector>       // for vector


//  template<class Arg>
//  struct not_zero: public std::unary_function<Arg, bool>
//  {
//      bool operator() (const Arg &a) const {
//          return a != 0;
//      }
//  };


typedef std::greater<int>              int_greater;
typedef std::binder2nd<int_greater>    bnd_greater;

int main ()
{
    // For convenience.
    typedef std::vector<int, std::allocator<int> >                    Vector;
    typedef std::ostream_iterator<int, char, std::char_traits<char> > Iter;

    // Populate a vector with elements from an array.
    const Vector::value_type arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Vector v (arr + 0, arr + sizeof arr / sizeof *arr);

    // Write out the contents to cout.
    std::copy (v.begin (), v.end (), Iter (std::cout," "));
    std::cout << std::endl << std::endl;

    // Move the 7 to the end of the vector.
    Vector::iterator result = std::remove (v.begin (), v.end (), 7);

    // Delete the 7 from the vector.
    v.erase (result, v.end ());

    std::copy (v.begin (), v.end (), Iter (std::cout, " "));
    std::cout << std::endl << std::endl;

    // Remove all non-zero elements beyond the fourth element.
    v.erase (std::remove_if (v.begin () + 4, v.end (), 
                             bnd_greater( int_greater (), 0 )), v.end ());

    std::copy (v.begin (), v.end (), Iter (std::cout, " "));
    std::cout << std::endl << std::endl;

    // Now remove all 3s on output.
    std::remove_copy (v.begin (), v.end (), Iter (std::cout, " "), 3);
    std::cout << std::endl << std::endl;

    // Now remove everything satisfying predicate on output.
    std::remove_copy_if (v.begin (), v.end (), Iter (std::cout, " "),
                         bnd_greater( int_greater (), 0 ) );

    // Return 0 on success, a non-zero value on failure
    return !!v.empty ();
}











