/************************************************************************
 *
 * 27.istream.get.stdcxx-60.cpp - regression test for STDCXX-60
 *
 * http://issues.apache.org/jira/browse/STDCXX-60
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
    // According to LWG issue 243
    // http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#243:
    //
    // "...if the sentry constructor [of an unformatted input function]
    // exits by throwing an exception or if the sentry object returns
    // false, when converted to a value of type bool, the function
    // returns without attempting to obtain any input. In either case
    // [...] unformatted input functions taking a character array of
    // non-zero size as an argument shall also store a null character
    // (using charT()) in the first location of the array."

    std::istringstream strm;

    strm.setstate (strm.failbit);

    char s [3] = { 'x', 'y', 'z' };
    strm.get (s, 2);

    assert ('\0' == s [0]);
    assert ('y' == s [1]);
    assert ('z' == s [2]);

    return 0;
}
