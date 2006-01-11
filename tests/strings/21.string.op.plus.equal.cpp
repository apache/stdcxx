/***************************************************************************
 *
 * 21.string.plus.equal.cpp - test exercising [lib.string.op+=]
 *
 * $Id$
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

#include <string>

#include <cmdopt.h>
#include <driver.h>

/**************************************************************************/

template <class _CharT>
class Lit { };

_RWSTD_SPECIALIZED_CLASS
class Lit<char>
{
public:
    static const char null[];
    static const char abcdefg[];
    static const char dummy[];
    static const char testString[];
    static const char differentString[];
    static const char veryLongTestString[];
    static const char a[];
    static const char b[];
    static const char longTestString[];
    static const char anotherTestString[];
    static const char testStringAnotherTestString[];
    static const char reallyLong[];
    static const char t[];
    static const char s[];
    static const char testStrings[];

};

const char Lit<char>:: null[] = "\0";
const char Lit<char>:: abcdefg[] = "abcdefg";
const char Lit<char>:: dummy[] = "dummy";
const char Lit<char>:: testString[] = "test string";
const char Lit<char>:: differentString[]= "different string";
const char Lit<char>:: veryLongTestString[] = "very long test string";
const char Lit<char>:: a[] = "a";
const char Lit<char>:: b[] = "b";
const char Lit<char>:: longTestString[] = "long test string";
const char Lit<char>:: anotherTestString[] = "another test string";
const char Lit<char>:: testStringAnotherTestString[] = "test stringanother test string";

const char Lit<char>:: reallyLong[] = "Really, really long test string that seems to never stop";
const char Lit<char>:: t[] = "t";
const char Lit<char>:: s[] = "s";
const char Lit<char>:: testStrings[] = "test strings";


#ifndef _RWSTD_NO_WCHAR_T
_RWSTD_SPECIALIZED_CLASS
class Lit<wchar_t>
{
public:
    static const wchar_t null[];
    static const wchar_t abcdefg[];
    static const wchar_t dummy[];
    static const wchar_t testString[];
    static const wchar_t differentString[];
    static const wchar_t veryLongTestString[];
    static const wchar_t a[];
    static const wchar_t b[];
    static const wchar_t longTestString[];
    static const wchar_t anotherTestString[];
    static const wchar_t testStringAnotherTestString[];
    static const wchar_t reallyLong[];
    static const wchar_t t[];
    static const wchar_t s[];
    static const wchar_t testStrings[];
};

const wchar_t Lit<wchar_t>:: null[] = L"\0";
const wchar_t Lit<wchar_t>:: abcdefg[] = L"abcdefg";
const wchar_t Lit<wchar_t>:: dummy[] = L"dummy";
const wchar_t Lit<wchar_t>:: testString[] = L"test string";
const wchar_t Lit<wchar_t>:: differentString[] = L"different string";
const wchar_t Lit<wchar_t>:: veryLongTestString[] = L"very long test string";
const wchar_t Lit<wchar_t>:: a[] = L"a";
const wchar_t Lit<wchar_t>:: b[] = L"b";
const wchar_t Lit<wchar_t>:: longTestString[] = L"long test string";
const wchar_t Lit<wchar_t>:: anotherTestString[] = L"another test string";
const wchar_t Lit<wchar_t>:: testStringAnotherTestString[] = L"test stringanother test string";
const wchar_t Lit<wchar_t>:: reallyLong[] = L"Really, really long test string that seems to never stop";
const wchar_t Lit<wchar_t>:: t[] = L"t";
const wchar_t Lit<wchar_t>:: s[] = L"s";
const wchar_t Lit<wchar_t>:: testStrings[] = L"test strings";
#endif

/**************************************************************************/

