/***************************************************************************
 *
 * access.cpp - test exercising [lib.string.access]
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

#include <stdexcept>   // for out_of_range
#include <string>      // for basic_string

#include <cmdopt.h>
#include <driver.h>

/**************************************************************************/

template <class charT>
class Lit;

_RWSTD_SPECIALIZED_CLASS
class Lit<char>
{
public:
    static const char null[];
    static const char a[];
    static const char e[];
    static const char s[];
    static const char t[];
    static const char x[];
    static const char T[];
    static const char _TypeT[];
    static const char space[];
    static const char testString[];
    static const char testString2[];
    static const char text[];
    static const char testNullString[];
    static const char testStringToBeWipedClean[];
    static const char test[];
    static const char anotherTest[];
    static const char testNull00String[];
    static const char tString[];
    static const char str[];
    static const char tNull00s[];
    static const char more[];
};

const char Lit<char>:: null[] = "\0";
const char Lit<char>:: a[] = "a";
const char Lit<char>:: e[] = "e";
const char Lit<char>:: s[] = "s";
const char Lit<char>:: t[] = "t";
const char Lit<char>:: T[] = "T";
const char Lit<char>:: _TypeT[] = "_TypeT";
const char Lit<char>:: x[] = "x";
const char Lit<char>:: space[] = " ";
const char Lit<char>:: testString[] = "test string";
const char Lit<char>:: testString2[] = "Test string";
const char Lit<char>:: text[] = "Text";
const char Lit<char>:: testNullString[] = "Test\0string";
const char Lit<char>:: testStringToBeWipedClean[] =
"Test string to be wiped clean";
const char Lit<char>:: test[] = "Test";
const char Lit<char>:: anotherTest[] = "Another test";
const char Lit<char>:: testNull00String[] = "Test\000 string";
const char Lit<char>:: tString[] = "t string";
const char Lit<char>:: str[] = "str";
const char Lit<char>:: tNull00s[] = "t\000 s";
const char Lit<char>:: more[] = "more";


#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
class Lit<wchar_t>
{
public:
    static const wchar_t null[];
    static const wchar_t a[];
    static const wchar_t e[];
    static const wchar_t s[];
    static const wchar_t t[];
    static const wchar_t T[];
    static const wchar_t _TypeT[];
    static const wchar_t x[];
    static const wchar_t space[];
    static const wchar_t testString[];
    static const wchar_t testString2[];
    static const wchar_t text[];
    static const wchar_t testNullString[];
    static const wchar_t testStringToBeWipedClean[];
    static const wchar_t test[];
    static const wchar_t anotherTest[];
    static const wchar_t testNull00String[];
    static const wchar_t tString[];
    static const wchar_t str[];
    static const wchar_t tNull00s[];
    static const wchar_t more[];

};

const wchar_t Lit<wchar_t>:: null[] = L"\0";
const wchar_t Lit<wchar_t>:: a[] = L"a";
const wchar_t Lit<wchar_t>:: e[] = L"e";
const wchar_t Lit<wchar_t>:: s[] = L"s";
const wchar_t Lit<wchar_t>:: t[] = L"t";
const wchar_t Lit<wchar_t>:: T[] = L"T";
const wchar_t Lit<wchar_t>:: _TypeT[] = L"_TypeT";
const wchar_t Lit<wchar_t>:: x[] = L"x";
const wchar_t Lit<wchar_t>:: space[] = L" ";
const wchar_t Lit<wchar_t>:: testString[] = L"test string";
const wchar_t Lit<wchar_t>:: testString2[] = L"Test string";
const wchar_t Lit<wchar_t>:: text[] = L"Text";
const wchar_t Lit<wchar_t>:: testNullString[] = L"Test\0string";
const wchar_t Lit<wchar_t>:: testStringToBeWipedClean[] =
L"Test string to be wiped clean";
const wchar_t Lit<wchar_t>:: test[] = L"Test";
const wchar_t Lit<wchar_t>:: anotherTest[] = L"Another test";
const wchar_t Lit<wchar_t>:: testNull00String[] = L"Test\000 string";
const wchar_t Lit<wchar_t>:: tString[] = L"t string";
const wchar_t Lit<wchar_t>:: str[] = L"str";
const wchar_t Lit<wchar_t>:: tNull00s[] = L"t\000 s";
const wchar_t Lit<wchar_t>:: more[] = L"more";

#endif   // _RWSTD_NO_WCHAR_T

