/**************************************************************************
 *
 * mask_array.cpp -- Mask array examples
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

typedef std::valarray<int>    valarray_t;
typedef std::valarray<bool>   maskarray_t;

int main(void) {

  // Create a valarray of ints.
  valarray_t::value_type   ibuf[10] = {0,1,2,3,4,5,6,7,8,9};
  valarray_t               vi(ibuf, 10);

  // Create a valarray of bools for a mask.
  maskarray_t::value_type  mbuf[10] = {1,0,1,1,1,0,0,1,1,0};
  maskarray_t              mask(mbuf,10);

  // Print out the valarray<int>.
  std::cout << "original valarray<int> vi\n\n" << vi << "\n\n";

  // Print out the valarray<bool>.
  std::cout << "original valarray<bool> mask\n\n" << mask << "\n\n";

  // Print a mask array.
  std::cout << "vi[mask]\n\n" << vi[mask] << "\n\n";

  // Double the values of the masked array and print out.
  vi[mask] += static_cast<valarray_t> (vi[mask]);
  std::cout << "vi[mask] += vi[mask]\n\n" << vi << std::endl;

  return 0;
}
