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


// implementation that relies on the system one-time initialization
// mechanism such as pthread_once()
_RWSTD_EXPORT int
__rw_once (__rw_once_t *once, void (*func)())
{
    _RWSTD_ASSERT (0 != once && 0 != func);

    return _RWSTD_THREAD_ONCE (once, func);
}


#elif defined (_RWSTD_NO_ATOMIC_OPS) && defined (_RWSTD_POSIX_THREADS)


// implementation that uses a mutex instead of pthread_once() or atomic
// operations
_RWSTD_EXPORT int
__rw_once (__rw_once_t *once, void (*func)())
{
    _RWSTD_ASSERT (0 != once && 0 != func);

    // _C_init may take on one of two valid values:
    // -1 for a properly initialized __rw_once_t object whose initializer
    //    hasn't run yet
    // +1 for ar __rw_once_t object whose initializer has already been
    //    executed
    // Any other value (including 0, for __rw_once_t objects that haven't
    // been properly initialized) is invalid.

    if (-1 == once->_C_init) {

        const int result = pthread_mutex_lock (&once->_C_mutex);
        if (result)
            return result;

        if (-1 == once->_C_init) {

            // entered by the first thread and only the first time around,
            // unless the initialization function throws

            _TRY {
                func ();
            }
            _CATCH (...) {
                pthread_mutex_unlock (&once->_C_mutex);
                _RETHROW;
            }

            once->_C_init += 2;
        }

        pthread_mutex_unlock (&once->_C_mutex);
    }

    // verify that initialization took place exactly once and help detect
    // uninitialized __rw_once_t objects to help catch problems on platforms
    // such as HP-UX that require pthread_once_t objects to be explicitly
    // initialized (i.e., not all bits tobe zeroed out) in order for
    // pthread_once() to succeed
    _RWSTD_ASSERT (1 == once->_C_init);

    return 0;
}


#elif defined (_RWSTD_REENTRANT)


// implementation that uses atomic operations
#ifndef _RWSTD_MSVC
_RWSTD_EXPORT int
__rw_once (__rw_once_t *once, void (*func)())
#else
_RWSTD_EXPORT int
__rw_once (__rw_once_t *once, void (*func)() throw (...)) throw (...)
#endif
{
    _RWSTD_ASSERT (0 != once && 0 != func);

    volatile int &init = once->_C_init;

restart:

    // cast init to int& (see STDCXX-792)
    // casting should be removed after fixing STDCXX-794
    if (init == 0 && 1 == _RWSTD_ATOMIC_PREINCREMENT (
            _RWSTD_CONST_CAST (int&, init), false)) {

        // entered by the first thread and only the first time around,
        // unless the initialization function throws

        _TRY {
            func ();
        }
        _CATCH (...) {
            _RWSTD_ATOMIC_PREDECREMENT (
                _RWSTD_CONST_CAST (int&, init), false);
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


// thread-unsafe implementation
_RWSTD_EXPORT int
__rw_once (__rw_once_t *once, void (*func)())
{
    _RWSTD_ASSERT (0 != once && 0 != func);

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
