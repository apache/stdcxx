/***********************************************************************
 *
 * 21.string.io.stdcxx-250.cpp - regression test for STDCXX-250
 *
 *   http://issues.apache.org/jira/browse/STDCXX-250
 *
 * $Id$
 *
 ***********************************************************************
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
 ***********************************************************************/

#include <cassert>
#include <istream>
#include <string>

int main ()
{
    struct MyBuf : std::streambuf {
        int_type underflow () {
            static int i = 0;
            // i == 0: sgect() invoked from sentry ctor
            // i == 1: sgetc() invoked from operator>>()
            // i == 2: sbumpc() invoked from operator>>()
            return 1 < i++ ? throw i : 'x';
        }
    } buf;

    std::istream is (&buf);

    std::string s;
    is >> s;

    assert ("x" == s);
    assert ((is.failbit | is.badbit) == is.rdstate ());

    return 0;
}
