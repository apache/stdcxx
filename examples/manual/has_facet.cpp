/**************************************************************************
 *
 * hasfacet.cpp - Example program of the hasfacet function template. 
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

#include <ios>        // for boolalpha 
#include <iostream>   // for cout
#include <locale>     // for ctype, has_facet(), locale

#include <examples.h>


// dummy facet, must derive from locale::facet
// and define a static member of type locale::id
struct MyFacet: public std::locale::facet
{
    static std::locale::id id;
};

std::locale::id MyFacet::id;


int main ()
{
    // create a copy of the locale imbued in cout
    const std::locale loc (std::cout.getloc ());

    std::cout << std::boolalpha
              << "std::has_facet<std::ctype<char> >(std::cout.getloc ()) = ";

    // see if ctype<char> is installed in the locale (expect true)
    std::cout << std::has_facet<std::ctype<char> >(loc) << '\n';
 
    std::cout << "std::has_facet<MyFacet>(std::cout.getloc ()) = ";

    // see if MyFacet is installed in the locale (expect false)
    std::cout << std::has_facet<MyFacet>(loc) << '\n';

    return 0;
}
