/**************************************************************************
 *
 * stdexcept.cpp - Illustrate the use of C++ Standard Library exceptions.
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
 * Copyright 1994-2005 Rogue Wave Software.
 * 
 **************************************************************************/

#include <string>      // for string
#include <stdexcept>   // for exception, runtime_error, out_of_range
#include <iostream>    // for cout

#include <examples.h>


#ifndef _RWSTD_NO_EXCEPTIONS

int main () {
    
    // First we'll incite and catch an exception from the C++ Standard
    // library class std::string by attempting to replace a substring
    // starting at a position beyond the end of the string object.
    try {
        std::string ().replace (100, 1, 1, 'c');
    }
    catch (std::out_of_range &e) {

        // Print out the exception string, which in this implementation
        // includes the location and the name of the function that threw
        // the exception along with the reason for the exception.
        std::cout << "Caught an out_of_range exception: "
                  << e.what () << '\n';
    }
    catch (std::exception &e) {

        std::cout << "Caught an exception of an unexpected type: "
                  << e.what () << '\n';
    }
    catch (...) {
        std::cout << "Caught an unknown exception\n";
    }
    
    // Throw another exception.
    try {
        throw std::runtime_error ("a runtime error");
    }
    catch (std::runtime_error &e) {
        std::cout << "Caught a runtime_error exception: "
                  << e.what () << '\n';
    }
    catch (std::exception &e) {
        std::cout << "Caught an exception of an unexpected type: "
                  << e.what () << '\n';
    } 
    catch (...) {
        std::cout << "Caught an unknown exception\n";
    }
   
    return 0;
}

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

int main () {
    
    std::cout << "Exception support disabled.\n";

    return 0;
}

#endif   // _RWSTD_NO_EXCEPTIONS
