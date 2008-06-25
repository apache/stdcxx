/***************************************************************************
 *
 * strstream.cpp - strstream example.
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

#include <iostream>    // for cout, ends
#include <strstream>   // for sstream

#include <examples.h>


int main ()
{
    // create a bi-directional strstream object 
    std::strstream inout;

    // write three complete lines of text to the stream
    inout << "  Dieses ist die Geschichte eines Mannes.\n"
          << "  C'est l'histoire d'un homme.\n"
          << "  This is the story of a man.\n"
          << std::ends;

    char line [80];

    // output the entire contents of the stream object to stdout
    std::cout << "Full text, " << inout.pcount ()
              << " characters:\n" << inout.str ();

    // unfreeze the streambuf so it cleans up its allocated memory 
    inout.freeze (false);

    // keep track of the number of characters extracted by getline
    std::streamsize gcount = 0;

    // extract lines from the stream, one at a time
    for (int i = 1; inout.getline (line, sizeof line); ++i) {

        // increment the number of extracted characters
        gcount += inout.gcount ();

        // write out the line number and its length
        std::cout << "\nLine " << i << ", " << inout.gcount ();

        if ('\0' == *line) {
            // incomplete line (missing newline character)
            std::cout << " character (no newline):\n"
                      << "  '\\0'\n";
        }
        else {
            // output the line to stdout
            std::cout << " characters:\n" << line;
        }
    }

    // write out the total number of extracted characters
    std::cout << "\nExtracted " << gcount << " characters.\n";
 
    return 0;
}
