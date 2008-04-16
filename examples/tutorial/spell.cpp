/**************************************************************************
 *
 * spell.cpp - A spell checking program.
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

#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <string>

#include <examples.h>


typedef
    std::set <std::string,
              std::less<std::string>, std::allocator<std::string> > stringset;
typedef
    std::ostream_iterator<std::string,
                          char, std::char_traits<char> >            ostrm_iter;
typedef
    std::istream_iterator<std::string,
                          char, std::char_traits<char>, 
                          std::ptrdiff_t>  istrm_iter;


void spellCheck (std::istream & dictionary, std::istream & text) {
    
    stringset   words, misspellings;
    std::string word;
    istrm_iter  eof, dstream (dictionary);

    // First read the dictionary.
    std::copy (dstream, eof, std::inserter (words, words.begin ()));

    // Next read the text.
    while (text >> word)
        if (! words.count (word))
            misspellings.insert (word);

    // Finally, output all misspellings.
    std::cout << std::endl << "Misspelled words:" << std::endl;

    std::copy (misspellings.begin (), misspellings.end (),
          ostrm_iter (std::cout, "\n"));
}

int main ()
{
    std::cout << "Enter text:";
    std::ifstream words ("words");
    spellCheck (words, std::cin);
    std::cout << "End of spell check program" << std::endl;
    return 0;
}
