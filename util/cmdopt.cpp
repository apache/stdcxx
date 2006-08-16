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

// disable Compaq/HP C++ pure libc headers to allow POSIX symbols
// such as SIGALRM or SIGKILL to be defined
#undef __PURE_CNAME

#include <assert.h>
#include <ctype.h> /* for isspace */
#include <errno.h> /* for errno */
#include <signal.h> /* for raise, signal, SIG_IGN */
#include <stdio.h> /* for *printf, fputs */
#include <stdlib.h> /* for exit */
#include <string.h> /* for str* */
#include <unistd.h> /* for sleep */

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
const char escape_code = '\\';
const char default_path_sep = '/';
const char suffix_sep = '.';
const size_t exe_suffix_len = 0;

static const char
usage_text[] = {
    "Usage: %s [OPTIONS] [targets]\n"
    "\n"
    "  Treats each token in targets as the path to an executable. Each target\n"
    "  enumerated is executed, and the output is processed after termination.\n"
    "  If the execution takes longer than a certain (configurable) amount of\n"
    "  time, the process is killed.\n"
    "\n"
    "    -d dir       Specify root directory for output reference files.\n"
    "    -h, -?       Display usage information and exit.\n"
    "    -t seconds   Set timeout before killing target (default is 10\n"
    "                 seconds).\n"
    "    -q           Set verbosity level to 0 (default).\n"
    "    -v           Increase verbosity of output.\n"
    "    -x opts      Specify command line options to pass to targets.\n"
    "    --           Terminate option processing and treat all arguments\n"
    "                 that follow as targets.\n"
    "    --compat     Use compatability mode test output parsing.\n"
    "    --nocompat   Use standard test output parsing (default).\n"
    "    --help       Display usage information and exit.\n"
    "    --exit=val   Exit immediately with the specified return code.\n"
    "    --sleep=sec  Sleep for the specified number of seconds.\n"
    "    --signal=sig Send itself the specified signal.\n"
    "    --ignore=sig Ignore the specified signal.\n"
    "\n"
    "  All short (single dash) options must be specified seperately.\n"
    "  If a short option takes a value, it may either be provided like\n"
    "  '-sval' or '-s val'.\n"
    "  If a long option take a value, it may either be provided like\n"
    "  '--option=value' or '--option value'.\n"
};

