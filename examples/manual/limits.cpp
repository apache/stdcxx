/**************************************************************************
 *
 * limits.cpp - Example program of numeric limits class used for 
 *              representing information about scalar types.
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/limits.cpp#12 $
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

#include <limits>      // for numeric_limits
#include <iostream>    // for cout

#include <examples.h>


template <class T>
void print_limits (std::ostream &strm, const char *tname, T)
{
#define PRINT_MEMBER(member)                                       \
    strm << "std::numeric_limits<" << tname << ">::" #member " = " \
         << std::numeric_limits<T>::member << '\n';

    PRINT_MEMBER (is_specialized);

    PRINT_MEMBER (min ());
    PRINT_MEMBER (max ());

    PRINT_MEMBER (digits);
    PRINT_MEMBER (digits10);

    PRINT_MEMBER (is_signed);
    PRINT_MEMBER (is_integer);
    PRINT_MEMBER (is_exact);
    PRINT_MEMBER (radix);

    PRINT_MEMBER (epsilon ());
    PRINT_MEMBER (round_error ());

    PRINT_MEMBER (min_exponent);
    PRINT_MEMBER (min_exponent10);
    PRINT_MEMBER (max_exponent);
    PRINT_MEMBER (max_exponent10);

    PRINT_MEMBER (has_infinity);
    PRINT_MEMBER (has_quiet_NaN);
    PRINT_MEMBER (has_signaling_NaN);
    PRINT_MEMBER (has_denorm);

    PRINT_MEMBER (has_denorm_loss);

    PRINT_MEMBER (infinity ());
    PRINT_MEMBER (quiet_NaN ());
    PRINT_MEMBER (signaling_NaN ());
    PRINT_MEMBER (denorm_min ());

    PRINT_MEMBER (is_iec559);
    PRINT_MEMBER (is_bounded);
    PRINT_MEMBER (is_modulo);

    PRINT_MEMBER (traps);
    PRINT_MEMBER (tinyness_before);

    PRINT_MEMBER (round_style);
}

int main ()
{
    print_limits<double>(std::cout, "double", 0.0);

    return 0;
}
