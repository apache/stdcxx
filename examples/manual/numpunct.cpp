/**************************************************************************
 *
 * numpunct.cpp - Example program for the numpunct facet. 

 * $Id: //stdlib/dev/examples/stdlib/manual/numpunct.cpp#12 $
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

#include <iostream>   // for cerr, cout
#include <locale>     // for locale, numpunct, use_facet
#include <stdexcept>  // for runtime_error

#include <examples.h>


#if defined (_AIX)
#  define de_DE "de_DE.ISO8859-1"
#elif defined (__hpux)
#  define de_DE "de_DE.iso88591"
#elif defined (__linux__)
#  define de_DE "de_DE"
#elif defined (__osf__)
#  define de_DE "de_DE.88591"
#elif defined (SNI)
#  define de_DE "De_DE.88591"
#elif defined (_WIN32) || defined (_WIN64)
#  define de_DE "german_germany.1252"
#else   // IRIX, SunOS, etc.
#  define de_DE "de"
#endif


int main ()
{
    typedef std::numpunct<char> Numpunct;

    try {
        // create a german locale
        const std::locale loc (de_DE);

        // obtain a numpunct facet for the german locale
        const Numpunct &np = std::use_facet<Numpunct>(loc);

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
        std::cerr << e.what() << '\n';
    }
    catch (...) {
        std::cerr << "Caught an unknown exception\n"; 
    }
    return 0;
}
