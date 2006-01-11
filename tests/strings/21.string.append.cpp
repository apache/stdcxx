/***************************************************************************
 *
 * 21.string.append.cpp - test exercising [lib.string.append]
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

template<class _CharT>
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

template <typename T>
void test_append (T, const char*)
{
    typedef std::char_traits<T>                     Traits;
    typedef std::allocator<T>                       Allocator;
    typedef std::basic_string<T, Traits, Allocator> String;

    rw_info (0, 0, 0, "Assignment operator with string parameter");

    {
        String t1(Lit<T>::abcdefg), t2;
        t2 = t1;
        // Basic string assignment over nullString
        rw_assert ((t2 == t1), 0, __LINE__, "B1");
    }

    {
        String t1(Lit<T>::abcdefg), t2(Lit<T>::dummy);
        t2 = t1;
        // Basic string assignment over dummy string
        rw_assert ((t2 == t1), 0, __LINE__, "B2");
    }

    {
        String t1, t2;
        t2 = t1;
        // Assignment of nullString over nullString
        rw_assert ((t2 == t1), 0, __LINE__, "B3");
    
        // Correctly references nullString
        rw_assert (t2.data () == String ().data (), 0, __LINE__, "B4");
    }

    {
        String t1, t2(Lit<T>::dummy);
        t2 = t1;
        // Assignment of nullString over dummy string
        rw_assert ((t2 == t1), 0, __LINE__, "B5");
    
        // Correctly references nullString
        rw_assert (t2.data () == String ().data (), 0, __LINE__, "B6");
    }
  
    rw_info (0, 0, 0, "Assignment operator with const char pointer");
  
    String nl;
    {
        String t1;
        //t1 = "";
        t1 = Lit<T>::null;
    
        // Assigning nullString literal over nullString
        rw_assert ((nl == t1), 0, __LINE__, "B7");
    
        // Returns the correct data
        const T* temp = t1.data();
        rw_assert ((0 == temp[0]), 0, __LINE__, "B8");

        // Returns the correct c_str
        rw_assert ((*t1.c_str() == Lit<T>::null[0]), 0, __LINE__, "B9");
    }

    {
        String t1(Lit<T>::testString);
        t1 = Lit<T>::null;
    
        // Assigning nullString literal over test string
        rw_assert ((nl == t1), 0, __LINE__, "B10");
    
        // Returns the correct data
        const T* temp = t1.data();
        rw_assert ((0 == temp[0]), 0, __LINE__, "B11");
    
        // Returns the correct c_str
        rw_assert ((*t1.c_str() == Lit<T>::null[0]), 0, __LINE__, "B12");
    }

    {
        String t1(Lit<T>::testString);
    
        t1 = Lit<T>::differentString;
        // Assigning longer literal over shorter literal
        rw_assert ((std::char_traits<T>::compare(
                        t1.data(), Lit<T>::differentString, 17) == 0), 
                   0, __LINE__, "B13");
    
        // Returns the correct c_str
        rw_assert ((std::char_traits<T>::compare(
                        t1.c_str(), Lit<T>::differentString, 17) == 0), 
                   0, __LINE__, "B14");
    
        // Returns the correct length
        rw_assert ((t1.length() == 16), 0, __LINE__, "B15");
    }

    {
        String t1(Lit<T>::veryLongTestString);
        t1 = Lit<T>::differentString;
    
        // Assigning shorter literal over longer literal
        rw_assert ((std::char_traits<T>::compare(
                        t1.data(), Lit<T>::differentString, 17) == 0), 
                   0, __LINE__, "B16");
    
        // Returns the correct c_str
        rw_assert ((std::char_traits<T>::compare(
                        t1.c_str(), Lit<T>::differentString, 17) == 0), 
                   0, __LINE__, "B17");
    
        // Returns the correct length
        rw_assert ((t1.length() == 16), 0, __LINE__, "B18");
    }
  
    rw_info (0, 0, 0, "Assignment operator with char reference");

    {
        String t1;
        t1 = Lit<T>::a[0];
    
        // Assigning character over nullString
        rw_assert ((t1 == Lit<T>::a), 0, __LINE__, "B19");
    
        // Returns correct length
        rw_assert ((t1.length() == 1), 0, __LINE__, "B20");
    }

    {
        String t1(1, Lit<T>::b[0]);
        t1 = Lit<T>::a[0];
        //t1 = 'a';
    
        // Assigning character over single char string
        rw_assert ((t1 == Lit<T>::a), 0, __LINE__, "B21");
    
        // Returns correct length
        rw_assert ((t1.length() == 1), 0, __LINE__, "B22");
    }

    {
        //String t1("Long test string");
        String t1(Lit<T>::longTestString);
        t1 = Lit<T>::a[0];
    
        // Assigning character over long char string
        rw_assert ((t1 == Lit<T>::a), 0, __LINE__, "B23");

        // Returns correct length
        rw_assert ((t1.length() == 1), 0, __LINE__, "B24");
    }

    {
        //String t1("Really, really long test string that seems to never stop");
        String t1(Lit<T>::reallyLong);
        t1 = Lit<T>::a[0];
    
        // Assigning character over really long char string
        rw_assert ((t1 == Lit<T>::a), 0, __LINE__, "B25");
    
        // Returns correct length
        rw_assert ((t1.length() == 1), 0, __LINE__, "B26");
    }
}

/**************************************************************************/

int run_test (int, char*[])
{
    if (rw_enabled ("char"))
        test_append (char (), "char");
    else
        rw_note (0, __FILE__, __LINE__, "char test disabled");

#ifndef _RWSTD_NO_WCHAR_T

    if (rw_enabled ("wchar_t"))
        test_append (wchar_t (), "wchar_t");
    else
        rw_note (0, __FILE__, __LINE__, "wchar_t test disabled");

#endif   // _RWSTD_NO_WCHAR_T

    return 0;

}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.append",
                    0 /* no comment */, run_test,
                    0 /* co command line options */);
}
