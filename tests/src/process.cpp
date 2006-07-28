/************************************************************************
 *
 * process.cpp - definitions of testsuite process helpers
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

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <rw_process.h>

#include <stddef.h>
#include <stdarg.h>       // for va_copy, va_list, ...
#include <stdlib.h>       // for free(), exit()
#include <string.h>       // for strchr()
#include <ctype.h>        // for isspace()

#include <errno.h>        // for errno

#include <driver.h>       // for rw_note(), ...
#include <rw_printf.h>    // for rw_fprintf()

#ifndef ENOMEM
#  define ENOMEM 12       // e.g., Linux, Solaris
#endif

/**************************************************************************/

_TEST_EXPORT int
rw_vasnprintf (char**, size_t*, const char*, va_list);

/**************************************************************************/

#if defined (_WIN32) || defined (_WIN64)
#  include <process.h>      // for spawnvp(), cwait()
#else
#  include <sys/types.h>
#  include <sys/wait.h>   // for waitpid()
#  include <unistd.h>     // for fork(), execv(), access()
#endif   // _WIN{32,64}

/**************************************************************************/

static int
_rw_vsystem (const char *cmd, va_list va)
{
    RW_ASSERT (0 != cmd);

    char buffer [256];
    char *buf = buffer;

    size_t bufsize = sizeof buffer;

    rw_vasnprintf (&buf, &bufsize, cmd, va);

    rw_note (0, "file:" __FILE__, __LINE__, "executing \"%s\"", buf);

    // avoid using const in order to prevent gcc warning on Linux
    // issued for WIFSIGNALED() et al: cast from `const int*' to
    // `int*' discards qualifiers from pointer target type:
    // see http://sourceware.org/bugzilla/show_bug.cgi?id=1392
    /* const */ int ret = system (buf);

    if (ret) {

#if !defined (_WIN32) && !defined (_WIN64)

        if (-1 == ret) {
            // system() failed, e.g., because fork() failed
            rw_error (0, __FILE__, __LINE__,
                      "system (\"%s\") failed: errno = %{#m} (%{m})",
                      buf);
        }
        else if (WIFSIGNALED (ret)) {
            // command exited with a signal
            const int signo = WTERMSIG (ret);

            rw_error (0, __FILE__, __LINE__,
                      "the command \"%s\" exited with signal %d (%{K})",
                      buf, signo, signo);
        }
        else {
            // command exited with a non-zero status
            const int status = WEXITSTATUS (ret);

            rw_error (0, __FILE__, __LINE__,
                      "the command \"%s\" exited with status %d",
                      buf, status);
        }
#else   // if defined (_WIN32) || defined (_WIN64)

        // FIXME: make this more descriptive
        rw_error (0, __FILE__, __LINE__,
                  "the command \"%s\" failed with code %d",
                  buf, ret);

#endif   // _WIN32

    }

    if (buf != buffer)
        free (buf);

    return ret;
}

/**************************************************************************/

_TEST_EXPORT int
rw_system (const char *cmd, ...)
{
    va_list va;
    va_start (va, cmd);

    const int ret = _rw_vsystem (cmd, va);

    va_end (va);
    return ret;
}

/**************************************************************************/

// splits command line to the array of parameters
// note: modifies the cmd string
// returns the number of parameters in cmd
// if argv != 0 fills argv up to size elements
static size_t
_rw_split_cmd (char* cmd, char** argv, size_t size)
{
    RW_ASSERT (0 != cmd);

    size_t ret = 0;

    for (char* end = cmd + strlen (cmd); cmd != end; /*do nothing*/) {
        // skip the leading spaces
        while (isspace (*cmd))
            ++cmd;

        if (end == cmd)
            break;

        if (argv && ret < size)
            argv [ret] = cmd;

        ++ret;

        if ('\'' == *cmd || '\"' == *cmd) {
            char* const cmd1 = cmd + 1;
            // search the closing quote
            if (char* pos = strchr (cmd1, *cmd)) {
                // found, remove the quotes
                // remove the opening quote
                memmove (cmd, cmd1, pos - cmd1);
                // remove the closing quote
                cmd = pos - 1;
                memmove (cmd, pos + 1, end - pos);
                end -= 2;
            } else {
                // not found
                break;
            }
        }

        // search the space
        while (*cmd && !isspace (*cmd))
            ++cmd;

        if (cmd != end)
            // found, replace to '\0'
            *cmd++ = '\0';
    }

    return ret;
}

