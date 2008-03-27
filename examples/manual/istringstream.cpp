/***************************************************************************
 *
 * istringstream.cpp - basic_istringstream example
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
 
#include <iomanip>    // for setw
#include <ios>        // for dec, hex, oct, right, showbase
#include <iostream>   // for endl, wcout, wostream
#include <locale>     // for isspace()
#include <sstream>    // for strstream, wistringstream
#include <string>     // for wstring

#include <examples.h>


int main ( )
{
    try {

        // create a read/write stream object
        std::wistringstream in (std::ios::in | std::ios::out);

        // associate the ostream object's buffer with the ifstream object
        std::wostream out (in.rdbuf ());

        // enable exceptions in both streams
        in.exceptions (std::ios::badbit);
        out.exceptions (std::ios::failbit | std::ios::badbit);

        // write string into out
        out << L"Il avait l'air heureux." << std::endl;

        // seek to the beginning of the stream
        in.seekg (0);

        wchar_t c;

        // output each space-separated word on a separate line
        while (in.get (c)) {
            if ((std::isspace)(c, in.getloc ())) 
                std::wcout << std::endl;
            else
                std::wcout.put(c);
        }

        std::wcout << std::endl;

        // clear stream state, failbit | eofbit set by last call to get ()
        in.clear ();

        // move back to the beginning of the stream
        in.seekg (0); 

        wchar_t buf [40];

        // guard against buffer overflow
        in.width (sizeof buf);

        // set right justification
        std::wcout << std::right;

        // does the same thing as the previous code
        // output each word on a separate line in a field of width 10
        while (in >> buf) {
            std::wcout.width (10);
            std::wcout << buf << std::endl;
        }
    
        std::wcout << std::endl;

        // clear flags, last in >> buf set fail bit
        // because of a newline at end of string
        in.clear (); 

        // output the base info before each integer
        out << std::showbase;

        std::wostream::pos_type pos= out.tellp ();

        const long l = 10;
        
        // output l in hex with a field with of 10 
        out << std::hex << std::setw (10) << l << std::endl;

        // output l in oct with a field with of 10
        out << std::oct << std::setw (10) << l << std::endl;

        // output l in dec with a field with of 10
        out << std::dec << std::setw (10) << l << std::endl;

        // move back to the beginning of the stream
        in.seekg (0);

        // output the entire stream
        std::wcout << in.rdbuf () << std::endl;

        // clear the flags 
        in.clear (); 

        // seek the input sequence to pos  
        in.seekg (pos);
  
        in.unsetf (std::ios::basefield);

        int a, b, d;

        // read the previous outputted integer
        in >> a >> b >> d;

        // output 3 times 10
        std::wcout << a << std::endl << b << std::endl << d << std::endl;

    }
    catch (std::ios::failure &e) {
        std::wcerr << e.what () << std::endl;
    }
 
    return 0;
}
