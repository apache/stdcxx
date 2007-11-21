// checking for class ::bad_exception

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


#ifndef _RWSTD_NO_EXCEPTION_SPECIFICATION
#  define throws(args)   throw args
#else
#  define throws(ignore)
#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION


#ifndef _RWSTD_NO_STD_SET_UNEXPECTED

namespace std {

#endif   // _RWSTD_NO_STD_SET_UNEXPECTED


typedef void (*unexpected_handler)();
unexpected_handler set_unexpected (void (*)()) throws (());


#ifndef _RWSTD_NO_STD_SET_UNEXPECTED

}   // namespace std

#  define SET_UNEXPECTED(handler)   std::set_unexpected (handler)

#else

#  define SET_UNEXPECTED(handler)   set_unexpected (handler)

#endif   // _RWSTD_NO_STD_SET_UNEXPECTED

class bad_exception { };


void foo () throws ((bad_exception))
{
    // throw an object of a disallowed type
    throw 0;
}

void handler ()
{
    // throw an object of a type not allowed by the exception-specification
    // of the function that already violated its specification (due to which
    // the handler has been called); this requires the exception object to be
    // replaced by the runtime library by one of type [std]::bad_exception
    throw "";
}

int main ()
{
    SET_UNEXPECTED (handler);

    try {
        // will fail if runtime library doesn't recognize ::bad_exception
        // by calling terminate()
        foo ();
    }
    catch (bad_exception&) {
        // successfully caught object thrown by runtime library
        return 0;
    }
    catch (...) {
        return 1;
    }

    // runtime library is broken
    return 2;
}
