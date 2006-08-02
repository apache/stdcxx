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
    void* const alloc = malloc (size);

    assert (0 != file);
    assert (0 < size);

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
    void* const alloc = realloc (source, size);

    assert (0 != file);
    assert (0 < size);

    if ( 0 == alloc )
        terminate ( 1, "malloc(%lu) at line %u of %s failed: %s\n", 
                 (unsigned long)size, line, file, strerror (errno));

    return alloc;
}
