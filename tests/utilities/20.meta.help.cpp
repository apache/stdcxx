// -*- C++ -*-
/***************************************************************************
 *
 * 20.meta.help.cpp - test exercising meta.help
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
 * Copyright 2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <rw_driver.h>
#include <rw/_defs.h>

// compile out all test code if extensions disabled
#ifndef _RWSTD_NO_EXT_CXX_0X

#include <type_traits>

/**************************************************************************/

template <class T, class U>
bool test_is_same_type (T, U)
{
    return 0;
}

template <class T>
bool test_is_same_type (T, T)
{
    return 1;
}

template <class T>
bool test_is_const (T&)
{
    return 0;
}

template <class T>
bool test_is_const (const T&)
{
    return 1;
}

template <class T, class U>
struct test_is_same
{
    enum { value = 0 };
};

template <class T>
struct test_is_same<T,T>
{
    enum { value = 1 };
};

enum enum_t {
    E_a, E_b, E_c
};

/**************************************************************************/

template <typename Type, Type Value>
void test_integral_constant(const char* name, const char* value)
{
    typedef std::integral_constant<Type, Value>
        integral_constant_type;

    // verify the type of value is the same as Type
    const bool a =
        test_is_same_type (integral_constant_type::value, Type ());
    rw_assert (a, 0, __LINE__,
               "integral_constant<%s, %s>::value is%{?}n't%{;} "
               "of type %s", name, value, !a, name);

    // verify the value is as expected
    const bool b =
        integral_constant_type::value == Value;
    rw_assert (b, 0, __LINE__,
               "integral_constant<%s, %s>::value is%{?}n't%{;} "
               "equal to %s", name, value, !b, value);

    // verify that the value is const
    const bool d =
        test_is_const (std::integral_constant<Type,Value>::value);
    rw_assert (d, 0, __LINE__,
               "integral_constant<%s, %s>::value is%{?}n't%{;} "
               "a constant", name, value, !d);

    // verify that the type typedef is correct
    const bool e =
        test_is_same<typename integral_constant_type::type,
                     integral_constant_type>::value;
    rw_assert (e, 0, __LINE__,
               "integral_constant<%s, %s>::type is%{?}n't%{;} "
               "same as integral_constant<%s, %s>",
               name, value, !e, name, value);

    // note: compiler should never allow an instantiation of
    // integral_constant<T, V> if T is not an integral type.
}

template <typename Type, bool Value>
void test_required_type (const char* name, const char* value)
{
    typedef Type integral_constant_type;

    // verify the type of the type typedef
    const bool a =
        test_is_same<typename integral_constant_type::value_type, bool>::value;
    rw_assert (a, 0, __LINE__,
               "%s::value_type is%{?}n't%{;} same as bool",
               name, !a);

    // verify the value is as expected
    const bool b =
        integral_constant_type::value == Value;
    rw_assert (b, 0, __LINE__,
               "%s::value is%{?}n't%{;} equal to %s",
               name, !b, value);

    // verify that Type is same as integral_constant<bool, Value>
    const bool c =
        test_is_same<std::integral_constant<bool, Value>, Type>::value;
    rw_assert (c, 0, __LINE__,
               "%s is%{?}n't%{;} of type integral_constant<bool, %s>",
               name, !c, value);

    test_integral_constant<typename integral_constant_type::value_type,
                           Value>("bool", value);
}

/**************************************************************************/

