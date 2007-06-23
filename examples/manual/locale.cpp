/**************************************************************************
 *
 * locale.cpp - Example program for the  locale class. 
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

#include <rw/_defs.h>

#if defined (_AIX) && defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION) 
#  define _RWSTD_NO_IMPLICIT_INCLUSION 
#endif

#include <algorithm>   // for copy, sort
#include <iostream>    // for cout
#include <iterator>    // for ostream_iterator
#include <string>      // for string
#include <vector>      // for vector

#include <codecvte.h>
#include <examples.h>

int main ()
{
    std::locale loc;  // Default locale

    // Construct new locale using default locale plus
    // user defined codecvt facet
    // This facet converts from ISO Latin 
    // Alphabet No. 1 (ISO 8859-1) to 
    // U.S. ASCII code page 437
    // This facet replaces the default for
    // codecvt<char,char,mbstate_t>
    std::locale my_loc (loc, new ex_codecvt);

    // imbue modified locale onto cout
    std::locale old = std::cout.imbue (my_loc);
    std::cout << "A \x93 jolly time was had by all" << std::endl;

    std::cout.imbue (old);
    std::cout << "A jolly time was had by all" << std::endl;

    // Create a vector of strings 
    std::vector<std::string, std::allocator<std::string> > v;
    v.insert (v.begin(), "antelope");
    v.insert (v.begin(), "bison");
    v.insert (v.begin(), "elk");

    typedef std::ostream_iterator<std::string, char, std::char_traits<char> >
        Iter;

    std::copy (v.begin (), v.end (), Iter (std::cout, " "));
    std::cout << std::endl;

    // Sort the strings using the locale as a comparitor
    std::sort (v.begin (), v.end (), loc);

    std::copy (v.begin (), v.end (), Iter (std::cout," "));

    std::cout << std::endl;

    return 0;
}
