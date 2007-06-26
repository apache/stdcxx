/************************************************************************
 *
 * thread.cpp - definitions of testsuite thread helpers
 *
 * $Id$
 *
 ************************************************************************
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
 * Copyright 2005-2006 Rogue Wave Software.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <rw_thread.h>
#include <stddef.h>     // for size_t
#include <string.h>     // for memset()

/**************************************************************************/

static long maxthreads;


#if defined (_RWSTD_POSIX_THREADS)
#  include <pthread.h>

extern "C" {

_TEST_EXPORT int
rw_thread_create (rw_thread_t *thr_id,
                  rw_thread_attr_t*,
                  void* (*thr_proc)(void*),
                  void *thr_arg)
{
#ifdef _RWSTD_OS_SUNOS

    static int concurrency_set;

    if (0 == concurrency_set) {
        pthread_setconcurrency (4);
        concurrency_set = 1;
    }

#endif   // _RWSTD_OS_SUNOS


    rw_thread_t tmpid;

    if (0 == thr_id) {
        thr_id = &tmpid;
    }

    pthread_t tid;

    // set the thread number *before* creating the thread
    // so that it's visible in thr_proc when it starts to
    // run even before pthread_create returns
    thr_id->threadno = maxthreads;

    const int result = pthread_create (&tid, 0, thr_proc, thr_arg);

    if (0 == result) {
        thr_id->id     = (long)tid;
        thr_id->handle = 0;
        ++maxthreads;
    }

    return result;
}


_TEST_EXPORT int
rw_thread_join (rw_thread_t thr_id, void **parg)
{
    const int result = pthread_join ((pthread_t)thr_id.id, parg);

    return result;
}

}   // extern "C"

/**************************************************************************/

#elif defined (_RWSTD_SOLARIS_THREADS)
#  include <thread.h>

extern "C" {

_TEST_EXPORT int
rw_thread_create (rw_thread_t *thr_id,
                  rw_thread_attr_t*,
                  void* (*thr_proc)(void*),
                  void *thr_arg)
{
    static int concurrency_set;

    if (0 == concurrency_set) {
        thr_setconcurrency (4);
        concurrency_set = 1;
    }

    rw_thread_t tmpid;

    if (0 == thr_id) {
        thr_id = &tmpid;
    }

    thread_t tid;

    // set the thread number *before* creating the thread
    // so that it's visible in thr_proc when it starts to
    // run even before thr_create returns
    thr_id->threadno = maxthreads;

    const int result =
        thr_create (0,          // stack_base
                    0,          // stack_size
                    thr_proc,   // start_func
                    thr_arg,    // arg
                    0,          // flags
                    &tid);      // new_thread_ID

    if (0 == result) {
        thr_id->id     = (long)tid;
        thr_id->handle = 0;
        ++maxthreads;
    }

    return result;
}


_TEST_EXPORT int
rw_thread_join (rw_thread_t thr_id, void **parg)
{
    const int result = thr_join ((thread_t)thr_id.id, 0, parg);

    return result;
}

}   // extern "C"

/**************************************************************************/

#elif defined (_RWSTD_DEC_THREADS)

#  include <setjmp.h>
#  include <cma.h>


extern "C" {

#  ifdef __PURE_CNAME

extern int sigsetjmp (jmp_buf, int);

#  endif


_TEST_EXPORT int
rw_thread_create (rw_thread_t *thr_id,
                  rw_thread_attr_t*,
                  void* (*thr_proc)(void*),
                  void *thr_arg)
{
    rw_thread_t tmpid;

    if (0 == thr_id) {
        thr_id = &tmpid;
    }

    int result = 0;

    cma_t_thread tid;

    // set the thread number *before* creating the thread
    // so that it's visible in thr_proc when it starts to
    // run even before cma_thread_create returns
    thr_id->threadno = maxthreads;

    TRY {
        // cma_thread_create() returns void but throws an exception on error
        cma_thread_create (&tid,        // new_thread
                           0,           // attr
                           thr_proc,    // start_routine
                           &thr_arg);   // arg

        thr_id->id     = tid.field1;
        thr_id->handle = (void*)tid.field2;
        ++maxthreads;
    }
    CATCH_ALL {
        result = -1;
    }
    ENDTRY

    return result;
}


_TEST_EXPORT int
rw_thread_join (rw_thread_t thr_id, void **parg)
{
    int status = 0;

    cma_t_thread tid = {
        thr_id.id, (long)thr_id.handle
    };

    TRY {
        // cma_thread_join() returns void but throws an exception on error
        cma_thread_join (&tid, 0, parg);
    }
    CATCH_ALL {
        status = -1;
    }
    ENDTRY

    return status;
}

}   // extern "C"

/**************************************************************************/

#elif defined (_WIN32) || defined (_WIN64)
#  include <windows.h>

