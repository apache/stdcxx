
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

extern "C"
{
    typedef void (*funptr_t)();
}

// take the address of the function in a way
// that foils even clever optimizers

union {
    funptr_t      pf;
    unsigned long ul;
} funptri;

#ifdef CHECK_DECL

#  if defined (__linux__) && defined (__GNUG__) && __GNUG__ < 3
     // prevent empty exception specifications from triggering
     // gcc 2.9x -fhonor-std link errors looking for std::terminate()
#    include <features.h>
#    undef __THROW
#    define __THROW
#  endif   // gcc < 3 on Linux

#  include HDRNAME

#  ifndef NONAMESPACE

namespace std { }
using namespace std;

#  endif   // NONAMESPACE

int main (int argc, char**)
{
    // with gcc, prevent intrinsics from causing false positives

#  if TAKE_ADDR

    funptri.pf = (funptr_t)&FUNNAME;

    return (argc < 1 ? !funptri.ul : !!funptri.ul) + 0;
    
#  else   // if !TAKE_ADDR

    // disable warnings about an unused variable
    (void)&argc;

    // call the function using the supplied arguments
    #ifdef FUN_PARAMS
        FUN_PARAMS;
    #endif
    (void)FUN;

    return 0;

#  endif   // TAKE_ADDR

}

#else   // if !defined (CHECK_DECL)

extern "C" void FUNNAME ();

int main (int argc, char**)
{
    funptri.pf = (funptr_t)&FUNNAME;

    return (argc < 1 ? !funptri.ul : !!funptri.ul) + 0;
}

#endif   // CHECK_DECL


