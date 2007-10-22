/************************************************************************
 *
 * 23.vector.modifiers.stdcxx-495.cpp - regression test for STDCXX-495
 *
 * https://issues.apache.org/jira/browse/STDCXX-495
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
#include <vector>

struct X
{
    int __val;

    X () : __val (0) { }
    X (int val) : __val (val) { }
    X (const X& x) : __val (x.__val) { }
    X& operator= (const X& x) { __val = x.__val; return *this; }
};

bool operator== (const X& left, const X& right)
{
    return left.__val == right.__val;
}

template <class T>
void test ()
{
    std::vector<T> vec;
    vec.push_back (T (1));
    vec.push_back (T (2));
    vec.insert (vec.begin (), vec.back ());
    assert (vec.front () == vec.back ());
}

int main ()
{
    test<int> ();
    test<X> ();

    return 0;
}
