// checking for std::nothrow_t

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

#include <stddef.h>   // for size_t

#include "config.h"


// will fail if namespaces aren't supported
namespace std
{
    struct nothrow_t { };
}


// force a failure if namespace std isn't honored
void nothrow_t ();


#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION
#  define throw()
#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION


// try to use ::operator new (size_t, std::nothrow_t)
// and guess whether std::nothrow_t is defined or not
void* operator new (size_t, const std::nothrow_t&) throw ();


int main ()
{
    ::operator new (1, std::nothrow_t ());

    return 0;
}
