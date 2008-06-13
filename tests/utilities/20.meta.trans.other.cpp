// -*- C++ -*-
/***************************************************************************
 *
 * 20.meta.trans.other.cpp - test exercising meta.trans.other
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
 * Copyright 1999-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <rw_driver.h>

// compile out all test code if extensions disabled
#ifndef _RWSTD_NO_EXT_CXX_0X

#include <type_traits>

/**************************************************************************/

template <class T>
struct is_char
{
    enum { val = 0 };
};

_RWSTD_SPECIALIZED_CLASS
struct is_char<char>
{
    enum { val = 1 };
};

// this function should available _only_ if T is char
template <class T>
int
enabled_if_char (_TYPENAME std::enable_if<is_char<T>::val>::type* = 0)
{
    return 1;
}

// this function should be available if T is _not_ char
template <class T>
int
enabled_if_char (_TYPENAME std::enable_if<!is_char<T>::val>::type* = 0)
{
    return 0;
}

static int _cond_if_char (void* = 0)
{
    return 0;
}

static int _cond_if_char (long* = 0)
{
    return 1;
}

template <class T>
int cond_if_char ()
{
    typedef std::conditional<is_char<T>::val, long, void>::type selector;
    return _cond_if_char ((selector*)0);
}


/**************************************************************************/

void test_trait (int line,
                 bool select,
                 const char* trait,
                 const char* typeT,
                 const char* typeU,
                 bool success)
{
    rw_assert (success, 0, line,
               "%s<%b, %s, %s>::type is%{?}n't%{;} %s as expected",
               trait, select, typeT, typeU, !success, select ? typeT : typeU);
}

void test_trait (int line, const char* trait,
                 const char* typeT, const char* typeU,
                 bool success)
{
    rw_assert (success, 0, line,
               "%s<%s>::type is%{?}n't{;} %s as expected",
               trait, typeT, success, typeU);
}

void test_enable_if (int line, const char* typeT, int got, int exp)
{
    const bool success = got == exp;

    rw_assert (success, 0, line, "%s is%{?}n't%{;} %b as expected",
               typeT, !success, exp);
}

#define TEST(Trait,TypeT,TypeU)                                      \
    test_trait(__LINE__, #Trait, #TypeT, #TypeU,                     \
               std::is_same<Trait<TypeT>::type, TypeU>::value)

/**************************************************************************/

static void test_aligned_storage ()
{
}

/**************************************************************************/

static void test_aligned_union ()
{
}

/**************************************************************************/

static void test_decay ()
{
    // equal to remove_extent<remove_reference<T>::type>::type* for arrays
    TEST (std::decay, int[ ], int*);
    TEST (std::decay, int[2], int*);
    TEST (std::decay, int[2][2], int (*)[2]);

    // equal to add_pointer<remove_reference<U>::type>::type for functions
    TEST (std::decay, int(), int (*)());
    TEST (std::decay, int(char, long), int (*)(char, long));

    TEST (std::decay, int(&)(), int (*)());
    TEST (std::decay, int(&)(char, long), int (*)(char, long));

    // otherwise equal to remove_cv<remove_reference<U>::type>::type
    TEST (std::decay,                int, int);
    TEST (std::decay, const          int, int);
    TEST (std::decay,       volatile int, int);
    TEST (std::decay, const volatile int, int);

    TEST (std::decay,                int&, int);
    TEST (std::decay, const          int&, int);
    TEST (std::decay,       volatile int&, int);
    TEST (std::decay, const volatile int&, int);
}

/**************************************************************************/

enum enum_t { E_a = 1 };

static void test_enable_if ()
{
    typedef std::enable_if<true, int>::type int_type;

#undef TEST
#define TEST(Cond,Expect)                                            \
    test_enable_if(__LINE__, #Cond, Cond, Expect)

    TEST (enabled_if_char<         char>(), 1);
    TEST (enabled_if_char<  signed char>(), 0);
    TEST (enabled_if_char<unsigned char>(), 0);

    TEST (enabled_if_char<         short>(), 0);
    TEST (enabled_if_char<  signed short>(), 0);
    TEST (enabled_if_char<unsigned short>(), 0);

    TEST (enabled_if_char<         int>(), 0);
    TEST (enabled_if_char<  signed int>(), 0);
    TEST (enabled_if_char<unsigned int>(), 0);

    TEST (enabled_if_char<         long>(), 0);
    TEST (enabled_if_char<  signed long>(), 0);
    TEST (enabled_if_char<unsigned long>(), 0);

    // miscellaneous other stuff to try and expose problems
    TEST (enabled_if_char<char    ()>(), 0);
    TEST (enabled_if_char<char (*)()>(), 0);
    TEST (enabled_if_char<char (&)()>(), 0);

    TEST (enabled_if_char<char    [1]>(), 0);
    TEST (enabled_if_char<char (*)[1]>(), 0);
    TEST (enabled_if_char<char (&)[1]>(), 0);

    TEST (enabled_if_char<enum_t>(), 0);
}

/**************************************************************************/

static void test_conditional ()
{
    TEST (cond_if_char<void>(), 0);
    TEST (cond_if_char<char>(), 1);

#undef TEST
#define TEST(Trait,Select,TypeT,TypeU,TypeV)                             \
    test_trait(__LINE__, Select, #Trait, #TypeT, #TypeU,                 \
        std::is_same<Trait<Select,TypeT,TypeU>::type, TypeV>::value)

    TEST (std::conditional,  true, char, long, char);
    TEST (std::conditional, false, char, long, long);
}

/**************************************************************************/

static int run_test (int, char*[])
{
    test_aligned_storage ();
    test_aligned_union ();
    test_decay ();
    test_enable_if ();
    test_conditional ();

    return 0;
}

/**************************************************************************/

#else // _RWSTD_NO_EXT_CXX_0X

/**************************************************************************/

static int run_test (int, char*[])
{
    rw_warn (0, 0, __LINE__,
             "test disabled because _RWSTD_NO_EXT_CXX_0X is defined");
    return 0;
}

#endif // !_RWSTD_NO_EXT_CXX_0X

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "meta.trans.other",
                    0 /* no comment */,
                    run_test,
                    0);
}


