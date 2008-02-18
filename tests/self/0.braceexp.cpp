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
#include <environ.h>      // for rw_putenv()

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


#define TEST(s,e) test (__LINE__, s, e)


////////////////////////////////////////////////////////////////////
// runs Bash 3.2 tests -- see bash-3.2/tests/braces.tests
static void
run_bash_tests ()
{
    TEST ("ff{c,b,a}",   "ffc ffb ffa");
    TEST ("f{d,e,f}g",   "fdg feg ffg");
    TEST ("{l,n,m}xyz",  "lxyz nxyz mxyz");
    TEST ("{abc\\,def}", "{abc,def}");
    TEST ("{abc}",       "{abc}");

    TEST ("\\{a,b,c,d,e}",          "{a,b,c,d,e}");
    TEST ("{x,y,\\{a,b,c}}",        "x} y} {a} b} c}");
    TEST ("{x\\,y,\\{abc\\},trie}", "x,y {abc} trie");

    TEST ("/usr/{ucb/{ex,edit},lib/{ex,how_ex}}",
          "/usr/ucb/ex /usr/ucb/edit /usr/lib/ex /usr/lib/how_ex");

    // we don't have eval
    // TEST ("XXXX\\{`echo a b c | tr ' ' ','`\\}", "XXXX{a,b,c}");
    // TEST ("eval echo XXXX\\{`echo a b c | tr ' ' ','`\\}",
    //       "XXXXa XXXXb XXXXc");

    TEST ("{}",        "{}");
    TEST ("{ }",       "{ }");
    TEST ("}",         "}");
    TEST ("{",         "{");
    TEST ("abcd{efgh", "abcd{efgh");

    TEST ("foo {1,2} bar", "foo 1 2 bar");

    // we don't have eval
    // TEST ("`zecho foo {1,2} bar`",  "foo 1 2 bar");
    // TEST ("$(zecho foo {1,2} bar)", "foo 1 2 bar");

#if 0   // not implemented yet

    // set the three variables
    rw_putenv ("var=baz:varx=vx:vary=vy");

    TEST ("foo{bar,${var}.}", "foobar foobaz.");
    TEST ("foo{bar,${var}}",  "foobar foobaz");

    TEST ("${var}\"{x,y}",    "bazx bazy");
    TEST ("$var{x,y}",        "vx vy");
    TEST ("${var}{x,y}",      "bazx bazy");

    // unset all three variables
    rw_putenv ("var=:varx=:vary=");

#endif   // 0

    TEST ("{1..10}", "1 2 3 4 5 6 7 8 9 10");

    // this doesn't work in Bash 3.2
    // TEST ("{0..10,braces}", "0 1 2 3 4 5 6 7 8 9 10 braces");

    // but this does
    TEST ("{{0..10},braces}", "0 1 2 3 4 5 6 7 8 9 10 braces");
    TEST ("x{{0..10},braces}y",
          "x0y x1y x2y x3y x4y x5y x6y x7y x8y x9y x10y xbracesy");

    TEST ("{3..3}",    "3");
    TEST ("x{3..3}y",  "x3y");
    TEST ("{10..1}",   "10 9 8 7 6 5 4 3 2 1");
    TEST ("{10..1}y",  "10y 9y 8y 7y 6y 5y 4y 3y 2y 1y");
    TEST ("x{10..1}y", "x10y x9y x8y x7y x6y x5y x4y x3y x2y x1y");

    TEST ("{a..f}", "a b c d e f");
    TEST ("{f..a}", "f e d c b a");

    TEST ("{a..A}",
          "a ` _ ^ ]  [ Z Y X W V U T S R Q P O N M L K J I H G F E D C B A");
    TEST ("{A..a}",
          "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [  ] ^ _ ` a");

    TEST ("{f..f}", "f");

    // mixes are incorrectly-formed brace expansions
    TEST ("{1..f}", "{1..f}");
    TEST ("{f..1}", "{f..1}");

    TEST ("0{1..9} {10..20}",
          "01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20");

    // do negative numbers work?
    TEST ("{-1..-10}", "-1 -2 -3 -4 -5 -6 -7 -8 -9 -10");
    TEST ("{-20..0}",
          "-20 -19 -18 -17 -16 -15 -14 -13 -12 -11 -10 "
          "-9 -8 -7 -6 -5 -4 -3 -2 -1 0");

    // weirdly-formed brace expansions -- fixed in post-bash-3.1
    TEST ("a-{b{d,e}}-c",    "a-{bd}-c a-{be}-c");
    TEST ("a-{bdef-{g,i}-c", "a-{bdef-g-c a-{bdef-i-c");
}


