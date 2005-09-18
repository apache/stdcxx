/************************************************************************
 *
 * environ.cpp - definitions of testsuite helpers
 *
 * $Id$
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

#include <environ.h>

#include <assert.h>   // for assert
#include <stdlib.h>   // for getenv, malloc, putenv
#include <string.h>   // for strchr, strlen, ...


extern "C" {

#ifndef _RWSTD_NO_PUTENV_CONST_CHAR

_RWSTD_DLLIMPORT int putenv (const char*) _LIBC_THROWS ();

#else   // if defined (_RWSTD_NO_PUTENV_CONST_CHAR)

_RWSTD_DLLIMPORT int putenv (char*) _LIBC_THROWS ();

#endif   // _RWSTD_NO_PUTENV_CONST_CHAR

}   // extern "C"


// sets one or more sep-separated environment variables
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

    for (const char *pvar = str; pvar && *pvar; ++nset) {

        const char *pend = strchr (pvar, sep);
        if (!pend)
            pend = pvar + strlen (pvar);

        const size_t varlen = pend - pvar;

        char* const envvar = (char*)malloc (pend - pvar + 1);
        memcpy (envvar, pvar, varlen);
        envvar [varlen] = '\0';

        // Note: calling Solaris 7 putenv() during program startup
        // (i.e., from ctors of namespace-scope objects) prevents
        // getenv() from finding that variable at program runtime
        ret = putenv (envvar);

        // determine wheteher putenv() made copy of the variable
        // or if it simply used the pointer passed to it; if the
        // former, deallocate the buffer dynamically allocated
        // above

        char namebuf [256];
        char* const equals = strchr (envvar, '=');
        if (equals) {
            assert (size_t (equals - envvar) < sizeof namebuf);

            memcpy (namebuf, envvar, equals - envvar);
            namebuf [equals - envvar] = '\0';

            const char* const var = getenv (namebuf);

            if (equals + 1 != var)
                free (envvar);
        }
        else
            free (envvar);

        pvar = pend + !!*pend;
    }

    if (1 == nset)
        return ret;

    return nset;
}
