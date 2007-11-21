// checking for operator new[] (size_t, void*)

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

#include <stddef.h>

#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION
#  define throw()
#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION

#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE
#    include "terminate.h"
#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


void* operator new[] (size_t, void*) throw ();

void* foo (size_t n, void *p) throw () { return ::operator new[] (n, p); }

void* (*operator_new_array)(size_t, void*);


struct S {
    char c_;
    char d_;

    S (): c_ ('c'), d_ ('d') { }
};


int main (int argc, char *argv[])
{
    (void)&argv;

    int result = 0;

    char buf [sizeof (S) + 32];

    void *p0 = buf;

    void *p1 = ::operator new[] (sizeof (S), p0);

    if (p1 != p0)
        result = result << 1 + 1;

    if (argc > 1)
        operator_new_array = &operator new[];
    else
        operator_new_array = &foo;
        
    p1 = operator_new_array (sizeof (S), p0);

    if (p1 != p0)
        result = (result << 1) + 1;

    S *s = new (p0) S [1];

    if (p1 != p0)
        result = (result << 1) + 1;

    if (s->c_ != 'c' || s->d_ != 'd')
        result = (result << 1) + 1;

    // prevent compilers from optimizing the code above away
    // without actually ever using the result (unless the
    // test is invoked with command line arguments)
    if (argc > 1)
        return result;

    return 0;
}
