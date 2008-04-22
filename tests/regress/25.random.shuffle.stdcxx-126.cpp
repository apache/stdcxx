/************************************************************************
 *
 * 25.random.shuffle.stdcxx-126.cpp - regression test for STDCXX-126
 *
 * http://issues.apache.org/jira/browse/STDCXX-126
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

#include <algorithm>
#include <iterator>


template <class IntT>
struct DiffConvertible
{
    DiffConvertible (IntT) { }
    operator IntT () const { return IntT (); }

private:
    template <class T> DiffConvertible (T);   // not defined
};


template <class IntT>
struct Generator
{
    DiffConvertible<IntT> operator() (IntT n) {
        return DiffConvertible<IntT>(n);
    }

private:
    template <class T> void operator() (T);   // not defined
};


// not defined
template <class IntT, class T> void operator+ (T, DiffConvertible<IntT>);
template <class IntT, class T> void operator+ (T, DiffConvertible<IntT>);

int main ()
{
    Generator<std::iterator_traits<int*>::difference_type> gen;

    int a = 0;

    std::random_shuffle<int*>(&a, &a, gen);

    return 0;
}
