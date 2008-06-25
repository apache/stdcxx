/************************************************************************
 *
 * cmdopt.h - Interface declaration for the option parsing subsystem
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

#ifndef RW_PARSE_OPTS_H
#define RW_PARSE_OPTS_H

#include "target.h" /* For struct target_opts */

extern const char* exe_name;
extern const char escape_code; /**< Escape character used in paths. */
extern const char default_path_sep; /**< Primary path seperator */
extern const char suffix_sep; /**< File suffix seperator. */
extern const size_t exe_suffix_len; /**< Length of executable suffix. */


/**
   Parses command line arguments for switches and options.

   @param argc number of command line arguments.
   @param argv command line arguments.
   @param defaults target_status structure containing default values.
   @param exe_opts handle to default child process arguments string
   @return number of command line arguments parsed.
*/
int 
eval_options (int argc, char** argv, struct target_opts* defaults, 
              const char** exe_opts);

/**
   Translates opts into an array that can be passed to exec().

   This method malloc ()s two blocks of memory.  The first block is the 
   generated argv index array.  This is the return value of this method.  The 
   second block is the parsed and split string contents the index referenced.  
   This block is stored in element 1 of the return array.  It is the 
   responsibility of the calling method to free () both blocks.

   @warning this logic is UTF-8 unsafe
   @warning I/O redirection command piping isn't supported in the parse logic
   @param opts option string to split
   @return the parsed argv array
*/
char**
split_opt_string (const char* opts);

/**
   Accessor method for current execution target.

   This function is defined in runall.cpp (not cmdopt.cpp).

   This value is a pointer into argv[0] of the current target.

   @returns (Base) name of current execution target.
*/
const char* get_target ();

#endif   /* RW_PARSE_OPTS_H */
