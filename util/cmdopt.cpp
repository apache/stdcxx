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

#include <assert.h> /* for assert() */
#include <ctype.h> /* for isspace */
#include <errno.h> /* for errno */
#include <signal.h> /* for raise, signal, SIG_IGN */
#include <stdio.h> /* for *printf, fputs */
#include <stdlib.h> /* for exit */
#include <string.h> /* for str* */
#if !defined (_WIN32) && !defined (_WIN64)
#  include <unistd.h> /* for sleep */

#  if defined (_XOPEN_UNIX)
#    include <sys/resource.h> /* for struct rlimit, RLIMIT_CORE, ... */
#  endif

#else
#  include <windows.h> /* for Sleep */
#endif   /* _WIN{32,64} */

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
#if !defined (_WIN32) && !defined (_WIN64)
const char escape_code = '\\';
const char default_path_sep = '/';
const char suffix_sep = '.';
const size_t exe_suffix_len = 0;
#else
const char escape_code = '^';
const char default_path_sep = '\\';
const char suffix_sep = '.';
const size_t exe_suffix_len = 4; /* strlen(".exe") == 4 */
#endif

struct limit_set child_limits = {
    { RLIM_SAVED_CUR, RLIM_SAVED_MAX },
    { RLIM_SAVED_CUR, RLIM_SAVED_MAX },
    { RLIM_SAVED_CUR, RLIM_SAVED_MAX },
    { RLIM_SAVED_CUR, RLIM_SAVED_MAX },
    { RLIM_SAVED_CUR, RLIM_SAVED_MAX },
    { RLIM_SAVED_CUR, RLIM_SAVED_MAX },
    { RLIM_SAVED_CUR, RLIM_SAVED_MAX },
    { RLIM_SAVED_CUR, RLIM_SAVED_MAX }
};

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
    "    --ulimit=lim Set child process usage limits (see below).\n"
    "\n"
    "  All short (single dash) options must be specified seperately.\n"
    "  If a short option takes a value, it may either be provided like\n"
    "  '-sval' or '-s val'.\n"
    "  If a long option take a value, it may either be provided like\n"
    "  '--option=value' or '--option value'.\n"
    "\n"
    "  --ulimit sets limits on how much of a given resource or resorces\n"
    "  child processes are allowed to utilize.  These limits take on two\n"
    "  forms, 'soft' and 'hard' limits.  Options are specified in the form\n"
    "  'resource:limit', where resource is a resource named below, and and\n"
    "  limit is a number, with value specifing the limit for the named\n"
    "  resource.  If multiple limits are to be set, they can be specified\n"
    "  either in multiple instances of the --ulimit switch, or by specifying\n"
    "  additional limits in the same call by seperating the pairs with\n"
    "  commas.  'Soft' limits are specified by providing the resource name\n"
    "  in lowercase letters, while 'hard' limits are specified by providing\n"
    "  the resource name in uppercase letters.  To set both limits, specify\n"
    "  the resource name in title case.\n"
    "\n"
    "  --ulimit modes:\n"
    "    core   Maximum size of core file, in bytes.\n"
    "    cpu    Maximum CPU time, in seconds.\n"
    "    data   Maximum data segment size, in bytes.\n"
    "    fsize  Maximum size of generated files, in bytes.\n"
    "    nofile Maximum number of open file descriptors.\n"
    "    stack  Maximum size of initial thread's stack, in bytes.\n"
    "    as     Maximum size of available memory, in bytes.\n"
    "\n"
    "  Note: Some operating systems lack support for some or all of the\n"
    "  ulimit modes.  If a system is unable to limit a given property, a\n"
    "  warning message will be produced.\n"
};

#if !defined (_WIN32) && !defined (_WIN64)

static void
rw_sleep (int seconds)
{
    sleep (seconds);
}


#ifdef __cplusplus

extern "C" {

#endif   /* __cplusplus */

static int
rw_signal (int signo, void (*func)(int))
{
    struct sigaction act;
    memset (&act, 0, sizeof act);

    // avoid extern "C"/"C++" mismatch due to an HP aCC 6 bug
    // (see STDCXX-291)
    if (func)
        memcpy (&act.sa_handler, &func, sizeof func);
    else
        act.sa_handler = 0;

    return 0 > sigaction (signo, &act, 0);
}

#ifdef __cplusplus

}   /* extern "C" */

#endif   /* __cplusplus */

#else   /* if defined (_WIN32) || defined (_WIN64) */

static void
rw_sleep (int seconds)
{
    Sleep (seconds * 1000);
}


static int
rw_signal (int signo, void (*func)(int))
{
    return SIG_ERR == signal (signo, func);
}

#endif   /* _WIN{32,64}*/

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
   Helper function to parse a ulimit value string

   @param opts ulimit value string to pares
   @see child_limits
