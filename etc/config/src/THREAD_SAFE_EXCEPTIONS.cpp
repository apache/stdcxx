// checking if exceptions are thread safe

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

#include "thread.h"
#include "nodbg.h"


extern "C" void* thread_proc (void *arg)
{
    long sum = 0;
    long n   = (char*)arg - (char*)0;

    for (long i = 0; i != 0x20000; ++i) {
        try {
            if (i % 2 == n)
                throw i;
            else
                throw arg;
        }
        catch (long) {
            ++sum;
        }
        catch (...) {
            sum += 2;
        }
    }

    return (void*)sum;
}


int main ()
{
    nodbg ();

    pthread_t tid [2];

    const unsigned N = sizeof tid / sizeof *tid;

    set_concurrency (N);

    unsigned i;

    for (i = 0; i != N; ++i)
        if (pthread_create (tid + i, 0, thread_proc, (void*)i))
            return 1;

    for (i = 0; i != N; ++i)
        pthread_join (tid [i], 0);

    return 0;
}
