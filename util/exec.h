/************************************************************************
 *
 * exec.h - Interface declaration for the child process subsystem
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

#ifndef RW_EXEC_H
#define RW_EXEC_H

#include "target.h" /* For struct target_opts */

/**
   Translates a human understandable signal name into a number usable by kill ().

   This method understands several formats for signal names.  They are as follows:
   - n
   - SIGFOO
   - FOO
   In this list, n denotes a number and FOO denotes a short signal name.

   @param signame a signal name to decode
   @returns the signal number or -1 if a number couldn't be determined
   @see signal_names []
*/
int get_signo (const char* signame);

/**
   Translates a signal number into a human understandable name

   @param signo a signal number
   @returns the human understandable name for the signal (minus the SIG 
   prefix), or "#n" if the the name for the number is unknown to the 
   function, where n is signo
   @see signal_names []
*/
const char* get_signame (int signo);

/**
   Entry point to the child process (watchdog) subsystem.

   On UNIX systems, this method fork ()s, creating a child process.  This 
   child process becomes a process group leader, redirects input and output 
   files, then exec ()s the target specified in argv [0], providing it with 
   argv to use as its argv array.  Meanwhile, the parent process calls the 
   wait_for_child method to monitor the child process, passing the return 
   value back to the calling method.

   On Windows, this method creates a process using the windows CreateProcess 
   API call. The startup context for this process is based on the context of 
   the exec utility, but with the standard * file handles redirected.  The 
   execution of the process is monitored with the WaitForSingleObject API 
   call.  If the process doesn't complete within the allowed timeout, it is 
   then killed.  On Windows NT systems, a soft kill of the process (via the 
   GenerateConsoleCtrlEvent API call) are first attempted attempted.  The 
   process is then hard killed via the TerminateProcess API call.

   @param exec_name name of the child executable
   @param argv argv array for child process
   @return structure describing how the child procees exited
   @see wait_for_child ()
*/
void exec_file (const struct target_opts* options, struct target_status* result);

#endif   /* RW_EXEC_H */
