/**************************************************************************
 *
 * map.cpp - Example program of map.
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

#include <iostream>    // for cout
#include <map>         // for map
#include <string>      // for string
#include <utility>     // for pair

#include <examples.h>


typedef std::map<std::string, int, std::less<std::string>,
                 std::allocator<std::pair<const std::string, int> > >
months_type;


// Print out a map.
inline std::ostream&
operator<< (std::ostream &out, const months_type &m)
{
    for (months_type::const_iterator it = m.begin (); it != m.end (); ++it)
        std::cout << (*it).first << " has " << (*it).second << " days\n";

    return out;
}


int main ()
{
    // Create a map of months and the number of days in the month.
    months_type months;

    typedef months_type::value_type value_type;

    // Put the months in the multimap.
    months.insert (value_type (std::string ("January"),   31));
    months.insert (value_type (std::string ("Febuary"),   28));
    months.insert (value_type (std::string ("Febuary"),   29));
    months.insert (value_type (std::string ("March"),     31));
    months.insert (value_type (std::string ("April"),     30));
    months.insert (value_type (std::string ("May"),       31));
    months.insert (value_type (std::string ("June"),      30));
    months.insert (value_type (std::string ("July"),      31));
    months.insert (value_type (std::string ("August"),    31));
    months.insert (value_type (std::string ("September"), 30));
    months.insert (value_type (std::string ("October"),   31));
    months.insert (value_type (std::string ("November"),  30));
    months.insert (value_type (std::string ("December"),  31));

    // Print out the months.  Second Febuary is not present.
    std::cout << months << std::endl;

    // Find the Number of days in June.
    months_type::iterator p = months.find (std::string ("June"));

    // Print out the number of days in June.
    if (p != months.end ())
        std::cout << std::endl << (*p).first << " has "
                  << (*p).second << " days\n";
  
    return 0;
}
