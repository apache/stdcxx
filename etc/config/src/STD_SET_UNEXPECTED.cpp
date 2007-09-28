// checking for std::set_unexpected()

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


#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION

#  define throw()

#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION

// will fail if namespaces aren't supported
namespace std {

typedef void (*unexpected_handler)();

// looking for a definition in compiler support library
unexpected_handler set_unexpected (unexpected_handler) throw ();

}

// will cause an ambiguity if namespace std isn't being honored
int set_unexpected ();

void handler () { }

int main (int argc, char *argv[])
{
    if (argc > 1) {
        std::unexpected_handler save = std::set_unexpected (handler);

        return save == handler;
    }

    return 0;
}
