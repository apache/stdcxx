/**************************************************************************
 *
 * auto_ptr.cpp - Example program of auto_ptr.
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <iostream>   // for cout, endl
#include <memory>     // for auto_ptr

#include <examples.h>


// A simple structure.
class X
{
    int i_;

public:

    X (int i) : i_ (i) {
        std::cout << "X::X (" << i_ << ')' << std::endl;
    }

    ~X () {
        std::cout << "X::~X [" << i_ << ']' << std::endl;
    }

    int get () const { return i_; }
};


int main ()
{
    // a implicitly initialized to 0 (the null pointer)
    std::auto_ptr<X> a;

    // Establish a scope.
    if (1) {
        // b will hold a pointer to an X.
        std::auto_ptr<X> b (new X (12345));

        // a will now be the owner of the underlying pointer.
        a = b;
    }

    std::cout << "b destroyed" << std::endl;

        // Output the value contained by the underlying pointer.
#ifndef _RWSTD_NO_NONCLASS_ARROW_RETURN
        std::cout << a->get () << std::endl;
#else
        std::cout << (*a).get () << std::endl;
#endif

    // The pointer will be delete'd when a is destroyed on leaving scope.
    return 0;
}
