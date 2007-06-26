/***************************************************************************
 *
 * ostream2.cpp - ostream example
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
 
#include <iostream>   // for wcout, wostream, endl
#include <sstream>    // for wistringstream

#include <examples.h>


int main ()
{
    const double f = 3.1415926;

    const wchar_t s[] = { 'K', 'e', 'n', 'a', 'v', 'o', ' ', '!', '\0' };

    // create a read/write stringbuf object on wide char
    // and attach it to an wistringstream object
    std::wistringstream in (std::wios::in | std::wios::out);
      
    // tie the wostream object to the wistringstream object
    std::wostream out (in.rdbuf ());

    out << L"test beginning !" << std::endl;
    out.setf (std::wios::fixed, std::wios::floatfield);

    // output f in fixed format
    out << f << std::endl; 

    // store the current put-pointer position  
    std::wostream::pos_type pos = out.tellp ();

    // output s 
    out << s << std::endl;

    // output the all buffer to standard output
    std::wcout << in.rdbuf () << std::endl;

    // position the get-pointer
    in.seekg (pos);

    // output s
    std::wcout << in.rdbuf () << std::endl;         

    return 0;
}
