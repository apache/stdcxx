/***************************************************************************
 *
 * ifstream.cpp - basic_ifstream example.
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
#include <iomanip>    // for setw
#include <ios>        // for dec, hex, oct, right, showbase
#include <iostream>   // for cerr, cout
#include <locale>     // for isspace()

#include <examples.h>


int main ( )
{
    const char ntbs[] = "Le minot passait la piece a frotter.";

    try {

        // create a read/write file-stream object
        std::ifstream in ("ifstream.results.out",
                          std::ios::in | std::ios::out | std::ios::trunc);

        if (!in.is_open ()) 
            throw (std::ios::failure ("open error"));

        // associate the ostream object's buffer with the ifstream object
        std::ostream out (in.rdbuf ());   

        // output ntbs in out
        out << ntbs << '\n';

        // seek to the beginning of the file
        in.seekg (0);

        char c;

        // output each space-separated word on a separate line
        while (in.get (c)) {
            if ((std::isspace)(c, in.getloc ())) 
                std::cout << '\n';
            else
                std::cout << c;
        }

        std::cout << '\n';

        // clear stream state, failbit | eofbit set by last call to get ()
        in.clear ();

        // move back to the beginning of the file
        in.seekg (0); 

        char buf [40];

        // guard against buffer overflow
        in.width (sizeof buf);

        // set right justification
        std::cout << std::right;

        // does the same thing as the previous code
        // output each word on a separate line in a field of width 10
        while (in >> buf) {
            std::cout.width (10);
            std::cout << buf << '\n';
        }
    
        std::cout << '\n';

        // clear the stream state
        in.clear (); 

        // output the base info before each integer
        out << std::showbase;

        std::ostream::pos_type pos= out.tellp ();

        const long x = 10;
        
        // output x in hex with a field with of 10 
        out << std::hex << std::setw (10) << x << '\n';

        // output x in oct with a field with of 10
        out << std::oct << std::setw (10) << x << '\n';

        // output x in dec with a field with of 10
        out << std::dec << std::setw (10) << x << '\n';

        // move back to the beginning of the file
        in.seekg (0);

        // output the entire file
        std::cout << in.rdbuf () << '\n';

        // clear the stream state
        in.clear (); 

        // seek the input sequence to pos  
        in.seekg (pos);
  
        long a = 0;
        long b = 0;
        long d = 0;

        in.unsetf (std::ios::basefield);

        // read the previous outputted integer
        in >> a >> b >> d;

        // output 3 times 10
        std::cout << a << '\n' << b << '\n' << d << '\n';
    }
    catch (std::ios::failure e) {
        std::cerr << e.what () << '\n';
    }
 
    return 0;
}
