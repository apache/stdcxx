// checking for extern template extension

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

// include a file containing the definition of a template
// and an extern template directive referencing an explicit
// instantiation of the same template in extern_template_imp.o

#include "extern_template_imp.h"

// establish a dependency on extern_template_imp.o to make sure
// the extern_template_imp.cpp is compiled before this file
#ifndef _RWSTD_NO_extern_template_imp

// link with the object file below
// LDOPTS = extern_template_imp.o

#endif   // _RWSTD_NO_extern_template_imp

int main ()
{
    // each of the member function of S<int> is defined to return 1
    // when explicitly instantiated in extern_template_imp.cpp but
    // to return 0 when #included here

    S<int> s;

    int res = 0;

    // verify that calls to inline member functions defined in
    // the body of the template, inline member functions defined
    // outside the body of the template, and out-of-line member
    // functions all compile and link
    res += s.foo ();
    res += s.bar ();
    res += s.baz ();

    // verify that the calls ultimately resolve to the definition
    // emitted by the explicit instantiation in extern_template_imp.o
    // and not the one here
    return !(3 == res);
}
