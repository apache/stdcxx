// checking for friend templates of templates

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


template <class T>
struct A { enum { i }; };

template <class T>
struct B
{
private:

    template <class U, class V>
    friend int foo (A<U>, B<V>);

    int bar (int i) const { return i; }
};

template <class T, class U>
int foo (A<T> a, B<U> b)
{
    return b.bar (a.i);
}

#ifndef _RWSTD_NO_EXPLICIT_FUNC_INSTANTIATION

// explicitly instantiate to detect bugs such as SunPro 5.3's PR #30227
template int foo (A<int>, B<double>);

#endif   // _RWSTD_NO_EXPLICIT_FUNC_INSTANTIATION

int baz ()
{
    A<int> a;
    B<double> b;

    return foo (a, b);
}
