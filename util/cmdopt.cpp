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
#include <stdio.h> /* for *printf, fputs */
#include <stdlib.h> /* for atoi, exit */
#include <string.h> /* for str* */

#include "util.h"

#include "cmdopt.h"

int timeout = 10; /**< Child process timeout.  Default 10 */
int compat = 0; /**< Test compatability mode switch.  Defaults to 0 (off) */
const char* exe_opts = ""; /**< Command line switches for child processes */
const char* in_root = ""; /**< Root directory for input/reference files */
const char* exe_name; /**< Alias for process argv[0]. */

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
    fputs ("    -d dir      Root directory for output reference files\n"
           "    -h, -?      Display usage information and exit\n"
           "    -t seconds  Watchdog timeout before killing target (default is "
"10 \n                seconds)\n"
           "    -x opts     Command line options to pass to targets\n"
           "    --          Terminate option processing and treat all "
           "following arguments\n                as targets\n", stderr);
    exit (status);
}

/**
   Parses command line arguments for switches and options.

   @param argc number of command line arguments
   @param argv command line arguments
   @return number of command line arguments parsed.
   @see timeout
   @see in_root
   @see exe_opts
*/
int 
eval_options (const int argc, /* const */ char* const argv[])
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
            if ('\0' == argv [i][2])
                val = argv [++i];
            else
                val = argv [i] + 2;

            timeout = atoi (val);
            break;
        case 'd':
            if ('\0' == argv [i][2])
                val = argv [++i];
            else
                val = argv [i] + 2;

            in_root = val;
            break;
        case 'x':
            if ('\0' == argv [i][2])
                val = argv [++i];
            else
                val = argv [i] + 2;

            exe_opts = val;
            break;
        case '-':
        {
            const size_t arglen = strlen (argv [i]);

            /* abort processing on --, eat token */
            if ('\0' == argv [i][2])
                return i+1;

            if (8 == arglen && 0 == memcmp ("--compat", argv [i], 8)){
                compat = 1;
                break;
            }
            else if (10 == arglen && 0 == memcmp ("--nocompat", argv [i], 10)){
                compat = 0;
                break;
            }
        }
            /* Intentionally falling through */
        default:
            printf ("Unknown option: %s\n", argv [i]);
            show_usage (1);
        }
    }

    return i;
}

/**
   Translates exe_opts into an array that can be passed to exec ().

   This method malloc ()s two blocks of memory.  The first block is the argv
   index array.  This is the return value of this method.  The second block
   is the parsed and split string contents the index referenced.  This block
   is stored in subscript 1 of the return array.  It is the responsibility of
   the calling method to free () both blocks.

   @warning this logic is UTF-8 unsafe
   @warning I/O redirection command piping isn't supported in the parse logic
   @return the parsed argv array
   @see exe_opts
*/
char**
split_child_opts ()
{
    char in_quote = 0;
    int in_escape = 0;
    int in_token = 0;
    const char *pos;
    char *target, *last;
    char **table_pos, **argv;

    assert (0 != exe_opts);

    if (0 == strlen (exe_opts)) {
        /* Alloc a an index array to hold the program name  */
        argv = (char**)RW_MALLOC (2 * sizeof (char*));

        /* And tie the two together */
        argv [1] = (char*)0;
        return argv;
    }

    table_pos = argv = (char**)RW_MALLOC (
        (strlen (exe_opts) + 5) * sizeof (char*) / 2);
    /* (strlen (exe_opts)+5)/2 is overkill for the most situations, 
       but it is just large enough to handle the worst case scenario
       (worst case is similar to 'x y' or 'x y z', requiring lengths
       of 4 and 5 respectively.)
    */

    last = target = argv[1] = (char*)RW_MALLOC (strlen (exe_opts) + 1);

    /* Transcribe the contents, handling escaping and splitting */
    for (pos = exe_opts; *pos; ++pos) {
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
                    *(++table_pos) = last;
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
        *(++table_pos) = last;
    }
    *(++table_pos) = (char*)0;/*And terminate the array*/

    return argv;
}
