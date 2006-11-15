/************************************************************************
 *
 * display.cpp - Definitions of the result display subsystem
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

#include <assert.h>
#include <stdio.h>      /* for fflush(), printf(), puts(), ... */

#include "cmdopt.h" /* for target_name -should this be moved? */
#include "exec.h" /* for get_signame */

#include "display.h"

/**
   Generates output header, designed for text output and console viewing.
*/
static void print_header_plain ()
{
    puts ("NAME                      STATUS WARN ASSERTS FAILED PERCNT    "
          "USER     SYS    REAL");
}

/**
   Generates target name listing, designed for text output and console viewing.
*/
static void print_target_plain (const struct target_opts*)
{
    const char* const target_name = get_target ();
    printf ("%-25.25s ", target_name);
    fflush (stdout);
}

/**
   Generates target result listing, designed for text output and console 
   viewing.
*/
static void print_status_plain (const struct target_status* status)
{
    unsigned valid_timing;
    assert (0 != status);
    assert (ST_OK <= status->status && ST_LAST > status->status);

    valid_timing = status->user && status->sys  && 
        ST_NOT_KILLED != status->status;

    if (status->status) /* if status is set, print it */
        printf ("%6s", short_st_name [status->status]);
    else if (status->signaled) /* if exit signal is non-zero, print it */
        printf ("%6s", get_signame (status->exit));
    else if (status->exit) /* if exit code is non-zero, print it */
        printf ("%6d", status->exit);
    else 
        printf ("     0");

    printf (" %4u", status->c_warn + status->l_warn + status->t_warn);

    /* Print assetions, if any registered */
    if (status->assert) {
        printf (" %7u %6u %5d%%", status->assert, status->failed, 
                100 * (status->assert - status->failed) / status->assert);
    }
    else if (valid_timing || status->wall)
        printf ("                      ");

    /* Print timings, if available */
    if (valid_timing)
        printf (" %7.3f %7.3f", (float) *status->user / TICKS_PER_SEC,
                (float) *status->sys / TICKS_PER_SEC);
    else if (status->wall)
        printf ("                ");

    if (status->wall)
        printf (" %7.3f\n", (float) *status->wall / TICKS_PER_SEC);
    else
        puts ("");
}

/**
   Placholder output footer function, unneeded for text output and console 
   viewing.
*/
static void print_footer_plain () {}

const char* const short_st_name [ST_LAST] = {
    "OK", /*ST_OK*/
    "COMP", /*ST_COMPILE*/
    "LINK", /*ST_LINK*/
    "EXIST", /*ST_EXIST*/
    "XPERM", /*ST_EXECUTE_FLAG*/
    "EXEC", /*ST_EXECUTE*/
    "NOUT", /*ST_NO_OUTPUT*/
    "NREF", /*ST_NO_REF*/
    "BREF", /*ST_BAD_REF*/
    "OUTPUT", /*ST_BAD_OUTPUT*/
    "FORMAT", /*ST_FORMAT*/
    "OFLOW", /*ST_OVERFLOW*/
    "ERROR", /*ST_SYSTEM_ERROR*/
    "KILLED", /*ST_KILLED*/
    "NKILL" /*ST_NOT_KILLED*/
};


void (*print_header) () = print_header_plain;
void (*print_target) (const struct target_opts*) = print_target_plain;
void (*print_status) (const struct target_status* status)  = print_status_plain;
void (*print_footer) () = print_footer_plain;
