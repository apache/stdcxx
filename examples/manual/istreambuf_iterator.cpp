/***************************************************************************
 *
 * istreambuf_iterator.cpp - istreambuf_iterator example
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
 
#include <fstream>    // for ofstream, istreambuf_iterator
#include <iostream>   // for cout
#include <cstdio>     // for tmpnam () and remove ()

#include <examples.h>


int main ( )
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

    // open the file is_iter.out for reading and writing
    std::ofstream out (fname, std::ios::out | std::ios::in | std::ios::trunc);

    // output the example sentence into the file
    out << "Ceci est un simple exemple pour demontrer le\n"
           "fonctionnement de istreambuf_iterator.";

    // seek to the beginning of the file
    out.seekp (0);

    // construct an istreambuf_iterator pointing to
    // the ofstream object underlying streambuffer
    std::istreambuf_iterator<char, std::char_traits<char> > iter (out.rdbuf ());

    // construct an end of stream iterator
    const std::istreambuf_iterator<char, std::char_traits<char> > end;

    std::cout << '\n';

    // output the content of the file
    while (!iter.equal (end)) {

        // use both operator++ and operator*
        std::cout << *iter++;
    }

    std::cout << '\n';

    if (fname) {
        // close the stream before removing the file
        out.close ();

        // remove the temporary file (must have a name)
        std::remove (fname);
    }

    return 0;
}
