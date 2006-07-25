/************************************************************************
 *
 * output.cpp - Definitions of the result parsing subsystem
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

#include <unistd.h>
    /* for close, dup, exec, fork - remove when removing diff dependancy*/
#include <sys/types.h>
#include <sys/wait.h> /* for waitpid, W* */

#include <sys/stat.h>

#include "cmdopt.h"
#include "exec.h"
    /* for get_signame - remove when removing diff dependancy */
#include "util.h"

#include "output.h"

#ifndef ENOENT
#  define ENOENT 2
#endif   // ENOENT


/**
   Parses output file out_name for test exec_name.
   
   This method tries to open out_name.  If this succedes, it then searches
   the file for a result summary as produced by the rwtest driver.
   If a result summary is found, it then parses the result and displays the
   result of the parsing.  Otherwise, and appropriate error messages is 
   displayed.

   Output messages produced:
     - OUTPUT\n
       Output file couldn't be opened
     - OFLOW\n
       Assertions counted overflows the (internal)storage variable
     - FORMAT\n
       Result summary can't be found in output file
     - 0 [assertion count] [failed assertion count] [pass rate]\n
       Parsed results

   @param exec_name the name of the executable that generated the output file
   @param out_name the name of the output file being parsed
*/
static void
check_test (const char* exec_name, const char* out_name)
{
    FILE* data = fopen (out_name, "r");
    unsigned r_lvl = 0, r_active = 0, r_total = 0;
    unsigned asserts = 0, failed = 0;
    int fmt_ok = 0;
    unsigned fsm = 0;
    char tok;

    assert (0 != exec_name);
    assert (0 != out_name);

    if (0 == data) {
        if (ENOENT != errno) {
            printf ("Error opening %s: %s\n", out_name, strerror (errno));
            return;
        }
        puts ("OUTPUT");
        return;
    }

    for (tok = fgetc (data); fsm < 6 && !feof (data); tok = fgetc (data)) {
        switch (tok) {
        case '\n':
            fsm = 1;
            break;
        case '#':
            fsm = (1 == fsm) ? 2 : 0;
            break;
        case ' ':
            fsm = (2 == fsm) ? 3 : ((4 == fsm) ? 5 : 0);
            break;
        case '|':
            fsm = (3 == fsm) ? 4 : 0;
            break;
        case '(':
            if (5 == fsm) {
                fseek (data, -6, SEEK_CUR);
                fsm++;
                break;
            }
        default:
            fsm = 0;
        }
    }

    if (!feof (data)) {
        while (3 == fscanf (data, "# | (S%u) %*s | %u | %u | %*u%% |\n", 
                            &r_lvl, &r_active, &r_total)) {
            if (6 < r_lvl) {
                /* The 0.new tests produces errors, and are all 
                   expected to be active, so invert the total */
                if (8 == r_lvl && 0 == strcmp (exec_name,"0.new"))
                    r_active = r_total-r_active;
                failed += r_active;
                asserts += r_total;
                if (failed < r_active || asserts < r_total) {
                    puts (" OFLOW");
                    return;
                }
            }
            /* else if (1 < r_lvl) warning*/
            fmt_ok = 1;
        }
    }
    
    if (fmt_ok) {
        unsigned pcnt = 0;
        if (asserts) {
            pcnt = 100 * (asserts - failed) / asserts;
        }
        printf ("     0 %6d %6d %5d%%\n", asserts, failed, pcnt);
    }
    else {
        puts ("FORMAT");
    }

    fclose (data);
}

/**
   Parses output file out_name for test exec_name.
   
   This method is a reimplementation of check_test ().  The difference between
   this method and check_test () is how it parses the results.  This version
   parses compatability layer output, rather than the test driver output.

   @param exec_name the name of the executable that generated the output file
   @param out_name the name of the output file being parsed
   @see check_test ()
*/
static void
check_compat_test (const char* out_name)
{
    FILE* data = fopen (out_name, "r");
    unsigned asserts = 0, failed = 0;
    int read = 0;
    unsigned fsm = 0;
    char tok;

    assert (0 != out_name);

    if (0 == data) {
        if (ENOENT != errno) {
            printf ("Error opening %s: %s\n", out_name, strerror (errno));
            return;
        }
        puts ("OUTPUT");
        return;
    }

    fseek (data, -64, SEEK_END); /* Seek near the end of the file */

    for (tok = fgetc (data); fsm < 4 && !feof (data); tok = fgetc (data)) {
        switch (tok) {
        case '\n':
            fsm = 1;
            break;
        case '#':
            if (1 == fsm || 2 == fsm)
                ++fsm;
            else
                fsm = 0;
            break;
        case ' ':
            if (3 == fsm) {
                ++fsm;
                break;
            }
        default:
            fsm = 0;
        }
    }

    if (!feof (data)) { /* leading "## A" eaten above */
        read = fscanf (data, "ssertions = %u\n## FailedAssertions = %u",
                        &asserts, &failed);
    }

    if (2 == read) {
        unsigned pcnt = 0;
        if (asserts) {
            pcnt = 100 * (asserts - failed) / asserts;
        }
        printf ("     0 %6d %6d %5d%%\n", asserts, failed, pcnt);
    }
    else {
        puts ("FORMAT");
    }

    fclose (data);
}

