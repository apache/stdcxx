/**************************************************************************
 *
 * money_get.cpp - Example program for the money_get facet. 
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
 * Copyright 1998-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <locale>     // for locale, money_get, use_facet
#include <sstream>    // for istringstream
#include <iostream>   // for cout, endl
#include <iterator>   // for istreambuf_iterator

#include <examples.h>


// hardcode the name of the English US locale for known systems
#if defined (__FreeBSD__) || defined (__osf__)
// FreeBSD and Tru64 UNIX
const char en_US[] = "en_US.ISO8859-1";
#elif defined (__hpux)
// HP-UX
const char en_US[] = "en_US.iso88591";
// Windows
#elif defined (_WIN32)
const char en_US[] = "English";
#else
// AIX, IRIX, Linux, Solaris
const char en_US[] = "en_US";
#endif


int main (int argc, char *argv[])
{
    // Get the monetary string and locale from the argument vector.
    const char* const buffer  = 1 < argc ? argv [1] : "$1,234.6789";
    const char* const locname = 2 < argc ? argv [2] : en_US;
    const bool        intl    = 3 < argc;

    std::string smon;
    long double fmon = 0.0;

    std::ios_base::iostate state = std::ios_base::goodbit;

    // Retrieve the money_get facet from the named locale.
    const std::locale loc (locname);

    typedef std::istreambuf_iterator<char> Iter;
    typedef std::money_get<char, Iter>     MoneyGet;

    const MoneyGet &mgf = std::use_facet<MoneyGet>(loc);

    {
        // Build an istringstream object from the buffer
        // and imbue the locale in it.
        std::istringstream ins (buffer);
        ins.imbue (loc);

        // Get a string representation of the monetary value.
        mgf.get (ins, Iter (), intl, ins, state, smon);
    }
    {
        std::istringstream ins (buffer);
        ins.imbue (loc);

        // Get a floating point representation of the monetary value.
        mgf.get (ins, Iter (), intl, ins, state, fmon);
    }

    // Output the original sequence and its string and floating point
    // representations.
    std::cout << buffer << " --> \"" << smon << "\" --> " << fmon << '\n';

    // Return 0 on success, non-zero on failure.
    return !(std::ios_base::eofbit == state);
}
