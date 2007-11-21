// checking for ::uncaught_exception()

/***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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
 * Copyright 1999-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include "config.h"


#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE
#    include "terminate.h"
#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


// looking for a definition in compiler support library
bool uncaught_exception ();


bool uncaught;

struct S {
    ~S () {
        uncaught = uncaught_exception ();
    }
};


int main (int argc, char *argv[])
{
    (void)&argv;

#ifndef _RWSTD_NO_EXCEPTIONS

    try {
        S s;

        // do not actually invoke uncaught_exception()
        // unless at least one command-line argument
        // is specified (done to prevent the compiler
        // from optimizing the function call away)
        if (argc > 1)
            throw 0;
    }
    catch (...) {
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    return uncaught;
}
