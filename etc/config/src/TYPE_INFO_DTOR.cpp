// checking for type_info dtor

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


#ifndef _RWSTD_NO_STD_TYPE_INFO
#  define NAMESPACE(name)   namespace name
#else
#  ifndef _RWSTD_NO_GLOBAL_TYPE_INFO
#    define NAMESPACE(ignore)   extern "C++"
#    define std                 /* empty */
#  else
#    ifndef _RWSTD_NO_RUNTIME_IN_STD
#      define NAMESPACE(name)   namespace name
#    else
#      define NAMESPACE(ignore)   extern "C++"
#      define std                 /* empty */
#    endif   // _RWSTD_NO_RUNTIME_IN_STD
#  endif   // _RWSTD_NO_GLOBAL_TYPE_INFO
#endif   // _RWSTD_NO_STD_TYPE_INFO


NAMESPACE (std) {


#if defined (__EDG__) || defined (__sgi)
   // let edg (or some of its derivatives) know about this fake type_info
#  if !defined (__GNUG__) && !defined (__INTEL_COMPILER)
#    pragma define_type_info
#  endif
#endif   // __EDG__


struct type_info
{
    virtual ~type_info ();
};


}   // namespace std


struct B0: virtual std::type_info { };
struct B1: virtual std::type_info { };

struct Derived: B0, B1
{
    virtual ~Derived ();
};


int dtor;

int main (int argc, char *argv[])
{
    // avoid executing the body of main unless explicitly requested
    // by specifying at least one command line argument (this foils
    // aggressive optimizers from eliminating the code)
    (void)&argv;
    if (argc < 2)
        return 0;

    // try to prevent the compiler from optimizing the dtor call away
    std::type_info *ptr;

    if (1 < argc)
        ptr = (std::type_info*)argv[2];
    else
        ptr = (Derived*)argv[1];

    // this will most like crash if evaluated but under normal
    // conditions the function should never reach this far
    delete ptr;

    return !(1 < argc ? 1 == dtor : 0 == dtor);
}


Derived::~Derived () { ++dtor; }
