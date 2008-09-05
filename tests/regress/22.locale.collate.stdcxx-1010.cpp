/************************************************************************
 *
 * 22.locale.collate.stdcxx-1010.cpp - regression test for STDCXX-1010
 *
 *   http://issues.apache.org/jira/browse/STDCXX-1010
 *
 * $Id:$
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
#include <locale>
#include <string>

int main ()
{
    std::locale french;

    // well-known locale names on popular systems
    const char* const names[] = {
        // generic:
        "fr_FR",
        // AIX:
        "fr_FR.8859-15@euro", "fr_FR.IBM-1252", "fr_FR.8859-15",
        "fr_FR.8859-15@preeuro", "fr_FR.IBM-1252@preeuro",
        "fr_FR.ISO8859-1", "fr_FR.IBM-1252@euro",
        // HP-UX:
        "fr_FR.iso88591", "fr_FR.iso885915@euro", "fr_FR.roman8",
        "fr_FR.utf8",
        // Solaris:
        "fr_FR.ISO8859-1", "fr_FR.ISO8859-15", "fr_FR.ISO8859-15@euro",
        "fr_FR.UTF-8", "fr_FR.UTF-8@euro",
        // Windows:
        "fr-FR",

        // sentinel
        0
    };

    // try to find the first matching locale on this system
    for (unsigned i = 0; names [i]; ++i) {
        try {
            french = std::locale (names [i]);
            if (std::has_facet<std::collate<char> >(french))
                break;
        }
        catch (...) {
        }
    }

    // fall back on the "C" locale...

    const char s0[] = "Et la marine va venir à Malte";

    const std::string s1 (s0);
    const std::string s2 (s0);

    const std::collate<char>& col =
        std::use_facet<std::collate<char> >(french);

    const int cmp =
        col.compare (s1.c_str (), s1.c_str () + s1.length (),
                     s2.c_str (), s2.c_str () + s2.length ());

    // verify that strings collate the same
    assert (0 == cmp);
    return cmp;
} 
