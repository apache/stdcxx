// checking the typename keyword

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

struct A
{
    typedef int I;
};


template <class T>
struct B
{
    typedef typename T::I I;
    typedef const T*      J;

    I foo (I) const;
    J bar (J) const;
};


template <class T>
typename B<T>::I B<T>::foo (typename B<T>::I i) const
{
    return i;
}


template <class T>
typename B<T>::J B<T>::bar (J j) const
{
    return j;
}


int foobar ()
{
    B<A> b;

    // instantiate B and its members
    return b.foo (0) + !b.bar (0);
}
