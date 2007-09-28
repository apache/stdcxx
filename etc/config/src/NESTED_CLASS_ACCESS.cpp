// checking member class access

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
private:
    enum { e };
    static int f (int i) { return i; }

protected:
    enum { g };
    static int h (int i) { return i; }

public:
    struct AA {
        // check that the compiler implements the resolution of cwg issue 45
        // http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#45
        static int ag () { return f (e); }
        static int ah (A a) { return a.f (e); }
    };
};


struct B: A
{
    struct BB {
        // exercise the same in a derived class
        static int bg () { return h (0); }
        static int bh (B b) { return b.h (b.g); }
    };
};


int foo ()
{
    return A::AA::ag () + A::AA::ah (A ()) + B::BB::bg () + B::BB::bh (B ());
}
