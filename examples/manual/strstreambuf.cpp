/***************************************************************************
 *
 * strstreambuf.cpp - strstreambuf example.
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
 
#include <iomanip>     // for setw
#include <ios>         // for dec, ends
#include <iostream>    // for cerr, cout
#include <strstream>   // for istream, ostrstream

#include <examples.h>


int main ()
{
    // create a write-only stream object
    std::ostrstream out;

    // tie the istream object to the ostrstream object
    std::istream in (out.rdbuf ());   

    // output to out
    out << "Anticonstitutionnellement is a big word!!!\n";

    // create a NTBS 
    const char s[] ="Le rat des villes et le rat des champs.";

    // output the NTBS
    out << s << '\n';

    // (try to) resize the buffer
    if (out.rdbuf ()->pubsetbuf (0, 128L))
        std::cout << "Successfully allocated buffer.\n";
    else
        std::cerr << "Failed to allocate buffer.\n";

    // output the contents of the buffer to standard output
    std::cout << in.rdbuf ();

    // format a number into out
    out << std::dec              // decimal base
        << std::setfill ('#')    // set fill character
        << std::setw (16)        // set field width
        << 0x100 << '\n';        // format
  
    // output the content of the input sequence to standard output
    std::cout << in.rdbuf( ) << '\n';

    // number of elements in the output sequence
    const std::streamsize pcount = std::streamsize (out.rdbuf ()->pcount ());

    std::cout << "Buffer size is " << pcount << '\n';

    // (try to) resize the buffer
    if (out.rdbuf ()->pubsetbuf (0, pcount * 2L))
        std::cout << "\nSuccessfully resized buffer to " << pcount * 2 << '\n';
    else
        std::cerr << "\nFailed to resize buffer to " << pcount * 2 << '\n';

    // NUL-terminate the character array before streaming it out
    out << std::ends;

    // output the contents of the streambuf object associated with out
    std::cout << out.rdbuf ()->str ();

    // unfreeze the streambuf so it cleans up its allocated memory
    out.rdbuf ()->freeze (false);

    return 0;
}
