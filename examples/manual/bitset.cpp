/**************************************************************************
 *
 * bitset.cpp - Example program for bitset.
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/bitset.cpp#13 $
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

#include <bitset>     // for bitset
#include <iostream>   // for cout, endl

#include <examples.h>

int main ()
{
    // Construct a bitset with 40 elements all initialized to 0.
    std::bitset<40> b;

    // Bitwise OR the bitset with 5.
    b |= 5;

    // Write bitset to the standard output.
    std::cout << "         4....:....3....:....2....:....1....:....0\n"
	      << "b = 5:    " << b << '\n';

    // Invert the bitset.
    b = ~b;

    std::cout << "b = ~b:   " << b << '\n'
	      << "b <<= " << b.count () << ": ";

    // Shift the bitset left by the number of bits set.
    b <<= b.count ();

    // Convert it to a string and write it out again.
    std::cout << b.to_string () << '\n';

    // Return 0 on success.
    return !(1 == b.count () && 1UL == (b >> (b.size () - 1)).to_ulong ());
}
