/************************************************************************
 *
 * 23.vector.stdcxx-1037.cpp - regression test for STDCXX-1037
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

template <typename T>
struct Alloc : std::allocator<T>
{
    Alloc (int i) : i_ (i) { }

    template <typename U> 
    struct rebind {
        typedef Alloc<U> other;
    };

    template <typename U>
    Alloc (const Alloc<U>& other)
        : std::allocator<T>(other), i_ (other.i_) { }

    template <typename U>
    Alloc& operator= (const Alloc<U>& other)
    {
        std::allocator<T>::operator= (other);
        i_ = other.i_;
        return *this;
    }

    int i_;
};

template <typename T>
bool operator== (const Alloc<T>& a1, const Alloc<T>& a2)
{
    return a1.i_ == a2.i_;
}

int main ()
{
    std::vector<int, Alloc<int> > v1 (Alloc<int> (1));
    v1.push_back (1);
    assert (1 == v1.size () && 1 == v1.front ());

    std::vector<int, Alloc<int> > v2( Alloc<int> (2));
    v2.push_back (2);
    assert (1 == v2.size () && 2 == v2.front ());

    v1.swap (v2);

    assert (1 == v1.size () && 2 == v1.front ());
    assert (1 == v2.size () && 1 == v2.front ());

    return 0;
}