*/
static bool
parse_limit_opts (const char* opts)
{
    static const struct {
        rw_rlimit* limit;
        const char* name;
        const char* caps;
        const char* mixd;
        size_t len;
    } limits[] = {
        {
#ifdef RLIMIT_CORE
            &child_limits.core,
#else
            0,
#endif   // RLIMIT_CORE
            "core", "CORE", "Core", 4 },
        {
#ifdef RLIMIT_CPU
            &child_limits.cpu,
#else
            0,
#endif   // RLIMIT_CPU
            "cpu", "CPU", "Cpu", 3 },
        {
#ifdef RLIMIT_DATA
            &child_limits.data,
#else
            0,
#endif   // RLIMIT_DATA
            "data", "DATA", "Data", 4 },
        {
#ifdef RLIMIT_FSIZE
            &child_limits.fsize,
#else
            0,
#endif   // RLIMIT_FSIZE
            "fsize", "FSIZE", "Fsize", 5 },
        {
#ifdef RLIMIT_NOFILE
            &child_limits.nofile,
#else
            0,
#endif   // RLIMIT_NOFILE
            "nofile", "NOFILE", "Nofile", 6 },
        {
#ifdef RLIMIT_STACK
            &child_limits.stack,
#else
            0,
#endif   // RLIMIT_STACK
            "stack", "STACK", "Stack", 5 },
        {
#ifdef RLIMIT_AS
            &child_limits.as,
#else
            0,
#endif   // RLIMIT_AS    
            "as", "AS", "As", 2 },
        { 0, 0, 0, 0, 0 }
    };

    const char* arg = opts;

    assert (0 != opts);

    while (arg && *arg) {

        const size_t arglen = strlen (arg);

        for (size_t i = 0; limits [i].name; ++i) {
            if (   limits [i].len < arglen
                && (   0 == memcmp (limits [i].name, arg, limits [i].len)
                    || 0 == memcmp (limits [i].caps, arg, limits [i].len)
                    || 0 == memcmp (limits [i].mixd, arg, limits [i].len))
                && ':' == arg [limits [i].len]) {

                // determine whether the hard limit and/or
                // the soft limit should be set
                const bool hard = isupper (arg [0]);
                const bool soft = islower (arg [1]);

                arg += limits [i].len + 1;

                if (!isdigit (*arg)) {
                    return 1;
                }

                char *end;
                const long lim = strtol (arg, &end, 10);

                arg = end;

                if ('\0' != *arg && ',' != *arg)
                    break;

                if (limits [i].limit) {
                    if (soft)
                        limits [i].limit->rlim_cur = lim;

                    if (hard)
                        limits [i].limit->rlim_max = lim;
                } else
                    warn ("Unable to process %s limit: Not supported\n", 
                          limits [i].name);
                break;
            }
        }

        if (',' == *arg) {
            ++arg;
        }
        else if ('\0' != *arg) {
            return 1;
        }
    }
    return 0;
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
    const char opt_ulimit[]   = "--ulimit";

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
                if (!isdigit (*optarg))
                    bad_value (optname, optarg);
                timeout = strtol (optarg, &end, 10);
                if (*end || errno)
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
                    if (!isdigit (*optarg))
                        bad_value (optname, optarg);
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
                    if (!isdigit (*optarg))
                        bad_value (optname, optarg);
                    const long nsec = strtol (optarg, &end, 10);
                    if ('\0' == *end && 0 <= nsec && !errno) {
                        rw_sleep (nsec);
                        break;
                    }
                }
            }
            else if (   sizeof opt_signal - 1 <= arglen
                     && !memcmp (opt_signal, argv [i], sizeof opt_signal - 1)) {
                optname = opt_signal;
                optarg  = get_long_val (argv, &i, sizeof opt_signal - 1);
                if (optarg && *optarg) {
                    const int signo = get_signo (optarg);
                    if (0 <= signo) {
                        if (0 > raise (signo))
                            terminate (1, "raise(%s) failed: %s\n",
                                       get_signame (signo), strerror (errno));
                        break;
                    }
                }
            }
            else if (   sizeof opt_ignore - 1 <= arglen
                     && !memcmp (opt_ignore, argv [i], sizeof opt_ignore - 1)) {
                optname = opt_ignore;
                optarg  = get_long_val (argv, &i, sizeof opt_ignore - 1);
                if (optarg && *optarg) {
                    const int signo = get_signo (optarg);
                    if (0 <= signo) {
                        if (rw_signal (signo, 0 /* SIG_IGN */))
                            terminate (1, "rw_signal(%s, ...) failed: %s\n",
                                       get_signame (signo), strerror (errno));
                        break;
                    }
                }
            }
            else if (   sizeof opt_ulimit - 1 <= arglen
                     && !memcmp (opt_ulimit, argv [i], sizeof opt_ulimit - 1)) {
                optname = opt_ulimit;
                optarg  = get_long_val (argv, &i, sizeof opt_ulimit - 1);
                if (optarg && *optarg) {
                    if (!parse_limit_opts (optarg)) {
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
