/***************************************************************************
 *
 * locale_classic.cpp - definition of std::locale::classic()
 *
 * $Id$
 *
 ***************************************************************************
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
 * Copyright 2001-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC
#include <rw/_defs.h>

#include <new>             // for placment new

#include <loc/_locale.h>   // for locale
#include "once.h"          // for __rw_once()


_RWSTD_NAMESPACE (__rw) { 

// static buffer for the classic "C" locale object
static union {
    void* _C_align;
    char  _C_buf [sizeof (_STD::locale)];
} __rw_classic;


// init-once flag for the classic "C" locale object
static __rw_once_t
__rw_classic_once_init = _RWSTD_ONCE_INIT;


extern "C" {

// one-time initializer for the classic "C" locale object
static void
__rw_init_classic ()
{
#ifdef _RWSTDDEBUG

    static int init;

    // paranoid check: verify that one-time initialization works
    _RWSTD_ASSERT (0 == init);

    ++init;

#endif   // _RWSTDDEBUG

    // construct the classic "C" locale in the provided buffer
    new (&__rw_classic) _STD::locale ("C");
}

}   // extern "C"

}   // namespace __rw


_RWSTD_NAMESPACE (std) {


/* static */ const locale& locale::
classic ()
{
    // initialize classic locale in the static buffer exactly once
    _RW::__rw_once (&_RW::__rw_classic_once_init, _RW::__rw_init_classic);

    // cast the address of the buffer to a locale pointer
    const locale* const pclassic =
        _RWSTD_REINTERPRET_CAST (locale*, &_RW::__rw_classic);

    _RWSTD_ASSERT (0 != pclassic->_C_body);

    return *pclassic;
}


}   // namespace std
