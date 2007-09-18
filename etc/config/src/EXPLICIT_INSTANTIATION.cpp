// checking for explicit instantiation

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

// IMPLICIT_INSTANTIATION.cpp depends on foo<T>(T)
template <class T>
T foo (T t) { return t; }

template <class T>
struct S
{
    T bar () {
        // explicit instantiation of S<T> (and, consequently,
        // S<T>::bar()) must "implicitly" instantiate foo<T>(T)
        return foo (T ());
    }
};

// the symbols expected to be produced by the explicit instantiation
// directives and their attributes are as follows:
//     TEXT             S<int>::bar()
//     WEAK (or TEXT)   foo<int>(int)
//     TEXT             S<char>::bar()
//     WEAK (or TEXT)   foo<char>(char)

// IMPLICIT_INSTANTIATION.cpp uses foo<int>(int)
template class S<int>;
template class S<char>;
