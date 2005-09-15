/***************************************************************************
 *
 * 21.string.assign.cpp - test exercising [lib.string.assign]
 *
 * $Id: //stdlib/dev/tests/stdlib/string/assign.cpp#1 $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 *
 **************************************************************************/

#include <stdexcept>   // for out_of_range
#include <string>

#include <cmdopt.h>
#include <driver.h>
#include <valcmp.h>

/**************************************************************************/

template <class charT>
struct Lit
{
    static const charT null[];
    static const charT space[];
    static const charT a[];
    static const charT n[];
    static const charT s[];
    static const charT x[];
    static const charT st[];
    static const charT abc[];
    static const charT tes[];
    static const charT xxx[];
    static const charT string[];
    static const charT firstString[];
    static const charT firstSecondString[];
    static const charT firstStringSecond[];
    static const charT firFirstString[];
    static const charT firstFirstStString[];
    static const charT firstStndring[];
    static const charT firstnString[];
    static const charT firstStrings[];
    static const charT sFirstString[];
    static const charT secondFirstString[];
    static const charT second[];
    static const charT coFirstString[];
    static const charT testString[];
    static const charT longTest[];
};

#define LIT(member) \
    template <class charT> const charT Lit<charT>::member[]


LIT (null) = { 0 };
LIT (space) = { ' ', 0 };
LIT (a) = { 'a', 0 };
LIT (n) = { 'n', 0 };
LIT (s) = { 's', 0 };
LIT (x) = { 'x', 0 };
LIT (st) = { 's', 't', 0 };
LIT (abc) = { 'a', 'b', 'c', 0 };
LIT (tes) = { 't', 'e', 's', 0 };
LIT (xxx) = { 'x','x','x', 0 };
LIT (string) = { 's', 't', 'r', 'i', 'n', 'g', 0 };
LIT (firstString) = {
    'F', 'i', 'r', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (firstSecondString) = {
    'F', 'i', 'r', 's', 't', 'S', 'e', 'c', 'o', 'n', 'd', ' ',
    's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (firstStringSecond) = {
    'F', 'i', 'r', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g',
    'S', 'e', 'c', 'o', 'n', 'd', 0
};
LIT (firFirstString) = {
    'F', 'i', 'r', 'F', 'i', 'r', 's', 't', ' ',
    's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (firstFirstStString) = {
    'F', 'i', 'r', 's', 't', 'F', 'i', 'r', 's', 't', ' ', 's', 't', ' ',
    's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (firstStndring) = {
    'F', 'i', 'r', 's', 't', ' ', 's', 't', 'n', 'd', 'r', 'i', 'n', 'g', 0
};
LIT (firstnString) = {
    'F', 'i', 'r', 's', 't', 'n', ' ', 's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (firstStrings) = {
    'F', 'i', 'r', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g', 's',  0
};
LIT (sFirstString) = {
    's', 'F', 'i', 'r', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (secondFirstString) = {
    'S', 'e', 'c', 'o', 'n', 'd', 'F', 'i', 'r', 's', 't', ' ',
    's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (second) = { 'S', 'e', 'c', 'o', 'n', 'd', 0 };
LIT (coFirstString) = {
    'c', 'o', 'F', 'i', 'r', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (testString) = {
    't', 'e', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (longTest) = {
    'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ',
    'v', 'e', 'r', 'y', ' ',  'l', 'o', 'n', 'g', ' ',
    't', 'e', 's', 't', ' ',
    's', 't', 'r', 'i', 'n', 'g', '.', 0
};

/**************************************************************************/

template <class charT>
void test_assign (charT, const char *cname)
{
    _RWSTD_UNUSED (cname);

    rw_info (0, 0, 0, "Assign member with string parameter");

    typedef std::char_traits<charT>                     Traits;
    typedef std::allocator<charT>                       Allocator;
    typedef std::basic_string<charT, Traits, Allocator> String;

    typedef typename String::size_type size_type;

#ifndef _RWSTD_NO_EXCEPTIONS

    {
        bool threw_error = false;
        String ts(Lit<charT>::testString), nl(Lit<charT>::abc);
        try { ts.assign(nl, nl.length() + 1, String::npos); }
        catch (std::out_of_range) { threw_error = true; }

        // Threw exception when pos too large
        rw_assert ((threw_error==true), 0, __LINE__, "A1");
    }

#endif   // _RWSTD_NO_EXCEPTIONS

  {
    String s1, s2, s3, nl;

    s1.assign(nl);
    s2.assign(nl, 0, 5);
    s3.assign(nl, 0, 0);

    // Assigned nullString to nullString
    rw_assert ((s1==nl) && (s2==nl) && (s3==nl), 0, __LINE__, "A2");

    // Correctly references nullString
    rw_assert (   String ().data () == s1.data ()
               && String ().data () == s2.data ()
               && String ().data () == s3.data (),
                  0, __LINE__, "A3");
  }
  {
    String s1, s2, s3, s4, s5, ts(Lit<charT>::testString);

    s1.assign (ts);              // Whole string
    s2.assign (ts, 0, 0);        // None of the string
    s3.assign (ts, 0, 3);        // First three letters
    s4.assign (ts, 5, 6);        // Second word
    s5.assign (ts, 2, 2);        // Middle

    // General Assignments
    
    rw_assert (s1 == ts, 0, __LINE__, "A4.1");
    rw_assert (s2 == Lit<charT>::null, 0, __LINE__, "A4.2");
    rw_assert (s3 == Lit<charT>::tes, 0, __LINE__, "A4.3");
    rw_assert (s4 == Lit<charT>::string, 0, __LINE__, "A4.4");
    rw_assert (s5 == Lit<charT>::st, 0, __LINE__, "A4.5");
  }

  rw_info (0, 0, 0, "Assign member with char pointer and maybe count");

  {
    String s1, s2, s3;
    s1.assign (Lit<charT>::null);
    s2.assign (Lit<charT>::null, 0, 5);
    s3.assign (Lit<charT>::null, 0, 0);

    // Assigned nullString to nullString
    rw_assert (   (s1==Lit<charT>::null)
               && (s2==Lit<charT>::null)
               && (s3==Lit<charT>::null),
               0, __LINE__, "A5");

    // Correctly references nullString
    rw_assert (   String ().data () == s1.data ()
               && String ().data () == s2.data ()
               && String ().data () == s3.data (),
               0, __LINE__, "A6");
  }
  {
    const charT * ts = Lit<charT>::testString;
    String s1, s2, s3;
    s1.assign(ts);            // Whole string
    s2.assign(ts, 0, 0);         // None of the string
    s3.assign(ts, 0, 3);         // First three letters

    // General Assignments
    rw_assert (s1 == ts && s2 == Lit<charT>::null && s3 == Lit<charT>::tes,
               0, __LINE__, "A7");
  }

  rw_info (0, 0, 0, "Assign member with char and repetition count");

  {
    String s1;
    s1.assign(0, Lit<charT>::space[0]);

    // Assigned nullString to nullString
    rw_assert ((s1==Lit<charT>::null), 0, __LINE__, "A8");

    // Correctly references nullString
    rw_assert (String ().data () == s1.data (), 0, __LINE__, "A9");
  }
  {
    String s1, s2, s3;
    s1.assign(1, Lit<charT>::x[0]);
    s2.assign(0, Lit<charT>::x[0]);
    s3.assign(3, Lit<charT>::x[0]);

    // General Assignments
    rw_assert (   (s1 == Lit<charT>::x)
               && (s2 == Lit<charT>::null)
               && (s3 == Lit<charT>::xxx),
               0, __LINE__, "A10");
  }
}

/**************************************************************************/

int run_test (int, char*[])
{
    if (rw_enabled ("char"))
        test_assign (char (), "char");
    else
        rw_note (0, __FILE__, __LINE__, "char test disabled");

#ifndef _RWSTD_NO_WCHAR_T

    if (rw_enabled ("wchar_t"))
        test_assign (wchar_t (), "wchar_t");
    else
        rw_note (0, __FILE__, __LINE__, "wchar_t test disabled");

#endif   // _RWSTD_NO_WCHAR_T

    return 0;

}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.assign",
                    0 /* no comment */, run_test,
                    0 /* co command line options */);
}
