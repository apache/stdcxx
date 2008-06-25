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
#include <string.h>     /* for strchr() */

#include "cmdopt.h" /* for target_name -should this be moved? */
#include "exec.h" /* for get_signame */

#include "display.h"
#include "target.h"   /* for target_status */

#include <time.h>      /* for CLK_TCK, CLOCKS_PER_SEC */

#ifndef _WIN32
#  include <unistd.h>   /* for _SC_CLK_TCK, sysconf() */
#endif

#if defined (_SC_CLK_TCK)

/* dynamically determine the number of clock ticks per second */
static const float TICKS_PER_SEC = float (sysconf (_SC_CLK_TCK));

#elif defined CLOCKS_PER_SEC

/* use the POSIX (and MSVC 6.0) CLOCKS_PER_SEC constant */
static const float TICKS_PER_SEC = CLOCKS_PER_SEC;

#elif defined CLK_TCK

/* use CLK_TCK if it's defined (e.g., pre-MSVC 6.0) */
static const float TICKS_PER_SEC = CLK_TCK;

#else

/* if all else fails, assume the standard 1 million ticks per second */
static const float TICKS_PER_SEC = 1000000UL;

#endif


/**
   ProcessStatus enum lookup table for 'short' (6 character) strings.
*/
static const char* const
short_st_name [ST_LAST] = {
    "OK", /*ST_OK*/
    "COMP", /*ST_COMPILE*/
    "LINK", /*ST_LINK*/
    "EXIST", /*ST_EXIST*/
    "XPERM", /*ST_EXECUTE_FLAG*/
    "EXEC", /*ST_EXECUTE*/
    "NOUT", /*ST_NO_OUTPUT*/
    "OUTPUT", /*ST_NO_REF*/
    "BREF", /*ST_BAD_REF*/
    "DIFF", /*ST_BAD_OUTPUT*/
    "FORMAT", /*ST_FORMAT*/
    "OFLOW", /*ST_OVERFLOW*/
    "ERROR", /*ST_SYSTEM_ERROR*/
    "KILLED", /*ST_KILLED*/
    "NKILL" /*ST_NOT_KILLED*/
};


/**
   ProcessStatus enum lookup table for descriptive strings.
*/
static const char* const
verbose_st_name [ST_LAST] = {
    "OK", /*ST_OK*/
    "Program failed to compile.", /*ST_COMPILE*/
    "Program failed to link.", /*ST_LINK*/
    "Program executable not found.", /*ST_EXIST*/
    "Program not executable.", /*ST_EXECUTE_FLAG*/
    "Program failed to execute.", /*ST_EXECUTE*/
    "Program generated no output.", /*ST_NO_OUTPUT*/
    "Program reference output missing.", /*ST_NO_REF*/
    "Bad reference.", /*ST_BAD_REF*/
    "Program produced unexpected output.", /*ST_BAD_OUTPUT*/
    "Program produced output in unexpected format.", /*ST_FORMAT*/
    "Arithmetic overflow.", /*ST_OVERFLOW*/
    "System error occurred.", /*ST_SYSTEM_ERROR*/
    "Process killed after a timeout.", /*ST_KILLED*/
    "Failed to kill process after a timeout." /*ST_NOT_KILLED*/
};


/**
   Prints an argv array, quoting elelemnts containing spaces.
*/
static int
print_argv (const char* const argv[], int newline)
{
    assert (0 != argv);

    const char* const* parg = argv;

    int nchars = 0;

    for (parg = argv; *parg; ++parg) {

        const char *fmt = "%s ";

        if (strchr (*parg, ' '))
            fmt = "\"%s\" ";

        nchars += printf (fmt, *parg);
    }

    if (newline)
        puts ("");

    return nchars;
}


/**
   Generates output header, designed for text output and console viewing.
*/
static void
print_header_plain (const char* const argv[])
{
    (void)&argv;

    puts ("NAME                                     STATUS WARN ASSERTS "
          "FAILED PERCNT    USER     SYS    REAL");
}


/**
   Generates output header in verbose mode.
*/
static void
print_header_verbose (const char* const argv[])
{
    print_argv (argv, 1 /* append newline */);
}



/**
   Generates target name listing, designed for text output and console viewing.
*/
static void
print_target_plain (const struct target_opts *defaults)
{
    const char* const target_name = get_target ();

    assert (0 == defaults->verbose);

    printf ("%-40.40s ", target_name);

    /* flush to prevent killing a signal from not displaying the text */
    fflush (stdout);
}


