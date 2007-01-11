/**************************************************************************
 *
 * limits.cpp - Example program of numeric limits class used for 
 *              representing information about scalar types.
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

#include <limits>      // for numeric_limits
#include <iostream>    // for cout

#include <examples.h>


std::ostream& operator<< (std::ostream &strm, std::float_denorm_style style)
{
    const char* name = "(unknown)";

    switch (style) {
    case std::denorm_absent: name = "std::denorm_absent"; break;
    case std::denorm_indeterminate: name = "std::denorm_indeterminate"; break;
    case std::denorm_present: name = "std::denorm_present"; break;
    }

    return strm << name;
}


template <class T>
void print_limits (std::ostream &strm, const char *tname, T)
{
#define PRINT_MEMBER(type, member)                     \
    strm << "    static " << type << " " #member " = "            \
         << std::numeric_limits<T>::member << ";\n"

    strm << "struct std::numeric_limits<" << tname << "> {\n";

    PRINT_MEMBER ("const bool", is_specialized);

    PRINT_MEMBER (tname, min ());
    PRINT_MEMBER (tname, max ());

    PRINT_MEMBER ("const int", digits);
    PRINT_MEMBER ("const int", digits10);

    PRINT_MEMBER ("const bool", is_signed);
    PRINT_MEMBER ("const bool", is_integer);
    PRINT_MEMBER ("const bool", is_exact);
    PRINT_MEMBER ("const int", radix);

    PRINT_MEMBER (tname, epsilon ());
    PRINT_MEMBER ("int", round_error ());

    PRINT_MEMBER ("const int", min_exponent);
    PRINT_MEMBER ("const int", min_exponent10);
    PRINT_MEMBER ("const int", max_exponent);
    PRINT_MEMBER ("const int", max_exponent10);

    PRINT_MEMBER ("const bool", has_infinity);
    PRINT_MEMBER ("const bool", has_quiet_NaN);
    PRINT_MEMBER ("const bool", has_signaling_NaN);
    PRINT_MEMBER ("const std::float_denorm_style", has_denorm);

    PRINT_MEMBER ("const bool", has_denorm_loss);

    PRINT_MEMBER (tname, infinity ());
    PRINT_MEMBER (tname, quiet_NaN ());
    PRINT_MEMBER (tname, signaling_NaN ());
    PRINT_MEMBER (tname, denorm_min ());

    PRINT_MEMBER ("const bool", is_iec559);
    PRINT_MEMBER ("const bool", is_bounded);
    PRINT_MEMBER ("const bool", is_modulo);

    PRINT_MEMBER ("const bool", traps);
    PRINT_MEMBER ("const bool", tinyness_before);

    PRINT_MEMBER ("const int", round_style);

    strm << "};\n";
}


int main ()
{
#define PRINT_LIMITS(T) print_limits (std::cout, #T, T ())

    // print bool values as "false" and "true"
    std::cout.setf (std::cout.boolalpha);

    // print the numeric limits for an integer type
    PRINT_LIMITS (int);

    std::cout << '\n';

    // print the numeric limits for a floating point type
    PRINT_LIMITS (double);

    return 0;
}
