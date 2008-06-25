/************************************************************************
 *
 * 27.filebuf.sgetn.stdcxx-161.cpp - regression test for STDCXX-161
 *
 * http://issues.apache.org/jira/browse/STDCXX-161
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

#include <fstream>
#include <cassert>
#include <cstdio>

#include <rw_file.h>

int main ()
{
    std::filebuf fb;

    const char* const tmpfname = rw_tmpnam (0);

    assert (0 != fb.open (tmpfname, std::ios::out));
    assert (6 == fb.sputn ("ABCDEF", 6));
    assert (0 != fb.close ());

    assert (0 != fb.open (tmpfname,
                          std::ios::in | std::ios::out | std::ios::stdio));

    assert (3 == fb.sputn ("abc", 3));
    assert (-1 != fb.pubsync ());

    char buf [3] = "";
    assert (3 == fb.sgetn (buf, sizeof buf));
    assert (0 == std::filebuf::traits_type::compare ("DEF", buf, 3));

    fb.close ();

    std::remove (tmpfname);

#ifndef _RWSTD_NO_WCHAR_T

    std::wfilebuf wfb;

    assert (0 != wfb.open (tmpfname, std::ios::out));
    assert (6 == wfb.sputn (L"ABCDEF", 6));
    assert (0 != wfb.close ());

    assert (0 != wfb.open (tmpfname,
                           std::ios::in | std::ios::out | std::ios::stdio));

    assert (3 == wfb.sputn (L"abc", 3));
    assert (-1 != wfb.pubsync ());

    wchar_t wbuf [3] = L"";
    assert (3 == wfb.sgetn (wbuf, sizeof wbuf / sizeof *wbuf));
    assert (0 == std::wfilebuf::traits_type::compare (L"DEF", wbuf, 3));

    wfb.close ();

    std::remove (tmpfname);

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}
