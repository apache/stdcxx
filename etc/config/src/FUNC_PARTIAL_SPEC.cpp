// checking function template overload

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

template <class T, class U>
struct A { };

template <class T, class U>
int foo (A<T, U>, T)
{
    return 0;
}

template <class T, class U>
int foo (A<T, U>, char)
{
    return 1;
}

template <class U>
int foo (A<char, U>, char)
{
    return 2;
}

int main ()
{
    A<double, double>  add;
    A<char, double>    acd;

    return !(0 == foo (add, 0.0) && 1 == foo (add, 'a') && 2 == foo (acd, 'a'));
}
