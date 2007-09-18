// checking for pointer exception specification

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

struct S
{
    void foo () throw () { }
    void bar () const throw (int) { }
    void foobar () throw (int, double) { }
};

void foo () throw (char, long)
{
    // exception specification is not part of a function type (15.4, p12)
    // but must be observed when assigning the adress of a function to
    // a pointer to a function (15.4, p3)
    void (*p_foo)() throw (char, long)         = &foo;

    void (S::*p_mfoo)() throw ()               = &S::foo;
    void (S::*p_mbar)() const throw (int)      = &S::bar;
    void (S::*p_mfoobar)() throw (int, double) = &S::foobar;

    // prevent "unused" warnings
    (void)&p_foo;
    (void)&p_mfoo;
    (void)&p_mbar;
    (void)&p_mfoobar;
}
