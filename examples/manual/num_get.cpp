/**************************************************************************
 *
 * numget.cpp - Example program for the numget facet. 
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
#include <sstream>    // for istringstream
#include <string>     // for string


int main ()
{
    typedef std::istreambuf_iterator<char, std::char_traits<char> > Iter;
  
    std::ios::iostate state;
    Iter              end;

    bool        bval  = false;
    long        lval  = 0L;
    long double ldval = 0.0;

    const std::locale loc;

    // Get a num_get facet
    const std::num_get<char, Iter> &ng =
        std::use_facet<std::num_get<char, Iter> >(loc);

#ifndef _RWSTD_NO_BOOL
    {
        // Build an istringstream from the buffer and construct
        // beginning and ending iterators on it.
        std::istringstream ins ("true");
        Iter begin (ins);

        // Set stream flags to recognize boolalpha input.
        ins.setf (std::ios::boolalpha);

        // Get the boolean value from the stream.
        ng.get (begin, end, ins, state, bval);
    }
#endif

    std::cout << bval << std::endl;

    {
        // Get the date
        std::istringstream ins ("2422235");
        Iter begin (ins);
        ng.get (begin, end, ins, state, lval);
    }

    std::cout << lval << std::endl;

    {
        // Get the weekday
        std::istringstream ins ("32324342.98908");
        Iter begin (ins);
        ng.get (begin, end, ins, state, ldval);
    }

    std::cout.setf (std::ios::fixed, std::ios::floatfield);
    std::cout << ldval << std::endl;

    return 0;
}
