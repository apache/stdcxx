/************************************************************************
 *
 * runall.cpp - Core logic for the exec utility
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

#include <assert.h>     /* for assert() */
#include <errno.h>      /* for errno */
#include <stdlib.h>     /* for exit(), free() */
#include <string.h>     /* for memcpy(), ... */
#include <stdio.h>      /* for FILE, fopen(), ... */

#include <ctype.h>      /* for isspace */
#include <limits.h>     /* for PATH_MAX */
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#  include <sys/wait.h>   /* for WIFEXITED(), ... */
#endif

#include "cmdopt.h"
#include "display.h"
#include "exec.h"
#include "output.h"
#include "util.h"

#include "target.h"

#ifndef ENOENT
#  define ENOENT 2
#endif   /* ENOENT */

#ifndef S_IXUSR
#  define S_IXUSR 0100
#endif   /* S_IXUSR */

#ifndef S_IXGRP
#  define S_IXGRP 0010
#endif   /* S_IXGRP */

#ifndef S_IXOTH
#  define S_IXOTH 0001
#endif   /* S_IXOTH */

#if !defined (PATH_MAX) || PATH_MAX < 128 || 4096 < PATH_MAX
   // deal  with undefined, bogus, or excessive values
#  undef  PATH_MAX
#  define PATH_MAX   1024
#endif

/**
   Utility function to rework the argv array

   target is either a 'bare' executable or a 'complex' executable.  A bare
   executable is the path to an executable.  A complex executable is the
   path to the executable, followed by a series of command line arguments.

   If target is a bare executable, the arguments in the returned argv array
   will be the target followed by the contents of the recieved argv array.
   
   If target is a complex executable, the arguments in the returned argv
   array will be target, transformed into an array.  If a token in the 
   argument string is '%x' (no quotes), the contents of the provided argv 
   array will be inserted into the return array at that point.

   It is the responsibility of the caller to free() the returned blocks of
   memory, which were allocated by a call to split_opt_string().

   @todo Figure out an escaping mechanism to allow '%x' to be passed to an
   executable

   @param target target to generate an argv array for
   @param argv program wide argv array for child processes
   @return processed argv array, usable in exec ()
*/
static char**
merge_argv (const char* target, char* const argv [])
{
    size_t tlen;
    char ** split;
    unsigned i, arg_count = 0, spl_count = 0, wld_count = 0;

    assert (0 != target);
    assert (0 != argv);

    tlen = strlen (target);
    split = split_opt_string (target);

    /* If the split of target only contains a single value, we may have a 
     bare executable name */
    if (!split [1]) {
        /* Check if last character in the target is whitespace */
        if (isspace (target [tlen])) {
            /* If it is, we've got a complex executable with no arguments.
               Therfore, return it as is.
            */
            return split;
        } /* Otherwise, it's a bare executable, so append argv */

        /* Figure out how many arguments we've got in argv*/
        for (/* none */; argv [arg_count]; ++arg_count);

        /* reallocate memory for copying them, extending the buffer */
        split = (char**)RW_REALLOC (split, (arg_count + 2) * sizeof (char*));
            
        /* And copy the pointers */
        for (i=0; i < arg_count; ++i)
            split [i+1] = argv [i];

        /* Then terminate the array*/
        split [++i] = (char*)0;

        return split;
    } /* Otherwise, it's a complex executable with 1 or more arguments */

    /* Figure out how many instances of '%x' we've got */
    for (spl_count = 1; split [spl_count]; ++spl_count) {
        if ('%' == split [spl_count][0] && 'x' == split [spl_count][1] 
            && '\0' == split [spl_count][2])
            ++wld_count;
    }

    /* If we don't have any instances of '%x', we have a valid argv array, 
       so return it as it is.
    */
    if (0 == wld_count)
        return split;

    /* Now we need to determine how large the argv array is */
    for (/* none */; argv [arg_count]; ++arg_count);

    if (0 == arg_count) {
        /* We want to shrink the array, removing the '%x' terms*/
        unsigned found = 0;
        for (i = 1; i <= spl_count; ++i) {
            if (split [i] && '%' == split [i][0] && 'x' == split [i][1] 
                && '\0' == split [i][2])
                ++found;
            else
                split [i - found] = split [i];
        }
    }
    else if (1 == arg_count) {
        /* We need to replace all the %x terms with argv [0] */
        
        for (i = 1; i < spl_count; ++i) {
            if ('%' == split [i][0] && 'x' == split [i][1] 
                && '\0' == split [i][2])
                split [i] = argv [0];
        }
    }
    else {
        /* We need to resize the split array to hold the insertion (s) */
        /* First, we realloc the array */
        const unsigned new_len = spl_count + (arg_count - 1) * wld_count;
        split = (char**)RW_REALLOC (split, sizeof (char**) * new_len);

        /* Then we itterate backwards through the split array, transcribing 
           elements as we go.  We have to go backwards, so we don't clobber
           data in the process.
        */
        for (/* none */; wld_count; --spl_count) {
            if (split [spl_count] && '%' == split [spl_count][0] 
                && 'x' == split [spl_count][1] 
                && '\0' == split [spl_count][2]) {
                --wld_count;
                for (i = arg_count; i; --i) {
                    split [spl_count + (arg_count - 1) * wld_count + i - 1] = 
                        argv [i - 1];
                }
            }
            else
                split [spl_count + (arg_count - 1) * wld_count] = 
                    split [spl_count];
        }
    }
    return split;
}