/**************************************************************************/

static rw_pid_t
_rw_vprocess_create (const char* cmd, va_list va)
{
    RW_ASSERT (0 != cmd);

    char buffer [256];
    char *buf = buffer;

    size_t bufsize = sizeof (buffer);

    rw_vasnprintf (&buf, &bufsize, cmd, va);

    const size_t MAX_PARAMS = 63;
    char* argv [MAX_PARAMS + 1] = { 0 };

    int ret;

    size_t argc = _rw_split_cmd (buf, argv, MAX_PARAMS);

    if (0 < argc && MAX_PARAMS >= argc)
        ret = rw_process_create (argv [0], argv);
    else {
        ret = -1;
        errno = ENOMEM;
    }

    if (buf != buffer)
        free (buf);

    return ret;
}

/**************************************************************************/

_TEST_EXPORT rw_pid_t
rw_process_create (const char* cmd, ...)
{
    va_list va;
    va_start (va, cmd);

    const int ret = _rw_vprocess_create (cmd, va);

    va_end (va);
    return ret;
}

/**************************************************************************/

_TEST_EXPORT rw_pid_t
rw_process_create (const char* path, char* const argv[])
{
#if defined (_WIN32) || defined (_WIN64)

    const rw_pid_t child_pid = spawnvp (P_NOWAIT, path, argv);

    if (-1 == child_pid)
        rw_error (0, __FILE__, __LINE__,
                  "spawnp (P_NOWAIT, %#s, %{As}) failed: errno = %{#m} (%{m})",
                  path);

#else

    rw_pid_t child_pid = -1;

    const int res = access (path, X_OK);

    if (0 == res) {

        child_pid = fork ();

        if (0 == child_pid) {
            // the child process
            execvp (path, argv);

            // the execvp returns only if an error occurs
            rw_fprintf (rw_stderr, "%s:%d execvp() failed: "
                        "errno = %{#m} (%{m})\n");

            exit (1);
        }
        else if (-1 == child_pid)
            rw_error (0, __FILE__, __LINE__,
                      "fork() failed: errno = %{#m} (%{m})");
    }
    else
        rw_error (0, __FILE__, __LINE__,
                  "access(%#s, X_OK) failed: errno = %{#m} (%{m})",
                  path);

#endif  // #if defined (_WIN32) || defined (_WIN64)

    return child_pid;
}

/**************************************************************************/

_TEST_EXPORT rw_pid_t
rw_waitpid (rw_pid_t pid, int* result)
{
#if defined (_WIN32) || defined (_WIN64)

    const rw_pid_t ret = cwait (result, pid, WAIT_CHILD);

    if (-1 == ret)
        rw_error (0, __FILE__, __LINE__,
                  "cwait(%#p, %li, WAIT_CHILD) failed: errno = %{#m} (%{m})",
                  result, pid);

#else

    int status = 0;
    const rw_pid_t ret = waitpid (pid, &status, 0);

    if (ret == pid) {
    
        if (WIFSIGNALED (status)) {
            // process exited with a signal
            const int signo = WTERMSIG (status);

            rw_error (0, __FILE__, __LINE__,
                      "the process (pid=%ld) exited with signal %d (%{K})",
                      long (ret), signo, signo);

            if (result)
                *result = signo;
        }
        else if (WIFEXITED (status)) {
            // process exited with a status
            const int retcode = WEXITSTATUS (status);

            if (retcode)
                rw_error (0, __FILE__, __LINE__,
                          "the process (pid=%ld) exited with return code %d",
                          long (ret), retcode);

            if (result)
                *result = retcode;
        }
        else if (result)
            *result = -1;
    }

    if (-1 == ret)
        rw_error (0, __FILE__, __LINE__,
                  "waitpid(%li, %#p, 0) failed: errno = %{#m} (%{m})",
                  pid, &status);

#endif  // #if defined (_WIN32) || defined (_WIN64)

    return ret;
}