/**
   Display command line switches for program and terminate.

   @param status status code to exit with.
*/
void 
show_usage (int status)
{
    FILE* const where = status ? stderr : stdout;

    assert (0 != exe_name);

    fprintf (where, usage_text, exe_name);

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
    Helper function to produce 'Bad argument' error message.
    
    @param opt name of option encountered
    @param val invalid value found
*/
static void
bad_value (const char* opt, const char* val)
{
    assert (0 != opt);

    terminate (1, "Bad argument for %s: %s\n", opt, val);
}

/**
    Helper function to produce 'Missing argument' error message.
    
    @param opt name of option missing argument
*/
static void
missing_value (const char* opt)
{
    assert (0 != opt);

    terminate (1, "Missing argument for %s\n", opt);
}

/**
    Helper function to produce 'Unknown option' error message.
    
    @param opt name of option encountered
*/
static void
bad_option (const char* opt)
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
eval_options (int argc, char **argv)
{
    const char opt_timeout[]  = "-t";
    const char opt_data_dir[] = "-d";
    const char opt_t_flags[]  = "-x";
    const char opt_compat[]   = "--compat";
    const char opt_exit[]     = "--exit";
    const char opt_help[]     = "--help";
    const char opt_ignore[]   = "--ignore";
    const char opt_nocompat[] = "--nocompat";
    const char opt_signal[]   = "--signal";
    const char opt_sleep[]    = "--sleep";

    int i;

    assert (0 != argv);

    if (1 == argc || '-' != argv [1][0])
        return 1;

    for (i = 1; i < argc && '-' == argv [i][0]; ++i) {

        /* the name of the option being processed */
        const char* optname = argv [i];

        /* the option's argument, if any */
        const char* optarg  = 0;

        char* end = 0;

        switch (argv [i][1]) {
        case '?':
        case 'h':
            show_usage (0);
        case 'r':
            ++i; /* Ignore -r option (makefile compat) */
            break;
        case 't':
            optname = opt_timeout;
            optarg  = get_short_val (argv, &i);
            if (optarg) {
                timeout = strtol (optarg, &end, 10);
                if (*end || timeout < 0 || errno)
                    bad_value (optname, optarg);
            }
            else
                missing_value (optname);

            break;

        case 'd':
            optname = opt_data_dir;
            in_root = get_short_val (argv, &i);
            if (!in_root)
                missing_value (optname);
            break;
        case 'x':
            optname  = opt_t_flags;
            exe_opts = get_short_val (argv, &i);
            if (!exe_opts)
                missing_value (optname);
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

            if (   sizeof opt_compat - 1 == arglen
                && !memcmp (opt_compat, argv [i], sizeof opt_compat)) {
                compat  = 1;
                break;
            }
            else if (   sizeof opt_nocompat - 1 == arglen
                     && !memcmp (opt_nocompat, argv [i], sizeof opt_nocompat)) {
                compat  = 0;
                break;
            }
            else if (   sizeof opt_exit - 1 <= arglen
                     && !memcmp (opt_exit, argv [i], sizeof opt_exit - 1)) {
                optname = opt_exit;
                optarg  = get_long_val (argv, &i, sizeof opt_exit - 1);
                if (optarg && *optarg) {
                    const long code = strtol (optarg, &end, 10);
                    if ('\0' == *end && !errno)
                        exit (code);
                }
            }
            else if (   sizeof opt_help - 1 == arglen
                     && !memcmp (opt_help, argv [i], sizeof opt_help - 1)) {
                optname = opt_help;
                show_usage (0);
                break;
            }
            else if (   sizeof opt_sleep - 1 <= arglen
                     && !memcmp (opt_sleep, argv [i], sizeof opt_sleep - 1)) {
                optname = opt_sleep;
                optarg  = get_long_val (argv, &i, sizeof opt_sleep - 1);
                if (optarg && *optarg) {
                    const long nsec = strtol (optarg, &end, 10);
                    if ('\0' == *end && 0 <= nsec && !errno) {
                        sleep (nsec);
                        break;
                    }
                }
            }
            else if (   sizeof opt_signal - 1 <= arglen
                     && !memcmp (opt_signal, argv [i], sizeof opt_signal - 1)) {
                optname = opt_signal;
                optarg  = get_long_val (argv, &i, sizeof opt_signal - 1);
                if (optarg && *optarg) {
                    const long signo = get_signo (optarg);
                    if (0 <= signo) {
                        if (0 > raise (signo))
                            terminate (1, "raise(%s) failed: %s\n",
                                       get_signame (signo), strerror (errno));
                        break;
                    }
                }
            }
            else if (   sizeof opt_ignore <= arglen
                     && !memcmp (opt_ignore, argv [i], sizeof opt_ignore - 1)) {
                optname = opt_ignore;
                optarg  = get_long_val (argv, &i, sizeof opt_ignore);
                if (optarg && *optarg) {
                    const long signo = get_signo (optarg);
                    if (0 <= signo) {
                        struct sigaction act;
                        memset (&act, 0, sizeof act);
                        act.sa_handler = SIG_IGN;
                        if (0 > sigaction (signo, &act, 0))
                            terminate (1, "sigaction(%s, ...) failed: %s\n",
                                       get_signame (signo), strerror (errno));
                        break;
                    }
                }
            }

            /* fall through */
        }
        default:
            if (optarg) {
                if (*optarg)
                    bad_value (optname, optarg);
                else
                    missing_value (optname);
            }

            if (argv [i])
                bad_option (argv [i]);
            else
                missing_value (optname);
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
split_opt_string (const char* opts)
{
    char in_quote = 0;
    int in_escape = 0;
    int in_token = 0;
    const char *pos;
    char *target, *last;
    char **table_pos, **argv;
    size_t optlen;

    assert (0 != opts);

    optlen = strlen (opts);

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
        case escape_code:
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
