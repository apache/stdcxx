/************************************************************************
 *
 * environ.cpp - definitions of testsuite helpers
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <environ.h>

#include <assert.h>   // for assert
#include <stdlib.h>   // for getenv, malloc, putenv
#include <string.h>   // for strchr, strlen, ...


extern "C" {

// char** environ;

#ifndef _RWSTD_NO_PUTENV_CONST_CHAR

_RWSTD_DLLIMPORT int putenv (const char*) _LIBC_THROWS ();

#else   // if defined (_RWSTD_NO_PUTENV_CONST_CHAR)

_RWSTD_DLLIMPORT int putenv (char*) _LIBC_THROWS ();

#endif   // _RWSTD_NO_PUTENV_CONST_CHAR


#ifdef _RWSTD_NO_UNSETENV
#  ifndef _RWSTD_NO_UNSETENV_IN_LIBC

_RWSTD_DLLIMPORT int unsetenv (const char*) _LIBC_THROWS ();

#    undef _RWSTD_NO_UNSETENV

#  endif   // _RWSTD_NO_UNSETENV_IN_LIBC
#endif   // _RWSTD_NO_UNSETENV

}   // extern "C"


// sets (or unsets) one or more sep-separated environment variables
_TEST_EXPORT int
rw_putenv (const char* str, int sep /* = -1 */)
{
    int nset = 0;
    int ret  = 0;

    if (!str) {
        str = getenv ("RW_PUTENV");
        if (str)
            sep = *str++;
        else
            return 0;
    }

    // set separator to NUL if it's invalid
    if (sep < 0 || int (_RWSTD_UCHAR_MAX) < sep)
        sep = 0;

    for (const char *pvar = str; pvar && *pvar; ++nset) {

        // look for separator (or the terminating NUL by default)
        const char *pend = strchr (pvar, sep);

        if (0 == pend)
            pend = pvar + strlen (pvar);

        const size_t varlen = pend - pvar;

        char* const envvar = (char*)malloc (pend - pvar + 1);
        if (0 == envvar)
            return -1;

        memcpy (envvar, pvar, varlen);
        envvar [varlen] = '\0';

        // look for the first equals sign
        const char* const equals = strchr (envvar, '=');

        char *var = 0;

        if (equals) {
            // add the variable to the environment or modify it if
            // it's already defined

            // Note: calling Solaris 7 putenv() during program startup
            // (i.e., from ctors of namespace-scope objects) prevents
            // getenv() from finding that variable at program runtime
            ret = putenv (envvar);

            // determine wheteher putenv() made copy of the variable
            // or if it simply used the pointer passed to it; if the
            // former, deallocate the buffer dynamically allocated
            // above

            char namebuf [256];
            assert (size_t (equals - envvar) < sizeof namebuf);

            memcpy (namebuf, envvar, equals - envvar);
            namebuf [equals - envvar] = '\0';

            var = getenv (namebuf);

            if (equals + 1 != var)
                free (envvar);
        }
        else if ((var = getenv (envvar))) {
            // try to remove variable from the environment

#ifndef _RWSTD_NO_UNSETENV
            ret = unsetenv (envvar);
#else
            ret = putenv (envvar);
#endif

            if (0 == ret) {
                // see if the variable has been removed
                var = getenv (envvar);
                if (var) {
                    // if not, zero-out the first byte of its name
                    // FIXME: make this more robust, e.g., by calling
                    // unsetenv() when provided or by manipulating
                    // the environment directly
                    *(var - 1 - varlen) = '\0';

#if 0   // disabled

                    char **penv = environ;
                    if (penv) {
                        while (*penv && *penv != (var - 1 - varlen))
                            ++penv;

                        while (*penv)
                            *penv = penv [1];
                    }

#endif   // 0/1

                }
            }

            free (envvar);
        }

        pvar = pend + !!*pend;
    }

    if (1 == nset)
        return ret;

    return nset;
}