/**
   Arbitrary constant controling static read buffer size.

   @see count_warnings
*/
#define READ_BUF_LEN 64

/**
   Compiler/linker output parser.

   This method tries to open the compiler/linker log file, based on the name
   of a the target file that was generated by the compiler/linker.
   Upon opening the file, it tries to count the number of warnings present.

   @param target name of target to parse the log for
   @param counter pointer to the counter to count warnings in.
   @param match string to match when detecting a warning.
*/
static
void count_warnings (const char* const target, unsigned* counter, 
                     const char* const match)
{
    size_t path_len;
    char* tmp_name;
    FILE* data;

    assert (0 != target);
    assert (0 != counter);

    if (0 == match)
        return; /* Don't have a match string, so don't tally warnings */

    path_len = strlen (target);
    tmp_name = (char*)RW_MALLOC (path_len + 5);
    memcpy (tmp_name, target, path_len + 1);
    strcat (tmp_name,".log");

    data = fopen (tmp_name, "r");
    if (data) {
        size_t pos = 0, limit = strlen (match);
        while (!feof (data)) {
            char buf [READ_BUF_LEN];
            size_t nread;   /* bytes read */

            /* First, read a block from the file into the buffer */
            nread = fread (buf, 1, sizeof buf, data);
            if (ferror (data)) {
                warn ("Error reading %s: %s\n", tmp_name, strerror (errno));
                break;
            }

            /* Then, look for the search string within it. */
            for (size_t i = 0; i < nread; ++i) {
                if (buf [i] != match [pos])
                    pos = 0;
                else if (++pos == limit) {
                    pos = 0;
                    ++*counter;
                }
            }
        }

        fclose (data);
    }
    else if (ENOENT != errno)
        warn ("Error opening %s: %s\n", tmp_name, strerror (errno));

    free (tmp_name);
}

