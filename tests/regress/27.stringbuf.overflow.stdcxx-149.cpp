/************************************************************************
 *
 * 27.stringbuf.overflow.stdcxx-149.cpp - regression test for STDCXX-149
 *
 * http://issues.apache.org/jira/browse/STDCXX-149
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
#include <sstream>

int main ()
{
    // Exercise the implementation of the resolution of LWG issue 432:
    // http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#432

    struct MyBuf : std::stringbuf {
        using std::stringbuf::eback;
        using std::stringbuf::egptr;
        using std::stringbuf::pbase;
        using std::stringbuf::pptr;
        using std::stringbuf::epptr;
        using std::stringbuf::overflow;
    } buf;

    buf.overflow ('x');

    const int write_positions = int (buf.epptr () - buf.pbase ());

    const int pass = 1 < write_positions;

    assert (pass);

    return 1 - pass;
}
