/**************************************************************************
 *
 * moneyget.cpp - Example program for the money_get facet. 
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#include <locale>     // for locale, money_get, use_facet
#include <sstream>    // for istringstream
#include <iostream>   // for cout, endl
#include <iterator>   // for istreambuf_iterator

#include <examples.h>


#if defined (__osf__)
const char en_US[] = "en_US.ISO8859-1";
#elif defined (__hpux)
const char en_US[] = "en_US.iso88591";
#elif defined (_WIN32) || defined (_WIN64)
const char en_US[] = "English";
#else
const char en_US[] = "en_US";
#endif


int main ()
{
    typedef std::istreambuf_iterator<char, std::char_traits<char> > Iter;
  
    const char buffer[] = "$100.02";

    std::string dest;
    long double ldest = 0.0;

    std::ios_base::iostate state = std::ios_base::goodbit;
    Iter end;

    // Retrieve the money_get facet from the global locale.
    const std::locale loc (en_US);

    const std::money_get<char, Iter> &mgf =
        std::use_facet<std::money_get<char, Iter> >(loc);

    {
        // Build an istringstream from the buffer and construct
        // a beginning iterator on it.
        std::istringstream ins (buffer);
        ins.imbue (loc);
        Iter begin (ins);

        // Get a string representation of the monetary value
        mgf.get (begin, end, false, ins, state, dest);
    }
    {
        // Build another istringstream from the buffer, etc.
        // so we have an iterator pointing to the beginning
        std::istringstream ins (buffer);
        ins.imbue (loc);
        Iter begin (ins);

        // Get a a long double representation of the monetary value
        mgf.get (begin, end, false, ins, state, ldest);
    }

    std::cout << buffer << " --> "
              << dest << " --> " << ldest << std::endl;

    // return 0 on success, non-zero on failure
    return !(std::ios_base::eofbit == state);
}