static int run_test (int, char*[])
{
#define TEST_INTEGRAL_CONSTANT(Type,Value) \
    test_integral_constant<Type, Value>(#Type,#Value)

    TEST_INTEGRAL_CONSTANT (bool, true);
    TEST_INTEGRAL_CONSTANT (bool, false);

    TEST_INTEGRAL_CONSTANT (char, '\0');
    TEST_INTEGRAL_CONSTANT (char,  _RWSTD_CHAR_MIN);
    TEST_INTEGRAL_CONSTANT (char,  _RWSTD_CHAR_MAX);

#ifndef _RWSTD_NO_WCHAR_T
    TEST_INTEGRAL_CONSTANT (wchar_t, L'\0');
    TEST_INTEGRAL_CONSTANT (wchar_t, _RWSTD_WCHAR_MIN);
    TEST_INTEGRAL_CONSTANT (wchar_t, _RWSTD_WCHAR_MAX);
#endif   // _RWSTD_NO_WCHAR_T

    TEST_INTEGRAL_CONSTANT (signed char, 0);
    TEST_INTEGRAL_CONSTANT (signed char, _RWSTD_SCHAR_MIN);
    TEST_INTEGRAL_CONSTANT (signed char, _RWSTD_SCHAR_MAX);

    TEST_INTEGRAL_CONSTANT (signed short, 0);
    TEST_INTEGRAL_CONSTANT (signed short, _RWSTD_SHRT_MIN);
    TEST_INTEGRAL_CONSTANT (signed short, _RWSTD_SHRT_MAX);

    TEST_INTEGRAL_CONSTANT (signed int, 0);
    TEST_INTEGRAL_CONSTANT (signed int, _RWSTD_INT_MIN);
    TEST_INTEGRAL_CONSTANT (signed int, _RWSTD_INT_MAX);

    TEST_INTEGRAL_CONSTANT (signed long, 0);
    TEST_INTEGRAL_CONSTANT (signed long, _RWSTD_LONG_MIN);
    TEST_INTEGRAL_CONSTANT (signed long, _RWSTD_LONG_MAX);

#ifndef _RWSTD_NO_LONG_LONG
    TEST_INTEGRAL_CONSTANT (signed long long, 0);
    TEST_INTEGRAL_CONSTANT (signed long long, _RWSTD_LLONG_MIN);
    TEST_INTEGRAL_CONSTANT (signed long long, _RWSTD_LLONG_MAX);
#endif   // _RWSTD_NO_LONG_LONG

    TEST_INTEGRAL_CONSTANT (unsigned char, 1);
    TEST_INTEGRAL_CONSTANT (unsigned char, _RWSTD_UCHAR_MIN);
    TEST_INTEGRAL_CONSTANT (unsigned char, _RWSTD_UCHAR_MAX);

    TEST_INTEGRAL_CONSTANT (unsigned short, 1);
    TEST_INTEGRAL_CONSTANT (unsigned short, _RWSTD_USHRT_MIN);
    TEST_INTEGRAL_CONSTANT (unsigned short, _RWSTD_USHRT_MAX);

    TEST_INTEGRAL_CONSTANT (unsigned int, 1);
    TEST_INTEGRAL_CONSTANT (unsigned int, _RWSTD_UINT_MIN);
    TEST_INTEGRAL_CONSTANT (unsigned int, _RWSTD_UINT_MAX);

    TEST_INTEGRAL_CONSTANT (unsigned long, 1);
    TEST_INTEGRAL_CONSTANT (unsigned long, _RWSTD_UINT_MIN);
    TEST_INTEGRAL_CONSTANT (unsigned long, _RWSTD_UINT_MAX);

#ifndef _RWSTD_NO_LONG_LONG
    TEST_INTEGRAL_CONSTANT (unsigned long long, 1);
    TEST_INTEGRAL_CONSTANT (unsigned long long, _RWSTD_ULLONG_MIN);
    TEST_INTEGRAL_CONSTANT (unsigned long long, _RWSTD_ULLONG_MAX);
#endif   // _RWSTD_NO_LONG_LONG

    TEST_INTEGRAL_CONSTANT (enum_t,  E_a);
    TEST_INTEGRAL_CONSTANT (enum_t,  E_b);
    TEST_INTEGRAL_CONSTANT (enum_t,  E_c);

#ifdef _ILL_FORMED
    TEST_INTEGRAL_CONSTANT (float, 1.f);
#endif // _ILL_FORMED

#define TEST_REQUIRED_TYPE(Type,Value) \
    test_required_type<Type, Value>(#Type, #Value);

    TEST_REQUIRED_TYPE (std::true_type , true);
    TEST_REQUIRED_TYPE (std::false_type, false);

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
                    "meta.help",
                    0 /* no comment */,
                    run_test,
                    0);
}
