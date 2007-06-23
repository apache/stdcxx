/**************************************************************************
 *
 * rwexcept.cpp - Example program demonstrating the use of the optional
 *                C++ Standard Library exception mechanism.
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

#include <exception>      // for exception
#include <iostream>       // for cerr
#include <vector>         // for vector
#include <cstdlib>        // for exit()

#include <rw/_error.h>    // for __rw_throw_proc, _RWSTD_ERROR_BAD_CAST

#include <examples.h>


// function called from within the library to throw an exception
void exception_handler (int id, char *what)
{
    std::cerr << "exception #" <<  id << ": " << what << '\n';

    // free what buffer
    if (what)
        __rw::__rw_free_what_buf (what);

    // a real program would call abort() here to prevent the potentially
    // dangerous destruction of objects with static storage duration
    // abort ();

    // introduce exit into the current scope if it's declared in namespace
    // std (as it should be) to allow referring to the function without
    // qualification when <cstdlib> incorrectly declares it at file scope
    using namespace std;

    // successfully exit the process
    exit (0);

    // return at your own risk
}


// prevent the allocation of more than 4 elements
struct Allocator: std::allocator<int>
{
    size_type max_size () const {
        return 4;
    }
};


int main ()
{
    typedef std::vector<int, Allocator> Vector;

    Vector v;

    for (Vector::size_type n = 0; n != 2 * v.max_size (); ++n) {
        try {
            // try to reserve storage for `n' values
            v.reserve (n);
        }
        catch (std::exception &e) {
            std::cerr << "caught an exception:" << e.what () << '\n';

            // after the first exception has been caught
            // replace the default throw procedure used
            // by the library with a user-defined exception
            // function and continue to iterate
            __rw::__rw_throw_proc = exception_handler;
        }
    }

    // should not reach
    std::cerr << "unexpected return from main\n";

    return 1;
}
