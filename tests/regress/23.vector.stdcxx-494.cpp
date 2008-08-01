/************************************************************************
 *
 * 23.vector.stdcxx-494.cpp - regression test for STDCXX-494
 *
 * https://issues.apache.org/jira/browse/STDCXX-611
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

#include <cassert>   // for assert()
#include <cstddef>   // for size_t
#include <memory>    // for allocator
#include <vector>    // for vector


template <class T>
struct EmptyAllocator: std::allocator<T>
{
    template <class U>
    struct rebind { typedef EmptyAllocator<U> other; };

    EmptyAllocator () { }
    EmptyAllocator (const EmptyAllocator &rhs)
        : std::allocator<T>(rhs) { }

    template <class U>
    EmptyAllocator (const EmptyAllocator<U> &rhs)
        : std::allocator<T>(rhs) { }

    void* operator new (std::size_t) { return 0; }
    void operator delete (void*) { }
};


int main ()
{
    // verify that vector doesn't derive from allocator
    // (otherwise the replacement operator new and delete
    // are in conflict)

    typedef std::vector<int, EmptyAllocator<int> > EmptyVector;

    delete new EmptyVector;

    // verifies empty base optimization
    assert (sizeof (std::vector<int>) == sizeof (EmptyVector));

    // assert that the size of vector is exactly 3 pointers
    assert (sizeof (std::vector<int>) == 3 * sizeof (int*));

    return 0;
}
