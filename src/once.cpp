/***************************************************************************
 *
 * once.cpp - one time initialization
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
 * Copyright 2007 Rogue Wave Software.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC
#include <rw/_defs.h>
#include <rw/_mutex.h>

#include "once.h"


_RWSTD_NAMESPACE (__rw) {


extern "C" {


#ifdef _RWSTD_THREAD_ONCE


_RWSTD_EXPORT int
__rw_once (__rw_once_t *once, void (*func)())
{
    return _RWSTD_THREAD_ONCE (once, func);
}


#elif defined (_RWSTD_REENTRANT)


_RWSTD_EXPORT int
__rw_once (__rw_once_t *once, void (*func)())
{
    _RWSTD_ASSERT (0 != once);

    volatile int &init = once->_C_init;

restart:

    if (init == 0 && 1 == _RWSTD_ATOMIC_PREINCREMENT (init, false)) {

        // entered by the first thread and only the first time around,
        // unless the initialization function throws

        _TRY {
            func ();
        }
        _CATCH (...) {
            _RWSTD_ATOMIC_PREDECREMENT (init, false);
            _RETHROW;
        }

        init = 1000;
    }
    else {
        // entered by the second and subsequent threads or on the second
        // and subsequent calls by the first thread after (or duing)
        // a successful initialization 

        for (int loop = 0; init < 1000; ++loop) {
            if (0 == init) {
                // first time initialization failed via an exception,
                // try again
                goto restart;
            }

            if (32 < loop) {
                // avoid wasting too many CPU cycles
                _RWSTD_THREAD_YIELD ();
            }
        }
    }

    return 0;
}


#else   // if !defined (_RWSTD_THREAD_ONCE)


_RWSTD_EXPORT int
__rw_once (__rw_once_t *once, void (*func)())
{
    _RWSTD_ASSERT (0 != once);

    // detect uninitialized __rw_once_t objects to help reveal problems
    // in reentrant code on platforms such as HP-UX that require
    // pthread_once_t objects to be explicitly initialized (i.e., not
    // all bits tobe zeroed out) in order for pthread_once() to succeed
    _RWSTD_ASSERT (-1 == once->_C_init || 1 == once->_C_init);

    if (once->_C_init == -1) {

        func ();

        once->_C_init = 1;
    }

    return 0;
}

#endif   // _RWSTD_THREAD_ONCE

}   // extern "C"

}   // namespace __rw
