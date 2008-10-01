/***************************************************************************
 *
 * _mutex-solaris.h - definitions of classes for thread safety
 *                    using Solaris threads
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

// SunOS 5.7 Threads Library:
//   "A statically  allocated  mutex does  not  need to be explicitly
//   initialized; by default, a statically allocated mutex is initialized
//   with  all  zeros and its scope is set to be within the calling
//   process."

#include <synch.h>
#include <thread.h>

#define _RWSTD_MUTEX_INIT(mutex)      mutex_init (&mutex, USYNC_THREAD, 0)
#define _RWSTD_MUTEX_DESTROY(mutex)   mutex_destroy (&mutex)
#define _RWSTD_MUTEX_LOCK(mutex)      mutex_lock (&mutex)
#define _RWSTD_MUTEX_UNLOCK(mutex)    mutex_unlock (&mutex)
#define _RWSTD_MUTEX_T                mutex_t
