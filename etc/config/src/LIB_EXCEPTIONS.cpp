// checking library exceptions

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

#if 0   // guard invalid preprocessor symbol below

   // establish a dependency on lib_exceptions.lib.*
#  ifndef _RWSTD_NO_lib_exceptions.lib

     // link this compiled translation unit with the lib below
     // LDOPTS = lib_exceptions.lib$(LIBSUFFIX)

#  endif   // _RWSTD_NO_lib_exceptions.lib
#endif   // 0


#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE
#    include "terminate.h"
#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


// defined in a library, throws an exception
int foo (int);


int main ()
{
    try {
        // expected to throw an exception
        return foo (1);
    }
    catch (...) {
        // success: exception caught
        return 0;
    }

    // failure: no exception thrown
    return 1;
}
