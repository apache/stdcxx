// checking if locale is thread safe

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

// determines whether each thread has its own C locale environment
// that's independent of those of all other threads or whether the
// C locale facility is process global (i.e., shared among all
// threads)


#include <locale.h>         // for LC_ALL, setlocale()
#include <string.h>         // for strcpy()

#include "locale_names.h"   // for test_locale_names
#include "thread.h"         // for pthread_t, pthread_create


static char
thread_locale [3][1024];


extern "C" {

void* thread_routine (void *arg)
{
    const char* const name = setlocale (LC_ALL, 0);

    if (name) {
        const unsigned long inx = (unsigned long)arg;

        strcpy (thread_locale [inx], name);
    }

    return 0;
}

}   // extern "C"

int main ()
{
    const char* locale_name = setlocale (LC_ALL, "C");

    // save the main thread's locale at index 0
    strcpy (thread_locale [0], locale_name);

    pthread_t thread [2];

    // create one thread and have it save its locale name at index 1
    if (pthread_create (thread + 0, 0, thread_routine, (void*)1))
        return 4;

    // find a named locale different from the initial one
    for (unsigned i = 0; i != nlocales; ++i) {
        // set the main thread's locale to one that's different
        // from that of the first thread
        locale_name = setlocale (LC_ALL, test_locale_names [i]);
        if (locale_name && strcmp (locale_name, thread_locale [0]))
            break;
    }

    // create another thread and have it save its locale name at index 2
    if (pthread_create (thread + 1, 0, thread_routine, (void*)2)) {
        return 3;
    }

    // join both (sub)threads
    if (pthread_join (thread [0], 0) || pthread_join (thread [1], 0))
        return 2;

    // return 0 (success) when the (sub)thread's locale is different
    // from the main thread's locale and 1 (failure) when they are
    // the same
    return    0 == strcmp (thread_locale [0], thread_locale [1])
           || 0 == strcmp (thread_locale [0], thread_locale [2]);
}
