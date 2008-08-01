// checking nondeduced context

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

#include "config.h"


// T::I is not deducible directly, but can be
// deduced if the T in const T* can (14.8.2.4, p3)
template <class T>
void foo (typename T::I, const T*);

template <class T>
struct A
{
    typedef T I;

    void bar () { foo (I (), this); }
};

// S<T>::I is not deducible directly, but can be deduced
// if the T in the second argument can (14.8.2.4, p3)
template <class T>
void foo (typename A<T>::I, const A<T>*);

template <class T>
void operator+ (typename A<T>::I, const A<T>&);

void foo ()
{
    A<int> a;

    1 + a;

    a.bar ();
}


template <class T>
struct B
{
    // U::I is not deducible directly, byt can be deduced if U can
    template <class U>
    void baz (const U&, typename U::I) { }
};


void bar ()
{
    A<int> a;
    B<int> b;

    b.baz (a, ' ');
}
