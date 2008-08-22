/************************************************************************
 *
 * 25.uninitialized.copy.stdcxx-976.cpp - regression test for STDCXX-976
 *
 * http://issues.apache.org/jira/browse/STDCXX-976
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

#include <memory>
#include <iterator>


template <class T>
struct InputIterator: std::iterator<std::input_iterator_tag, T>
{
    T *p_;

    InputIterator (T *p): p_ (p) { }
    InputIterator (const InputIterator &rhs): p_ (rhs.p_) { }

    InputIterator& operator= (const InputIterator &rhs)
    { p_ = rhs.p_; return *this; }

    T operator* () const { return *p_; }
    
    InputIterator& operator++ () { return ++p_, *this; }
    InputIterator operator++ (int) {
        return ++p_, InputIterator (p_ - 1);
    }

    bool operator== (const InputIterator &rhs) const { return p_ == rhs.p_; }
    bool operator!= (const InputIterator &rhs) const { return p_ != rhs.p_; }
};

class Test
{
    // private and not defined
    Test (const Test&);

public:
    Test () {}
    Test (Test&) {}
    ~Test () {}
};


int main ()
{
    typedef InputIterator<char>                Iter;
    typedef InputIterator<const char>          CIter;
    typedef InputIterator<volatile char>       VIter;
    typedef InputIterator<const volatile char> CVIter;

    char src [5] = "abcd";
    char dst [5];

    std::uninitialized_copy (Iter (src),   Iter (src + 5),   dst);
    std::uninitialized_copy (CIter (src),  CIter (src + 5),  dst);
    std::uninitialized_copy (VIter (src),  VIter (src + 5),  dst);
    std::uninitialized_copy (CVIter (src), CVIter (src + 5), dst);


    Test test;
    volatile Test vtest;

    std::uninitialized_copy (&test, &test, &test);
    std::uninitialized_copy (&test, &test, &vtest);

    return 0;
}
