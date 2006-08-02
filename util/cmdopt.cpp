/************************************************************************
 *
 * cmdopt.cpp - Definitions of the option parsing subsystem
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
#include <ctype.h> /* for isspace */
#include <errno.h> /* for errno */
#include <signal.h> /* for kill, SIG_IGN */
#include <stdio.h> /* for *printf, fputs */
#include <stdlib.h> /* for exit */
#include <string.h> /* for str* */
#include <unistd.h> /* for getpid */

#include "exec.h"
#include "util.h"

#include "cmdopt.h"

int timeout = 10; /**< Child process timeout.  Default 10. */
int compat = 0; /**< Test compatability mode switch.  Defaults to 0 (off). */
unsigned verbose = 0;
const char* exe_opts = ""; /**< Global command line switches for child 
                                processes. */
const char* in_root = ""; /**< Root directory for input/reference files. */
const char* exe_name; /**< Alias for process argv [0]. */
const char* target_name;

/**
   Display command line switches for program and terminate.

   @param status status code to exit with.
*/
void 
show_usage (int status)
{
    assert (0 != exe_name);
    fprintf (stderr, "Usage: %s [OPTIONS] [targets]\n", exe_name);
    fputs ("\n"
           "  Treats each token in targets as the path to an executable.  "
           "Each target\n  enumerated is executed, and the output is "
           "processed after termination.  If\n  execution takes longer "
           "than a certain (configurable) period of time, the\n  process is "
           "killed\n\n", stderr);
    fputs ("    -d dir       Root directory for output reference files\n"
           "    -h, -?       Display usage information and exit\n"
           "    -t seconds   Watchdog timeout before killing target (default "
           "is 10 \n                 seconds)\n"
           "    -q           Set verbosity level to 0 (default)\n"
           "    -v           Increase verbosity of output\n"
           "    -x opts      Command line options to pass to targets\n", 
           stderr);
    fputs ("    --           Terminate option processing and treat all "
           "following arguments\n                 as targets\n"
           "    --compat     Use compatability mode test output parsing\n"
           "    --nocompat   Use standard test output parsing (default)\n"
           "    --exit=val   Exit (now) with a return code of val\n"
           "    --sleep=sec  sleep() (now) for sec seconds\n"
           "    --signal=sig Send self signal sig (now)\n"
           "    --ignore=sig Ignore signal sig\n", stderr);
    fputs ("\n"
           "  All short (single dash) options must be specified seperately.\n"
           "  If a short option takes a value, it may either be provided like"
           "\n  '-sval' or  '-s val'\n"
           "  If a long option take a value, it may either be provided like\n"
           "  '--option=value' or '--option value'\n", stderr);
          
    exit (status);
}

/**
    Helper function to read the value for a short option
    
    @param argv argument array
    @param idx reference to index for option
*/
static char*
get_short_val (char* const* argv, int* idx)
{
    assert (0 != argv);
    assert (0 != idx);

    if ('\0' == argv [*idx][2])
        return argv [++(*idx)];
    else
        return argv [*idx] + 2;
}

/**
    Helper function to read the value for a long option
    
    @param argv argument array
    @param idx reference to index for option
    @param offset length of option name (including leading --)
*/
static char*
get_long_val (char* const* argv, int* idx, unsigned offset)
{
    assert (0 != argv);
    assert (0 != idx);

    if ('\0' == argv [*idx][offset])
        return argv [++(*idx)];
    else if ('=' == argv [*idx][offset])
        return argv [*idx] + offset + 1;
    else
        return (char*)0;
}

/**
    Helper function to produce 'Unknown option' error message.
    
    Terminates via show_usage.
    
    @param opt name of option encountered
*/
static void
bad_option (char* const opt)
{
    assert (0 != opt);
    warn ("Unknown option: %s\n", opt);
    show_usage (1);
}

