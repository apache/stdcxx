/***************************************************************************
 *
 * once.h - one time initialization helpers
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
 * Copyright 2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <rw/_defs.h>


#ifdef _RWSTD_REENTRANT
#  ifdef _RWSTD_OS_SUNOS

#    include <pthread.h>
#    include <thread.h>

#    define _RWSTD_THREAD_ONCE(once, func)   pthread_once (once, func)
#    define _RWSTD_THREAD_YIELD()            thr_yield ()
#    define _RWSTD_ONCE_INIT                 PTHREAD_ONCE_INIT

typedef pthread_once_t __rw_once_t;

#  elif defined (_RWSTD_POSIX_THREADS)

#    include <pthread.h>

#    ifndef _RWSTD_NO_PTHREAD_ONCE

typedef pthread_once_t __rw_once_t;

#      define _RWSTD_THREAD_ONCE(once, func)   pthread_once (once, func)
#      define _RWSTD_ONCE_INIT                 PTHREAD_ONCE_INIT

#    else   // if defined (_RWSTD_NO_PTHREAD_ONCE)

_RWSTD_NAMESPACE (__rw) {

struct __rw_once_t {
    pthread_mutex_t _C_mutex;
    int             _C_init;
};

}   // namespace __rw

#      define _RWSTD_ONCE_INIT   { PTHREAD_MUTEX_INITIALIZER, -1 }

#    endif   // _RWSTD_NO_PTHREAD_ONCE

#    ifndef _RWSTD_NO_SCHED_YIELD
#      define _RWSTD_THREAD_YIELD()   sched_yield ()
#    endif   // _RWSTD_NO_SCHED_YIELD
#  elif defined (_RWSTD_DCE_THREADS)

#    if defined (_RWSTD_NO_DCE_PTHREAD_H)
#      include <pthread.h>
#    else
#      include <dce/pthread.h>
#    endif

_RWSTD_NAMESPACE (__rw) {

typedef pthread_once_t __rw_once_t;

}   // namespace __rw

#    define _RWSTD_THREAD_ONCE(once, func)   pthread_once (once, func)
#    define _RWSTD_THREAD_YIELD()            pthread_yield ()
#    define _RWSTD_ONCE_INIT                 PTHREAD_ONCE_INIT

#  elif defined (_WIN32)

#    include <windows.h>

_RWSTD_NAMESPACE (__rw) {

struct __rw_once_t { int _C_init; };

}   // namespace __rw

#    define _RWSTD_THREAD_YIELD()   Sleep (0)
#    define _RWSTD_ONCE_INIT        { 0 }
#  else   // !_WIN32

_RWSTD_NAMESPACE (__rw) {

struct __rw_once_t { int _C_init; };

}   // namespace __rw

#    define _RWSTD_ONCE_INIT   { 0 }
#  endif   // _WIN32
#else   // if !defined (_RWSTD_REENTRANT)

_RWSTD_NAMESPACE (__rw) {

struct __rw_once_t { int _C_init; };

}   // namespace __rw

   // defined to a non-zero value to help detect uninitialized
   // __rw_once_t objects
#  define _RWSTD_ONCE_INIT   { -1 }
#endif   // _RWSTD_REENTRANT


#ifndef _RWSTD_THREAD_YIELD
#  define _RWSTD_THREAD_YIELD()   (void)0
#endif   // _RWSTD_THREAD_YIELD


_RWSTD_NAMESPACE (__rw) {

extern "C" {

#if !defined (_RWSTD_MSVC) || !defined (_RWSTD_REENTRANT)

_RWSTD_EXPORT int
__rw_once (__rw_once_t*, void (*)());

#else   // _RWSTD_MSVC && _RWSTD_REENTRANT

// MSVC by default assumes that functions with C linkage don't
// throw exceptions and issues warning "function assumed not
// to throw an exception but does". Specifying an exception
// specification using the throw(...) extension prevents this
// problem.
// Note: functions with C linkage passed as an argument to
// __rw_once() should also be declared with the appropriate
// exception specification if it throws an exception in order
// to avoid resource leaks due to destructors of objects with
// auto storage duration not being run otherwise.
_RWSTD_EXPORT int
__rw_once (__rw_once_t*, void (*)() throw (...)) throw (...);

#endif   // !_RWSTD_MSVC || !_RWSTD_REENTRANT

}   // extern "C"


}   // namespace __rw
