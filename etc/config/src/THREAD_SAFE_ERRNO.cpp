// checking if errno is thread safe

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

#include <errno.h>
#include "thread.h"


void *errno_addr = 0;


extern "C" void* thread_routine (void*)
{
    errno_addr = &errno;
    return 0;
}


int main ()
{
    pthread_t thread;

    int err;

    // create a thread that gets the address of errno
    if ((err = pthread_create (&thread, 0, thread_routine, 0))) {
        return 3;
    }

    if ((err = pthread_join (thread, 0))) {
        return 2;
    }

    // compare the address of the (presumably) thread-local
    // errno variable against that of the global errno and
    // return 0 if they are different, 1 otherwise
    return !(&errno != errno_addr);
}
