// checking for member template overloads on return type

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

template <class T, class U, class V>
struct A { };

template <class T>
struct B { };

template <class T>
struct C { };

int n;

template <int N>
struct D
{
    template <class T, class U, class V>
    A<T, U, V>
    foo (T = T ()) { ++n; return A<T, U, V>(); }

    template <class T, class U>
    A<T, U, C<T> >
    foo (T t = T ()) { ++n; return foo<T, U, C<T> >(t); }

    template <class T>
    A<T, B<T>, C<T> >
    foo (T t = T ()) { ++n; return foo<T, B<T> >(t); }

    A<int, B<int>, C<int> >
    foo (int i = int ()) { ++n; return foo<int>(i); }
};

int main ()
{
    D<0> d;

    d.foo ();

    return !(4 == n);
}
