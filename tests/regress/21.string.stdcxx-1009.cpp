/************************************************************************
*
* 21.string.stdcxx-1009.cpp - regression test for STDCXX-1009
*
* https://issues.apache.org/jira/browse/STDCXX-1009
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

#include <string>

int main ()
{
    std::string s (4, int ('5'));
    s.assign (4, int ('5'));
    s.append (4, int ('5'));
    s.insert (s.begin (), 4, int ('5'));
    s.replace (s.begin (), s.end (), 4, int ('5'));

    return 0;
}