/**
   Generates target name listing, designed for text output and console viewing.
*/
static void
print_target_verbose (const struct target_opts *defaults)
{
    assert (defaults->verbose);

    printf ("%s ", "Executing \"");
    print_argv (defaults->argv, 0 /* no newline */);

    /* print stdin, stdout, and stderr redirections */
    if (defaults->infname && *defaults->infname)
        printf (" <%s", defaults->infname);

    if (defaults->outfname && *defaults->outfname)
        printf (" >%s 2>&1", defaults->outfname);

    puts ("\"");

    /* flush to prevent killing a signal from not displaying the text */
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

    valid_timing =    status->usr_time != (clock_t)-1
                   && status->sys_time != (clock_t)-1
                   && ST_NOT_KILLED != status->status;

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
    if (   (unsigned)-1 != status->assert
        && 0 == status->status 
        && 0 == status->exit) {
        if (status->assert) {
            const int percnt = int (  100.0
                                    * (status->assert - status->failed)
                                    / status->assert);
            printf (" %7u %6u %5d%%", status->assert, status->failed, percnt);
        }
        else {
            printf ("       0 %6u   100%%", status->failed);
        }
    }
    else if (valid_timing || status->wall_time != (clock_t)-1)
        printf ("                      ");

    /* Print timings, if available */
    if (valid_timing)
        printf (" %7.3f %7.3f", (float)status->usr_time / TICKS_PER_SEC,
                (float)status->sys_time / TICKS_PER_SEC);
    else if (status->wall_time != (clock_t)-1)
        printf ("                ");

    if (status->wall_time != (clock_t)-1)
        printf (" %7.3f\n", (float)status->wall_time / TICKS_PER_SEC);
    else
        puts ("");
}


/**
   Generates verbose target result listing.
*/
static void
print_status_verbose (const struct target_status* status)
{
    unsigned valid_timing;

    assert (0 != status);
    assert (ST_OK <= status->status && ST_LAST > status->status);

    valid_timing =    status->usr_time != (clock_t)-1
                   && status->sys_time != (clock_t)-1
                   && ST_NOT_KILLED != status->status;

    if (status->status) /* if status is set, print it */
        printf ("  Status: %s\n", verbose_st_name [status->status]);
    else if (status->signaled) /* if exit signal is non-zero, print it */
        printf ("  Process signalled:    %s\n", get_signame (status->exit));
    else {
        printf ("  Exit status:          %6d%s\n"
                "  Compiler warnings:    %6u\n"
                "  Linker warnings:      %6u\n"
                "  Runtime warnings:     %6u\n",
                status->exit, 0 == status->exit ? " (success)" : "",
                status->c_warn,
                status->l_warn,
                status->t_warn);

        /* Print assetions, if any registered */
        if ((unsigned)-1 != status->assert && status->assert) {
            printf ("  Failed assertions:    %6u\n"
                    "  Total assertions:     %6u\n",
                    status->failed, status->assert);
        }
    }

    /* Print timings, if available */
    if (valid_timing) {
        const float wall = (float)status->wall_time / TICKS_PER_SEC;
        const float user = (float)status->usr_time  / TICKS_PER_SEC;
        const float sys  = (float)status->sys_time  / TICKS_PER_SEC;

        printf ("  Times:\n"
                "    Real               %7.3fs\n"
                "    User               %7.3fs\n"
                "    Sys                %7.3fs\n",
                wall, user, sys);
    }

    puts ("");
}


/**
   Placholder output footer function, unneeded for text output and console 
   viewing.
*/
static void
print_footer_plain (int count, const struct target_status *summary)
{
    /* compute cumulative times for all targets */
    const float wall = (float)summary->wall_time / TICKS_PER_SEC;
    const float user = (float)summary->usr_time  / TICKS_PER_SEC;
    const float sys  = (float)summary->sys_time  / TICKS_PER_SEC;

    printf ("PROGRAM SUMMARY:\n"
            "  Programs:             %9d\n"
            "  Non-zero exit status: %9d\n"
            "  Signalled:            %9d\n"
            "  Compiler warnings:    %9u\n"
            "  Linker warnings:      %9u\n"
            "  Runtime warnings:     %9u\n",
            count,
            summary->exit,
            summary->signaled,
            summary->c_warn,
            summary->l_warn,
            summary->t_warn);

    if ((unsigned)-1 != summary->assert) {
        /* print assertion counters only when they're valid */
        printf ("  Assertions:           %9u\n"
                "  Failed assertions:    %9u\n",
                summary->assert,
                summary->failed);
    }

    printf (            "  Cumulative times:\n"
            "    Real                      %7.3fs\n"
            "    User                      %7.3fs\n"
            "    Sys                       %7.3fs\n",
            wall,
            user,
            sys);
}


static void
print_footer_verbose (int count, const struct target_status *summary)
{
    print_footer_plain (count, summary);
}


/**
   Sets the output functions referenced.
*/
void set_output_format (enum OutputFmt format)
{
    if (FMT_VERBOSE == format) {
        print_header = print_header_verbose;
        print_target = print_target_verbose;
        print_status = print_status_verbose;
        print_footer = print_footer_verbose;
    }
    else {
        /* only two formats implemented */
        assert (FMT_PLAIN == format);

        print_header = print_header_plain;
        print_target = print_target_plain;
        print_status = print_status_plain;
        print_footer = print_footer_plain;
    }
}


void (*print_header) (const char* const[]) = print_header_plain;
void (*print_target) (const struct target_opts*) = print_target_plain;
void (*print_status) (const struct target_status*) = print_status_plain;
void (*print_footer) (int, const struct target_status*) = print_footer_plain;
