/************************************************************************
 *
 * 27.stringbuf.seekoff.stdcxx-295.cpp - regression test for STDCXX-295
 *
 * http://issues.apache.org/jira/browse/STDCXX-295
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
#include <sstream>

int main()
{
    std::istringstream in;
    std::ostringstream out;
    std::stringstream inout;

    // DR453:
    // http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#453
    //
    // changes 27.7.1.3, p10 (seekoff, called by tellg() and tellp())
    // to read:
    //
    // For a sequence to be positioned, if its next pointer (either
    // gptr() or pptr()) is a null pointer and the new offset newoff
    // is nonzero, the positioning operation fails.

    assert (0 == in.tellg ());
    assert (0 == out.tellp ());
    assert (0 == inout.tellg ());
    assert (0 == inout.tellp ());

#ifndef _RWSTD_NO_WCHAR_T

    std::wistringstream win;
    std::wostringstream wout;
    std::wstringstream winout;

    assert (0 == win.tellg ());
    assert (0 == wout.tellp ());
    assert (0 == winout.tellg ());
    assert (0 == winout.tellp ());

#endif   // _RWSTD_NO_WCHAR_T

}
