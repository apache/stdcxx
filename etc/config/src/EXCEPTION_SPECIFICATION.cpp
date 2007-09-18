// checking exception specification

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

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    abort ();
}

}   // std

#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD

extern "C" {

void handle_ABRT (int)
{
    // exit successfully
    exit (0);
}

}   // extern "C"

// test functionality; 
struct X {};
struct Y {};

void foobar () { throw Y (); }
void foo () throw (X) { foobar (); }

// test syntax
void bar (int) throw (int) {}

struct A { };
struct B
{
     B () throw (A*) { }
    ~B () throw ()   { }
};

int main ()
{
    signal (SIGABRT, handle_ABRT);

    try {
        // must not return
        foo ();
    }
    catch (X) {
        return 1;
    }
    catch (Y) {
        return 2;
    }
    catch (...) {
        return 3;
    }

    return 4;
}