extern "C" {

_TEST_EXPORT int
rw_thread_create (rw_thread_t *thr_id,
                  rw_thread_attr_t*,
                  void* (*thr_proc)(void*),
                  void *thr_arg)
{
    int result = 0;

    rw_thread_t tmpid;

    if (0 == thr_id)
        thr_id = &tmpid;

    DWORD nid;   // numerical id

    typedef DWORD ThreadProc (LPVOID);

    LPTHREAD_START_ROUTINE win32_thr_proc =
        _RWSTD_REINTERPRET_CAST (LPTHREAD_START_ROUTINE, thr_proc);

    // set the thread number *before* creating the thread
    // so that it's visible in thr_proc when it starts to
    // run even before CreateThread returns
    thr_id->threadno = maxthreads;

    const HANDLE hthread =
        CreateThread (0,                // lpThreadAttributes
                      0,                // dwStackSize
                      win32_thr_proc,   // lpStartAddress
                      thr_arg,          // lpParameter
                      0,                // dwCreationFlags
                      &nid);            // lpThreadId

    if (INVALID_HANDLE_VALUE == hthread) {
        thr_id->id     = -1;
        thr_id->handle = 0;
        result         = -1;
    }
    else {
        thr_id->id     = nid;
        thr_id->handle = hthread;
        ++maxthreads;
    }

    return result;
}


_TEST_EXPORT int
rw_thread_join (rw_thread_t thr_id, void **parg)
{
    int result = 0;

    const DWORD retcode = WaitForSingleObject (thr_id.handle, INFINITE);

    if (WAIT_OBJECT_0 == retcode) {
        if (parg) {
            DWORD exit_code;

            if (GetExitCodeThread (thr_id.handle, &exit_code))
                *parg = (void*)exit_code;
            else
                result = -1;
        }
    }
    else {
        result = -1;
    }

    return result;
}

}   // extern "C"

/**************************************************************************/

#else   // unknown/missing threads environment

#  include <errno.h>

#  ifndef ENOTSUP
#    if defined (_RWSTD_OS_AIX)
#      define ENOTSUP    124
#    elif defined (_RWSTD_OS_HP_UX)
#      define ENOTSUP    252
#    elif defined (_RWSTD_OS_IRIX64)
#      define ENOTSUP   1008
#    elif defined (_RWSTD_OS_LINUX)
#      define ENOTSUP    524
#    elif defined (_RWSTD_OS_OSF1)
#      define ENOTSUP     99
#    elif defined (_RWSTD_OS_SUNOS)
#      define ENOTSUP     48
#    elif defined (_WIN32) || defined (_WIN64)
#      define ENOTSUP ENOSYS
#    else
#      define ENOTSUP   9999
#    endif
#  endif   // ENOTSUP

extern "C" {

_TEST_EXPORT int
rw_thread_create (rw_thread_t*,
                  rw_thread_attr_t*,
                  void* (*)(void*),
                  void*)
{
    _RWSTD_UNUSED (maxthreads);

    return ENOTSUP;
}


_TEST_EXPORT int
rw_thread_join (rw_thread_t, void**)
{
    return ENOTSUP;
}

}   // extern "C"

#endif   // threads environment


extern "C" {


_TEST_EXPORT int
rw_thread_pool (rw_thread_t *thr_id, size_t nthrs,
                rw_thread_attr_t*,
                void* (*thr_proc)(void*),
                void **thr_arg)
{
    // small buffer for thread ids when invoked with (thr_id == 0)
    rw_thread_t id_buf [16];

    const bool join = 0 == thr_id;

#ifndef _RWSTD_REENTRANT

    // when not reentrant/thread safe emulate the creation
    // of a single thread and then waiting for it to finish
    // by simply calling the thread procedure

    if (1 == nthrs && join) {

        if (0 == thr_id) {
            thr_id = id_buf;
            memset (thr_id, 0, sizeof *thr_id);
        }

        // when the thr_arg pointer is 0 pass the address
        // of each thread's id as the argument to thr_proc
        void* const arg = thr_arg ? thr_arg [0] : (void*)(thr_id);

        void* const thr_result = thr_proc (arg);

        if (thr_arg)
            thr_arg [0] = thr_result;

        return 0;
    }

#endif   // !_RWSTD_REENTRANT

    bool delete_ids = false;

    if (0 == thr_id) {
        // save thread idsso that they (and no other threads)
        // can be joined later
        if (sizeof id_buf / sizeof *id_buf < nthrs) {
            delete_ids = true;
            thr_id     = new rw_thread_t [nthrs];
        }
        else
            thr_id = id_buf;
    }

    // create a pool of threads storing their id's
    // in consecutive elements of the thr_id array
    for (size_t i = 0; i != nthrs; ++i) {

        // when the thr_arg pointer is 0 pass the address
        // of each thread's id as the argument to thr_proc
        void* const next_arg = thr_arg ? thr_arg [i] : (void*)(thr_id + i);

        if (rw_thread_create (thr_id + i, 0, thr_proc, next_arg)) {
            if (delete_ids)
                delete[] thr_id;

            return int (i + 1);
        }
    }

    // invoking the function with a 0 thr_id pointer
    // is a request to join all threads in the pool
    if (join) {
        for (size_t i = 0; i != nthrs; ++i) {

            // avoid advancing through the thr_arg array
            // when it's 0 (and have rw_thread_join() simply
            // ignore the thread's return value)
            void** next_arg = thr_arg ? thr_arg + i : 0;

            rw_thread_join (thr_id [i], next_arg);
        }

        if (delete_ids)
            delete[] thr_id;
    }

    return 0;
}

}   // extern "C"
