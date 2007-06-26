/**************************************************************************
 *
 * gslice_array.cpp -- Generalized array slice examples
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

#include <iostream>    // for cout, endl
#include <valarray.h>  // Includes valarray and provides stream inserter.
#include <examples.h>  // Generic header for all examples.

typedef std::valarray<int> valarray_t;

int main(void) {

  valarray_t::value_type
      ibuf[22] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                   11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 };

  // Create a valarray of ints
  valarray_t  vi (ibuf,(sizeof ibuf / sizeof *ibuf));

  // Print out the valarray
  std::cout << "original valarray vi:\n\n" << vi << "\n\n";

  // Get a two dimensional diagonal slice out of the middle
  std::size_t length_ary[] = {4, 2};
  std::size_t stride_ary[] = {4, 6};

  std::valarray<std::size_t> length_val(length_ary, 2);
  std::valarray<std::size_t> stride_val(stride_ary, 2);


  // Print out the slices starting at positions 0 and 2 and respectively.
  std::cout << "vi[gslice(0,[4,2],[4,6])]\n\n"
            << vi[std::gslice(0,length_val,stride_val)] << "\n\n";

  std::cout << "vi[gslice(2,[4,2],[4,6])]\n\n"
            << vi[std::gslice(2,length_val,stride_val)] << "\n\n";

  // Multiply the first slice by the second.
  vi[std::gslice(0,length_val,stride_val)]
      *= static_cast<valarray_t > (vi[std::gslice(2,length_val,stride_val)]);

  std::cout << "vi[gslice(0,[4,2],[4,8])] *= vi[gslice(2,[4,2],[4,6])]\n\n"
            << vi << std::endl;

  return 0;

}

