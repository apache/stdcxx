/************************************************************************
 *
 * 0.braceexp.cpp - tests exercising the rw_brace_expand() helper
 *
 * $Id$
 *
 ************************************************************************
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

#include <rw_braceexp.h>

#include <stdio.h>        // for fprintf(), stderr
#include <stdlib.h>       // for free()
#include <string.h>       // for strcmp()

// the number of failed tests
static int nerrors;

static void
test (int line, const char* brace_expr, const char* expect)
{
    char buf [128];

    char* result = rw_brace_expand (brace_expr, buf, sizeof (buf), ' ');

    const bool equal =   (expect && result)
                       ? !strcmp (expect, result)
                       : expect == result;

    if (!equal) {

        ++nerrors;

        fprintf (stderr,
                 "%d. rw_brace_expand(\"%s\", ...) failed. "
                 "expected \"%s\" got \"%s\"\n",
                 line, brace_expr,
                 expect ? expect : "(null)",
                 result ? result : "(null)");
    }

    if (result != buf)
        free (result);
}

int main ()
{
#define TEST(s,e) test (__LINE__, s, e)

    TEST ("a", "a");
    TEST ("a\\b", "ab");

    TEST ("{0}"  , "{0}");
    TEST ("\\{0}", "{0}");
    TEST ("{\\0}", "{0}");
    TEST ("{0\\}", "{0}");

    TEST ("{0..1}", "0 1");
    TEST ("\\{0..1}", "{0..1}");
    TEST ("{\\0..1}", "{0..1}");
    TEST ("{0\\..1}", "{0..1}");
    TEST ("{0..\\1}", "{0..1}");
    TEST ("{0..1\\}", "{0..1}");

    TEST ("a{0}",      "a{0}");
    TEST ("a{0}b",     "a{0}b");
    TEST ("a\\{0\\}b", "a{0}b");
    TEST ("a\\{0,123,4\\}b", "a{0,123,4}b");

    TEST ("{0..a}", "{0..a}");
    TEST ("{a..0}", "{a..0}");

    TEST ("{0..0}", "0");
    TEST ("{0..5}", "0 1 2 3 4 5");
    TEST ("{4..2}", "4 3 2");
    TEST ("{a..g}", "a b c d e f g");
    TEST ("{g..c}", "g f e d c");
    TEST ("{A..G}", "A B C D E F G");
    TEST ("{G..C}", "G F E D C");

    TEST ("{,}", "");
    TEST ("{abc,def}", "abc def");
    TEST ("{ab\\c,d\\ef}", "abc def");
    TEST ("abc{d,e,f}", "abcd abce abcf");
    
    TEST ("z{c,a{d..f}a,c}z", "zcz zadaz zaeaz zafaz zcz");
    TEST ("z{c,a{d,e,f}a,c}z", "zcz zadaz zaeaz zafaz zcz");
    
    TEST ("{abc,{,d,e,f,}}", "abc d e f");
    TEST ("{abc,{,d,e,f,}}{x,y}", "abcx abcy x y dx dy ex ey fx fy x y");
    TEST ("{abc,{,d\\,e\\,f,}}", "abc d,e,f");

    TEST ("A{0..3}", "A0 A1 A2 A3");
    TEST ("A{0..2}{6..7}", "A06 A07 A16 A17 A26 A27");
    TEST ("A{A}{0..3}", "A{A}{0..3}");
    TEST ("A{0..3}{A}", "A0{A} A1{A} A2{A} A3{A}");

    TEST ("A{0,{4..7}{,}}", "A0 A4 A4 A5 A5 A6 A6 A7 A7");
    TEST ("a{1,3,x{5..9}y}b", "a1b a3b ax5yb ax6yb ax7yb ax8yb ax9yb");
    TEST ("{en,es}_{US,MX}.*", "en_US.* en_MX.* es_US.* es_MX.*");

    // return 0 on success, 1 on failure
    return !(0 == nerrors);
}
