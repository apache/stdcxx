/**************************************************************************
 *
 * numpunct.cpp - Example program for the numpunct facet. 

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

#include <iostream>   // for cerr, cout
#include <locale>     // for locale, numpunct, use_facet
#include <stdexcept>  // for runtime_error

#include <examples.h>


static std::locale
make_german_locale ()
{
    static const char* const
    locale_names [] = {
        "de_DE.ISO8859-1",       // AIX, Solaris, Tru64
        "de_DE.iso88591",        // HP-UX, Linux
        "de_DE.88591",
        "De_DE.88591",           // Reliant
        "de_DE",
        "de",                    // Linux, Solaris
        "German",
        "german",                // Linux
        "deutsch",               // Linux
        "german_germany.1252",   // Windows
        0                        // (sentinel)
    };

    std::locale german;

    // iterate over the know locale names above until a valid one
    // is found (i.e., one that doesn't cause locale to throw)
    for (const char* const *names = locale_names; ; ) {
        try {
            german = std::locale (names [0]);
            break;
        }
        catch (std::runtime_error&) {
            // continue trying until the next name is null
            if (0 == *++names)
                throw;
        }
        catch (...) {
            throw;
        }
    }

    return german;
}


int main ()
{
    typedef std::numpunct<char> Numpunct;

    try {
        // try to contruct a german locale
        const std::locale german (make_german_locale ());

        // obtain a numpunct facet for the german locale
        const Numpunct &np = std::use_facet<Numpunct>(german);

        std::cout <<   "German locale"
                  << "\nDecimal point       = " << np.decimal_point ()
                  << "\nThousands separator = " << np.thousands_sep ()
                  << "\nTrue name           = " << np.truename ()
                  << "\nFalse name          = " << np.falsename()
                  << '\n';
    }
    catch (std::runtime_error& e) {
        // a runtime_error will be thrown if the locale cannot be constructed
        std::cerr << "Caught runtime_error:\n"; 
        std::cerr << e.what () << '\n';
    }
    catch (...) {
        std::cerr << "Caught an unknown exception\n"; 
    }
    return 0;
}
