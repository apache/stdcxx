/**************************************************************************
 *
 * concord.cpp - Concordance sample program.
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

#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <string>

#include <examples.h>


// Split a line of text into words.
void split (const std::string& text, const std::string& separators,
            std::list<std::string,std::allocator<std::string> > &words)
{

    const std::string::size_type n = text.length ();
    std::string::size_type start   = text.find_first_not_of (separators);

    while (start < n) {
        std::string::size_type stop = text.find_first_of (separators, start);
        if (stop > n) stop = n;
        words.push_back (text.substr (start, stop-start));
        start = text.find_first_not_of (separators, stop+1);
    }
}


class concordance
{
    typedef std::multimap<std::string,int,
                          std::less<std::string>,
                          std::allocator<std::pair<const std::string,
                                                   int> > > wordDictType;
public:
    void addWord (std::string, int);
    void readText (std::istream &);
    void printConcordance (std::ostream &);
private:
    wordDictType wordMap;
};


void concordance::addWord (std::string word, int line)
{
    // First get range of entries with same key.
    wordDictType::iterator low = wordMap.lower_bound (word);
    wordDictType::iterator high = wordMap.upper_bound (word);

    // Loop over entires, see if any match current line.
    for ( ; low != high; ++low)
        if ( (*low).second == line)
            return;

    // Didn't occur, add now.
    wordMap.insert (wordDictType::value_type (word, line));
}


void allLower (std::string &s)
{
    for (std::size_t i = 0; i < s.size (); i++)
        if ((std::isupper)(s [i], std::locale ()))
            s [i] = (std::tolower)(s [i], std::locale ());
}


void concordance::readText (std::istream & in)
{
    std::string line;

    for (int i = 1; std::getline (in, line); i++) {
        allLower (line);
        std::list<std::string, std::allocator<std::string> > words;
        split (line, " , .;:", words);
        std::list<std::string, std::allocator<std::string> >::iterator wptr;
        for (wptr = words.begin (); wptr != words.end (); ++wptr)
            addWord (*wptr, i);
    }
}


void concordance::printConcordance (std::ostream & out)
{
    std::string            lastword ("");
    wordDictType::iterator pairPtr;
    wordDictType::iterator stop = wordMap.end ();

    for (pairPtr = wordMap.begin (); pairPtr != stop; ++pairPtr)
        // If word is same as previous, just print line number.
        if (lastword == (*pairPtr).first)
            out << ' ' << (*pairPtr).second;
        else {
            // First entry of word.
            lastword = (*pairPtr).first;
            std::cout << '\n' << lastword << ": " << (*pairPtr).second;
        }
    std::cout << '\n';
}


int main ()
{
    std::cout << "Concordance sample program, from Chapter 7\n"
              << "Enter text, then end-of-file:\n";

    concordance words;

    words.readText (std::cin);    
    words.printConcordance (std::cout);

    std::cout << "End of concordance sample program\n";

    return 0;
}
