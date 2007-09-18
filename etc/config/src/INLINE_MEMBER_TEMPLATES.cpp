// checking for inline member templates

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
    template <class T>
    struct AB { };

    template <class T>
    void foo (T) { }
};


template <class T>
struct B
{
    template <class U>
    struct BB { };

    template <class U>
    void foo (U) { }
};


// check that member templates are usable
void foo (A a, A::AB<int> aab, B<int> b, B<char>::BB<int> bbb)
{
    a.foo (aab);

    b.foo (bbb);
}
