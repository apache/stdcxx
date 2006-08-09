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

#include <assert.h>
#include <errno.h>
#include <stdio.h> /* for printf, puts, FILE, f* */
#include <stdlib.h> /* for exit, free */
#include <string.h> /* for str* */

#include <ctype.h> /* for isspace */
#include <sys/types.h>

#include <sys/stat.h>

#include "cmdopt.h"
#include "exec.h"
#include "output.h"
#include "util.h"

#ifndef ENOENT
#  define ENOENT 2
#endif   // ENOENT

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

   @todo Figure out an escaping mechanism to allow '%x' to be passed to an
   executable

   @param target target to generate an argv array for
   @param argv program wide argv array for child processes
   @return processed argv array, usable in exec ()
*/
static char** const
merge_argv (char* const target, char* const argv [])
{
    const size_t tlen = strlen (target);
    char ** split = split_opt_string (target);
    unsigned i, arg_count = 0, spl_count = 0, wld_count = 0;

    assert (0 != target);
    assert (0 != argv);

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
   Preflight check to ensure that target is something that should be run.

   This method checks to see if target exists and is theoretically executable.
   A special case is the situation where the target name ends in .o, 
   indicating that the target only needed to compile, and doesn't need to
   be run.  If there are problems, or the target is a .o file, a result is 
   displayed and 0 is returned.

   Output messages produced:
     - COMP\n
       Target failed to compile
     - LINK\n
       Target failed to link
     - XPERM\n
       Target lacks execute permissions
     - 0\n
       Target .o file exists (check passes)

   @param target the path to the executable to check
   @return 1 if valid target to run, 0 otherwise.
*/
static int
check_target_ok (const char* target)
{
    struct stat file_info;
    int exists = 1;

    if (0 > stat (target, &file_info)) {
        if (ENOENT != errno) {
            warn ("Error stating %s: %s\n", target, strerror (errno));
            puts ("ERROR");
            return 0;
        }
        file_info.st_mode = 0; /* force mode on non-existant file to 0 */
        exists = 0; /* note that it doesn't exist */
    }
    if (0 == (file_info.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
        /* This is roughly equivlent to the -x bash operator.
           It checks if the file can be run, /not/ if we can run it
        */
        const size_t path_len = strlen (target);
        char* tmp_name;

        /* If target is a .o file, check if it exists */
        if ('.' == target [path_len-1] && 'o' == target [path_len]) {
            if (exists)
                puts ("     0");
            else
                puts ("  COMP");
            return 0;
        }
            
        /* If the target exists, it doesn't have valid permissions */
        if (exists) {
            puts (" XPERM");
            return 0;
        }

        /* Otherwise, check for the .o file */
        tmp_name = (char*)RW_MALLOC (path_len + 3);
        memcpy (tmp_name, target, path_len + 1);
        strcat (tmp_name,".o");

        if (0 > stat (tmp_name, &file_info)) {
            if (ENOENT != errno) {
                warn ("Error stating %s: %s\n", tmp_name, strerror (errno));
                puts ("ERROR");
            }
            else
                puts ("  COMP");
        }
        else {
            puts ("  LINK");
        }
                
        free (tmp_name);
        return 0;
    }
    return 1;
}

/**
   Post target execution result analysis.

   This method examines the content of result, dispatching the processing
   to check_test () or check_example () if target had a return code of 0.
   Otherwise, this method determines why target terminated, based on the
   return code and outputs that reason.

   Output messages produced:
     - EXIST\n
       Return code 126 (target doesn't exist?)
     - EXEC\n
       Return code 127 (target couldn't be run?)
     - NKILL\n
       Couldn't kill the child executable
     - n\n
       Child exited with the non-zero return code n
     - name\n
       Child terminated upon recieving the signal SIGname

   @param target the path to the executable that was run
   @param exec_name the short name of the executable
   @param result the return code data from running the target
   @see check_test ()
   @see check_example ()
*/
static void
process_results (const char* target, const struct exec_attrs* result)
{
    if (0 == result->status) {
        parse_output (target);
    } 
    else if (WIFEXITED (result->status)) {
        const int retcode = WEXITSTATUS (result->status);
        switch (retcode) {
        case 126:
            puts (" EXIST");
            break;
        case 127:
            puts ("  EXEC");
            break;
        default:
            printf ("%6d\n", retcode);
        }
    }
    else if (WIFSIGNALED (result->status)) {
        printf ("%6s\n", get_signame (WTERMSIG (result->status)));
    }
    else if (WIFSTOPPED (result->status)) {
        printf ("%6s\n", get_signame (WSTOPSIG (result->status)));
    }
    else if (-1 == result->status && -1 == result->killed) {
        puts (" NKILL");
    }
    else {
        printf ("(%d|%d)\n", result->status, result->killed);
    }
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
        mark = ('/' == *pos || '\\' == *pos) ? pos + 1 : mark;

    return mark;
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
run_target (char* target, char** argv)
{
    struct exec_attrs status;
    char** childargv = merge_argv (target, argv);

    assert (0 != target);
    assert (0 != argv);
    assert (0 != childargv);

    target_name = rw_basename (childargv [0]);

    printf ("%-25.25s ", target_name);
    fflush (stdout);

    if (!check_target_ok (childargv [0]))
        return;

    status = exec_file (childargv);

    process_results (childargv [0], &status);
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
    exe_name = argv [0];

    if (1 < argc && '-' == argv [1][0]) {
        const int nopts = eval_options (argc, argv);

        if (0 > nopts)
            return 1;

        argc -= nopts;
        argv += nopts;
    }
    else {
        --argc;
        ++argv;
    }

    if (0 < argc) {
        int i;
        char** childargv = split_opt_string (exe_opts);

        assert (0 != childargv);
        puts ("NAME                      STATUS ASSERTS FAILED PERCNT");

        for (i = 0; i < argc; ++i) {
            run_target (argv [i], childargv);
        }

        if (childargv [0])
            free (childargv [0]);
        free (childargv);
    }

    return 0;
}
