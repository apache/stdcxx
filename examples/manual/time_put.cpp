/**************************************************************************
 *
 * time_put.cpp - Example program for the std::time_put facet. 
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/time_put.cpp#7 $
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

// NOTE: due to a SunPro bug, <ctime> must preceed <locale> (see PR #26255)
#include <ctime>      // for struct tm
#include <iostream>   // for cout, endl
#include <locale>     // for locale, time_put

#include <examples.h>


int main ()
{
    // zero-out all members (including any non-standard ones)
    std::tm t = std::tm ();

    // assign individual members being careful to avoid relying
    // on the implementation-defined layout of the struct
    t.tm_sec   = 56;   // seconds
    t.tm_min   = 34;   // minutes
    t.tm_hour  = 14;   // hours (i.e., 2 PM)
    t.tm_mday  = 29;   // day of month
    t.tm_mon   =  1;   // month (February)
    t.tm_year  = 84;   // years since 1900 (i.e., 1984)
    t.tm_yday  = 59;   // day of the year
    t.tm_wday  =  3;   // day of the week (Wednesday)
    t.tm_isdst =  0;   // Daylight Saving Time

    // an array of all supported format specifiers
    const char fmt[][4] = {
        "%a", "%A", "%b", "%B", "%c", "%C", "%d", "%D", "%e", "%F",
        "%g", "%G", "%h", "%H", "%I", "%j", "%k", "%l", "%m", "%M",
        "%n", "%p", "%r", "%R", "%S", "%t", "%T", "%u", "%U", "%V",
        "%w", "%W", "%x", "%X", "%y", "%Y", "%z", "%Z", "%%",

        "%Ec", "%EC", "%Ex", "%EX", "%Ey", "%EY",
        "%Od", "%Oe", "%OH", "%OI", "%Om", "%OM", "%OS",
        "%Ou", "%OU", "%OV", "%Ow", "%OW", "%Oy"
        
    };

    // retrieve the time_put facet installed in cout
    const std::time_put<char> &tput =
        std::use_facet<std::time_put<char> >(std::cout.getloc ());

    // print out the value of t formatted according to each
    // of the format/ specifiers defined above
    for (std::size_t i = 0; i != sizeof fmt / sizeof *fmt; ++i) {
        std::cout << "   \"" << fmt [i] << "\"   \"";

        if ('n' == fmt [i][1])
            std::cout << "\\n";
        else if ('t' == fmt [i][1])
            std::cout << "\\t";
        else
            tput.put (std::ostreambuf_iterator<char>(std::cout.rdbuf ()),
                      std::cout, '\0', &t, fmt [i],
                      fmt [i] + std::char_traits<char>::length (fmt [i]));

        std::cout << "\"\n";
    }

    return 0;
}
