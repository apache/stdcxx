/**************************************************************************
 *
 * failure.cpp - Example program demonstrating the use of ios::failure
 *               and its extendsions provided by this implementation.
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <iostream>
#include <stdexcept>

#include <examples.h>


#ifndef _RWSTD_NO_EXCEPTIONS

int main ()
{
    const std::ios::iostate allbits =
        std::ios::badbit | std::ios::eofbit | std::ios::failbit;
                
    // Enable all exceptions in cin.
    std::cin.exceptions (allbits);

    while (!std::cin.eof ()) {

        // Clear state bits.
        std::cin.clear ();

        try {
            short i;
            char  ch;

            // Try to extract a character and an integer.
            std::cin >> ch >> i;

            std::cout << ch << ' ' << i << '\n';
        }

#ifndef _RWSTD_NO_EXT_FAILURE

        // Classes badbit_set, eofbit_set, and failbit_set are
        // derived from failure and are optional extensions of
        // this implementation.

        catch (const std::ios::badbit_set &e) {
            std::cout << "Caught std::ios::badbit_set: "
                      << e.what () << '\n';
        }
        catch (const std::ios::eofbit_set &e) {
            std::cout << "Caught std::ios::eofbit_set: "
                      << e.what () << '\n';
        }
        catch (const std::ios::failbit_set &e) {
            std::cout << "Caught std::ios::failbit_set: "
                      << e.what () << '\n';
        }

#endif   // _RWSTD_NO_EXT_FAILURE

        catch (const std::ios::failure &e) {
            std::cout << "Caught std::ios::failure: "
                      << e.what () << '\n';
        }
        catch (const std::exception &e) {
            std::cout << "Caught std::exception: "
                      << e.what () << '\n';

            return 1;   // Indicate error.
        }
        catch (...) {
            std::cout << "Caught an unknown exception\n";

            return 1;   // Indicate error.
        }
    }

    return 0;
}

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

int main ()
{
    std::cout << "Exceptions not supported\n";

    return 0;
}


#endif   // _RWSTD_NO_EXCEPTIONS
