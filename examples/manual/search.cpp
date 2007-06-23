/**************************************************************************
 *
 * search.cpp - Example program of search algorithm.
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

#include <algorithm>
#include <list>
#include <iostream>

int main ()
{
#ifndef _RWSTD_NO_NAMESPACE
  using namespace std;
#endif

  //
  // Initialize a list sequence and subsequence with characters.
  //
  char seq[40]    = "Here's a string with a substring in it";
  char subseq[10] = "substring";
  list<char,allocator<char> > sequence(seq, seq+38);
  list<char,allocator<char> > subseqnc(subseq, subseq+9);
  //
  // Print out the original sequence.
  //
  cout << endl << "The subsequence, " << subseq << ", was found at the ";
  cout << endl << "location identified by a '*'" << endl << "     ";
  //
  // Create an iterator to identify the location of 
  // subsequence within sequence.
  //
  list<char,allocator<char> >::iterator place;
  //
  // Do search.
  //
  place = search(sequence.begin(), sequence.end(),
                 subseqnc.begin(), subseqnc.end());
  //
  // Identify result by marking first character with a '*'.
  //
  *place = '*';
  //
  // Output sequence to display result.
  //
  for (list<char,allocator<char> >::iterator i = sequence.begin(); i != sequence.end(); i++)
    cout << *i;
  cout << endl;

  return 0;
}
