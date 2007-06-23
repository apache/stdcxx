/***************************************************************************
 *
 * istream1.cpp - istream example
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
 
#include <fstream>    // for basic_ofstream
#include <iostream>   // for basic_istream, cout
#include <cstdio>     // for tmpnam () and remove ()

#include <examples.h>

int main ()
{
#ifndef _RWSTD_STRICT_ANSI

    // use an extension of this implementation: NULL file name argument
    // creates a temporary file; closing the file stream object or its
    // associated filebuf removes the temporary file
    const char* const fname = 0;
    
#else   // if defined (_RWSTD_STRICT_ANSI)

    char fnamebuf [L_tmpnam];

    // create a temporary filename
    const char* const fname = std::tmpnam (fnamebuf);

    if (!fname)
        return 1;

#endif   // _RWSTD_STRICT_ANSI
  
    // typedefs for convenience
    typedef std::basic_istream<char, std::char_traits<char> >         Input;
    typedef std::basic_ofstream<Input::char_type, Input::traits_type> Output;

    Input::char_type s [200];

    // open a file for read and write operations 
    Output out (fname, std::ios::in | std::ios::out | std::ios::trunc);

    // tie the istream object to the ofstream filebuf 
    Input in (out.rdbuf ());

    double x = 3.14159;
    int    i = 3;

    // output to the file
    out << "He lifted his head and pondered.\n"
        << x << '\n' << i << '\n';
  
    // seek to the beginning of the file
    in.seekg (0);

    // read from the file using formatted functions
    in.getline (s, sizeof s);
    in >> x >> i;

    // seek to the beginning of the file
    in.seekg (0, std::ios::beg);

    // output the all file to the standard output
    std::cout << in.rdbuf ();

    // seek to the beginning of the file
    in.seekg (0);
 
    // read the first line in the file
    // "He lifted his head and pondered."
    in.getline (s, sizeof s / 2);

    std::cout << s << '\n';

    // read the second line in the file
    in.getline (s, sizeof s / 2);

    std::cout << s << '\n';

    // seek to the beginning of the file
    in.seekg (0);

    // read the first line in the file
    // "He lifted his head and pondered."
    in.get (s, sizeof s / 2);

    // remove the newline character
    in.ignore ();

    std::cout << s << '\n';

    // read the second line in the file
    // 3.14159
    in.get (s, sizeof s / 2);

    std::cout << s << '\n';

    // remove the newline character
    in.ignore ();

    // store the current file position   
    const Input::pos_type position = in.tellg ();
 
    out << "\nreplace the int\n";

    // move back to the previous saved position
    in.seekg (position);   

    // for convenience
    const Input::int_type eof = Input::traits_type::eof ();

    // output the remainder of the file
    // this is equivalent to "std::cout << in.rdbuf ();"
    while (!Input::traits_type::eq_int_type (in.peek (), eof))
        std::cout << Input::traits_type::to_char_type (in.get ()); 
 
    std::cout << "\n\n\n";

    if (fname) {
        // close the stream before removing the file
        out.close ();

        // remove the temporary file (must have a name)
        std::remove (fname);
    }

    return 0;
}
