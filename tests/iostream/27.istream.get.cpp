/************************************************************************
 *
 * 27.istream.get.cpp - test exercising istream::get()
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
#include <sstream>
#include <cstdio>
#include <cstring>

#include <rw_driver.h>


template <class CharT, class FileStream>
void test_get (FileStream &f, const char *type,
                const CharT array [], std::size_t)
{
    rw_assert (!!f, 0, __LINE__, "basic_ifstream<%s, Traits>", type);

    const CharT delim = f.widen ('\xff');

    CharT buf [5] = { 0 };   // 4 chars + terminating null
    const std::size_t bufsize = sizeof buf / sizeof *buf;

    std::size_t n = 0;

    // read chars into character buffer, at most 4 at a time
    while (f.get (buf, bufsize, delim)) {
        
        rw_assert (f.good () && array [n] == *buf, 0, __LINE__,
		   "%d. basic_ifstream<%s>::get (%p, %d, '\\x%x')",
                   n, type, buf, bufsize, delim);

        ++n;

	CharT ch;
	f.get (ch);

        rw_assert (f.good () &&  ch == array [n], 0, __LINE__,
		   "%d. basic_ifstream<%s>::get ('\\x%x'); expected '\\x%x'",
                   n, type, ch, array [n]);
	++n;
    }

    // reached the end of stream
    rw_assert ((std::ios::eofbit | std::ios::failbit) == f.rdstate (),
               0, __LINE__, "basic_ifstream<%s>::get(...)", type);
}


template <class CharT>
void run_test (CharT, const char *tname)
{
    typedef CharT                       char_type;
    typedef std::char_traits<char_type> traits_type;

    CharT array [256];   // array of chars

    const std::size_t nelems = sizeof array / sizeof *array;

    // use tmpname(buffer) instead of tmpnam(0) because HP-UX returns 0
    // for tmpnam (0) on MT environments
    char fname [L_tmpnam];
    std::tmpnam (fname);

    rw_fatal (0 != fname, 0, __LINE__, "tmpnam(%#p) != 0", fname);

    if (1) {
        std::basic_ofstream<char_type, traits_type> f (fname);

        rw_assert (!!f, 0, __LINE__,
                   "basic_ofstream <%s>::basic_ofstream (#%s)",
                    tname, fname);

        // populate array with some chars with '\xff' interspersed

        CharT *p = array;

        for (std::size_t i = 0; std::size_t (i) < nelems / 2; ++i) {
            *p++ = char_type (i + 1);

            // separate each character with a `\xff'
            *p++ = f.widen ('\xff');
        }

        // write array to stream in one shot
        f.write (array, nelems);

        rw_assert (f.good (), 0, __LINE__,
                   "basic_ofstream<%s>::write (%p, %d)",
                   tname, array, nelems);

        f.close ();

        rw_assert (f.good (), 0, __LINE__,
                   "basic_ofstream<%s>::close()", tname);
    }

    if (1) {
        std::basic_ifstream<char_type, traits_type> f (fname);

        rw_assert (!!f, 0, __LINE__,
                   "basic_ifstream<%s>::basic_ifstream (%#s)",
                   tname, fname);

        // see if we can read the whole thing in correctly
        test_get  (f, tname, array, nelems);

        // should have reached EOF above
        rw_assert ((std::ios::eofbit | std::ios::failbit) == f.rdstate (),
                   0, __LINE__,
                   "basic_ifstream<%s>::get(...)", tname);

        f.close ();

        // successful close should have no effect on rdstate
        rw_assert ((std::ios::eofbit | std::ios::failbit) == f.rdstate (),
                   0, __LINE__,
                   "basic_ifstream<%s>::get(...)", tname);
    }

    std::remove (fname);
}


static int run_test (int, char**)
{
    run_test (char (), "char");

#ifndef _RWSTD_NO_WCHAR_T

    run_test (wchar_t (), "wchar_t");
    
#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "istream.get",
                    0 /* no comment */,
                    run_test,
                    "", 0);
}
