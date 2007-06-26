/**************************************************************************
 *
 * indirect_array.cpp -- Indirect array examples
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
typedef std::valarray<std::size_t> selector_t;

int main(void) {

  // Create a valarray of integers.
  valarray_t::value_type vbuf[10] = {0,1,2,3,4,5,6,7,8,9};
  valarray_t vi(vbuf, (sizeof vbuf / sizeof *vbuf));

  // Create a valarray of indices for a selector.
  selector_t::value_type sbuf[6] = {0,2,3,4,7,8};
  selector_t selector(sbuf, (sizeof sbuf / sizeof *sbuf));

  // Print out the valarray<int>.
  std::cout << "original valarray vi\n\n" << vi << "\n\n";

  // Print out the selective array.
  std::cout << "vi[0,2,3,4,7,8]\n\n" <<  vi[selector] << "\n\n";

  // Double the selected values.
  vi[selector] += vi[selector];

  // Print out the modified valarray.
  std::cout << "vi[0,2,3,4,7,8] += vi[0,2,3,4,7,8]\n\n" << vi << std::endl;

  return 0;
}

