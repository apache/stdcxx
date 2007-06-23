/***************************************************************************
 *
 * setlocale.h
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_LOC_RW_SETLOCALE_H_INCLUDED
#define _RWSTD_LOC_RW_SETLOCALE_H_INCLUDED


#include "podarray.h"
#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) { 

// class provides synchronized access to the C setlocale() facility
// if the first argument is 0, ctor locks, obtains the name of the
// current C locale (by calling setlocale (0)), and unlocks
// if the first argument is not 0, ctor locks and obtains and saves
// the name of the current C locale, and sets the C locale to that
// named by the first argument; dtor then resets the global C locale
// to what it was before the ctor invocation and unlocks

struct _RWSTD_EXPORT __rw_setlocale
{
    __rw_setlocale (const char* = 0, int = _RWSTD_LC_ALL, int = 0);

    ~__rw_setlocale ();

    // returns the saved name (will not be 0)
    const char* name () const {
        return _C_name;
    }

private:

    __rw_setlocale (const __rw_setlocale&);   // not defined
    void operator= (const __rw_setlocale&);   // not defined

    char  _C_namebuf [80];   // used if locale name fits
    char *_C_name;           // _C_namebuf if name fits, otherwise allocated
    int   _C_guard;          // non-0 when object owns the global lock
    int   _C_cat;            // saved LC_XXX category
};


char* __rw_locale_name (int, const char*, __rw_chararray&);


}   // namespace __rw


#endif   // _RWSTD_LOC_RW_SETLOCALE_H_INCLUDED
