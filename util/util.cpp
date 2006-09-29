/************************************************************************
 *
 * util.cpp - Utility function definitions for the runall utility
 *
 * $Id$
 *
 ************************************************************************
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 **************************************************************************/

#include <assert.h> /* for assert */
#include <errno.h> /* for errno */
#include <stdio.h> /* for vfprintf */
#include <stdlib.h> /* for exit, malloc */
#include <stdarg.h> /* for va_* */
#include <string.h> /* for strerror */
#include <sys/types.h> /* for size_t */

#include "cmdopt.h" /* for exe_name, target_name */

#include "util.h"

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

/**
   Wrapper for exit (), providing a terminal error message on stderr.

   @param state non-zero status code to exit () with
   @param format printf () format string to display on stderr
*/
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

/**
   Wrapper for malloc (), providing return value checking.

   @param size number of bytes of memory to allocate
   @param file name of file calling method
   @param line line number in file method was called from
   @return (non-null) pointer to allocated bock of memory
*/
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

/**
   Wrapper for realloc(), providing return value checking.

   @param source pointer to memory block to reallocate
   @param size number of bytes of memory to allocate
   @param file name of file calling method
   @param line line number in file method was called from
   @return (non-null) pointer to allocated bock of memory
*/
void*
guarded_realloc (void* source, const size_t size, const char* const file, 
                 const unsigned line)
{
    void* alloc;

    assert (0 != file);
    assert (0 < size);

    alloc = realloc (source, size);

    if ( 0 == alloc )
        terminate ( 1, "malloc(%lu) at line %u of %s failed: %s\n", 
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

    assert (0 != data_dir);
    assert (0 != target_name);
    assert (0 != subdir);
    assert (0 != mode);

    root_len = strlen (data_dir);
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
