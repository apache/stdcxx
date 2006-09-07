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
   Status codes to denote result of analysis.
*/
enum ProcessStatus {
    ST_OK = 0, /**< Default (OK) status */
    ST_COMPILE, /**< Target failed to compile */
    ST_LINK, /**< Target failed to link */
    ST_EXIST, /**< Target doesn't exist? */
    ST_EXECUTE_FLAG, /**< Target lacks X flag */
    ST_EXECUTE, /**< Target failed to execute? */
    ST_NO_OUTPUT, /**< Target produced no output */
    ST_NO_REF, /**< No reference file found */
    ST_BAD_REF, /**< Invalid reference file found */
    ST_BAD_OUTPUT, /**< Incorrect output found */
    ST_FORMAT, /**< Incorrectly formatted (test) output found */
    ST_OVERFLOW, /**< Assertion counter overflow */
    ST_SYSTEM_ERROR, /**< System error during file system operation */
    ST_KILLED, /**< Target killed by exec utility */
    ST_NOT_KILLED, /** Target not killed by exec utility */
    ST_LAST /**< Array terminator */
};

/**
   ProcessStatus enum lookup table for 'short' (6 character) strings.
*/
extern const char* short_st_name [ST_LAST];

/**
   Structure encapsulating the results extracted from a run.
*/
struct target_status {
    char** argv; /**< Target argv array. */
    int exit; /**< exit code for process. */
    int signal; /**< Termination signal for process. */
    enum ProcessStatus status; /**< Textual process status. */
    unsigned warn; /**< Number of (test) warnings. */
    unsigned assert; /**< Number of (test) assertions. */
    unsigned failed; /**< Number of failed (test) assertions. */
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
extern void (*print_header) ();

/**
   Prints the formatted header column for a target row.

   This method uses the target_name global (defined in cmdopt.h), and flushes
   stdout after printing.

   @see target_name
*/
extern void (*print_target) (const struct target_status* status);

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
extern void (*print_footer) ();

#endif   // RW_DISPLAY_H
