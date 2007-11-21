// checking partial template specialization

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

// primary template
template <class T, class U>
struct PartiallySpecialized
{
    enum { e = 0 };
};

// required by std::vector<bool>
template <class U>
struct PartiallySpecialized<int, U>
{
    enum { f = 1 };
};

// required by std::iterator_traits
template <class U>
struct PartiallySpecialized<int, U*>
{
    enum { g = 2 };
};

// required by std::iterator_traits
template <class U>
struct PartiallySpecialized<int, const U*>
{
    enum { h = 3 };
};

// required by std::allocator<const T>
template <class T, class U>
struct PartiallySpecialized<const T, const U>
{
    enum { i = 4 };
};

// required by __rw_is_same
// exercises bug 638 in SunPro 5.7, patch 117831-02 2005/03/30
template <class T>
struct PartiallySpecialized<T, T>
{
    enum { j = 5 };
};

// compile only, do not link
int foo ()
{
#define PS PartiallySpecialized

    return !(   0 == PS<char, int>::e
             && 1 == PS<int, char>::f
             && 2 == PS<int, char*>::g
             && 3 == PS<int, const char*>::h
             && 4 == PS<const int, const char>::i
             && 5 == PS<long, long>::j);
}
