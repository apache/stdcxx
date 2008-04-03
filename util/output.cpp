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
   Arbitrary constant controling static read buffer size.

   @see check_example ()
   @see rbread ()
*/
#define DELTA_BUF_LEN 64

/** 
    This structure is used to encapsulate the data involved in a backwards
    file read.
*/
struct readback {
    FILE* src;
    char buf[DELTA_BUF_LEN];
    long bpos;
};

/**
   Initializes the provided readback structure, with the provided file
   handle.

   @param rb pointer to readback structure to initialize
   @param src file handle to initialize rb with.
   @returns true if successfully initialized, false otherwise.
*/
static bool
rbinit (struct readback* rb, FILE* const src)
{
    const size_t buflen = sizeof rb->buf;
    long fpos;
    assert (0 != rb);
    assert (0 != src);

    rb->src = src;
    if (-1 == fseek (rb->src, 0, SEEK_END))
        return false;
    fpos = ftell (rb->src);
    if (-1 == fpos)
        return false;

    if (fpos <= buflen)
        rb->bpos = fpos;
    else
        rb->bpos = buflen;

    if (-1 == fseek (rb->src, fpos - rb->bpos, SEEK_SET))
        return false;

    return rb->bpos == fread (rb->buf, 1, rb->bpos, rb->src);
}

/** 
   This method is semi-analagous to feof.
   
   @param rb pointer to readback structure to check begin-of-file state for
   @returns true if structure points to the begining of the file, false 
   otherwise
*/
static bool
rbbof (const struct readback* const rb)
{
    assert (0 != rb);
    assert (0 != rb->src);
    return (0 == rb->bpos) && (0 == ftell (rb->src));
}

/**
   Syncronizes the file handle underlying a readback structure to the
   pointer in the structure.  This method is called if you wish to start
   reading forward from the current point in the readback structure.

   @param rb pointer to the readback structure to syncronize.
   @return true if the structure was successfully syncronized, false 
   otherwise.
*/
static bool
rbsync (struct readback* const rb)
{
    assert (0 != rb);
    assert (0 != rb->src);
    const size_t buflen = sizeof rb->buf;
    long fpos = ftell (rb->src) - buflen;
    if (0 > fpos)
        fpos=0;
    fpos += rb->bpos;
    rb->bpos=0;

    return -1 != fseek (rb->src, fpos, SEEK_SET);
}

/**
   Updates the provided readback structure, returning the last unread 
   character in the file.  This method is semi-analagous to fgetc.
   
   @param rb pointer to readback structure to read from.
   @return EOF if beginning of file or I/O error, read character
   otherwise.
*/
static char
rbgetc (struct readback* rb)
{
    const size_t buflen = sizeof rb->buf;
    assert (0 != rb);
    assert (0 != rb->src);
    if (!rb->bpos) {
        const size_t bufdelta = buflen << 1;
        long fpos = ftell (rb->src);
        long seek;

        if (-1 == fpos)
            return EOF;

        if (bufdelta <= fpos) {
            seek = fpos - bufdelta;
            rb->bpos = buflen;
        }
        else if (buflen < fpos) {
            seek = 0;
            rb->bpos = fpos - buflen;
        }
        else {
            fseek (rb->src, 0, SEEK_SET);
            return EOF;
        }

        if (-1 == fseek (rb->src, seek, SEEK_SET))
            return EOF;

        if (rb->bpos != fread (rb->buf, 1, rb->bpos, rb->src))
            return EOF;
    }
    assert (0 < rb->bpos && rb->bpos <= buflen);

    return rb->buf [--rb->bpos];
}

/**
   This method is semi-analagous to fscanf, with some key differences.
   First, it opperates on a readback structure.  Second, it scans until
   either the search pattern is matched, or until the begining of the file
   is reached.  Third, it reads backwards from the end of the file.

   Limitations: string to match can't end in a repeated set of characters
   (ie: 'singing' has the repeated characters 'ing')

   @param rb pointer to readback structure to operate on
   @param match string to search for
   @return true if match is found, false if begining of file is reached
*/
static bool
rbscanf (struct readback* rb, const char* const match)
{
    char tok;
    size_t matched;
    size_t count;
    assert (0 != rb);
    assert (0 != match);

    matched = count = strlen (match) - 1;
    
    for(tok = rbgetc(rb); matched && !rbbof (rb); tok = rbgetc (rb)) {
        if (tok == match [matched])
            --matched;
        else
            matched = count;
    }
    return !matched;
}

/**
   This method is semi-analagous to fscanf, reading backwards on a readback
   structure, starting from the end of the file.

   This method searches backwards for the first (last) number and captures
   it.  It then checks if the match string exists directly after (before)
   the captured value.  If this is the case, a match is considered to have
   been found, and the captured value is copied to val.  Otherwise, the 
   search process is restarted.  If the search hits the beginning of the 
   file, matching is considered to have failed, and val is unaltered.

   Limitations: string to match can't end in a repeated set of characters
   (ie: 'singing' has the repeated characters 'ing'.) or a repeated set
   of characters, bordering on either side of one or more numbers (ie: 
   'xyzzy9zzy' has the repeated characters 'zzy')

   @param rb pointer to readback structure to operate on.
   @param match string to search for.
   @param val reference to variable to store captured value in.
   @return true if match is found, false if begining of file is reached.
*/
static bool
rbscanf (struct readback* rb, const char* const match, unsigned& val)
{
    size_t count;
    unsigned tval =0;
    unsigned radix;
    assert (0 != rb);
    assert (0 != match);

    count = strlen (match) - 1;
    
    while (!rbbof(rb)) {
        char tok;
        size_t matched = count;

        tval=0;
        radix=1;

        /* Search for a numeric digit */
        for (tok = rbgetc (rb); 
            (tok < '0' || tok > '9') && !rbbof (rb); 
             tok = rbgetc (rb)) /* Do nothing */;

        /* Read in the number. */
        for ( ; tok >= '0' && tok <= '9' && !rbbof (rb); 
            tok = rbgetc (rb)) {
            tval += (tok-'0') * radix;
            radix *= 10;
        }

        /* Make certain the content prior to the number in the file 
           matches the search pattern. */
        for( ; matched && tok == match [matched] && !rbbof (rb); 
            tok = rbgetc (rb), --matched) /* Do nothing */;

        if (tok == match [matched]) {
            val = tval;
            return true;
        }
    }
    return false;
}

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
    struct readback buf;
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

    if (!rbinit (&buf, data)) {
        status->status = ST_SYSTEM_ERROR;
        return;
    }

    if (rbbof (&buf)) {
        /* target produced no output (regression test?) */
        status->status = ST_NO_OUTPUT;
        return;
    }

    if (!rbscanf (&buf, "| INACTIVE |\n# +")) {
        status->status = ST_FORMAT;
        return;
    }

    rbsync(&buf);

    /* While it'd probably be (slightly) faster to seek to a fixed position 
       after the file pointer has been synced, this should be more reliable 
    */
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
    struct readback buf;

    assert (0 != data);
    assert (0 != status);

    if (!rbinit (&buf, data)) {
        status->status = ST_SYSTEM_ERROR;
    } 
    else if (rbbof (&buf)) {
        /* target produced no output (regression test?) */
        status->status = ST_NO_OUTPUT;
    }
    else if (   !rbscanf (&buf, "## FailedAssertions = ", status->failed)
             || !rbscanf (&buf, "## Assertions = ", status->assert)
	     || !rbscanf (&buf, "## Warnings = ", status->t_warn)) {
        status->status = ST_FORMAT;
    }
}


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

    data = fopen (out_name, "rb");

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
