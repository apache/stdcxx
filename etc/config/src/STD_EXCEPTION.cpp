// checking for class std::exception

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

#include "config.h"

#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE
#    include "terminate.h"
#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


// will fail if namespaces aren't supported
namespace std {

class exception { };

}

// force a failure if namespace std isn't honored
class exception { };


struct B { virtual ~B () { } };
struct D1: B { };
struct D2: B { };

int main ()
{
    D1 d1;
    B &b = d1;

    try {
        // throws [std::] bad_cast
        D2 &d2 = dynamic_cast<D2&>(b);
    }
    catch (exception&) {
        // fail if namespace std isn't honored
        return 1;
    }
    catch (std::exception&) {
        return 0;
    }
    catch (...) {
        // fail if cast throws anything other than an std::exception-derivative
        return 1;
    }

    // fail if cast doesn't throw
    return 2;
}
