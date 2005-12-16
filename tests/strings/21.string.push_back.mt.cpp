/***************************************************************************
 *
 * 21.string.push_back.mt.cpp - test exercising the thread safety
 *                              of the push_back member function
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 *
 **************************************************************************/

#include <string>

#include <cassert>      // for assert
#include <cstdio>       // for puts

#include <driver.h>     // for rw_test
#include <rwthread.h>   // for rw_thread_pool
#include <valcmp.h>     // for rw_equal

/**************************************************************************/

#define MAX_THREADS      36
#define MAX_LOOPS    100000

const char to_append [MAX_THREADS + 1] = {
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
};

const std::string shared0;
const std::string shared1 (64, '_');

/**************************************************************************/

int rw_opt_nthreads = 4;
int rw_opt_nloops   = MAX_LOOPS;

extern "C" {

static void*
thread_func (void *arg)
{
    const rw_thread_t* const pthread = (rw_thread_t*)arg;

    // compute an index unique to this thread
    const std::size_t thr_inx = std::size_t (pthread->threadno) % MAX_THREADS;

    // compute the expected strings unique to this thread
    const std::string expect0 = shared0 + to_append [thr_inx];
    const std::string expect1 = shared1 + to_append [thr_inx];

    // get the data pointer and the length of each string
    // in order to avoid referencin the string objects below
    const char* const expect0_data = expect0.data ();
    const std::size_t expect0_len  = expect0.length ();

    const char* const expect1_data = expect1.data ();
    const std::size_t expect1_len  = expect1.length ();

    for (size_t i = 0; i != std::size_t (rw_opt_nloops); ++i) {

        // create copies of global strings
        std::string local0 (shared0);
        std::string local1 (shared1);

        // modify the local copies
        local0.push_back (to_append [thr_inx]);
        local1.push_back (to_append [thr_inx]);

        // verify that the local copies have the expected length
        assert (expect0_len == local0.length ());
        assert (expect1_len == local1.length ());

        // verify that the local copies have the expected data
        assert (0 == rw_strncmp (expect0_data, local0.data ()));
        assert (0 == rw_strncmp (expect1_data, local1.data ()));
      
        if (60 < rw_opt_nloops && 0 == i % (rw_opt_nloops / 60)) {
            std::putc (to_append [thr_inx], stdout);
            std::fflush (stdout);
        }
    }

    return 0;
}

}   // extern "C"

/**************************************************************************/

static int
run_test (int, char**)
{
    // check that the number of threads is non-negative
    rw_fatal (-1 < rw_opt_nthreads, 0, 0,
              "number of threads must be non-negative. got %d",
              rw_opt_nthreads);

    // check that the number of loops is non-negative
    rw_fatal (-1 < rw_opt_nloops, 0, 0,
              "number of loops must be non-negative. got %d",
              rw_opt_nloops);

    rw_info (0, 0, 0, "running %d thread%{?}s%{;}, %zu iteration%{?}s%{;} each",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops);

    // create and start a pool of threads and wait for them to finish
    const int result = rw_thread_pool (0, std::size_t (rw_opt_nthreads),
                                       0, thread_func, 0);

    std::puts ("");

    rw_assert (0 == shared0.size (), 0, 0,
               "shared empty string modifed: expected \"\", got %{#S}",
               &shared0);

    const std::string expect (64, '_');

    rw_assert (64 == shared1.size () && std::string (64, '_') == shared1,
               0, 0, "shared non-empty string modifed: "
               "expected %{#S}, got %{#S}",
               &expect, &shared1);

    return result;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.push_back",
                    "thread safety", run_test,
                    "|-nloops# "
                    "|-nthreads# ",
                    &rw_opt_nloops,
                    &rw_opt_nthreads);
}
