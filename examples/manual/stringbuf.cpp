/***************************************************************************
 *
 * stringbuf.cpp - basic_stringbuf example.
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
 
#include <iostream>   // for cout, endl
#include <sstream>    // for istringstream, ostringstream
#include <string>     // for string

#include <examples.h>


int main ()
{
    // create a read/write string-stream object on tiny char
    // and attach it to an ostringstream object
    std::ostringstream out_1 (std::ios::in | std::ios::out);

    // tie the istream object to the ostringstream object
    std::istream in_1 (out_1.rdbuf ());   

    // output to out_1
    out_1 << "Here is the first ouput";

    // create a string object on tiny char 
    std::string string_ex = "L'heure est grave!";

    // open a read only stringstream object and initialize it
    std::istringstream in_2 (string_ex);

    // output in_1 to the standard output
    std::cout << in_1.rdbuf () << std::endl;

    // reposition in_1 at the beginning
    in_1.seekg (0);

    // output in_2 to the standard output
    std::cout << in_2.rdbuf () << std::endl;

    // reposition in_2 at the beginning
    in_2.seekg (0);

    // get the current put position (equivalent to out_1.tellp ())
    std::stringbuf::pos_type pos =
        out_1.rdbuf ()->pubseekoff (0, std::ios::cur, std::ios::out);

    // append the content of the stringbuf pointed to by in_2
    // to the one  pointed at by out_1
    out_1 << ' ' << in_2.rdbuf ();

    // output in_1 to the standard output
    std::cout << in_1.rdbuf () << std::endl;

    // position the get sequence (equivalent to in_1.seekg (pos))
    in_1.rdbuf ()->pubseekpos (pos, std::ios::in);

    // output "L'heure est grave!"
    std::cout << in_1.rdbuf () << std::endl;  

    return 0;
}
