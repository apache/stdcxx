// checking vsnprintf() return value

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

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

#include "config.h"


#ifdef _RWSTD_NO_VSNPRINTF
#  if !defined (_MSC_VER)

extern "C" int vsnprintf (char*, size_t, const char*, va_list);

#  else   // if defined (_MSC_VER)

extern "C" int _vsnprintf (char*, size_t, const char*, va_list);

#    define vsnprintf _vsnprintf

#  endif   // MSVC

#endif   // _RWSTD_NO_VSNPRINTF


int my_snprintf (char *buf, size_t size, const char *fmat, ...)
{
    va_list va;
    va_start (va, fmat);

    const int ret = vsnprintf (buf, size, fmat, va);

    va_end (va);

    return ret;
}


int test_vsnprintf (int success)
{
    char buf [] = "0123456789";

    const char data [] = "abcdef";

    const size_t bufsize  = success ? sizeof buf : 4U;
    const size_t datasize = sizeof data;
    
    const int ret = my_snprintf (buf, bufsize, "%s", data);

    // 7.19.6.12, p3 of C99:
    //
    // The vsnprintf function returns the number of characters that would
    // have been written had n been sufficiently large, not counting the
    // terminating null character, or a negative value if an encoding error
    // occurred. Thus, the null-terminated output has been completely
    // written if and only if the returned value is nonnegative and less
    // than n.

    if (success) {
        if (datasize - 1 == ret) {
            // expected behavior, no output
            return 0;
        }
        else if (datasize == ret) {
            printf ("#define _RWSTD_NO_VSNPRINTF_NUL "
                    "/* return value includes NUL on success */\n");
            return -1;
        }
        else if (ret < 0) {
            printf ("#define _RWSTD_NO_VSNPRINTF_POS "
                    "/* return value negative on success */\n");
            return 1;
        }
        else {
            printf ("#define _RWSTD_NO_VSNPRINTF_VALID "
                    "/* return value bogus on success */\n");
            return 1;
        }
    }
    else {
        if (datasize - 1 == ret) {
            // expected behavior, no output
            return 0;
        }
        else if (ret < 0) {
            // HP-UX and Windows are known to return -1 on buffer overflow
            printf ("#define _RWSTD_NO_VSNPRINTF_VFLOW_VALID "
                    "/* return value negative on overflow */\n");
            return 1;
        }
        else if (3 == ret) {
            // IRIX and Tru64 UNIX are known to return the number
            // of bytes actually written which is useless for
            // determing the size of the buffer
            printf ("#define _RWSTD_NO_VSNPRINTF_VFLOW_VALID "
                    "/* returns the number of non-NUL bytes written */\n");
            return 1;
        }
        else if (4 == ret) {
            printf ("#define _RWSTD_NO_VSNPRINTF_VFLOW_VALID "
                    "/* returns the number of bytes written */\n");
            return 1;
        }
        else {
            printf ("#define _RWSTD_NO_VSNPRINTF_VFLOW_VALID "
                    "/* return value bogus on overflow */\n");
            return 1;
        }
    }
}


int main ()
{
    test_vsnprintf (0);
    test_vsnprintf (1);

    return 0;
}
