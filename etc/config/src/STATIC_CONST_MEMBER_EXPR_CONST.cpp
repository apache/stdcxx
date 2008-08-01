// checking for type-dependent constant expressions 

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

struct A { typedef int I; };

template <class T>
struct B
{
    typedef int           I;
    typedef typename T::I J;

    static const I i = 0;
    static const J j = 1;
};

template <class T>
const typename B<T>::I B<T>::i;

template <class T>
const typename B<T>::J B<T>::j;

template <class T>
int foo (T)
{
    enum { i = B<T>::i };
    enum { j = B<T>::j };

    return i + j;
}

int bar ()
{
    return foo (A ());
}
