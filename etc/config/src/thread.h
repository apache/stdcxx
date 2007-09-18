
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

#if defined (_WIN32)

// definitions of Win32 POSIX compatibility layer functions

#include <windows.h>

extern "C" {

struct pthread_attr_t;

struct pthread_t
{
    long  id;
    void* handle;
};

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void*), void *arg)
{
    int result = 0;

    pthread_t tmpid;

    if (0 == thread)
        thread = &tmpid;

    DWORD nid;

    const HANDLE hthread = CreateThread (0, 0,
        (LPTHREAD_START_ROUTINE)start_routine,
        arg, 0, &nid);

    if (INVALID_HANDLE_VALUE == hthread) {
        thread->id     = -1;
        thread->handle = 0;
        result         = -1;
    }
    else {
        thread->id     = nid;
        thread->handle = hthread;
    }

    return result;
}

int pthread_join(pthread_t thread, void **value_ptr)
{
    int result = 0;

    const DWORD retcode = WaitForSingleObject (thread.handle, INFINITE);

    if (WAIT_OBJECT_0 == retcode) {
        if (value_ptr) {
            DWORD exit_code;

            if (GetExitCodeThread (thread.handle, &exit_code))
                *value_ptr = (void*)exit_code;
            else
                result = -1;
        }
    }
    else
        result = -1;

    return result;
}

}   // extern "C"

#  define set_concurrency(ignore)   0
#else   // if !defined (_WIN32)
#  include <pthread.h>

#  if defined (__sun) && defined (__SVR4)
     // use the Solaris threads API in case this is Solaris 2.6
     // where the POSIX pthread_setconcurrency() API is missing
#    include <thread.h>
#    define set_concurrency(n)   thr_setconcurrency (n)
#  else
#    define set_concurrency(n)   pthread_setconcurrency (n)
#  endif   // __sun && __SVR4
#endif   // _WIN32
