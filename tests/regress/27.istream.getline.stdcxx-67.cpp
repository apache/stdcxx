/************************************************************************
 *
 * 27.istream.getline.stdcxx-67.cpp - regression test for STDCXX-67
 *
 * http://issues.apache.org/jira/browse/STDCXX-67
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
    std::istringstream strm ("\n\n");

    char s [3] = { 'x', 'y', 'z' };

    strm.getline (s, 0);

    // 27.6.1.3, p17, bullet 2:
    // Characters are extracted and stored until c == delim for the next
    // available input character c.

    assert (strm.goodbit == strm.rdstate ());
    assert (1 == strm.gcount ());
    assert ('x' == s [0]);
    assert ('y' == s [1]);
    assert ('z' == s [2]);

    strm.getline (s, 1);

    // 27.6.1.3, p17, bullet 3:
    // Characters are extracted and stored until (n - 1) characters
    // are stored (in which case the function calls setstate(failbit)).
    // p20:
    // In any case, it then stores a null character (using charT()) into
    // the next successive location of the array.

    assert (strm.goodbit == strm.rdstate ());
    assert (1 == strm.gcount ());
    assert ('\0' == s [0]);
    assert ('y' == s [1]);
    assert ('z' == s [2]);

    strm.clear ();
    strm.getline (s, 2);

    // 27.6.1.3, p17, bullet 1:
    // Characters are extracted and stored until end-of-file occurs on
    // the input sequence (in which case the function calls setstate(eofbit))
    // p20:
    // In any case, it then stores a null character (using charT()) into
    // the next successive location of the array.

    assert ((strm.eofbit | strm.failbit) == strm.rdstate ());
    assert (0 == strm.gcount ());
    assert ('\0' == s [0]);
    assert ('y' == s [1]);
    assert ('z' == s [2]);

    return 0;
}
