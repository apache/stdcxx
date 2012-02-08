/************************************************************************
 *
 * 26.valarray.binary.stdcxx-1061.cpp - regression test for STDCXX-1061
 *
 *   http://issues.apache.org/jira/browse/STDCXX-1061
 *
 * $Id: 26.valarray.binary.stdcxx-1061.cpp 590132 2007-10-30 16:01:33Z faridz $
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
#include <cstddef>
#include <valarray>

int main ()
{
    const int a[] = { 0, 1, 0, 3, 0, -5, 0, -7, 0, -11 };

    const std::valarray<int>  v0 (a, sizeof a / sizeof *a);
    const std::valarray<bool> v1 = std::operator&& (v0, 1);

    for (std::size_t i = 0; i < sizeof a / sizeof *a; ++i)
        assert ((a [i] && 1) == v1 [i]);
    
    return 0;
}

