// checking partial ordering of function templates

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
struct S { };

template <class T>
int operator== (const T &a, const T &b)
{
    return a.foobar (b);
}

template <class T>
int foo (T &a, T &b)
{
    return a.foobar (b);
}

template <class T>
int operator== (const S<T>&, const S<T>&)   // <-- testing
{
    return 1;
}

template <class T>
int foo (S<T> &a, S<T> &b)   // <-- testing
{
    return a == b;
}

int bar ()
{
    S<int> s;
    return !(s == s && foo (s, s));
}