/**
   Sanity test macro for file descriptor operations.
   
   @killme this should be removed after removing the dependancy on the
   posix diff utility
   
   @param op human understandable name for operation
   @param x variable to test the result for
   @see check_example
*/
#define FILE_TEST(op, x)                                                \
    if (-1==(x))                                                        \
        terminate (2, op " failed: %s\n", strerror (errno))

/**
   Parses output file out_name for the example exec_name.
   
   This method tries to compare out_name against a reference output file and
   displays a result code.  The reference file is determined by exec_name and
   the in_root global variable.
   This method relies on the POSIX diff utility at this time.  This dependancy
   needs to be removed, though doing so will require a rewrite of the method.

   Reference file locations:
     - [in_root]/manual/out/[exec_name].out
     - [in_root]/tutorial/out/[exec_name].out

   Output messages produced:
     - NOREF\n
       No reference file could be found
     - OUTPUT\n
       Output file differs from the reference file
     - 0\n
       Output file matches the reference file (check passes)

   @param exec_name the name of the executable that generated the output file
   @param out_name the name of the output file being parsed
   @see in_root
   @see FILE_TEST ()
   @todo remove dependancy on POSIX diff utility
*/
static void
check_example (const char* const exec_name, const char* const out_name)
{
    struct stat file_info;
    const size_t root_len = strlen (in_root);
    char* const ref_name = (char*)RW_MALLOC (root_len 
                                            + strlen (exec_name) + 19);
    int state = -1;

    assert (0 != in_root);
    assert (0 < root_len);
    assert (0 != exec_name);
    assert (0 != out_name);

    /* Try in_root/manual/out/exec_name.out */
    memcpy (ref_name, in_root, root_len+1);
    strcat (ref_name, "/manual/out/");
    strcat (ref_name, exec_name);
    strcat (ref_name, ".out");

    if (0 > stat (ref_name, &file_info)) {
        if (ENOENT != errno) {
            printf ("stat (%s) error: %s\n", ref_name, strerror (errno));
            free (ref_name);
            return;
        }
                        
        /* If that doesn't exist, try 
           in_root/tutorial/out/exec_name.out */
        memcpy (ref_name, in_root, root_len+1);
        strcat (ref_name, "/tutorial/out/");
        strcat (ref_name, exec_name);
        strcat (ref_name, ".out");

        if (0 > stat (ref_name, &file_info)) {
            if (ENOENT != errno)
                printf ("stat (%s) error: %s\n", ref_name, strerror (errno));
            else
                puts (" NOREF");

            free (ref_name);
            return;
        }
    }

    const pid_t child_pid = fork ();

    if (0 == child_pid) {   /* child */
        /* Cache stdout (hopefully) for use if execv () fails */
        int error_cache = dup (2);
        FILE* error_file;
        FILE_TEST ("dup (stderr)", error_cache);

        FILE_TEST ("close (stdin)",close (0));
        FILE_TEST ("close (stdin)",close (1));
        FILE_TEST ("close (stderr)",close (2));

        /* Todo: diff with --strip-trailing-cr on windows */
        execlp ("diff", "diff", ref_name, out_name, (char *)0);

        if ((error_file = fdopen (error_cache, "a")))
            fprintf (error_file, "execlp (\"diff\", ...) error: %s\n",
                     strerror (errno));

        exit (2);
    }

    while (1) {
        const pid_t wait_pid = waitpid (child_pid, &state, 0);

        if (child_pid == wait_pid) {

            if (WIFEXITED (state)) {
                const int retcode = WEXITSTATUS (state);                
                switch (retcode) {
                case 0:
                    puts ("     0");
                    break;
                case 1:
                    puts ("OUTPUT");
                    break;
                default:
                    printf ("diff returned %d\n", retcode);
                }
                break;
            }
            else if (WIFSIGNALED (state)) {
                printf ("diff exited with %s\n", 
                        get_signame (WTERMSIG (state)));
                break;
            }
/*
            else if (WIFSTOPPED (state)) {
                printf ("process %d stopped\n", (int)child_pid);
            }
            else if (WIFCONTINUED (state)) {
                printf ("process %d continued\n", (int)child_pid);
            }
*/
        }
    }

    free (ref_name);
}

/**
   Dispatch method that invoks another method to parse the output file.

   @param target the path to the executable that generated the output file
   @param exec_name the name of the executable that generated the output file
   @see check_test ()
   @see check_compat_test ()
   @see check_example ()
*/
void
parse_output (const char* target, const char* exec_name)
{
    const size_t path_len = strlen (target);
    char* const out_name = (char*)RW_MALLOC (path_len + 5);
    memcpy (out_name, target, path_len + 1);
    strcat (out_name,".out");

    if (!strlen (in_root)) {
        /* If there is not an input directory, look at the assertion tags */
        if (!compat)
            check_test (exec_name, out_name);
        else
            check_compat_test (out_name);
    }
    else {
        /* Otherwise, diff against the output file */
        check_example (exec_name, out_name);
    }
    free (out_name);
}
