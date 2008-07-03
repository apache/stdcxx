// checking for member templates

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

struct C
{
    template <class T>
    struct CA;   // fully defined below

    template <class T>
    void foo (T);   // fully defined below
};


// verify that member templates may be defined outside
// the body of the class of which they are a member
template <class T>
struct C::CA { };

template <class T>
void C::foo (T) { }


template <class T>
struct D
{
    template <class U>
    struct DA;   // fully defined below

    template <class U>
    void foo (U);   // fully defined below
};


// verify that member templates may be defined outside
// the body of the template of which they are a member
template <class T>
template <class U>
struct D<T>::DA { };

template <class T>
template <class U>
void D<T>::foo (U) { }


// check that member templates are usable
void bar (C c, C::CA<int> cca, D<int> d, D<char>::DA<int> dda)
{
    // all used types are complete
    c.foo (cca);

    // all used types are complete
    d.foo (dda);
}
