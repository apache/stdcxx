/************************************************************************
 *
 * display.h - Interface declaration for the result display subsystem
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

#ifndef RW_DISPLAY_H
#define RW_DISPLAY_H

#include "target.h" /* For target_opts */

/**
   Output format mode enumeration.

   Used to determine the mode used in producing output.
*/
enum OutputFmt {
    FMT_PLAIN = 0, /**< plain text output. */
    FMT_XTERM, /**< xterm colored output. */
    FMT_VERBOSE, /**< verbose text output. */
    FMT_INDEP, /**< Non-targeted formated output. */
    FMT_HTML /**< HTML formated output. */
};

/**
   Sets the output functions referenced.
*/
void set_output_format (enum OutputFmt format);

/**
   Sets the column order and widths (if applicable) for output.

   @param format custom format string for output.
*/
extern void (*set_header) (const char* format);

/**
   Prints the table preamble formatting, followed by the formatted header row.
*/
extern void (*print_header) (const char* const argv[]);

/**
   Prints the formatted header column for a target row.

   This method uses the target_name global (defined in cmdopt.h), and flushes
   stdout after printing.

   @see target_name
*/
extern void (*print_target) (const struct target_opts* options);

/**
   Updates the display of a (non-final) status indicator

   This function is only useful for FMT_XTERM.

   @param time 0 > time indicates -signal number, otherwise time remaining
*/
extern void (*set_progress) (int time);

/**
   Prints the formatted results for a target row.

   @param status describes the results of the run
*/
extern void (*print_status) (const struct target_status* status);


/**
   Prints the closing formatting for the table.
*/
extern void (*print_footer) (int count, const struct target_status* status);


#endif   /* RW_DISPLAY_H */
