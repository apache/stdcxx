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

#include <sys/types.h>

#include <sys/stat.h>

#include "cmdopt.h"
#include "util.h"

#include "output.h"

#ifndef ENOENT
#  define ENOENT 2
#endif   /* ENOENT */


/**
   Parses output file out_name for test target_name.
   
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

   @param data pointer to file structure for output file being parsed
*/
static void
check_test (FILE* data)
{
    unsigned r_lvl = 0, r_active = 0, r_total = 0;
    unsigned asserts = 0, failed = 0;
    int fmt_ok = 0;
    unsigned fsm = 0;
    char tok;

    assert (0 != target_name);
    assert (0 != data);

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
                if (8 == r_lvl && 0 == strcmp (target_name, "0.new"))
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
        printf ("     0 %7d %6d %5d%%\n", asserts, failed, pcnt);
    }
    else {
        puts ("FORMAT");
    }
}

/**
   Parses output file out_name for test target_name.
   
   This method is a reimplementation of check_test ().  The difference between
   this method and check_test () is how it parses the results.  This version
   parses compatability layer output, rather than the test driver output.

   @param data pointer to file structure for output file being parsed
   @see check_test ()
*/
static void
check_compat_test (FILE* data)
{
    unsigned asserts = 0, failed = 0;
    int read = 0;
    unsigned fsm = 0;
    char tok;

    assert (0 != data);

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
        printf ("     0 %7d %6d %5d%%\n", asserts, failed, pcnt);
    }
    else {
        puts ("FORMAT");
    }
}

/**
   Arbitrary constant controling static read buffer size.

   @see check_example ()
*/
#define DELTA_BUF_LEN 64

/**
   Parses output file out_name for the example target_name.
   
   This method tries to compare out_name against a reference output file and
   displays a result code.  The reference file is determined by target_name and
   the in_root global variable.
   This method relies on the POSIX diff utility at this time.  This dependancy
   needs to be removed, though doing so will require a rewrite of the method.

   Reference file locations:
     - [in_root]/manual/out/[target_name].out
     - [in_root]/tutorial/out/[target_name].out

   Output messages produced:
     - NOREF\n
       No reference file could be found
     - OUTPUT\n
       Output file differs from the reference file
     - 0\n
       Output file matches the reference file (check passes)

   @todo add logic to handle differing line endings (CR vs LF vs CRLF)

   @param output_name the name of the output file
   @param data pointer to file structure for output file being parsed
   @see in_root
*/
static void
check_example (char* const out_name, FILE* fout)
{
    struct stat file_info;
    const size_t root_len = strlen (in_root);
    char* const ref_name = (char*)RW_MALLOC (root_len 
                                             + strlen (target_name) + 19);

    FILE* fref;   /* reference file (expected output) */

    assert (0 != in_root);
    assert (0 < root_len);
    assert (0 != target_name);
    assert (0 != fout);

    /* Try in_root/manual/out/target_name.out */
    memcpy (ref_name, in_root, root_len+1);
    strcat (ref_name, "/manual/out/");
    strcat (ref_name, target_name);
    strcat (ref_name, ".out");

    if (0 > stat (ref_name, &file_info)) {
        if (ENOENT != errno) {
            warn ("stat (%s) error: %s\n", exe_name, ref_name, 
                  strerror (errno));
            puts ("BADREF");
            free (ref_name);
            return;
        }
                        
        /* If that doesn't exist, try 
           in_root/tutorial/out/target_name.out */
        memcpy (ref_name, in_root, root_len+1);
        strcat (ref_name, "/tutorial/out/");
        strcat (ref_name, target_name);
        strcat (ref_name, ".out");

        if (0 > stat (ref_name, &file_info)) {
            if (ENOENT != errno) {
                warn ("stat (%s) error: %s\n", exe_name, ref_name, 
                      strerror (errno));
                puts ("BADREF");
            }
            else
                puts (" NOREF");

            free (ref_name);
            return;
        }
    }

    fref = fopen (ref_name, "r");

    if (0 == fref) {
        int cache = errno; /* caching errno, as puts could alter it */
        if (ENOENT != cache)
            warn ("Error opening %s: %s\n", ref_name, strerror (cache));
        puts ("BADREF");
    }
    else {
        int match = 1;   /* do the two files match? */

        while (!feof (fref) && !feof (fout)) {

            char buf [2][DELTA_BUF_LEN];

            size_t nread [2];   /* bytes read from the output/ref file */

            /* First, read a block from the files into the buffer */
            nread [0] = fread (buf [0], sizeof buf [0], 1, fout);
            if (ferror (fout)) {
                warn ("Error reading %s: %s\n", out_name, strerror (errno));
                match = 0;
                break;
            }

            nread [1] = fread (buf [1], sizeof buf [1], 1, fref);
            if (ferror (fref)) {
                warn ("Error reading %s: %s\n", ref_name, strerror (errno));
                match = 0;
                break;
            }

            /* Then, check if the amount of data read or the state of the 
               files differs
            */
            if (nread [0] != nread [1] || feof (fref) != feof (fout)) {
                match = 0;
                break;
            }

            /* Finally, check if the contents of the buffers differ */
            if (0 != memcmp (buf [0], buf [1], nread [0])) {
                match = 0;
                break;
            }
        }

        if (match)
            puts ("     0");
        else
            puts ("OUTPUT");

        fclose (fref);
    }
    free (ref_name);
}

/**
   Umbrella (dispatch) function to analyse the (saved) output of target

   @param target the path to the executable that generated the output file 
   being parsed.
*/
void
parse_output (const char* target)
{
    const size_t path_len = strlen (target);
    char* const out_name = (char*)RW_MALLOC (path_len + 5);
    FILE* data;

    assert (0 != target);

    memcpy (out_name, target, path_len + 1);
    strcat (out_name,".out");

    data = fopen (out_name, "r");

    if (0 == data) {
        if (ENOENT != errno)
            warn ("Error opening %s: %s\n", out_name, strerror (errno));
        puts ("OUTPUT");
    }
    else {
        if (!strlen (in_root)) {
            /* If there is not an input directory, look at the assertion tags */

            if (!compat)
                check_test (data);
            else
                check_compat_test (data);
        }
        else {
            /* Otherwise, diff against the output file */
            check_example (out_name, data);
        }
        fclose (data);
    }
    free (out_name);
}
