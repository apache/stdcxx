/************************************************************************
 *
 * system.cpp - definitions of testsuite helpers
 *
 * $Id:$
 *
 ************************************************************************
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

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <system.h>

#include <driver.h>
#include <printf.h>

#include <stdarg.h>   // for va_copy, va_list, ...
#include <stdlib.h>   // for system

#if !defined  (_WIN32) && !defined (_WIN64)
#  include <unistd.h>
#  include <sys/wait.h>   // for WIFEXITED(), WIFSIGNALED(), WTERMSIG()
#endif

/**************************************************************************/

_TEST_EXPORT int
rw_vasnprintf (char**, size_t*, const char*, va_list);

/**************************************************************************/

static int
_rw_vsystem (const char *cmd, va_list va)
{
    _RWSTD_ASSERT (0 != cmd);

    char buffer [256];
    char *buf = buffer;

    size_t bufsize = sizeof buffer;

    rw_vasnprintf (&buf, &bufsize, cmd, va);

    rw_note (0, __FILE__, __LINE__,
             "executing \"%s\"", buf);

    const int ret = system (buf);

    if (ret) {

#if !defined (_WIN32)

        if (-1 == ret) {
            // system() failed, e.g., because fork() failed
            rw_error (0, __FILE__, __LINE__,
                      "system (\"%s\") failed: errno = %{#m} (%{m})", buf);
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
#else   // if defined (_WIN32)

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