template<typename T>
void test_plus_equal (T, const char*)
{
    typedef std::char_traits<T>                     Traits;
    typedef std::allocator<T>                       Allocator;
    typedef std::basic_string<T, Traits, Allocator> String;

    rw_info (0, 0, 0, "Plus-equal operator with string parameter");

    {
        String t1;
        String t2;

        // Assigning nullString plus-equal nullString
        t1 += t2;

        rw_assert ((t1 == Lit<T>::null), 0, __LINE__, "B27");

        // Correctly references nullString
        rw_assert (t1.data () == String ().data (), 0, __LINE__, "B28");

        // Correctly returns length
        rw_assert ((t1.length() == 0), 0, __LINE__, "B29");
    }

    {
        String t1;
        String t2(Lit<T>::testString);


        t1 += t2;

        // Assigning nullString plus-equal char string
        rw_assert ((t1 == t2), 0, __LINE__, "B30");

        // Correctly returns length
        rw_assert ((t1.length() == t2.length()), 0, __LINE__, "B31");
    }

    {
        String t1(Lit<T>::testString);
        String t2;

        t1 += t2;

        // Assigning char string plus-equal nullString
        rw_assert ((t1==Lit<T>::testString), 0, __LINE__, "B32");

        // Correctly returns length
        rw_assert ((t1.length() == 11), 0, __LINE__, "B33");
    }

    {
        String t1 (Lit<T>::testString);
        String t2 (Lit<T>::anotherTestString);

        typename String::size_type t1_len = t1.length ();
        typename String::size_type t2_len = t2.length ();

        t1 += t2;

        // Assigning char string plus-equal char string
        rw_assert ((t1 == Lit<T>::testStringAnotherTestString),
                   0, __LINE__, "B34");

        // Correctly returns length
        rw_assert ((t1.length() == (t1_len+t2_len)), 0, __LINE__, "B35");
    }

    rw_info (0, 0, 0, "Plus-equal operator with const char pointer");

    {
        String t1;

        t1 += Lit<T>::null;

        // Assigning nullString plus-equal nullString
        rw_assert ((t1 == Lit<T>::null), 0, __LINE__, "B36");

        // Correctly references nullString
        rw_assert (t1.data () == String ().data (), 0, __LINE__, "B37");

        // Correctly returns length
        rw_assert ((t1.length() == 0), 0, __LINE__, "B38");
    }

    {
        String t1;
        t1 += Lit<T>::testString;

        // Assigning nullString plus-equal char string
        rw_assert ((t1==Lit<T>::testString), 0, __LINE__, "B39");

        // Correctly returns length
        rw_assert ((t1.length() == 11), 0, __LINE__, "B40");
    }

    {
        String t1(Lit<T>::testString);
        t1 += Lit<T>::null;

        // Assigning char string plus-equal nullString
        rw_assert ((t1 == Lit<T>::testString), 0, __LINE__, "B41");

        // Correctly returns length
        rw_assert ((t1.length() == 11), 0, __LINE__, "B42");
    }

    {
        String t1(Lit<T>::testString);

        typename String::size_type t1_len = t1.length();
        typename String::size_type t2_len =
            std::char_traits<T>::length(Lit<T>::anotherTestString);

        t1 += Lit<T>::anotherTestString;

        // Assigning char string plus-equal char string
        rw_assert ((t1 == Lit<T>::testStringAnotherTestString),
                   0, __LINE__, "B43");

        // Correctly returns length
        rw_assert ((t1.length() == (t1_len+t2_len)), 0, __LINE__, "B44");
    }

    rw_info (0, 0, 0, "Plus-equal operator with char parameter");
    {
        String t1;
        t1 += Lit<T>::null[0];

        // Assigning nullString plus-equal zero
        rw_assert ((t1 != Lit<T>::null), 0, __LINE__, "B45");

        // Correctly returns length
        rw_assert ((t1.length() == 1), 0, __LINE__, "B46");
    }

    {
        String t1;

        t1 += Lit<T>::t[0];

        // Assigning nullString plus-equal char
        rw_assert ((t1 == Lit<T>::t), 0, __LINE__, "B47");

        // Correctly returns length
        rw_assert ((t1.length() == 1), 0, __LINE__, "B48");
    }

    {
        String t1(Lit<T>::testString);

        t1 += Lit<T>::s[0];

        // Assigning char string plus-equal char
        rw_assert ((t1 == Lit<T>::testStrings), 0, __LINE__, "B49");

        // Correctly returns length
        rw_assert ((t1.length() == 12), 0, __LINE__, "B50");
    }
}

/**************************************************************************/

int run_test (int, char*[])
{
    if (rw_enabled ("char"))
        test_plus_equal (char (), "char");
    else
        rw_note (0, __FILE__, __LINE__, "char test disabled");

#ifndef _RWSTD_NO_WCHAR_T

    if (rw_enabled ("wchar_t"))
        test_plus_equal (wchar_t (), "wchar_t");
    else
        rw_note (0, __FILE__, __LINE__, "wchar_t test disabled");

#endif   // _RWSTD_NO_WCHAR_T

    return 0;

}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.op+=",
                    0 /* no comment */, run_test,
                    0 /* co command line options */);
}
