/**************************************************************************
 *
 * collate.cpp - Example program of collate facet. 
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

#include <iostream>   // for cout
#include <locale>     // for collate

#include <examples.h>

int main ()
{
    const std::string s1 ("blue");
    const std::string s2 ("blues");

    std::cout <<   "String 1 : " << s1
              << "\nString 2 : " << s2 << '\n';

    // obtain a reference to the collate facet
    // installed in the locale imbued in cout
    const std::collate<char>& co =
        std::use_facet<std::collate<char> >(std::cout.getloc ());

    // use the facet to compare the two strings
    int res = co.compare (s1.c_str (), s1.c_str () + s1.length (), 
                          s2.c_str (), s2.c_str () + s2.length () - 1);
    
    // strings should collate equal
    std::cout << "Strings \""
              << s1.substr(0, s1.length ()) << "\" and \""
              << s2.substr(0, s2.length () - 1)
              << "\" should compare equal      : "
              << (res ? "not " : "") << "equal\n";

    res = co.compare (s1.c_str (), s1.c_str () + s1.length (), 
                      s2.c_str (), s2.c_str () + s2.length ());

    // strings should collate unequal
    std::cout << "Strings \""
              << s1.c_str () << "\" and \"" << s2.c_str ()
              << "\" should compare NOT equal : "
              << (res ? "not " : "") << "equal\n";

    // compute hash values for the two strings
    // expected values:
    //   ILP32:     431029 and      6896579
    //   LP64:  1651275109 and 422726428019
    const long hash1 = co.hash (s1.c_str (), s1.c_str () + s1.length ());
    const long hash2 = co.hash (s2.c_str (), s2.c_str () + s2.length ());

    // normalize the two hash values for portability
    // between 32-bit and 64-bit longs
    const long norm1 =  440460L;
    const long norm2 = 7898347L;

    // expect 431029 on output
    std::cout << "Normalized hash value for \"" << s1.c_str () << "\"  : "
              <<  hash1 % norm1
              << "\nNormalized hash value for \"" << s2.c_str () << "\" : "
              <<  hash2 % norm2
              << '\n';

    return 0;
}
