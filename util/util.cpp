/************************************************************************
 *
 * util.cpp - Utility function definitions for the exec utility
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

#include <assert.h> /* for assert */
#include <errno.h> /* for errno */
#include <signal.h> /* for sigaction(), signal() */
#include <stdio.h> /* for vfprintf */
#include <stdlib.h> /* for exit, malloc */
#include <stdarg.h> /* for va_* */
#include <string.h> /* for strerror */

#include <sys/stat.h> /* for stat() */
#include <sys/types.h> /* for size_t */

#ifndef _WIN32
#  include <unistd.h> /* for sleep() */
#else
#  include <windows.h> /* for Sleep() */
#endif   /* _WIN32 */


#include "cmdopt.h" /* for exe_name, target_name */

#include "util.h"


#ifdef _WIN32
#  define DEV_NULL   "NUL:"
#else
#  define DEV_NULL   "/dev/null"
#endif   // _WIN32


void
warn (const char* const format, ...)
{
    const char* const target_name = get_target ();
    va_list args;

    assert (0 != format);

    if (target_name)
        fprintf (stderr, "%s (%s): ", exe_name, target_name);
    else
        fprintf (stderr, "%s: ", exe_name);

    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end (args);
}

void
terminate (const int state, const char* const format, ...)
{
    const char* const target_name = get_target ();
    va_list args;

    assert (0 != format);
    assert (0 != state);

    if (target_name)
        fprintf (stderr, "%s (%s): ", exe_name, target_name);
    else
        fprintf (stderr, "%s: ", exe_name);

    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end (args);

    exit (state);
}

void*
guarded_malloc (const size_t size, const char* const file, const unsigned line)
{
    void* alloc;

    assert (0 != file);
    assert (0 < size);

    alloc = malloc (size);

    if (0 == alloc)
        terminate (1, "malloc (%lu) at line %u of %s failed: %s\n", 
                 (unsigned long)size, line, file, strerror (errno));

    return alloc;
}

void*
guarded_realloc (void* source, const size_t size, const char* const file, 
                 const unsigned line)
{
    void* alloc;

    assert (0 != file);
    assert (0 < size);

    alloc = realloc (source, size);

    if (0 == alloc)
        terminate (1, "malloc(%lu) at line %u of %s failed: %s\n", 
                 (unsigned long)size, line, file, strerror (errno));

    return alloc;
}

char*
reference_name (const char* data_dir, const char* subdir, const char* mode)
{
    size_t root_len, cmp_len, dir_len, mode_len, net_len;
    char* ref_name;
    char* tail;
    const char* const target_name = get_target ();

    assert (0 != target_name);
    assert (0 != subdir);
    assert (0 != mode);

    root_len = data_dir ? strlen (data_dir) : 0;
    cmp_len = strlen (target_name) - exe_suffix_len;
    dir_len = strlen (subdir);
    mode_len = strlen (mode);
    net_len = root_len + cmp_len + dir_len + mode_len * 2 + 5;
    /* 5 comes from 3 path seperator characters, the suffix seperator 
       character, and the trailing null */
    tail = ref_name = (char*)RW_MALLOC (net_len);

    memcpy (tail, data_dir, root_len);
    tail += root_len;
    *tail++ = default_path_sep;
    memcpy (tail , subdir, dir_len);
    tail += dir_len;
    *tail++ = default_path_sep;
    memcpy (tail , mode, mode_len);
    tail += mode_len;
    *tail++ = default_path_sep;
    memcpy (tail , target_name, cmp_len);
    tail += cmp_len;
    *tail++ = suffix_sep;
    memcpy (tail , mode, mode_len);
    tail += mode_len;
    *tail = '\0';

    return ref_name;
}


/**
   Composes the name of an input file, based on target

   Takes a data directory and an executable name, and tries to open an input 
   file based on these variables.  If a file is found in neither of two 
   locattions derived from these variables, this method tries to fall back on 
   /dev/null.

   Source file locations:
     - [data_dir]/manual/in/[target].in
     - [data_dir]/tutorial/in/[target].in
     - /dev/null

   @param data_dir the path of the reference data directory
   @param target the name of executable being run
   @returns the name of the file
*/
char*
input_name (const char* data_dir, const char* target)
{
    char* fname = 0;
    int stat_result = 0;
    struct stat sb;

    assert (0 != target);

    if (data_dir && *data_dir) {

        /* Try data_dir/manual/in/target.in */
        fname       = reference_name (data_dir, "manual", "in");
        stat_result = stat (fname, &sb);
    
        if (0 == stat_result)
            return fname;

        free (fname);

        /* Try data_dir/tutorial/in/target.in */
        fname       = reference_name (data_dir, "tutorial", "in");
        stat_result = stat (fname, &sb);

        if (0 == stat_result)
            return fname;

        free (fname);
    }

    /* If we didn't find a source file, use /dev/null */
    fname = (char*)RW_MALLOC (sizeof DEV_NULL);
    strcpy (fname, DEV_NULL);
    return fname;
}


char*
output_name (const char* target)
{
    const char* suffix = "out";
    const size_t sfx_len = strlen (suffix);
    const size_t exe_len = strlen (target) - exe_suffix_len;
    char* const tmp_name = (char*)RW_MALLOC (exe_len + sfx_len + 2);
    
    memcpy (tmp_name, target, exe_len);
    *(tmp_name + exe_len) = suffix_sep;
    memcpy (tmp_name + exe_len + 1, suffix, sfx_len + 1);
    return tmp_name;
}


#ifndef _WIN32

void
rw_sleep (int seconds)
{
    sleep (seconds);
}


#  ifndef _RWSTD_EDG_ECCP

#    ifdef __cplusplus

extern "C" {

#    endif   /* __cplusplus */

int
rw_signal (int signo, void (*func)(int))
{
    struct sigaction act;
    memset (&act, 0, sizeof act);

    /* avoid extern "C"/"C++" mismatch due to an HP aCC 6 bug
       (see STDCXX-291) */
    if (func)
        memcpy (&act.sa_handler, &func, sizeof func);
    else
        act.sa_handler = 0;

    return 0 > sigaction (signo, &act, 0);
}

#    ifdef __cplusplus

}   /* extern "C" */

#    endif   /* __cplusplus */

#  else   /* if defined (_RWSTD_EDG_ECCP) */

#    ifdef __cplusplus

extern "C" {

#    endif   /* __cplusplus */

int
rw_signal (int signo, void (*func)(int))
{
    return SIG_ERR == signal (signo, func);
}

#    ifdef __cplusplus

}   /* extern "C" */

#    endif   /* __cplusplus */
#  endif   /* _RWSTD_EDG_ECCP */
#else   /* ifdef _WIN32 */

void
rw_sleep (int seconds)
{
    Sleep (seconds * 1000);
}


int
rw_signal (int signo, void (*func)(int))
{
    return SIG_ERR == signal (signo, func);
}

#endif   /* _WIN32 */
