/***************************************************************************
 *
 * diagnostic.cpp
 *
 * $Id: //stdlib/dev/source/stdlib/util/diagnostic.cpp#18 $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#include "diagnostic.h"
#include "scanner.h"          // for token_t
#include "loc_exception.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <set>


static std::set<int> disabled;

static bool warn = true;
static bool info = false;

// write a warning or error message to standard output.  If it is a warning
// that is issued and that warning has not been disabled then return true.
bool issue_diag (int type, bool, const Scanner::token_t *token,
                 const char *fmt, ...) 
{
    bool enabled = false;

    if (0 == fmt) {

        // special treatment when format string is 0

        if (W_DISABLE == type) {
            enabled = warn;
            warn    = false;
        }
        else if (I_ENABLE == type) {
            enabled = info;
            info    = true;
        }
        else {
            enabled = disabled.find (type) == disabled.end ();
            disabled.insert (type);
        }

        return enabled;
    }

    const bool is_info  = I_FIRST <= type && type <= I_LAST;
    const bool is_warn  = !is_info && W_FIRST <= type && type <= W_LAST;
    const bool is_error = !is_info && !is_warn;

    if (is_warn && (!warn || disabled.end () != disabled.find (type)))
        return enabled;

    if (is_info && !info)
        return enabled;

    // all errors and those warnings that are not disabled should be written
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

    if (token) {
        // if the token pointer is non-zero, find the file and line
        // the token appears on and print it out, followed by a line
        // underscoring the token that caused the diagnostic with
        // a string of carets ('^')

        std::FILE* const f = std::fopen (token->file, "r");

        if (f) {
            int i;
            char line [1024];

            for (i = 0; i < token->line; i++) {
                std::fgets (line, 1024, f);
            }

            std::fprintf (stderr, "\t\t%s\t\t", line);
            
            // tok->col is the column number where the first character
            // in the token begins.  Go through the line saving tabs
            // so that the '^' will line up with the token

            for (i = 0; i < token->column; i++) {
                if (line [i] == '\t')
                    std::fprintf (stderr, "\t");
                else
                    std::fprintf (stderr, " ");
            }

            for (unsigned int j = 0; j < token->name.size(); j++) 
                std::fprintf (stderr, "^");

            std::fprintf (stderr, "\n");
            std::fclose (f);
        }
    }

    if (is_error) {
        throw loc_exception ();
    }

    return enabled;
}
