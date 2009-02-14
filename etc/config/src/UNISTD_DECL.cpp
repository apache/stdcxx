// checking the contents of <unistd.h>

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

#include "config.h"

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

#if defined (_WIN32) && !defined (__CYGWIN__)
#  include <io.h>
#else
#  include <unistd.h>
#endif   // _WIN32

// Windows defines the equivalent SSIZE_T in the platform SDK
// as the signed equivalent of size_t which is defined as long
// on WIN32 and long long/__int64 on WIN64
#if defined (_WIN64)
#  define ssize_t __int64
#elif defined (_WIN32)
#  define ssize_t long
#endif

#if !defined (STDIN_FILENO)
#  define STDIN_FILENO    0
#  define STDOUT_FILENO   1
#  define STDERR_FILENO   2
#endif   // STDIN_FILENO


static void print_macros ()
{
    struct {
        int value;
        const char *name;
    } macros[] = {

#define MACRO(name)   { name, #name }
        MACRO (STDIN_FILENO),
        MACRO (STDOUT_FILENO),
        MACRO (STDERR_FILENO),

        // open(2) oflag constants
        MACRO (O_RDONLY),
        MACRO (O_WRONLY),
        MACRO (O_RDWR),

#if defined (O_ACCMODE)
        // fcntl(2) file access mask
        MACRO (O_ACCMODE),
#endif  // O_ACCMODE

        MACRO (O_APPEND),
        MACRO (O_EXCL),
        MACRO (O_CREAT),
        MACRO (O_TRUNC),

        // lseek(2) whence constants
        MACRO (SEEK_SET),
        MACRO (SEEK_CUR),
        MACRO (SEEK_END)

#if defined (F_GETFL)
        // fcntl(2) cmd argument
        , MACRO (F_GETFL)
#endif   // F_GETFL
    };

    for (unsigned i = 0; i != sizeof macros / sizeof *macros; ++i)
        printf ("#define _RWSTD_%-20s %#8x   // %s\n",
                macros [i].name, macros [i].value, macros [i].name);
}

// POSIX: off_t is a signed integral type
const char* off_t_name (short)                { return "short"; }
const char* off_t_name (int)                  { return "int"; }
const char* off_t_name (long)                 { return "long"; }

// POSIX: ssize_t is a signed integral type
const char* ssize_t_name (short)              { return "short"; }
const char* ssize_t_name (unsigned short)     { return "unsigned short"; }
const char* ssize_t_name (int)                { return "int"; }
const char* ssize_t_name (unsigned int)       { return "unsigned int"; }
const char* ssize_t_name (long)               { return "long"; }
const char* ssize_t_name (unsigned long)      { return "unsigned long"; }

#ifndef _RWSTD_NO_LONG_LONG
#  define LONG_LONG   long long
#elif defined (_WIN32)
#  define LONG_LONG   __int64
#endif   // _RWSTD_NO_LONG_LONG, _WIN32


#if defined (LONG_LONG)

#  define STRSTR(x)   #x
#  define STR(x)      STRSTR (x)

const char* off_t_name (LONG_LONG)
{
    return STR (LONG_LONG);
}

const char* ssize_t_name (LONG_LONG)
{
    return STR (LONG_LONG);
}

const char* ssize_t_name (unsigned LONG_LONG)
{
    return STR (unsigned LONG_LONG);
}

#endif   // LONG_LONG


int main ()
{
    print_macros ();

    off_t off = 0;
    printf ("#define _RWSTD_OFF_T %s   // off_t\n", off_t_name (off));

    ssize_t s = 0;
    printf ("#define _RWSTD_SSIZE_T %s   // ssize_t\n", ssize_t_name (s));

    return 0;
}
