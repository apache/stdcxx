/***************************************************************************
 *
 * ostreambuf_iterator.cpp - ostreambuf_iterator example
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
 
#include <fstream>    // for filebuf
#include <iostream>   // for cout
#include <iterator>   // for streambuf_iterator's
#include <cstdio>     // for L_tmpnam, tmpnam and remove

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

    // create a filebuf object
    std::filebuf buf;

    // open the file and link it to the filebuf object
    buf.open (fname, std::ios::in | std::ios::out | std::ios::trunc);

    // create an ostreambuf_iterator and link it to 
    // the filebuf object
    std::ostreambuf_iterator<char, std::char_traits<char> > out_iter (&buf);

    const char charset[] =
        "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    // output characters into the file using the ostreambuf_iterator
    for (unsigned i = 0; charset [i]; ++i)
        out_iter = charset [i];

    // seek to the beginning of the file
    buf.pubseekpos(0);
  
    // create an istreambuf_iterator and link it to  the filebuf object
    std::istreambuf_iterator<char, std::char_traits<char> > in_iter (&buf);

    // construct an end of stream iterator
    std::istreambuf_iterator<char, std::char_traits<char> > end;

    std::cout << '\n';

    // output the contents of the file
    while (!in_iter.equal (end))
        std::cout << *in_iter++;

    std::cout << '\n';

    if (fname) {
        // close the stream before removing the file
        buf.close ();

        // remove the temporary file (must have a name)
        std::remove (fname);
    }

    return 0;
}
