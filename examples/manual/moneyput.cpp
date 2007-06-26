/**************************************************************************
 *
 * moneyput.cpp - Example program for the money_put facet. 
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

#include <locale>     // for locale, money_put, use_facet
#include <iostream>   // for cout, endl
#include <iterator>   // for ostreambuf_iterator
#include <string>     // for string

#include <examples.h>


int main ()
{
    typedef std::ostreambuf_iterator<char, std::char_traits<char> > Iter;
  
    const std::string buffer ("10002");
    const long double ldval = 10002; 

    // Construct a ostreambuf_iterator on cout
    Iter begin (std::cout);

    const std::locale loc;

    // Get a money put facet
    const std::money_put<char, Iter> &mp = 
        std::use_facet<std::money_put<char, Iter> >(loc);

    // Put out the string representation of the monetary value
    std::cout << buffer << " --> ";
    mp.put (begin, false, std::cout, ' ', buffer);

    // Put out the long double representation of the monetary value
    std::cout << std::endl << ldval << " --> ";
    mp.put (begin, false, std::cout, ' ', ldval);

    std::cout << std::endl;

    return 0;
}
