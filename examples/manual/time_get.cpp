/**************************************************************************
 *
 * timeget.cpp - Example program for the time_get facet. 
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

// NOTE: A SunPro bug requires time.h preceed locale. PR #26255
#include <ctime>      // for struct tm
#include <locale>     // for locale, time_get
#include <sstream>    // for stringstream
#include <iostream>   // for cout, endl


// Print out a tm struct value in one atomic operation
std::ostream& operator<< (std::ostream &os, const std::tm &t)
{
    std::stringstream strm;

    strm << "Daylight Savings = "   << t.tm_isdst
         << "\nDay of year      = " << t.tm_yday
         << "\nDay of week      = " << t.tm_wday
         << "\nYear             = " << t.tm_year
         << "\nMonth            = " << t.tm_mon
         << "\nDay of month     = " << t.tm_mday
         << "\nHour             = " << t.tm_hour
         << "\nMinute           = " << t.tm_min
         << "\nSecond           = " << t.tm_sec
         << '\n';

    // guard for thread safety and output synchronization
    const std::ostream::sentry guard (os);

    if (guard)
        os.rdbuf ()->sputn (strm.str ().c_str (), strm.str ().size ());
    else
        os.setstate (os.failbit);

    return os;
}


int main ()
{
    typedef std::istreambuf_iterator<char, std::char_traits<char> > Iter;

    // time struct to parse date into
    static std::tm timeb;  // zero initialized

    // Unused, required by time_get
    std::ios_base::iostate state;

    // Stream object to read from
    std::istringstream ins ("");

    // Iterators into the stream object
    Iter begin (ins);
    Iter end;

    const std::locale loc ("C");

    // Get a reference to the time_get facet in locale loc.
    const std::time_get<char, Iter> &tg =
        std::use_facet<std::time_get<char, Iter> >(loc);

    // Display time_base::dateorder value.
    std::cout << "time_base::dateorder == " << tg.date_order () << ".\n";
  
    // Insert date string into stream.
    ins.str ("04/07/69");

    // get_date from the stream and output tm contents.
    tg.get_date (begin, end, ins, state, &timeb);
    std::cout << "Date: Apr 7 1969\n" << timeb << std::endl;

    // Insert weekday string into stream.
    ins.str ("Monday");

    // get_weekday from the stream and output tm contents.
    tg.get_weekday (begin, end, ins, state, &timeb);
    std::cout << "Weekday: Monday\n" << timeb << std::endl;
  
    // Insert time string into stream.
    ins.str ("06:47:32");

    // get_time from the stream and output tm contents.
    tg.get_time (begin, end, ins, state, &timeb);
    std::cout << "Time: 06:47:32\n" << timeb << std::endl;

    return 0;
}
