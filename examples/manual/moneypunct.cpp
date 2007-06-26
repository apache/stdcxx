/**************************************************************************
 *
 * moneypunct.cpp - Example program for the moneypunct facet. 
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

#include <climits>    // for CHAR_MAX
#include <clocale>    // for lconv, localeconv()
#include <iostream>   // for cout, endl
#include <locale>     // for locale

#include <examples.h>


int main ()
{
    // Get a moneypunct facet from the classic "C" locale
    const std::moneypunct<char, false> &mp =
        std::use_facet<std::moneypunct<char, false> >(std::locale::classic ());

    const std::lconv* const lc = std::localeconv ();
    
    // Expected output:
    // Punctuator             C++   C99
    // Decimal point        = '.'   "."
    // Thousands separator  = ','   ""
    // Currency symbol      = ""    ""
    // Negative Sign        = ""    ""
    // Positive Sign        = ""    ""
    // Digits after decimal = 0    CHAR_MAX

    std::cout << "Punctuator             C++   C99\n";

    std::cout << "Decimal point        = '"
              << mp.decimal_point () << "'   \""
              << lc->decimal_point << '"';

    std::cout << "\nThousands separator  = '"
              << mp.thousands_sep () << "'   \""
              << lc->thousands_sep << '"';

    std::cout << "\nCurrency symbol      = \""
              << mp.curr_symbol () << "\"    \""
              << lc->currency_symbol << '"';

    std::cout << "\nNegative Sign        = \""
              << mp.negative_sign () << "\"    \""
              << lc->negative_sign << '"';

    std::cout << "\nPositive Sign        = \""
              << mp.positive_sign () << "\"    \""
              << lc->positive_sign << '"';

    std::cout << "\nDigits after decimal = "
              << mp.frac_digits () << "    ";

    // since the value of CHAR_MAX depends on the signedness of char,
    // if lc->frac_digits equals CHAR_MAX print the string "CHAR_MAX"
    // to maintain the same output across all architectures, otherwise
    // print the actual value

    if (CHAR_MAX == lc->frac_digits)
        std::cout << "CHAR_MAX";
    else {
        // this block should not be entered in a conforming environment
        // since, according to 7.11, p2 of C99, frac_digits is required
        // to be equal to CHAR_MAX in the "C" locale

        // cast lconv::frac_digits from char to int to print out
        // the numeric value of the member
        std::cout << int (lc->frac_digits)
                  << " (libc bug: expected CHAR_MAX, i.e., "
                  << unsigned ((unsigned char)CHAR_MAX)
                  << ")";
    }

    std::cout << std::endl;

    return 0;
}
