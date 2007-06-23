/***************************************************************************
 *
 * istrstream.cpp - istrstream example.
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

#include <iostream>    // for cout
#include <strstream>   // for strstream

#include <examples.h>


int main ()
{
    // create two constant istrstream objects and initialize their
    // underlying strstreambufs with string literals; the objects
    // do not create copies of the literals but use them directly
    // for efficiency
    std::istrstream a ("Ce n'est pas l'homme qui prend la mer, ");
    std::istrstream b ("c'est la mer qui prend l'homme.");

    // create a dynamic strstream object
    std::strstream out;

    // output the contents of the streams into out
    out << a.rdbuf () << b.str () << '\n';
  
    // output the contents of out to standard output
    std::cout << out.rdbuf () << '\n';
  
    // output the contents of the stream objects to standard output
    std::cout << a.str () << '\n' << b.rdbuf () << '\n';

    // the dtors of a and b will not release any storage
    // since the objects did not allocate any

    // the dtor of out will release allocated storage since
    // neither freeze() nor str() was called on the object

    return 0;
}
