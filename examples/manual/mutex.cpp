/**************************************************************************
 *
 * mutex.cpp - Example program for mutex.
 *
 * This program illustraes how to mutexes are used to synchronize access
 * to resources in a multi-threaded program.
 * 
 * NOTE: this program exposes implementation details of the Rogue Wave(R)
 *       C++ Standard library which are subject to change without notice
 *       from one version of the library to another. Portable code should
 *       avoid relying on any implementation details.
 *
 * $Id$
 *
 ***************************************************************************
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <iostream>      // for cout
#include <rw/_mutex.h>   // for __rw::__rw_mutex

#include <examples.h>


#ifdef _RWSTD_REENTRANT

// An integer shared amongst multiple threads.
int shared;

// A mutex used to synchronize updates to shared.
__rw::__rw_mutex shared_mutex;

// Increment shared by one.  Uses a mutex directly.
void increment_shared ()
{
    shared_mutex._C_acquire();  // Lock the mutex.
    ++shared;
    shared_mutex._C_release();  // Unlock the mutex.
}


// Decrement shared by one.  Uses a mutex guard.
void decrement_shared ()
{
    _RWSTD_MT_GUARD (shared_mutex);  // Acquire the lock on shared_mutex.
    --shared;
    // The lock on shared is released when destructor is called on guard.
}


int exchange_shared ()
{
    int new_value = shared;

    new_value *= new_value;

    // atomically assign new_value to shared and return shared's old value
    int old_value = _RWSTD_ATOMIC_SWAP (shared, new_value, shared_mutex);

    return old_value;
}

#endif   // _RWSTD_REENTRANT


int main()
{
#ifdef _RWSTD_REENTRANT

    increment_shared();
    decrement_shared();

    std::cout << "Multi Threading enabled" << std::endl;

#else   // if !defined (_RWSTD_REENTRANT)

    std::cout << "Multi Threading not enabled" << std::endl;

#endif   // _RWSTD_REENTRANT

    return 0;
}
