// checking for explicit instantiation of members

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
T foo (T t) { return t; }

template <class T>
struct S
{
    T bar () const {
        // explicit instantiation of S<T> (and, consequently,
        // S<T>::bar()) must "implicitly" instantiate foo<T>(T)
        return foo (T ());
    }

    static void baz (T t) {
        foo (t);
    }
};

template int S<int>::bar () const;
template double S<double>::bar () const;

template void S<char>::baz (char);
template void S<unsigned>::baz (unsigned);
