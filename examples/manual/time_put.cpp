/**************************************************************************
 *
 * time_put.cpp - Example program for the std::time_put facet. 
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
 * Copyright 1998-2006 Rogue Wave Software.
 * 
 **************************************************************************/

// NOTE: due to a SunPro bug, <ctime> must preceed <locale> (see PR #26255)
#include <ctime>       // for struct tm
#include <exception>   // for exception
#include <iostream>    // for cout, endl
#include <locale>      // for locale, time_put
#include <sstream>     // for ostringstream

#include <examples.h>


int main (int argc, char *argv[])
{
    // zero-out all members (including any non-standard ones)
    std::tm t = std::tm ();

    // assign individual members, being careful to avoid relying
    // on the implementation-defined layout of the struct
    t.tm_sec   = 56;   // seconds
    t.tm_min   = 34;   // minutes
    t.tm_hour  = 14;   // hours (i.e., 2 PM)
    t.tm_mday  =  3;   // day of month
    t.tm_mon   =  1;   // month (February)
    t.tm_year  = 84;   // years since 1900 (i.e., 1984)
    t.tm_yday  = 33;   // day of the year
    t.tm_wday  =  5;   // Sunday-based weekday (Friday)
    t.tm_isdst =  0;   // Daylight Saving Time

    // array of all supported conversion specifiers
    static const struct {
        // const below commented out to avoid compiler warnings
        // such as HP aCC's Warning 578: A class member may not
        // be a const array since there is no way to initialize
        // such a member. 
        /* const */ char spec [4];    // specifier
        /* const */ char desc [40];   // description
    } fmt[] = {
        // plain (unmodified) conversion specifiers
        { "%a", "abbreviated weekday name" },
        { "%A", "full weekday name" },
        { "%b", "abbreviated month name" },
        { "%B", "full month name" },
        { "%c", "date and time" },
        { "%C", "two-digit year [00,99]" },
        { "%d", "month number [01,31]" },
        { "%D", "date as \"%m/%d/%y\"" },
        { "%e", "month number [ 1,31]" },
        { "%F", "date as \"%Y-%m-%d\"" },
        { "%g", "week-based year [00,99]" },
        { "%G", "week-based year" },
        { "%h", "same as \"%b\"" },
        { "%H", "hour [00-23]" },
        { "%I", "hour [01-12]" },
        { "%j", "day of the year [001,366]" },
        { "%k", "hour [ 0,23]" },
        { "%l", "hour [ 1,12]" },
        { "%m", "month number [01,12]" },
        { "%M", "minute [00,59]" },
        { "%n", "newline character" },
        { "%p", "a.m. or p.m. equivalent" },
        { "%r", "AM/PM time" },
        { "%R", "24-hour time" },
        { "%S", "second [00,60]" },
        { "%t", "tab character" },
        { "%T", "time as \"%H:%M:%S\"" },
        { "%u", "Monday-based weekday [1,7]" },
        { "%U", "week number [00,53]" },
        { "%V", "week numver [01,53]" },
        { "%w", "weekday [0,6]" },
        { "%W", "week number [00,53]" },
        { "%x", "date" },
        { "%X", "time" },
        { "%y", "two-digit year [00,99]" },
        { "%Y", "year" },
        { "%z", "offset from UTC" },
        { "%Z", "time zone" },
        { "%%", "percent character" },

        // modified conversion specifiers
        { "%Ec", "" },
        { "%EC", "" },
        { "%Ex", "" },
        { "%EX", "" },
        { "%Ey", "" },
        { "%EY", "" },
        { "%Od", "" },
        { "%Oe", "" },
        { "%OH", "" },
        { "%OI", "" },
        { "%Om", "" },
        { "%OM", "" },
        { "%OS", "" },
        { "%Ou", "" },
        { "%OU", "" },
        { "%OV", "" },
        { "%Ow", "" },
        { "%OW", "" },
        { "%Oy", "" }
    };

    if (1 < argc) {
        try {
            // imbue the locale specified by the argument in cout
            std::cout.imbue (std::locale (argv [1]));
        }
        catch (std::exception &ex) {
            // print out the reason for the failure and exit
            std::cerr << ex.what () << '\n';
            return 1;
        }
    }

    // retrieve the time_put facet installed in cout
    const std::time_put<char> &tput =
        std::use_facet<std::time_put<char> >(std::cout.getloc ());

    // left-justify text
    std::cout.setf (std::cout.left);

    // print out the time value formatted according to each
    // of the conversion specifiers defined above
    for (std::size_t i = 0; i != sizeof fmt / sizeof *fmt; ++i) {

        const char* const spec = fmt [i].spec;

        std::ostringstream tmp;

        // format the directive, in quotes, in a temporary stringstream
        tmp << '"' << spec << '"';

        // format the quoted directive in a field of the given width
        std::cout << "  ";
        std::cout.width (8);
        std::cout << tmp.str ();

        // clear the temporary stringstream
        tmp.str ("");

        tmp << '"';
        if ('n' == spec [1])
            tmp << "\\n";
        else if ('t' == spec [1])
            tmp << "\\t";
        else
            tput.put (std::ostreambuf_iterator<char>(tmp.rdbuf ()),
                      std::cout, '\0', &t, spec,
                      spec + std::char_traits<char>::length (spec));

        tmp << '"';

        // format the string in a field of the given width
        std::cout.width (32);
        std::cout << tmp.str ();

        // clear the temporary stringstream
        tmp.str ("");

        const char* const d = spec + 1 + ('E' == *spec || 'O' == *spec);

        if ('c' == *d || 'r' == *d || 'x' == *d || 'X' == *d) {

            if (*fmt [i].desc) {
                // format the description of the directive
                tmp << fmt [i].desc << " (";
            }

            tmp << "\"";

            // extension of this implementation:
            // for %c, %r, %x, and %X, write out the formatting
            // string that the conversion specifier expands to
            tput.put (std::ostreambuf_iterator<char>(tmp.rdbuf ()),
                      std::cout, '\0', 0, spec,
                      spec + std::char_traits<char>::length (spec));

            tmp << "\"";

            if (*fmt [i].desc)
                tmp << ')';
        }
        else if (*fmt [i].desc) {
            // format the description of the directive
            tmp << fmt [i].desc;
        }

        std::cout << tmp.str () << '\n';
    }

    return 0;
}
