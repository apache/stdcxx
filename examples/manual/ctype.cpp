/**************************************************************************
 *
 * ctype.cpp - Example program of ctype facet. 
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
 * Copyright 1994-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/
 
#include <iostream>   // for cout, endl
#include <locale>     // for ctype

#include <examples.h>

int main ()
{
    std::locale loc;
    char s [] = "blues Power";

    // Get a reference to the ctype<char> facet.
    const std::ctype<char>& ct = std::use_facet<std::ctype<char> >(loc);

    // Check the classification of the 'a' character.
    std::cout << ct.is (std::ctype_base::alpha, 'a') << std::endl;
    std::cout << ct.is (std::ctype_base::punct, 'a') << std::endl;

    // Scan for the first upper case character.
    std::cout << *ct.scan_is (std::ctype_base::upper,
                              s, s + sizeof s / sizeof *s)
              << std::endl;

    // Convert characters to upper case.
    (ct.toupper)(s, s + sizeof s / sizeof *s);
    std::cout << s << std::endl;
  
    return 0;
}
