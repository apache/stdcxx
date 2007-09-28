// checking for dummy default arguments

/***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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
 * Copyright 1999-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

template <class T>
T foo (const T* = 0) { return T (); }

template <>
int foo<int>(const int*) { return 1; }

template <>
long foo<long>(const long*) { return 2; }


struct X { };

template <class T>
struct A {
    int i_;

    A (int i): i_ (i) { }
};

template <class T>
struct B {
    int i_;

    B (int i): i_ (i) { }
};


template <class T>
T bar (const X&, const T* = 0) { return T (); }

template <>
A<int> bar<A<int> >(const X&, const A<int>*) { return A<int>(1); }

template <>
B<long> bar<B<long> >(const X&, const B<long>*) { return B<long>(2); }


int main ()
{
    int result = 0;

    if (0 != foo<char>())
        result = result << 1 + 1;

    if (1 != foo<int>())
        result = result << 1 + 1;

    if (2 != foo<long>())
        result = result << 1 + 1;

    X x;

    if (0 != bar<char>(x))
        result = result << 1 + 1;

    if (1 != bar<A<int> >(x).i_)
        result = result << 1 + 1;

    if (2 != bar<B<long> >(x).i_)
        result = result << 1 + 1;

    return result;
}
