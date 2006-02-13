/***************************************************************************
 *
 * stringstream.cpp - stringstream example.
 *
 * $Id$
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

#include <iostream>   // for wcout
#include <sstream>    // for wstringstream

#include <examples.h>


int main ()
{
    // create a bi-directional wstringstream object 
    std::wstringstream inout;

    // write out three lines to the stream
    inout << "Dieses ist die Geschichte eines Mannes.\n"
          << "C'est l'histoire d'un homme.\n"
          << "This is the story of a man.\n";

    std::wstring line;

    // extract the first line
    std::getline (inout, line);

    // output the first line to stdout
    std::wcout << "\nDeutsch:\n" << line;

    // extract the second line
    std::getline (inout, line);

    // output the second line to stdout
    std::wcout << "\nFrancais:\n" << line;

    // extract the third line
    std::getline (inout, line);

    // output the third line to stdout
    std::wcout << "\nEnglish:\n" << line << "\n\n";
 
    // output the contents of the stream object to stdout
    std::wcout << inout.str () << '\n';

    return 0;
}
