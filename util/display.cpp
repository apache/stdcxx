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

void print_header_plain ()
{
    puts ("NAME                      STATUS ASSERTS FAILED PERCNT");
}

void print_target_plain (const struct target_status*)
{
    printf ("%-25.25s ", target_name);
    fflush (stdout);
}

void print_status_plain (const struct target_status* status)
{
    assert (ST_OK <= status->status && ST_LAST > status->status);

    if (status->status) /* if status is set, print it */
        printf ("%6s\n", short_st_name [status->status]);
    else if (status->exit) /* if exit code is non-zero, print it */
        printf ("%6d\n", status->exit);
    else if (status->signal) /* if exit signal is non-zero, print it */
        printf ("%6s\n", get_signame (status->signal));
    else if (!status->assert) /* if the assertion count is 0, it's an example */
        puts ("     0");
    else {
        unsigned pcnt = 0;
        if (status->assert) {
            pcnt = 100 * (status->assert - status->failed) / status->assert;
        }
        printf ("     0 %7u %6u %5d%%\n", status->assert, status->failed, pcnt);
    }
}

void print_footer_plain () {}

const char* short_st_name [ST_LAST] = {
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
void (*print_target) (const struct target_status*) = print_target_plain;
void (*print_status) (const struct target_status* status)  = print_status_plain;
void (*print_footer) () = print_footer_plain;
