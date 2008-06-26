/************************************************************************
 *
 * 27.filebuf.close.stdcxx-969.cpp - regression test for STDCXX-969
 *
 * http://issues.apache.org/jira/browse/STDCXX-969
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
 **************************************************************************/

#include <cassert>
#include <fstream>

int main ()
{
    struct Filebuf : std::filebuf
    {
        char* pubgptr () const {
            return gptr ();
        }

        char* pubpptr () const {
            return pptr ();
        }
    };

    Filebuf fb;
    fb.open (__FILE__, std::ios::in);

    fb.sgetc ();
    fb.close ();

    // verify that get area is not active after close()
    assert (0 == fb.pubgptr ());
    // verify that put area is not active after close()
    assert (0 == fb.pubpptr ());

    // verify that calling pubsync() after close()
    // doesn't abort the program on MSVC 8.0 and later
    fb.pubsync ();

    return 0;
}
