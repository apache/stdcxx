/***************************************************************************
 *
 * 23.containers.stdcxx-152.cpp - regression test for STDCXX-152
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

#include <map>
#include <set>
#include <string>
#include <utility>

using namespace std;

template <class T>
struct TestMap
{
    typedef map<T, TestMap> Map;
    typedef typename Map::value_type value_type;
    Map data;
};


template <class T>
struct TestMultiMap
{
    typedef multimap<T, TestMultiMap> MultiMap;
    typedef typename MultiMap::value_type value_type;
    MultiMap data;
};


template <class T>
struct TestSet
{
    typedef set<pair<T, TestSet> > Set;
    typedef typename Set::value_type value_type;
    Set data;
};

bool operator< (const pair<string, TestSet<string> >& lhs,
                const pair<string, TestSet<string> >& rhs)
{
    return lhs.first < rhs.first;
}


template <class T>
struct TestMultiSet
{
    typedef multiset<pair<T, TestMultiSet> > MultiSet;
    typedef typename MultiSet::value_type value_type;
    MultiSet data;
};

bool operator< (const pair<string, TestMultiSet<string> >& lhs,
                const pair<string, TestMultiSet<string> >& rhs)
{
    return lhs.first < rhs.first;
}


int main ()
{
    TestMap<string> test1;
    test1.data.insert (make_pair ("test1", test1));

    TestMultiMap<string> test2;
    test2.data.insert (make_pair ("test2", test2));

    TestSet<string> test3;
    test3.data.insert (make_pair ("test3", test3));

    TestMultiSet<string> test4;
    test4.data.insert (make_pair ("test4", test4));

    return 0;
}
