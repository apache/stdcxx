/**************************************************************************
 *
 * toupper.cpp - Example program of the toupper and tolower functions. 
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
 
#include <iomanip>    // for setw
#include <ios>        // for oct
#include <iostream>   // for cout, endl
#include <locale>     // for tolower(), toupper()

#include <examples.h>


int main ()
{
    std::cout << std::oct;
    std::cout.fill ('0');

    // compute tolower and toupper of printable ASCII chararcters
    for (int c = ' '; c != '~' + 1; ++c)
        std::cout << "std::toupper/lower ('\\" << std::setw (3) << c
                  << "', std::locale ()) = '"
                  << std::toupper (char (c), std::cout.getloc ())
                  << "' / "
                  << std::tolower (char (c), std::cout.getloc ())
                  << "'\n";
 
    return 0;
}
