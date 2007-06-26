/**************************************************************************
 *
 * multimap.cpp - Example program for multimap class.
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

#include <iostream>   // for cout, ostream
#include <map>        // for multimap
#include <string>     // for string

#include <examples.h>


typedef std::multimap<int, std::string, std::less<int>,
                      std::allocator<std::pair<const int, std::string> > >
months_type;


// Print out a multimap.
inline std::ostream&
operator<< (std::ostream &out, const months_type &m)
{
    for (months_type::const_iterator it = m.begin (); it != m.end (); ++it)
        std::cout << (*it).second << " has " << (*it).first << " days\n";

    return out;
}


int main ()
{
    // Create a multimap of months and the number of days in the month.
    months_type months;

    typedef months_type::value_type value_type;

    // Put the months in the multimap with the number of days
    // as the not necessarily unique key. 
    months.insert (value_type (31, std::string ("January")));
    months.insert (value_type (28, std::string ("Febuary")));
    months.insert (value_type (31, std::string ("March")));
    months.insert (value_type (30, std::string ("April")));
    months.insert (value_type (31, std::string ("May")));
    months.insert (value_type (30, std::string ("June")));
    months.insert (value_type (31, std::string ("July")));
    months.insert (value_type (31, std::string ("August")));
    months.insert (value_type (30, std::string ("September")));
    months.insert (value_type (31, std::string ("October")));
    months.insert (value_type (30, std::string ("November")));
    months.insert (value_type (31, std::string ("December")));

    // Print out the months.
    std::cout << "All months of the year\n" << months << std::endl;

    // Find all months with 30 days.
    std::pair<months_type::iterator, months_type::iterator> p = 
        months.equal_range (30);

    // Print out the 30 day months.
    std::cout << "\nMonths with 30 days\n";

    for ( ; p.first != p.second; ++p.first)
        std::cout << (*p.first).second << '\n';

    return 0;
}
