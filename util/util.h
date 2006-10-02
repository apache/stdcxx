/************************************************************************
 *
 * util.h - Utility macros / prototypes for the runall utility
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

#ifndef RW_UTIL_H
#define RW_UTIL_H

/**
   Generates a non-terminal error message on stderr.

   @param format printf () format string to display on stderr
*/
void warn (const char* const format, ...);

/**
   Wrapper for exit (), providing a terminal error message on stderr.

   @param state non-zero status code to exit () with
   @param format printf () format string to display on stderr
*/
void terminate (const int state, const char* const format, ...);

/* Note: RW_MALLOC should be used rather than malloc within the runall 
   utility.  This macro calls the guarded_malloc function which performs 
   validation on inputs and outputs.  guarded_malloc shouldn't be called 
   directly, as it needs file and line information, provided by RW_MALLOC.
*/

#define RW_MALLOC(size)                             \
    guarded_malloc(size, __FILE__, __LINE__)
/**
   Wrapper for malloc (), providing return value checking.

   @param size number of bytes of memory to allocate
   @param file name of file calling method
   @param line line number in file method was called from
   @return (non-null) pointer to allocated bock of memory
*/
void* guarded_malloc (const size_t size, const char* const file, 
                      const unsigned line);

#define RW_REALLOC(source, size)                 \
    guarded_realloc(source, size, __FILE__, __LINE__)
/**
   Wrapper for realloc(), providing return value checking.

   @param source pointer to memory block to reallocate
   @param size number of bytes of memory to allocate
   @param file name of file calling method
   @param line line number in file method was called from
   @return (non-null) pointer to allocated bock of memory
*/
void* guarded_realloc (void* source, const size_t size, 
                       const char* const file, const unsigned line);

/**
   Generates the name of a reference (input/output) file.

   This function allocates memory which is to be freed by the caller.

   @param data_dir location of example data directory
   @param subdir example subdirectory to reference
   @param mode type of file to generate name for (should be 'in' or 'out')
   @return translation of 'data_dir/subdir/mode/target_name.mode'
*/
char* reference_name (const char* data_dir, const char* subdir, 
                      const char* mode);

/**
   Generates the name of the output file for the executable target.

   This function allocates memory which is to be freed by the caller.

   @param path of target to generate output name for
   @return translation of 'target.out'
*/
char* output_name (const char* target);
#endif   /* RW_UTIL_H */
