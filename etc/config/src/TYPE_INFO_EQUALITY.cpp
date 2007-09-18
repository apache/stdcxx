// checking for type_info::operator==()

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


class type_info
{
public:

    bool operator== (const type_info&) const;

    virtual ~type_info ();
};

#ifdef _RWSTD_NO_TYPE_INFO_DTOR

type_info::~type_info () { }

#endif   // _RWSTD_NO_TYPE_INFO_DTOR

}   // namespace std


struct B { virtual ~B () { } };
struct D: B { };
struct E: B { };
struct F: B { };


int main (int argc, char *argv[])
{
    D d;
    E e;
    F f;

    B *b1 = &d;
    B *b2 = &e;

    if (argc > 3) {
        b1 = &d;
        b2 = &f;
    }
    else if (argc > 2) {
        b1 = &d;
        b2 = &d;
    }

    if (argc > 1)
        return typeid (*b1) == typeid (*b2);

    // link only test 
    return 0;
}