int main ()
{
    TEST ("", "");

    TEST ("a", "a");
    TEST ("a\\b", "ab");

    TEST ("{",     "{");
    TEST ("}",     "}");
    TEST ("{}",    "{}");
    TEST ("{ }",   "{ }");
    TEST ("{  }",  "{  }");   // is this right?
    TEST ("}{",    "}{");
    TEST ("}{}",   "}{}");
    TEST ("}{}{",  "}{}{");
    TEST ("{{",    "{{");
    TEST ("}}",    "}}");
    TEST ("{{ }",  "{{ }");
    TEST ("{{ }}", "{{ }}");

    TEST ("{0}"  , "{0}");
    TEST ("\\{0}", "{0}");
    TEST ("{\\0}", "{0}");
    TEST ("{0\\}", "{0}");

    TEST ("{\\{}", "{{}");
    TEST ("{\\}}", "{}}");

    TEST ("{0..1}", "0 1");
    TEST ("\\{0..1}", "{0..1}");
    TEST ("{\\0..1}", "{0..1}");
    TEST ("{0\\..1}", "{0..1}");
    TEST ("{0.\\.1}", "{0..1}");
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
    TEST ("{+5..6}", "5 6");
    TEST ("{6..+7}", "6 7");
    TEST ("{+7..+8}", "7 8");
    TEST ("{11..21}", "11 12 13 14 15 16 17 18 19 20 21");

    TEST ("{-3..-1}", "-3 -2 -1");
    TEST ("{+3..-2}", "3 2 1 0 -1 -2");
    TEST ("{-3..+2}", "-4 -3 -2 -1 0 1 2");

    TEST ("{a..g}", "a b c d e f g");
    TEST ("{g..c}", "g f e d c");
    TEST ("{A..G}", "A B C D E F G");
    TEST ("{G..C}", "G F E D C");
    TEST ("{AB..CD}", "{AB..CD}");

    TEST ("{,}", "");
    TEST ("{,",  "{,");
    TEST ("a{,}", "a a");
    TEST ("b{,",  "b{,");
    TEST ("{c,}", "c");
    TEST ("{d,",  "{d,");
    TEST ("{,e}", "e");
    TEST ("{,f",  "{,f");
    TEST ("{,}g", "g g");
    TEST ("a{,}b", "ab ab");

    TEST ("{,,}", "");
    TEST ("a{,,}", "a a a");
    TEST ("{b,,}", "b");
    TEST ("{,c,}", "c");
    TEST ("{,,d}", "d");
    TEST ("{,,}e", "e e e");
    TEST ("a{,,}b", "ab ab ab");

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

    TEST ("{A,a}{I,i}{X,x}", "AIX AIx AiX Aix aIX aIx aiX aix");
    TEST ("A{0,{4..7}{,}}", "A0 A4 A4 A5 A5 A6 A6 A7 A7");
    TEST ("a{1,3,x{5..9}y}b", "a1b a3b ax5yb ax6yb ax7yb ax8yb ax9yb");
    TEST ("{en,es}_{US,MX}.*", "en_US.* en_MX.* es_US.* es_MX.*");

    // verify we pass tests from the Bash test suite
    run_bash_tests ();

    // return 0 on success, 1 on failure
    return !(0 == nerrors);
}
