/************************************************************************
*
* 21.string.replace.stdcxx-170.cpp - regression test for STDCXX-170
*
* https://issues.apache.org/jira/browse/STDCXX-170
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
#include <string>

int main ()
{
    std::string s ("abc");

    s.replace (s.begin (), s.begin (), s.begin () + 1, s.begin () + 2);
    assert ("babc" == s);

    s = "abc";

    s.replace (s.begin (), s.begin (), s.rbegin () + 1, s.rbegin () + 2);
    assert ("babc" == s);

    s = "abc";

    s.replace (s.begin (), s.begin (), &s [1], &s [2]);
    assert ("babc" == s);

    return 0;
}