/**************************************************************************/

template <class charT>
void test_at (charT, const char *cname)
{
    rw_case (0, 0,
             "basic_string<charT>::at(size_type)",
             "charT=%s", cname);

    typedef std::char_traits<charT>                     Traits;
    typedef std::allocator<charT>                       Allocator;
    typedef std::basic_string<charT, Traits, Allocator> String;

#ifndef _RWSTD_NO_EXCEPTIONS

    {
        bool threw_error = false;
        const String cts(Lit<charT>::testString);
        try { cts.at(cts.length()+1); }
        catch (std::out_of_range)  { threw_error = true; }
    
        // Const version throws exception past end
        rw_assert (threw_error, 0, __LINE__, "A1");
    
        threw_error = false;
        try { cts.at(cts.length()); }
        catch (std::out_of_range) { threw_error = true; }
    
        // And it throws one on the ending null
        rw_assert (threw_error, 0, __LINE__, "A2");

        String ts(Lit<charT>::testString);
        threw_error = false;
        try { ts.at(ts.length()) = Lit<charT>::a[0]; }
        catch (std::out_of_range) { threw_error = true; }

        // Non-const version throws exception at end
        rw_assert (threw_error, 0, __LINE__, "A3");
    
        threw_error = false;
        try { ts.at(ts.length()+4) = Lit<charT>::a[0]; }
        catch (std::out_of_range) { threw_error = true; }
    
        // Non-const version does throw exception past end
        rw_assert (threw_error, 0, __LINE__, "A4");
    }

#endif   // _RWSTD_NO_EXCEPTIONS

}

/**************************************************************************/

template <class charT>
void test_operator_index (charT, const char *cname)
{
    rw_case (0, 0,
             "basic_string<charT>::operator[](size_type)",
             "charT=%s", cname);

    typedef std::char_traits<charT>                     Traits;
    typedef std::allocator<charT>                       Allocator;
    typedef std::basic_string<charT, Traits, Allocator> String;

#ifndef _RWSTD_NO_EXCEPTIONS

    {
        String ts (10, Lit<charT>::space[0]);
    
        bool threw_error = false;
        try { ts.at(12); }
        catch (std::out_of_range) { threw_error = true; }
    
        // At member range checks
        rw_assert (threw_error, 0, __LINE__, "A7");

#  ifdef _RWSTD_BOUNDS_CHECKING

        threw_error = false;
        try { ts[14];   /* Index past end. */ }
        catch (std::out_of_range) { threw_error = true; }
    
        // Sqare brackets NOW range check
        rw_assert (threw_error, 0, __LINE__, "A8");

#  endif   // _RWSTD_BOUNDS_CHECKING

    }

#endif   //_RWSTD_NO_EXCEPTIONS 

    {
        String ts (Lit<charT>::test);
    
        // Character index 0 correct
        rw_assert (ts [0] == Lit<charT>::T [0], 0, __LINE__, "A9");
    
        // Character index 1 correct
        rw_assert (ts [1] == Lit<charT>::e [0], 0, __LINE__, "A10");
    
        // Character index 2 correct
        rw_assert (ts [2] == Lit<charT>::s [0], 0, __LINE__, "A11");
    
        // Character index 3 correct
        rw_assert (ts [3] == Lit<charT>::t [0], 0, __LINE__, "A12");
    
        // Character at end correct
        //
        // NOTE: operator[] throws an exception if _RWSTD_BOUNDS_CHECKING
        // is defined. ONLY on constant strings can we step one beyond
        // the size of the string and get eos().
        //
        // rw_assert (ts [4] == '\0', 0, __LINE__, "A15");
    
        ts [2] = Lit<charT>::x [0];
        // Changing character works
        rw_assert (ts == Lit<charT>::text, 0, __LINE__, "A14");
    }
}

/**************************************************************************/

template <class charT>
void test_access (charT, const char *cname)
{
    test_at (charT (), cname);

    test_operator_index (charT (), cname);
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    if (rw_enabled ("char"))
        test_access (char (), "char");
    else
        rw_note (0, __FILE__, __LINE__, "char test disabled");

#ifndef _RWSTD_NO_WCHAR_T

    if (rw_enabled ("wchar_t"))
        test_access (wchar_t (), "wchar_t");
    else
        rw_note (0, __FILE__, __LINE__, "wchar_t test disabled");

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.access",
                    0 /* no comment */,
                    run_test,
                    0 /* co command line options */);
}
