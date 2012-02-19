/************************************************************************
*
* 23.vector.bool.stdcxx-1062.cpp - regression test for STDCXX-1062
*
* https://issues.apache.org/jira/browse/STDCXX-1062
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

#define _RWSTD_BOUNDS_CHECKING

#include <cassert>
#include <vector>
#include <stdexcept>

int main()
{
    std::vector<bool> v(1);

    try {
        v.at (v.size());
        assert (!"Expect std::out_of_range, got nothing");
    }
    catch (const std::out_of_range&) {
    }
    catch ( ... ) {
        assert (!"Expect out_of_range, got other exception");
    }

    try {
        v [v.size()];
        assert (!"Expect std::out_of_range, got nothing");
    }
    catch (const std::out_of_range&) {
    }
    catch ( ... ) {
        assert (!"Expect out_of_range, got other exception");
    }

    return 0;
}