/**
   Parses command line arguments for switches and options.

   @param argc number of command line arguments
   @param argv command line arguments
   @return number of command line arguments parsed.
   @see timeout
   @see compat
   @see in_root
   @see exe_opts
*/
int 
eval_options (const int argc, char* const argv [])
{
    int i;

    char* val;

    assert (0 != argv);

    if (1 == argc || '-' != argv [1][0])
        return 1;

    for (i = 1; i < argc && '-' == argv [i][0]; ++i) {

        switch (argv [i][1]) {
        case '?':
        case 'h':
            show_usage (0);
        case 'r':
            ++i; /* Ignore -r option (makefile compat) */
            break;
        case 't':
            val = get_short_val (argv, &i);
            timeout = strtol (val, &val, 10);
            if (*val || errno)
                terminate (1, "Unknown value for -t: %s\n", val);
            break;
        case 'd':
            in_root = get_short_val (argv, &i);
            break;
        case 'x':
            exe_opts = get_short_val (argv, &i);
            break;
        case 'v':
            ++verbose;
            break;
        case 'q':
            verbose = 0;
            break;
        case '-':
        {
            const size_t arglen = strlen (argv [i]);

            /* abort processing on --, eat token */
            if ('\0' == argv [i][2])
                return i+1;

            if (8 == arglen && 0 == memcmp ("--compat\0", argv [i], 9)) {
                compat = 1;
                break;
            }
            else if (10 == arglen && 0 == memcmp ("--nocompat\0", argv [i], 
                                                  11)) {
                compat = 0;
                break;
            }
            else if (6 <= arglen && 0 == memcmp ("--exit", argv [i], 6)) {
                val = get_long_val (argv, &i, 6);
                if (val) {
                    int code = strtol (val, &val, 10);
                    if (*val || errno)
                        terminate (1, "Unknown value for --exit: %s\n", val);
                    exit (code);
                }
                else
                    bad_option (argv [i]);
            }
            else if (7 <= arglen && 0 == memcmp ("--sleep", argv [i], 7)) {
                val = get_long_val (argv, &i, 7);
                if (val) {
                    int duration = strtol (val, &val, 10);
                    if (*val || errno)
                        terminate (1, "Unknown value for --sleep: %s\n", val);
                    sleep (duration);
                    break;
                }
                else
                    bad_option (argv [i]);
            }
            else if (8 <= arglen && 0 == memcmp ("--signal", argv [i], 8)) {
                val = get_long_val (argv, &i, 8);
                if (val) {
                    int sig = get_signo (val);
                    if (0 > sig)
                        terminate (1, "Unknown signal name for --signal: "
                                   "%s\n", val);
                    
                    /* Ignore kill errors (what should we do with them?) */
                    (void)kill (getpid (), sig);

                    /* Not certain what we should do if we don't terminate by 
                       signal */
                    break;
                }
                else
                    bad_option (argv [i]);
            }
            else if (8 <= arglen && 0 == memcmp ("--ignore", argv [i], 8)) {
                val = get_long_val (argv, &i, 8);
                if (val) {
                    struct sigaction act;
                    int sig = get_signo (val);
                    if (0 > sig)
                        terminate (1, "Unknown signal name for --ignore: "
                                   "%s\n", val);
                    
                    memset (&act, 0, sizeof act);
                    act.sa_handler = SIG_IGN;
                    (void)sigaction (sig, &act, 0);
                    /* Ignore sigaction errors (what should we do with them?) 
                     */
                    break;
                }
                else
                    bad_option (argv [i]);
            }
            else
                bad_option (argv [i]);
        }
        default:
            bad_option (argv [i]);
        }
    }

    return i;
}

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
split_opt_string (const char* const opts)
{
    char in_quote = 0;
    int in_escape = 0;
    int in_token = 0;
    const char *pos;
    char *target, *last;
    char **table_pos, **argv;
    const size_t optlen = strlen (opts);

    assert (0 != opts);

    if (0 == optlen) {
        /* Alloc a an index array to hold the program name  */
        argv = (char**)RW_MALLOC (sizeof (char*));

        /* And tie the two together */
        argv [0] = (char*)0;
        return argv;
    }

    table_pos = argv = (char**)RW_MALLOC ((optlen + 3) * sizeof (char*) / 2);
    /* (strlen (opts)+3)/2 is overkill for the most situations, but it is just 
       large enough to handle the worst case scenario.  The worst case is a 
       string similar to 'x y' or 'x y z', requiring array lengths of 4 and 5 
       respectively.
    */

    last = target = argv [0] = (char*)RW_MALLOC (optlen + 1);

    /* Transcribe the contents, handling escaping and splitting */
    for (pos = opts; *pos; ++pos) {
        if (in_escape) {
            *(target++) = *pos;
            in_escape = 0;
            continue;
        }
        if (isspace (*pos)) {
            if (in_quote) {
                *(target++) = *pos;
            }
            else {
                if (in_token) {
                    *(target++) = '\0';
                    *(table_pos++) = last;
                    in_token = 0;
                }
                last = target;
            }
            continue;
        }
        in_token = 1;
        switch (*pos) {
        case '\\':
            in_escape = 1;
            break;
        case '"':
        case '\'':
            if (*pos == in_quote) {
                in_quote = 0;
                break;
            }
            else if (0 == in_quote) {
                in_quote = *pos;
                break;
            }
            /* intentionally falling through (in a quote and quote didn't 
               match opening quote.
            */
        default:
            *(target++) = *pos;
        }
    }

    if (in_token) { /* close and record the final token */
        *(target++) = '\0';
        *(table_pos++) = last;
    }
    *table_pos = (char*)0;/*And terminate the array*/

    return argv;
}
