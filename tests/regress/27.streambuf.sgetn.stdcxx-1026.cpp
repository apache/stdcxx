/************************************************************************
 *
 * 27.streambuf.sgetn.stdcxx-1026.cpp - regression test for STDCXX-1026
 *
 * http://issues.apache.org/jira/browse/STDCXX-1026
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
 **************************************************************************/

#include <cassert>
#include <istream>
#include <streambuf>

int main ()
{
    static int x = '0';

    struct MyBuf : std::streambuf {
        // unbuffered, always successfully reads one character
        int_type underflow () {
            assert (gptr () == egptr ());
            return x++;
        }
    } sb;

    // "endless" stream that never reaches EOF
    std::istream endless (&sb);

    char s [4] = "";

    endless.read (s, sizeof s);
 
    // expect to find stream in good state (i.e., not EOF) and
    // to have extracted as many characters as requested with
    // just as many calls to underflow()
    assert (endless.good ());
    assert (sizeof s == endless.gcount ());
    assert ('0' == s [0] && '1' == s [1] && '2' == s [2] && '3' == s [3]);
}
