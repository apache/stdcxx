/***************************************************************************
 *
 * diagnostic.cpp
 *
 * $Id$
 *
 ***************************************************************************
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include "diagnostic.h"
#include "scanner.h"          // for Scanner::token_t
#include "loc_exception.h"

#include <cstdarg>            // for va_list, ...
#include <cstdio>             // for puts(), fprintf(), ...
#include <set>                // for set


// set of disabled warnings
static std::set<int> disabled;

static bool warn = true;    // warnings (on by default)
static bool info = false;   // info messages (off by default)


// write a warning or error message to standard output.  If it is a warning
// that is issued and that warning has not been disabled then return true.
bool issue_diag (int type, bool, const Scanner::token_t *token,
                 const char *fmt, ...) 
{
    bool enabled = false;

    if (0 == fmt) {

        // special treatment when format string is 0: a request
        // to enable or disable this type of diagnostic, e.g.,
        // in response to a command line option

        if (W_DISABLE == type) {
            // disable all warnings
            enabled = warn;
            warn    = false;
        }
        else if (I_ENABLE == type) {
            // enable all informational messages
            enabled = info;
            info    = true;
        }
        else {
            // disable a specific warning and return its previous
            // setting (i.e., enabled or disabled)
            enabled = disabled.find (type) == disabled.end ();
            disabled.insert (type);
        }

        return enabled;
    }

    const bool is_info  = I_FIRST <= type && type <= I_LAST;
    const bool is_warn  = !is_info && W_FIRST <= type && type <= W_LAST;
    const bool is_error = !is_info && !is_warn;

    if (is_warn && (!warn || disabled.end () != disabled.find (type))) {
        // warning disabled
        return enabled;
    }

    if (is_info && !info) {
        // info disabled
        return enabled;
    }

    // all errors and those warnings that are not disabled
    // must be issued
    enabled = true;

    if (token && token->file)
        std::fprintf (stderr, "%s:%d: ",  token->file, token->line);

    if (is_error)
        std::fprintf (stderr, "Error %-3d: ", type);
    else if (is_warn)
        std::fprintf (stderr, "Warning %-3d: ", type);
    else
        std::fprintf (stderr, "Note %-3d: ", type);
        
    // get the variable sized argument and pass it to vfprintf
    // to be printed
    std::va_list va;
    va_start (va, fmt);
    std::vfprintf (stderr, fmt, va);
    va_end (va);

    // if the token pointer is non-zero, find the file and line
    // the token appears on and print it out, followed by a line
    // underscoring the token that caused the diagnostic with
    // a string of carets ('^')

    std::FILE* const ftok = token ? std::fopen (token->file, "r") : 0;

    if (ftok) {
        int i;
        char line [1024];   // FIXME: handle longer lines

        // advance to the specified line in the file
        for (i = 0; i < token->line; ++i) {
            if (0 == std::fgets (line, 1024, ftok)) {
                *line = '\0';
                break;
            }
        }

        if (i == token->line && '\0' != *line) {
            std::fputs ("\t\t", stderr);
            std::fputs (line, stderr);
            std::fputs ("\t\t", stderr);
            
            // tok->col is the column number where the first character
            // in the token begins.  Go through the line saving tabs
            // so that the '^' will line up with the token

            for (i = 0; i < token->column; ++i)
                std::fputc (line [i] == '\t' ? '\t' : ' ', stderr);

            for (unsigned j = 0; j < token->name.size (); ++j)
                std::fputc ('^', stderr);

            std::fputc ('\n', stderr);
        }

        std::fclose (ftok);
    }

    if (is_error) {
        // throw an exception if the diagnostic is a hard error
        throw loc_exception ();
    }

    // return otherwise (i.e., the diagnostic is not an error)
    return enabled;
}
