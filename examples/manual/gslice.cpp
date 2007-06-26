/**************************************************************************
 *
 * gslice.cpp - Generalized slice example program.
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

#include <iostream>    // fo cout, endl
#include <valarray.h>  // Includes valarray and provides stream inserter.
#include <examples.h>  // Generic header for all examples.

typedef std::valarray<int>    valarray_t;
typedef std::valarray<std::size_t> selector_t;

int main(void) {

  // Create a valarray of ints.
  valarray_t::value_type ibuf[27] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,
                                      9,  10, 11, 12, 13, 14, 15, 16, 17,
                                      18, 19, 20, 21, 22, 23, 24, 25, 26 };
  valarray_t  vi(ibuf, 27);

  // Create length and stride valarray
  selector_t::value_type length_buf[3] = {3,3,3};
  selector_t::value_type stride_buf[3] = {9,3,1};
  selector_t length_val(length_buf, 3);
  selector_t stride_val(stride_buf, 3);

  // Print out the valarray<int>.
  std::cout << "original valarray vi\n\n" << vi << "\n\n";

  // Print out all three dimensions (the entire valarray).
  std::cout << "vi[gslice(0,{3,3,3},{9,3,1})]\n\n"
            << vi[std::gslice(0,length_val,stride_val)] << "\n\n";

  // Print a two dimensional slice out of the middle.
  selector_t::value_type length_buf2[] = {3, 3};
  selector_t::value_type stride_buf2[] = {3, 1};
  selector_t             length_val2(length_buf2, 2);
  selector_t             stride_val2(stride_buf2, 2);

  std::cout << "vi[gslice(9,{3,3},{3,1})]\n\n"
            << vi[std::gslice(9,length_val2,stride_val2)] << "\n\n";

  // Print another two dimensional slice out of the middle but
  // orthoganal to one we just did.
  stride_val2[0] = 9;
  stride_val2[1] = 1;

  std::cout << "vi[gslice(3,{3,3},{9,1})]\n\n"
            << vi[std::gslice(3,length_val2,stride_val2)] << "\n\n";

  // Print out the last plane in the middle -- orthoganal to both
  // of the previous ones.
  stride_val2[0] = 3;
  stride_val2[1] = 9;

  std::cout << "vi[gslice(1,{3,3},{3,9})]\n\n"
            << vi[std::gslice(1,length_val2,stride_val2)] << "\n\n";

  // Now how about a diagonal slice, upper left front to lower right
  // back.
  stride_val2[0] = 3;
  stride_val2[1] = 10;

  std::cout << "vi[gslice(0,{3,3},{3,10})]\n\n"
            << vi[std::gslice(0,length_val2,stride_val2)] << "\n\n";

  return 0;
}