/**
   Preflight check to ensure that target is something that should be run.

   This method checks to see if target exists and is theoretically executable.
   If a problem is detected, the condition is recorded in the status structure
   and 0 is returned.  This method also attempts to determine the number of 
   compile and link warnings that occurred, using count_warnings.

   A special case is the situation where the target name ends in .o, 
   indicating that the target only needed to compile, and doesn't need to
   be run.  Processing for this case is currently disabled as it is unused.

   @param target the path to the executable to check
   @param status status object to record results in.
   @return 1 if valid target to run, 0 otherwise.
*/
static int
check_target_ok (const char* target, struct target_status* status)
{
    struct stat file_info;
    int exists = 1;
    size_t path_len;
    char* tmp_name;

    assert (0 != target);
    assert (0 != status);

    path_len = strlen (target);

#ifndef _WIN32
    /* Otherwise, check for the .o file on non-windows systems */
    tmp_name = (char*)RW_MALLOC (path_len + 3);
    memcpy (tmp_name, target, path_len + 1);
    strcat (tmp_name,".o");
#else
    /* Or the target\target.obj file on windows systems*/
    {
        const char* const target_name = get_target ();
        size_t target_len = strlen (target_name);
        size_t tmp_len = path_len + target_len - 2;
        /* - 2 comes from removing 4 characters (extra .exe) and adding 2 
           characters (\ directory seperator and trailing null) */
        tmp_name = (char*)RW_MALLOC (tmp_len);
        memcpy (tmp_name, target, path_len - 4);
        tmp_name [path_len - 4] = default_path_sep;
        memcpy (tmp_name + path_len - 3, target_name, target_len);
        tmp_name [tmp_len - 4] = 'o';
        tmp_name [tmp_len - 3] = 'b';
        tmp_name [tmp_len - 2] = 'j';
        tmp_name [tmp_len - 1] = '\0';
    }
#endif   /* _WIN32 */

    count_warnings (target, &status->l_warn, "warning:");
    count_warnings (tmp_name, &status->c_warn, "warning:");

    if (0 > stat (target, &file_info)) {
        if (ENOENT != errno) {
            warn ("Error stating %s: %s\n", target, strerror (errno));
            status->status = ST_SYSTEM_ERROR;
            free (tmp_name);
            return 0;
        }
        file_info.st_mode = 0; /* force mode on non-existant file to 0 */
        exists = 0; /* note that it doesn't exist */
    }

    if (0 == (file_info.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
        /* This is roughly equivlent to the -x bash operator.
           It checks if the file can be run, /not/ if we can run it
        */

#if 0 /* Disable .o target check as unused */
        /* If target is a .o file, check if it exists */
        if ('.' == target [path_len-1] && 'o' == target [path_len]) {
            if (!exists)
                status->status = ST_COMPILE;
            free (tmp_name);
            return 0;
        }
#endif
            
        /* If the target exists, it doesn't have valid permissions */
        if (exists) {
            status->status = ST_EXECUTE_FLAG;
            free (tmp_name);
            return 0;
        }

        if (0 > stat (tmp_name, &file_info)) {
            if (ENOENT != errno) {
                warn ("Error stating %s: %s\n", tmp_name, strerror (errno));
                status->status = ST_SYSTEM_ERROR;
            }
            else
                status->status = ST_COMPILE;
        }
        else {
            status->status = ST_LINK;
        }
                
        free (tmp_name);
        return 0;
    }
    free (tmp_name);
    return 1;
}

/**
   (Re)implementation of the POSIX basename function.

   This is a simplistic (re)implementation of the basename function
   specified in the XSI extension to the IEEE Std 1003.1 (POSIX) standard.

   @warning this method is UTF-8 unsafe
   @warning this method assumes there are no trailing slashes in the path name
   @warning this method retuns a pointer referencing a position inside the 
   provided path.  As such, the returned string isn't a new string, but rather 
   an alias to the provided string.
   @param path path name to determine the basename for
   @return final element in path name
*/
static const char*
rw_basename (const char* path)
{
    const char *pos, *mark;

    assert (0 != path);

    for (mark = pos = path; '\0' != *pos; ++pos)
#ifndef _WIN32
        mark = (default_path_sep == *pos) ? pos + 1 : mark;
#else
        mark = (default_path_sep == *pos || '/' == *pos) ? pos + 1 : mark;
#endif   /* _WIN32 */

    return mark;
}

static const char* target_name;

const char* get_target ()
{
    return target_name;
}

/**
   High level method to run target, using childargv as arguments.

   This method preflights the execution of target, runs it using the watchdog
   subsystem, then processes the results from that subsystem.

   @param target path to target executable
   @param null ((char*)0) terminated array of parameters to be passed to 
   target when it is run
   @see check_target_ok
   @see exec_file
   @see process_results
*/
static void
run_target (struct target_status     *summary,
            const char               *target,
            const struct target_opts *target_template)
{
    struct target_opts options;
    struct target_status results;

    assert (0 != target);
    assert (0 != target_template);
    assert (0 != target_template->argv);

    memcpy (&options, target_template, sizeof options);
    memset (&results, 0, sizeof results);

    /* create the argv array for this target */
    options.argv = merge_argv (target, options.argv);

    assert (0 != options.argv);
    assert (0 != options.argv [0]);

    target_name = rw_basename (options.argv [0]);

    /* create the names of files to redirect stdin and stdout */
    options.infname  = input_name (options.data_dir, target_name);
    options.outfname = output_name (options.argv [0]);

    print_target (&options);

    if (check_target_ok (options.argv [0], &results)) {
        exec_file (&options, &results);
        if (0 == results.exit && 0 == results.signaled)
            parse_output (&options, &results);
    }

    print_status (&results);

    if (summary) {
        /* increment summary counters */
        if (0 == results.signaled && results.exit)
            ++summary->exit;

        /* add cumulative times (when valid) */
        if (results.usr_time != (clock_t)-1)
            summary->usr_time += results.usr_time;

        if (results.sys_time != (clock_t)-1)
            summary->sys_time += results.sys_time;

        if (results.wall_time != (clock_t)-1)
            summary->wall_time += results.wall_time;

        summary->signaled += results.signaled;
        summary->c_warn   += results.c_warn;
        summary->l_warn   += results.l_warn;
        summary->t_warn   += results.t_warn;

        if ((unsigned)-1 != results.assert) {
            /* increment assertion counters only when they're valid */
            summary->assert += results.assert;
            summary->failed += results.failed;
        }
    }

    /* free data dynamically allocated for this target alone */
    free (options.argv [0]);
    free (options.argv);
    free ((char*)options.infname);
    free ((char*)options.outfname);
}


/**
   Entry point to the application.

   Passes arguments to the option processing subsystem, then processes all
   remaing arguments as targets using run_target

   @param argc number of arguments recieved
   @param argv array of arguments recieved
   @return 0 upon successfull completeion of execution
*/

int
main (int argc, char *argv [])
{
    struct target_opts target_template;
    const char* exe_opts = "";
    const char* const* const saved_argv = (const char* const*)argv;

    exe_name = argv [0];

    if (1 < argc && '-' == argv [1][0]) {
        const int nopts =
            eval_options (argc, argv, &target_template, &exe_opts);

        if (0 > nopts)
            return 1;

        argc -= nopts;
        argv += nopts;
    }
    else {
        /* initialize data members */
        memset (&target_template, 0, sizeof target_template);

        --argc;
        ++argv;
    }

    /* set the program output mode */
    if (target_template.verbose)
        set_output_format (FMT_VERBOSE);
    else
        set_output_format (FMT_PLAIN);

    if (0 < argc) {
        struct target_status summary;

        int i;

        target_template.argv = split_opt_string (exe_opts);

        assert (0 != target_template.argv);

        /* print out the program's argv array in verbose mode */
        print_header (target_template.verbose ? saved_argv : 0);

        memset (&summary, 0, sizeof summary);

        /* number of program's executed */
        int progs_count = 0;

        for (i = 0; i < argc; ++i) {
            const char* target = argv [i];

            if ('@' == target [0]) {
                /* read targets from specified file */
                const char* lst_name = target + 1;
                FILE* lst = fopen (lst_name, "r");
                if (0 == lst) {
                    warn ("Error opening %s: %s\n", lst_name, strerror (errno));
                    break;
                }

                while (!feof (lst)) {
                    char buf [PATH_MAX];
                    target = fgets (buf, int (sizeof buf), lst);

                    if (ferror (lst)) {
                        warn ("Error reading %s: %s\n", lst_name, strerror (errno));
                        break;
                    }

                    if (target) {
                        /* remove terminating newline character if present */
                        assert (buf == target);
                        if (char* pos = strchr (buf, '\n'))
                            *pos = '\0';
                        if (*target) {
                            ++progs_count;
                            run_target (&summary, target, &target_template);
                        }
                    }
                }

                fclose (lst);
            }
            else {
                ++progs_count;
                run_target (&summary, target, &target_template);
            }
        }

        print_footer (progs_count, &summary);

        if (target_template.argv [0])
            free (target_template.argv [0]);
        free (target_template.argv);
    }

    free (target_template.core);
    free (target_template.cpu);
    free (target_template.data);
    free (target_template.fsize);
    free (target_template.nofile);
    free (target_template.stack);
    free (target_template.as);

    return 0;
}
