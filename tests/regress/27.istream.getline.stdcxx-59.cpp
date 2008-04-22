/************************************************************************
 *
 * 27.istream.getline.stdcxx-59.cpp - regression test for STDCXX-59
 *
 * http://issues.apache.org/jira/browse/STDCXX-59
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

int main ()
{
    // According to lib.istream.unformatted, p20:
    // "[getline...] stores a null character (using charT())
    // into the next successive location of the array."

    std::istringstream strm;

    strm.setstate (strm.failbit);

    char s [3] = { 'x', 'y', 'z' };
    strm.getline (s, 2);

    assert ('\0' == s [0]);
    assert ('y' == s [1]);
    assert ('z' == s [2]);

#ifndef _RWSTD_NO_WCHAR_T

    std::wistringstream wstrm;

    wstrm.setstate (wstrm.failbit);

    wchar_t ws [3] = { L'x', L'y', L'z' };
    wstrm.getline (ws, 2);

    assert (L'\0' == ws [0]);
    assert (L'y' == ws [1]);
    assert (L'z' == ws [2]);

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}
