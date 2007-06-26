/***************************************************************************
 *
 * fstream.cpp - fstream example.
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

#include <fstream>    // for fstream
#include <iostream>   // for cout
#include <string>     // for string

#include <examples.h>

int main ( )
{
    // create a bi-directional fstream object
    std::fstream inout ("test_fstream.out",
                        std::ios::in | std::ios::out | std::ios::trunc);

    // write out three lines to the file
    inout << "Dieses ist die Geschichte eines Mannes.\n"
          << "C'est l'histoire d'un homme.\n"
          << "This is the story of a man.\n";

    std::string line;

    // seek to the beginning of the file
    inout.seekg (0);

    // extract the first line
    std::getline (inout, line);

    // output the first line to standard output
    std::cout << "\nDeutsch:\n" << line << '\n';

    // obtain current position in file
    const std::fstream::pos_type pos = inout.tellg ();

    // extract the second line
    std::getline (inout, line);

    // output the second line to standard output
    std::cout << "Francais:\n" << line << '\n';

    // extract the third line
    std::getline (inout, line);

    // output the third line to standard output
    std::cout << "English:\n" << line << '\n';
 
    // move the put sequence before the second line
    inout.seekp (pos);

    // replace the second and third lines
    inout << "This is the story of a man.\n"
          << "C'est l'histoire d'un homme.";

    // seek to the beginning of the file
    inout.seekg (0);

    // output the entire contents of the fstream object's buffer to stdout
    std::cout << "\n" << inout.rdbuf () << '\n';

    return 0;
}
