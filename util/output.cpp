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
#include "display.h"
#include "util.h"

#include "output.h"

#ifndef ENOENT
#  define ENOENT 2
#endif   /* ENOENT */


/**
   Parses contents of the open file handle data for test target_name.
   
   This method reads the data file handle, looking for the output summary from 
   the test driver.  The results of this search (and the parsing of this 
   summary, if found) are stored in the status structure.

   @param data pointer to file structure for output file being parsed
   @param status status object to record results in.
*/
static void
check_test (FILE* data, struct target_status* status)
{
    unsigned r_lvl    = 0;   /* diagnostic severity level */
    unsigned r_active = 0;   /* number of active diagnostics */
    unsigned r_total  = 0;   /* total number of diagnostics */

    int fmt_ok = 0;          /* is output format okay? */

    unsigned fsm = 0;        /* state */
    char tok;                /* current character */

    const char* const target_name = get_target ();

    assert (0 != target_name);
    assert (0 != data);
    assert (0 != status);

    tok = fgetc (data);

    if (feof (data)) {
        /* target produced no output (regression test?) */
        status->status = ST_NO_OUTPUT;
        return;
    }

    for ( ; fsm < 6 && !feof (data); tok = fgetc (data)) {
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
                status->failed += r_active;
                status->assert += r_total;
                if (   status->failed < r_active 
                    || status->assert < r_total
                    || (unsigned int)-1 == status->assert) {
                    status->status = ST_OVERFLOW;
                    return;
                }
            }
            else if (1 < r_lvl) {
                status->t_warn += r_active;
                if (status->t_warn < r_active) {
                    status->t_warn = (unsigned int)-1;
                }
            }
            fmt_ok = 1;
        }
    }
    
    if (!fmt_ok)
        status->status = ST_FORMAT;
}

/**
   Parses contents of the open file handle data for test target_name.
   
   This method is a reimplementation of check_test ().  The difference between
   this method and check_test () is how it parses the results.  This version
   parses compatability layer output, rather than the test driver output.

   @param data pointer to file structure for output file being parsed
   @see check_test ()
*/
static void
check_compat_test (FILE* data, struct target_status* status)
{
    int read = 0;
    unsigned fsm = 0;
    char tok;

    assert (0 != data);
    assert (0 != status);

    tok = fgetc (data);

    if (feof (data)) {
        /* target produced no output (regression test?) */
        status->status = ST_NO_OUTPUT;
        return;
    }

    for ( ; !feof (data); tok = fgetc (data)) {
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
            if (3 == fsm) 
                ++fsm;
            else
                fsm = 0;
            break;
        case 'W':
            if (4 == fsm && !feof (data)) /* leading "## W" eaten */
                read = fscanf (data, "arnings = %u\n## Assertions = %u\n"
                       "## FailedAssertions = %u",
                       &status->t_warn, &status->assert, &status->failed);
        default:
            fsm = 0;
        }
    }
    if (3 != read) {
        status->status = ST_FORMAT;
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
   reports the result of the comparison.  If the comparison fails to result in
   a match, status->status is set to the appropriate error state.  The 
   reference file name is generated by the reference_name function.

   Reference file locations:
     - [data_dir]/manual/out/[target_name].out
     - [data_dir]/tutorial/out/[target_name].out

   @todo add logic to handle differing line endings (CR vs LF vs CRLF)

   @param out_name the name of the output file
   @param fout pointer to file structure for output file being parsed
   @param status status object to record results in.
   @see reference_name
*/
static void
check_example (const char* const data_dir, char* const out_name, FILE* fout, 
               struct target_status* status)
{
    struct stat file_info;
    char* ref_name;

    FILE* fref;   /* reference file (expected output) */

    assert (0 != out_name);
    assert (0 != fout);
    assert (0 != status);

    /* Mark as an example by setting assertions to -1 */
    status->assert = (unsigned)-1;

    /* Try data_dir/manual/out/target_name.out */
    ref_name = reference_name (data_dir, "manual", "out");

    if (0 > stat (ref_name, &file_info)) {
        if (ENOENT != errno) {
            warn ("stat (%s) error: %s\n", exe_name, ref_name, 
                  strerror (errno));
            status->status = ST_BAD_REF;
            free (ref_name);
            return;
        }
                        
        /* If that doesn't exist, try 
           data_dir/tutorial/out/target_name.out */
        free (ref_name);
        ref_name = reference_name (data_dir, "tutorial", "out");

        if (0 > stat (ref_name, &file_info)) {
            if (ENOENT != errno) {
                warn ("stat (%s) error: %s\n", exe_name, ref_name, 
                      strerror (errno));
                status->status = ST_BAD_REF;
            }
            else
                status->status = ST_NO_REF;

            free (ref_name);
            return;
        }
    }

    fref = fopen (ref_name, "r");

    if (0 == fref) {
        int cache = errno; /* caching errno, as puts could alter it */
        if (ENOENT != cache)
            warn ("Error opening %s: %s\n", ref_name, strerror (cache));
        status->status = ST_BAD_REF;
    }
    else {
        int match = 1;   /* do the two files match? */

        while (!feof (fref) && !feof (fout)) {

            char buf [2][DELTA_BUF_LEN];

            size_t nread [2];   /* bytes read from the output/ref file */

            /* First, read a block from the files into the buffer */
            nread [0] = fread (buf [0], 1, sizeof buf [0], fout);
            if (ferror (fout)) {
                warn ("Error reading %s: %s\n", out_name, strerror (errno));
                match = 0;
                break;
            }

            nread [1] = fread (buf [1], 1, sizeof buf [1], fref);
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

        if (!match)
            status->status = ST_BAD_OUTPUT;

        fclose (fref);
    }
    free (ref_name);
}

void
parse_output (const struct target_opts* options, struct target_status* status)
{
    char* out_name;
    FILE* data;

    assert (0 != status);
    assert (0 != options->argv [0]);
    out_name = output_name (options->argv [0]);

    data = fopen (out_name, "r");

    if (0 == data) {
        if (ENOENT != errno)
            warn ("Error opening %s: %s\n", out_name, strerror (errno));
        status->status = ST_NO_OUTPUT;
    }
    else {
        if (0 == options->data_dir || '\0' == *options->data_dir) {
            /* If there is not an input directory, look at the assertion tags */

            if (!options->compat)
                check_test (data, status);
            else
                check_compat_test (data, status);
        }
        else {
            /* Otherwise, diff against the output file */
            check_example (options->data_dir, out_name, data, status);
        }
        fclose (data);
    }
    free (out_name);
}
