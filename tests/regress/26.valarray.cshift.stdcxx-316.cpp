/************************************************************************
 *
 * 26.valarray.cshift.stdcxx-316.cpp - regression test for STDCXX-316
 *
 * http://issues.apache.org/jira/browse/STDCXX-316
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

// Test to detect memory corruption (such as the use of uninitialized
// storage) in valarray. 

#include <cassert>
#include <valarray>

struct S {
    const S* const self;
    S (): self (this) { }
    S (const S &s): self (this) { assert (&s == s.self); }
    ~S () { assert (this == self); }

    void operator= (const S &s) { assert (this == self && &s == s.self); }
};


int main ()
{
    const std::valarray<S> a (2);

    a.cshift (1);

    return 0;
}
