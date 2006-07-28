/************************************************************************
 *
 * rw_process.h - declarations of testsuite process helpers
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
 **************************************************************************/

#ifndef RW_PROCESS_H_INCLUDED
#define RW_PROCESS_H_INCLUDED


#include <testdefs.h>


typedef long rw_pid_t;

_TEST_EXPORT int
rw_system (const char*, ...);

// returns pid of the created process or -1 on error
// (in which case errno is set to an appropriate value)
_TEST_EXPORT rw_pid_t
rw_process_create (const char*, ...);

// note: argv[0] should be equal to path
// returns pid of the created process or -1 on error
// (in which case errno is set to an appropriate value)
_TEST_EXPORT rw_pid_t
rw_process_create (const char*, char* const []);

// result is a pointer to a buffer where the result code
// of the specified process will be stored, or NULL
// returns pid of the specified process or -1 on error
// (in which case errno is set to an appropriate value)
// Errors:
//   ECHILD: no specified process exists
_TEST_EXPORT rw_pid_t
rw_waitpid (rw_pid_t, int*);

#endif   // RW_PROCESS_H_INCLUDED
