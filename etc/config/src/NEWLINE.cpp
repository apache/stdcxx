// checking for newline (CR or CR-LF)

/***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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
 * Copyright 1999-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <stdio.h>
#include <stddef.h>


int main ()
{
    // determine the byte sequence that corresponds to the '\n'
    // source character, i.e., one of
    //   <CR>     -- Apple II and Mac OS
    //   <LF>     -- UNIX systems
    //   <CR><LF> -- CP/M, DOS, Windows
    //   <LF><CR> -- unknown

    char fname [256];
    if (0 == tmpnam (fname))
        return 1;

    FILE *fp = fopen (fname, "w");
    if (0 == fp)
        return 2;

    fprintf (fp, "%s", "a\nb");
    fclose (fp);

    fp = fopen (fname, "rb");
    if (0 == fp)
        return 3;

    char buffer [32] = "";

    size_t nbytes = fread (buffer, 1, sizeof buffer, fp);

    fclose (fp);
    remove (fname);

    char *pc;

    for (pc = buffer; *pc && *pc != '\n'; ++pc);

    if ('\0' == *pc) {

        for (pc = buffer; *pc && *pc != '\r'; ++pc);

        if ('\0' == *pc) {

#if 'A' == '\x41'   // ASCII

            // no newline?
            return 4;
            

#elif 'A' == '\xc1'   // EBCDIC

            
            for (pc = buffer; *pc && *pc != '\x15'; ++pc);

            if ('\0' == *pc) {
                // no newline?
                return 4;
            }

            printf ("#define _RWSTD_NEWLINE \"\\x15\"\n");
            printf ("#define _RWSTD_NEWLINE_NEL\n");

#endif   // ASCII or EBCDIC

        }
        else {

            printf ("#define _RWSTD_NEWLINE \"\\r\"\n");
            printf ("#define _RWSTD_NEWLINE_CR\n");
        }
    }
    else {
        if ('\r' == pc [1]) {
            printf ("#define _RWSTD_NEWLINE \"\\n\\r\"\n");
            printf ("#define _RWSTD_NEWLINE_LF_CR\n");
        }
        if ('\r' == pc [-1]) {
            printf ("#define _RWSTD_NEWLINE \"\\r\\n\"\n");
            printf ("#define _RWSTD_NEWLINE_CR_LF\n");
        }
        else {
            printf ("#define _RWSTD_NEWLINE \"\\n\"\n");
            printf ("#define _RWSTD_NEWLINE_LF\n");
        }
    }

    return 0;
}
