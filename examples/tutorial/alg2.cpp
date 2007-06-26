/**************************************************************************
 *
 * alg2.cpp - Examples using STL generic algorithms that search for 
 *            elements that satisfy a condition.
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

#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <cstring>
#include <iostream>

#include <examples.h>


void split (const std::string & text, const std::string & separators,
            std::list<std::string, std::allocator<std::string> > & words) {

    const std::string::size_type n = text.length ();
    std::string::size_type start   = text.find_first_not_of (separators);

    while (start < n) {
        std::string::size_type stop = text.find_first_of (separators, start);
        if (stop > n)
            stop = n;
        words.push_back (text.substr (start, stop - start));
        start = text.find_first_not_of (separators, stop+1);
    }
}


bool isLeapYear (unsigned int year)
{
    return !(year % 1000 && year % 100 && year % 4);
}


// Illustrate use of the find function.
void find_test () {
    
    std::cout << "Test of algorithm find\n";

    const int vintageYears[] = { 1967, 1972, 1974, 1980, 1995 };

    const int* start = vintageYears;
    const int* stop  =   vintageYears
                       + sizeof vintageYears / sizeof *vintageYears;
    const int* where = std::find_if (start, stop, isLeapYear);

    if (where != stop)
        std::cout << "first vintage leap year is " << *where << '\n';
    else
        std::cout << "no vintage leap years" << '\n';
        
    where = std::find (start, stop, 1995);

    if (where != stop)
        std::cout << "1995 is position " << where - start
                  << " in sequence"      << '\n';
    else
        std::cout << "1995 does not occur in sequence\n";
}


void find_adjacent_test () {

    std::cout << "Test of algorithm find adjacent\n";

    const char text[] = "The bookkeeper carefully opened the door";
    
    const char* start = text;
    const char* stop  = text + sizeof text;
    const char* where = start;
    
    std::cout << "In the text " << text << '\n';

    while ((where = std::adjacent_find (where, stop)) != stop) {
        std::cout << "double "   << *where << " in position "
                  << where-start << '\n';
        ++where;
    }
}


// Illustrate the use of the search function.
void search_test () {

    std::cout << "Test of algorithm search\n";

    char base[] = "aspirations";
    char text[] = "ration";
    
    const char* const where = std::search (base, base+std::strlen (base),
                                           text, text+std::strlen (text));
    
    if (*where != '\0')
        std::cout << "substring begins in position "
                  << where - base << '\n';
    else
        std::cout << "substring does not occur in text\n";
}


// Illustrate use of max_element and min_element algorithms.
void max_min_example () {

    std::cout << "Test of max and min algorithms \n";
    
    // make a std::vector of "random" numbers between 0 and 99.
    const int rn[] = {
        83, 86, 77, 15, 93, 35, 86, 92, 49, 21,
        62, 27, 90, 59, 63, 26, 40, 26, 72, 36,
        11, 68, 67, 29, 82
    };

    std::vector<int, std::allocator<int> >
        numbers (rn, rn + (sizeof rn/sizeof *rn));

    // Print the maximum and minimum.
    std::vector<int, std::allocator<int> >::iterator max =
        std::max_element (numbers.begin (), numbers.end ());

    std::cout << "largest value was " << *max << '\n';

    std::vector<int, std::allocator<int> >::iterator min =
        std::min_element (numbers.begin (), numbers.end ());

    std::cout << "smallest value was " << *min << '\n';

    // Example using std::strings.
    std::string text = "it was the best of times, "\
                       "it was the worst of times.";
    std::list<std::string, std::allocator<std::string> >words;

    split (text, " ., !:;", words);

    std::cout << "The smallest word is "
              << * std::min_element (words.begin (), words.end ())
              << " and the largest word is "
              << * std::max_element (words.begin (), words.end ())
              << '\n';
}


// Illustrate the use of the mismatch function.
void mismatch_test (const char * a, const char * b) 
{
    std::pair<const char *, const char *> differPositions (0, 0);
    const char *aDiffPos;
    const char *bDiffPos;

    if (std::strlen (a) < std::strlen (b)) {
        differPositions = std::mismatch (a, a + std::strlen (a), b);
        aDiffPos = differPositions.first;
        bDiffPos = differPositions.second;
    }
    else {
        differPositions = std::mismatch (b, b + std::strlen (b), a);
        aDiffPos = differPositions.second;
        bDiffPos = differPositions.first;
    }
        
    std::cout << "string " << a;

    if (*aDiffPos == *bDiffPos)
        std::cout << " is equal to ";
    else if (*aDiffPos < *bDiffPos)
        std::cout << " is less than ";
    else
        std::cout << " is greater than ";

    std::cout << b << '\n';
}


int main ()
{
    std::cout << "STL generic algorithms -- Searching Algorithms\n";

    find_test ();
    find_adjacent_test ();
    search_test ();
    max_min_example ();
    mismatch_test ("goody", "goody");
    mismatch_test ("good", "goody");
    mismatch_test ("goody", "good");
    mismatch_test ("good", "fred");
    mismatch_test ("fred", "good");
    
    std::cout << "End of search algorithms test program\n";

    return 0;
}
